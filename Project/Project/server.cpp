#include <iostream>
#include <WS2tcpip.h>
#include <string>
#include <fstream>
#include <cstdlib>
#include <cstdio>
#include <chrono>
#include <thread>
#include <filesystem>
#include <vector>
#include <sstream>
#include <ctime>
#pragma comment(lib, "ws2_32.lib")
#define _CRT_SECURE_NO_WARNINGS

namespace fs = std::filesystem;

std::string GetServerIpAddress() {
    char hostName[256];
    gethostname(hostName, sizeof(hostName));

    struct addrinfo* result = nullptr;
    struct addrinfo hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    if (getaddrinfo(hostName, nullptr, &hints, &result) == 0) {
        char ipAddress[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &((struct sockaddr_in*)result->ai_addr)->sin_addr, ipAddress, INET_ADDRSTRLEN);
        freeaddrinfo(result);
        return ipAddress;
    }

    return "Unknown";
}

struct ClientInfo {
    SOCKET socket;
    sockaddr_in address;
    bool isAdmin;
};

std::vector<ClientInfo> clients;

std::string getCurrentDateTime() {
    auto now = std::chrono::system_clock::now();
    std::time_t currentTime;
    std::time(&currentTime);
    std::tm localTime;
    localtime_s(&localTime, &currentTime);
    std::stringstream ss;
    ss << std::put_time(&localTime, "%Y-%m-%d %X");
    return ss.str();
}

void createFile(const std::string& fileName, const std::string& content) {
    std::ofstream file(fileName);
    if (file.is_open()) {
        file << content;
        file.close();
    }
}

std::string compileExecuteFiles(std::string user_file) {
#ifdef _WIN32
    std::string compileCommand = "g++ -o " + user_file + ".exe " + user_file;
    std::string executeCommand = user_file + ".exe";
#else
    std::string compileCommand = "g++ -o " + user_file + " " + user_file;
    std::string executeCommand = "./" + user_file;
#endif

    int compileResult = system(compileCommand.c_str());

    if (compileResult != 0) {
        std::cerr << "Error compiling code." << std::endl;
        return "Error compiling code.";
    }

    FILE* pipe = _popen(executeCommand.c_str(), "r");

    if (pipe) {
        char output[4096];
        std::string result;

        while (fgets(output, sizeof(output), pipe) != nullptr) {
            result += output;
        }

        _pclose(pipe);
        return result;
    }
    else {
        std::cerr << "Error executing code." << std::endl;
        return "Error executing code.";
    }
}

std::string compileAndExecuteCode() {
#ifdef _WIN32
    std::string compileCommand = "g++ -o server_executable server_executable.cpp";
    std::string executeCommand = ".\\server_executable";
#else
    std::string compileCommand = "g++ -o server_executable server_executable.cpp";
    std::string executeCommand = "./server_executable";
#endif

    int compileResult = system(compileCommand.c_str());

    if (compileResult != 0) {
        std::cerr << "Error compiling code." << std::endl;
        return "Error compiling code.";
    }

    FILE* pipe = _popen(executeCommand.c_str(), "r");

    if (pipe) {
        char output[4096];
        std::string result;

        while (fgets(output, sizeof(output), pipe) != nullptr) {
            result += output;
        }

        _pclose(pipe);
        return result;
    }
    else {
        std::cerr << "Error executing code." << std::endl;
        return "Error executing code.";
    }
}

void sendOutputToClient(const std::string& output, SOCKET udpSocket, const sockaddr_in& client) {
    sendto(udpSocket, output.c_str(), output.size() + 1, 0, (sockaddr*)&client, sizeof(client));
}

void executeFile(const std::string& fileName, SOCKET udpSocket, const sockaddr_in& client, bool isAdmin) {
    std::string output;

#ifdef _WIN32
    std::string executeCommand = fileName + ".exe";
    output = "Executing: " + fileName + "\n" + compileExecuteFiles(fileName);
#else
    std::string executeCommand = "./" + fileName;
    output = "Executing: " + fileName + "\n" + compileExecuteFiles(fileName);
#endif

    sendOutputToClient(output, udpSocket, client);
}

void readFile(const std::string& fileName, SOCKET udpSocket, const sockaddr_in& client) {
    std::ifstream file(fileName);

    if (file.is_open()) {
        std::ostringstream fileContents;
        fileContents << file.rdbuf();
        file.close();

        sendOutputToClient(fileContents.str(), udpSocket, client);
    }
    else {
        sendOutputToClient("Error reading file.", udpSocket, client);
    }
}

void writeFile(const std::string& fileName, const std::string& content, SOCKET udpSocket, const sockaddr_in& client, bool isAdmin) {
    if (isAdmin) {
        std::ofstream file(fileName);

        if (file.is_open()) {
            file << content;
            file.close();

            sendOutputToClient("File written successfully.", udpSocket, client);
        }
        else {
            sendOutputToClient("Error writing file.", udpSocket, client);
        }
    }
    else {
        sendOutputToClient("Permission denied.", udpSocket, client);
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

    sockaddr_in serverHint;
    serverHint.sin_family = AF_INET;
    serverHint.sin_port = htons(1065);
    serverHint.sin_addr.S_un.S_addr = INADDR_ANY;

    bind(udpSocket, (sockaddr*)&serverHint, sizeof(serverHint));

    std::cout << "Server is listening on IP: " << GetServerIpAddress() << ", Port: " << ntohs(serverHint.sin_port) << std::endl;

    std::string cppProgram = R"(
#include <iostream>

int main() {
    // Your code here

    return 0;
}
)";

    createFile("server_executable.cpp", cppProgram);

    char buf[4096];

    sockaddr_in client;
    int clientLength = sizeof(client);

    while (true) {
        ZeroMemory(&client, sizeof(client));
        ZeroMemory(buf, sizeof(buf));

        int bytesIn = recvfrom(udpSocket, buf, sizeof(buf), 0, (sockaddr*)&client, &clientLength);

        if (bytesIn == SOCKET_ERROR) {
            std::cerr << "Error receiving from client" << std::endl;
            continue;
        }

        char clientIp[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client.sin_addr, clientIp, INET_ADDRSTRLEN);

        std::cout << getCurrentDateTime() << " - Message received from " << clientIp << ":" << ntohs(client.sin_port) << " - " << buf << std::endl << std::flush;

        if (std::string(buf) == "EXECUTE_CODE") {
            sendto(udpSocket, "READY_FOR_CODE", sizeof("READY_FOR_CODE"), 0, (sockaddr*)&client, sizeof(client));

            ZeroMemory(buf, sizeof(buf));
            int codeBytes = recvfrom(udpSocket, buf, sizeof(buf), 0, (sockaddr*)&client, &clientLength);
            std::string userCode(buf, 0, codeBytes);

            std::string fullCode = cppProgram;
            size_t pos = cppProgram.find("// Your code here");
            if (pos != std::string::npos) {
                fullCode.replace(pos, 18, userCode);
            }

            createFile("server_executable.cpp", fullCode);

            std::string output = compileAndExecuteCode();

            sendOutputToClient(output, udpSocket, client);
        }
        else if (std::string(buf) == "ADMIN") {
            sendto(udpSocket, "ENTER_PASSWORD", sizeof("ENTER_PASSWORD"), 0, (sockaddr*)&client, sizeof(client));

            ZeroMemory(buf, sizeof(buf));
            int passwordBytes = recvfrom(udpSocket, buf, sizeof(buf), 0, (sockaddr*)&client, &clientLength);
            std::string enteredPassword(buf, 0, passwordBytes);

            if (enteredPassword == "12345") {
                sendto(udpSocket, "ADMIN_ACCESS_GRANTED", sizeof("ADMIN_ACCESS_GRANTED"), 0, (sockaddr*)&client, sizeof(client));

                while (true) {
                    ZeroMemory(buf, sizeof(buf));
                    int adminRequestBytes = recvfrom(udpSocket, buf, sizeof(buf), 0, (sockaddr*)&client, &clientLength);
                    std::string adminRequest(buf, 0, adminRequestBytes);

                    if (adminRequest == "READ_FILE") {
                        sendto(udpSocket, "ENTER_FILE_NAME", sizeof("ENTER_FILE_NAME"), 0, (sockaddr*)&client, sizeof(client));

                        ZeroMemory(buf, sizeof(buf));
                        int fileNameBytes = recvfrom(udpSocket, buf, sizeof(buf), 0, (sockaddr*)&client, &clientLength);
                        std::string fileName(buf, 0, fileNameBytes);

                        readFile(fileName, udpSocket, client);
                    }
                    else if (adminRequest == "WRITE_FILE") {
                        sendto(udpSocket, "ENTER_FILE_NAME", sizeof("ENTER_FILE_NAME"), 0, (sockaddr*)&client, sizeof(client));

                        ZeroMemory(buf, sizeof(buf));
                        int fileNameBytes = recvfrom(udpSocket, buf, sizeof(buf), 0, (sockaddr*)&client, &clientLength);
                        std::string fileName(buf, 0, fileNameBytes);

                        sendto(udpSocket, "ENTER_FILE_CONTENT", sizeof("ENTER_FILE_CONTENT"), 0, (sockaddr*)&client, sizeof(client));

                        ZeroMemory(buf, sizeof(buf));
                        int contentBytes = recvfrom(udpSocket, buf, sizeof(buf), 0, (sockaddr*)&client, &clientLength);
                        std::string content(buf, 0, contentBytes);

                        writeFile(fileName, content, udpSocket, client, true);
                    }
                    else if (adminRequest == "EXECUTE_FILE") {
                        sendto(udpSocket, "ENTER_FILE_NAME", sizeof("ENTER_FILE_NAME"), 0, (sockaddr*)&client, sizeof(client));

                        ZeroMemory(buf, sizeof(buf));
                        int fileNameBytes = recvfrom(udpSocket, buf, sizeof(buf), 0, (sockaddr*)&client, &clientLength);
                        std::string fileName(buf, 0, fileNameBytes);

                        executeFile(fileName, udpSocket, client, true);
                    }
                    else if (adminRequest == "EXECUTE_CODE") {
                        sendto(udpSocket, "READY_FOR_CODE", sizeof("READY_FOR_CODE"), 0, (sockaddr*)&client, sizeof(client));

                        ZeroMemory(buf, sizeof(buf));
                        int codeBytes = recvfrom(udpSocket, buf, sizeof(buf), 0, (sockaddr*)&client, &clientLength);
                        std::string userCode(buf, 0, codeBytes);

                        std::string fullCode = cppProgram;
                        size_t pos = cppProgram.find("// Your code here");
                        if (pos != std::string::npos) {
                            fullCode.replace(pos, 18, userCode);
                        }

                        createFile("server_executable.cpp", fullCode);

                        std::string output = compileAndExecuteCode();

                        sendOutputToClient(output, udpSocket, client);
                    }
                    else if (adminRequest == "QUIT") {
                        sendto(udpSocket, "ADMIN_QUIT", sizeof("ADMIN_QUIT"), 0, (sockaddr*)&client, sizeof(client));
                        break;
                    }
                    else {
                        sendto(udpSocket, "Invalid command.", sizeof("Invalid command."), 0, (sockaddr*)&client, sizeof(client));
                    }

                    // Add a delay to allow the console to display the output
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }
            }
            else {
                sendto(udpSocket, "ADMIN_ACCESS_DENIED", sizeof("ADMIN_ACCESS_DENIED"), 0, (sockaddr*)&client, sizeof(client));
            }
        }
        else if (std::string(buf) == "GUEST") {
            while (true) {
                ZeroMemory(buf, sizeof(buf));
                int guestRequestBytes = recvfrom(udpSocket, buf, sizeof(buf), 0, (sockaddr*)&client, &clientLength);
                std::string guestRequest(buf, 0, guestRequestBytes);
                if (guestRequest == "READ_FILE") {
                    sendto(udpSocket, "ENTER_FILE_NAME", sizeof("ENTER_FILE_NAME"), 0, (sockaddr*)&client, sizeof(client));

                    ZeroMemory(buf, sizeof(buf));
                    int fileNameBytes = recvfrom(udpSocket, buf, sizeof(buf), 0, (sockaddr*)&client, &clientLength);
                    std::string fileName(buf, 0, fileNameBytes);

                    readFile(fileName, udpSocket, client);
                }
                else if (guestRequest == "QUIT") {
                    sendto(udpSocket, "GUEST_QUIT", sizeof("GUEST_QUIT"), 0, (sockaddr*)&client, sizeof(client));
                    break;
                }
                else {
                    sendto(udpSocket, "Invalid command.", sizeof("Invalid command."), 0, (sockaddr*)&client, sizeof(client));
                }

                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
        else {
            sendto(udpSocket, "Unknown command.", sizeof("Unknown command."), 0, (sockaddr*)&client, sizeof(client));
        }
    }

    closesocket(udpSocket);
    WSACleanup();

    return 0;
}