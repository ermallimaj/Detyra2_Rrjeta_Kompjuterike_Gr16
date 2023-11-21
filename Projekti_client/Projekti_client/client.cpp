#include <iostream>
#include <winsock2.h>
using namespace std;

#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable : 4996)

#define SERVER "127.0.0.1"
#define BUFLEN 512
#define PORT 8888

int main()
{
    system("title UDP Client");

    WSADATA ws;
    if (WSAStartup(MAKEWORD(2, 2), &ws) != 0)
    {
        cout << "Failed. Error Code: " << WSAGetLastError() << endl;
        return 1;
    }

    sockaddr_in server;
    int client_socket;
    if ((client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
    {
        cout << "socket() failed with error code: " << WSAGetLastError() << endl;
        return 2;
    }

    memset((char*)&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.S_un.S_addr = inet_addr(SERVER);

    while (true)
    {
        char message[BUFLEN];
        cout << "Select how do you want to continue \n As guest enter guest \n As admin write code \n -> ";
        cin.getline(message, BUFLEN);

        if (sendto(client_socket, message, strlen(message), 0, (sockaddr*)&server, sizeof(sockaddr_in)) == SOCKET_ERROR)
        {
            cout << "sendto() failed with error code: " << WSAGetLastError() << endl;
            return 3;
        }

        if (strcmp(message, "guest") == 0)
        {
            char answer[BUFLEN] = {};
            int answer_len;
            int alen = sizeof(sockaddr_in);
            if (answer_len = recvfrom(client_socket, answer, BUFLEN, 0, (sockaddr*)&server, &alen) == SOCKET_ERROR)
            {
                cout << "recvfrom() failed with error code: " << WSAGetLastError() << endl;
                exit(0);
            }
            cout << "Server: " << answer << endl;

            char message1[BUFLEN];
            cout << "Press the option you want to chose: ";
            cin.getline(message1, BUFLEN);
            if (sendto(client_socket, message1, strlen(message1), 0, (sockaddr*)&server, sizeof(sockaddr_in)) == SOCKET_ERROR)
            {
                cout << "sendto() failed with error code: " << WSAGetLastError() << endl;
                return 3;
            }

            char answer1[BUFLEN] = {};
            int answer1_len;
            int alen1 = sizeof(sockaddr_in);
            if (answer1_len = recvfrom(client_socket, answer1, BUFLEN, 0, (sockaddr*)&server, &alen1) == SOCKET_ERROR)
            {
                cout << "recvfrom() failed with error code: " << WSAGetLastError() << endl;
                exit(0);
            }
            cout << "Server: " << answer1 << endl;
            exit(0);
        }
    }

    closesocket(client_socket);
    WSACleanup();
}