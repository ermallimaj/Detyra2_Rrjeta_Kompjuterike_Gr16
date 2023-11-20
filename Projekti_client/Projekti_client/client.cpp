#include <iostream>
#include <winsock2.h>
#include <fstream>
#include <Ws2tcpip.h>

using namespace std;

#define SERVER "127.0.0.1"
#define BUFLEN 512
#define PORT 8888

int main() {
    system("title UDP Client");
    WSADATA ws;
    cout << "Initialising Winsock..." << endl;
    if (WSAStartup(MAKEWORD(2, 2), &ws) != 0) {
        cout << "Failed.Error Code:" << WSAGetLastError() << endl;
        return 1;
    }
    cout<<"Initialised"<<endl;

    sockaddr_in server;
    int client_socket;
    if ((client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR) {
        cout << "socket() failed with error code:" << WSAFetLastError() << endl;
           return 2;
    }

    memset((char)&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.S_un.S_addr = inet_addr(SERVER);

    while (true)
    {
        char message[BUFLEN];
        cout << "Select how do you want to continue \n As guest enter guest \n As admin write code \n ->";
        cin.getline(message, BUFLEN);

        if (sendto(client_socket, message, strlen(message), 0, (sockaddr)&server, sizeof(sockaddr_in)) == SOCKET_ERROR)
        {
            cout << "sendto() failed with error code:" << WSAGetLastError() << endl;
            return 3;
        }
    }

    closesocket(client_socket);
    WSACleanup();

}