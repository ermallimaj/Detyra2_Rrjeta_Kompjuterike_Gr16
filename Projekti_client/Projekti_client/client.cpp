#include <iostream>
#include <winsock2.h>
#include <fstream>
#include <Ws2tcpip.h>

using namespace std;

#define SERVER "127.0.0.1"
#define BUFLEN 512
#define PORT 8888

int main() {
    system("title UDP C++ Client Gr16");
    WSADATA ws;
    printf("Initialising Winsock...");
    if (WSAStartup(MAKEWORD(2, 2), &ws) != 0) {
        printf("Failed. Error Code: %d", WSAGetLastError());
        return 1;
    }
    printf("Initialised.\n");

    sockaddr_in server;
    int client_socket;
    if ((client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR) {
        printf("socket() failed with error code: %d", WSAGetLastError());
        return 2;
    }

    memset((char*)&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.S_un.S_addr = inet_addr(SERVER);

}