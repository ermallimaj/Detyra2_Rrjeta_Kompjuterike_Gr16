#include <iostream>
#include <fstream>
#include <winsock2.h>
using namespace std;

#pragma comment(lib,"ws2_32.lib")
#pragma warning(disable:4996)
#define SERVER "127.0.0.1"
#define BUFLEN 512
#define PORT 8888

int main()
{
    system("title UDP Server");

    sockaddr_in server, client;

    WSADATA wsa;
    printf("Initialising Winsock...");
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        printf("Failed. Error Code: %d", WSAGetLastError());
        exit(0);
    }
    printf("Initialised.\n");

    SOCKET server_socket;
    if ((server_socket = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
    {
        printf("Could not create socket: %d", WSAGetLastError());
        exit(EXIT_FAILURE);
    }
    printf("Socket created.\n");

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    if (bind(server_socket, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
    {
        printf("Bind failed with error code: %d", WSAGetLastError());
        exit(EXIT_FAILURE);
    }
    puts("Bind done.");

    while (true)
    {
        printf("Waiting for data... \n");
        fflush(stdout);
        char message[BUFLEN] = {};
        int message_len;
        int slen = sizeof(sockaddr_in);
        if ((message_len = recvfrom(server_socket, message, BUFLEN, 0, (sockaddr*)&client, &slen)) == SOCKET_ERROR)
        {
            printf("recvfrom() failed with error code: %d", WSAGetLastError());
            exit(EXIT_FAILURE);
        }

        printf("Received packet from %s:%d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
        printf("Data: %s\n", message);

        cin.getline(message, BUFLEN);
    }

    closesocket(server_socket);
    WSACleanup();
}
