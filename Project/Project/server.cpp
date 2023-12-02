#include <iostream>
#include <winsock2.h>
#include <string>
#include <fstream>
#include <unordered_set>
#include <unordered_map>

#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable :4996)

#define BUFLEN 512
#define PORT 8888

using namespace std;

// Metoda per te shkruar ne file
void writeFile(const string& filename,const string& content){
    ofstream file(filename,ios::out | ios::binary);
    if(file.is_open()){
        file <<content;
        file.close();
    }else{
        cout<<"Error opening file: " <<filename <<endl;
    }
}

// Metoda per append ne file

void appendFile(const string& filename,const string& content){
    ofstream file(filename,ios::out |ios::app |ios::binary);
       if (file.is_open())
    {
        file << content;
        file.close();
    }
    else
    {
        cout << "Error opening file: " << filename << endl;
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
