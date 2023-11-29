#include <iostream>
#include <WS2tcpip.h>
#include <string>
#include <thread>
#include <fstream>
#pragma comment(lib, "ws2_32.lib")

void receiveOutputAndPrint(SOCKET udpSocket) {
    char buf[4096];
    ZeroMemory(buf, sizeof(buf));

    int bytesIn = recv(udpSocket, buf, sizeof(buf), 0);

    if (bytesIn == SOCKET_ERROR) {
        std::cerr << "Error receiving from server" << std::endl;
    }
    else {
        std::cout << "SERVER> " << std::string(buf, 0, bytesIn) << std::endl;
    }
}

void receiveFileContent(SOCKET udpSocket, const std::string& fileName) {
    char buf[4096];
    ZeroMemory(buf, sizeof(buf));

    int contentBytes = recv(udpSocket, buf, sizeof(buf), 0);

    if (contentBytes == SOCKET_ERROR) {
        std::cerr << "Error receiving file content from server" << std::endl;
    }
    else {
        std::cout << "SERVER> Content of file '" << fileName << "':\n" << std::string(buf, 0, contentBytes) << std::endl;
    }
}

int main() {
    WSADATA wsData;
    WORD ver = MAKEWORD(2, 2);

    int wsOk = WSAStartup(ver, &wsData);
    if (wsOk != 0) {
        std::cerr << "Can't initialize winsock! Quitting" << std::endl;
        return -1;
    }

    SOCKET udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (udpSocket == INVALID_SOCKET) {
        std::cerr << "Can't create a socket! Quitting" << std::endl;
        return -1;
    }

    sockaddr_in server;
    server.sin_family = AF_INET;

    std::cout << "Enter the server's IP address: ";
    std::string serverIp;
    std::getline(std::cin, serverIp);
    inet_pton(AF_INET, serverIp.c_str(), &server.sin_addr);

    std::cout << "Enter the server's port: ";
    std::string serverPortStr;
    std::getline(std::cin, serverPortStr);
    server.sin_port = htons(std::stoi(serverPortStr));

    int adminAttempts = 0;
    std::string password;

    while (adminAttempts < 3) {
        std::cout << "Enter password to connect as admin (Attempt " << adminAttempts + 1 << "/3): ";
        std::getline(std::cin, password);

        sendto(udpSocket, "ADMIN", sizeof("ADMIN"), 0, (sockaddr*)&server, sizeof(server));
        sendto(udpSocket, password.c_str(), password.size() + 1, 0, (sockaddr*)&server, sizeof(server));

        receiveOutputAndPrint(udpSocket);

        char buf[4096];
        ZeroMemory(buf, sizeof(buf));
        int bytesIn = recv(udpSocket, buf, sizeof(buf), 0);

        if (bytesIn != SOCKET_ERROR && std::string(buf) == "ADMIN_ACCESS_GRANTED") {
            std::cout << "Admin access granted." << std::endl;
            break;
        }
        else {
            std::cout << "Incorrect password." << std::endl;
            adminAttempts++;
        }
    }

    if (adminAttempts == 3) {
        std::cout << "Admin access denied. Connecting as guest." << std::endl;
        sendto(udpSocket, "GUEST", sizeof("GUEST"), 0, (sockaddr*)&server, sizeof(server));
    }

    std::string message;

    while (true) {
        std::cout << "Enter command (type QUIT to exit): " << std::endl;

        std::string command;
        std::getline(std::cin, command);

        sendto(udpSocket, command.c_str(), command.size() + 1, 0, (sockaddr*)&server, sizeof(server));

        if (command == "QUIT") {
            break;
        }

        if (command == "READ_FILE") {
            receiveOutputAndPrint(udpSocket);

            std::cout << "Enter the file name to read: ";
            std::string fileName;
            std::getline(std::cin, fileName);

            sendto(udpSocket, fileName.c_str(), fileName.size() + 1, 0, (sockaddr*)&server, sizeof(server));

            receiveFileContent(udpSocket, fileName);
        }
        else if (command == "WRITE_FILE") {
            receiveOutputAndPrint(udpSocket);

            std::cout << "Enter the file name to write: ";
            std::string fileName;
            std::getline(std::cin, fileName);

            sendto(udpSocket, fileName.c_str(), fileName.size() + 1, 0, (sockaddr*)&server, sizeof(server));

            std::cout << "Enter the content to write in the file (type QUIT to cancel): " << std::endl;

            std::string content;
            std::string line;

            while (std::getline(std::cin, line) && line != "QUIT") {
                content += line + '\n';
            }

            sendto(udpSocket, content.c_str(), content.size() + 1, 0, (sockaddr*)&server, sizeof(server));

            receiveOutputAndPrint(udpSocket);
        }
        else if (command == "EXECUTE_FILE") {
            receiveOutputAndPrint(udpSocket);

            std::cout << "Enter the file name to execute: ";
            std::string fileName;
            std::getline(std::cin, fileName);

            sendto(udpSocket, fileName.c_str(), fileName.size() + 1, 0, (sockaddr*)&server, sizeof(server));

            receiveOutputAndPrint(udpSocket);
        }
        else if (command == "WRITE_CODE") {
            receiveOutputAndPrint(udpSocket);

            std::cout << "Enter C++ code (type QUIT to cancel): " << std::endl;

            std::string code;
            std::string line;

            while (std::getline(std::cin, line) && line != "QUIT") {
                code += line + '\n';
            }

            sendto(udpSocket, code.c_str(), code.size() + 1, 0, (sockaddr*)&server, sizeof(server));

            receiveOutputAndPrint(udpSocket);
        }
        else {
            receiveOutputAndPrint(udpSocket);
        }
    }

    closesocket(udpSocket);
    WSACleanup();

    return 0;
}
