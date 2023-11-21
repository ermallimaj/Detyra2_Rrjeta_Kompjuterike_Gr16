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
    system("title UDP C++ Server Group Number 16");

    sockaddr_in server, client;

    WSADATA wsa;
    printf("Initialising Winsock...");
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        printf("Failed. Error Code: %d", WSAGetLastError());
        exit(EXIT_FAILURE);
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

        if (strcmp(message, "12345") == 0) {
            sendto(server_socket, "---Hello client you have all Permission to continue---\n You can read files, write files and execute files \n Press 1) to read files \n Press 2) to write files \n Press 3) to delete files", strlen("---Hello client you have all Permission to continue---\n You can read files, write files and execute files \n Press 1) to read files \n Press 2) to write files \n Press 3) to delete files"), 0, (sockaddr*)&client, sizeof(sockaddr_in));
            printf("----This client has all Permissions---- \n");
            printf("Received packet from %s:%d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
            printf("Data: %s\n", message);

            while (true)
            {
                printf("Waiting for command... \n");
                fflush(stdout);
                char command[BUFLEN] = {};
                int command_len;
                int slen_command = sizeof(sockaddr_in);
                if ((command_len = recvfrom(server_socket, command, BUFLEN, 0, (sockaddr*)&client, &slen_command)) == SOCKET_ERROR)
                {
                    printf("recvfrom() failed with error code: %d", WSAGetLastError());
                    exit(EXIT_FAILURE);
                }

                if (strcmp(command, "1") == 0) {
                    cout << "Client chose to read files \n";
                    sendto(server_socket, " \n Press 1) to read test1.txt \n Press 2) to read test2.txt", strlen(" \n Press 1) to read test1.txt \n Press 2) to read test2.txt"), 0, (sockaddr*)&client, sizeof(sockaddr_in));

                    char choice[BUFLEN] = {};
                    int choice_len;
                    int slen_choice = sizeof(sockaddr_in);
                    if ((choice_len = recvfrom(server_socket, choice, BUFLEN, 0, (sockaddr*)&client, &slen_choice)) == SOCKET_ERROR)
                    {
                        printf("recvfrom() failed with error code: %d", WSAGetLastError());
                        exit(EXIT_FAILURE);
                    }

                    if (strcmp(choice, "1") == 0) {
                        cout << "Client selected to read test1.txt \n";
                        ifstream input("test1.txt");
                        char data[BUFLEN];
                        while (!input.eof()) {
                            input.getline(data, BUFLEN);
                            sendto(server_socket, data, strlen(data), 0, (sockaddr*)&client, sizeof(sockaddr_in));
                        }
                    }
                    else if (strcmp(choice, "2") == 0) {
                        cout << "Client selected to read test2.txt \n";
                        ifstream input("test2.txt");
                        char data[BUFLEN];
                        while (!input.eof()) {
                            input.getline(data, BUFLEN);
                            sendto(server_socket, data, strlen(data), 0, (sockaddr*)&client, sizeof(sockaddr_in));
                        }
                    }
                    else {
                        sendto(server_socket, "You chose a wrong number \n", strlen("You chose a wrong number \n"), 0, (sockaddr*)&client, sizeof(sockaddr_in));
                        cout << "Client chose a wrong input  \n";
                    }
                }

                else if (strcmp(command, "2") == 0) {
                    cout << "Client selected to write files \n";
                    sendto(server_socket, " \n Press 1) to write to test1.txt \n Press 2) to write to test2.txt", strlen("  \n Press 1) to write to test1.txt \n Press 2) to write to test2.txt"), 0, (sockaddr*)&client, sizeof(sockaddr_in));

                    char choice[BUFLEN] = {};
                    int choice_len;
                    int slen_choice = sizeof(sockaddr_in);
                    if ((choice_len = recvfrom(server_socket, choice, BUFLEN, 0, (sockaddr*)&client, &slen_choice)) == SOCKET_ERROR)
                    {
                        printf("recvfrom() failed with error code: %d", WSAGetLastError());
                        exit(EXIT_FAILURE);
                    }

                    if (strcmp(choice, "1") == 0) {
                        cout << "Client selected to write to test1.txt \n";
                        ofstream myfile;
                        myfile.open("test1.txt");
                        sendto(server_socket, "Write ...", strlen("Write ..."), 0, (sockaddr*)&client, sizeof(sockaddr_in));

                        char content[BUFLEN] = {};
                        int content_len;
                        int slen_content = sizeof(sockaddr_in);
                        if ((content_len = recvfrom(server_socket, content, BUFLEN, 0, (sockaddr*)&client, &slen_content)) == SOCKET_ERROR)
                        {
                            printf("recvfrom() failed with error code: %d", WSAGetLastError());
                            exit(EXIT_FAILURE);
                        }

                        myfile << content;
                        myfile.close();
                        sendto(server_socket, "You have written to test1.txt successfully", strlen("You have written to test1.txt successfully"), 0, (sockaddr*)&client, sizeof(sockaddr_in));
                    }

                    else if (strcmp(choice, "2") == 0) {
                        cout << "Client selected to write to test2.txt \n";
                        ofstream myfile;
                        myfile.open("test2.txt");
                        sendto(server_socket, "Write ...", strlen("Write ..."), 0, (sockaddr*)&client, sizeof(sockaddr_in));

                        char content[BUFLEN] = {};
                        int content_len;
                        int slen_content = sizeof(sockaddr_in);
                        if ((content_len = recvfrom(server_socket, content, BUFLEN, 0, (sockaddr*)&client, &slen_content)) == SOCKET_ERROR)
                        {
                            printf("recvfrom() failed with error code: %d", WSAGetLastError());
                            exit(EXIT_FAILURE);
                        }

                        myfile << content;
                        myfile.close();
                        sendto(server_socket, "You have written to test2.txt successfully", strlen("You have written to test2.txt successfully"), 0, (sockaddr*)&client, sizeof(sockaddr_in));
                    }

                    else {
                        sendto(server_socket, "You chose a wrong number \n", strlen("You chose a wrong number \n"), 0, (sockaddr*)&client, sizeof(sockaddr_in));
                        cout << "Client chose a wrong input  \n";
                    }
                }

                else if (strcmp(command, "3") == 0) {
                    cout << "Client selected to delete files \n";
                    sendto(server_socket, " \n Press 1) to delete test1.txt \n Press 2) to delete test2.txt", strlen(" \n Press 1) to delete test1.txt \n Press 2) to delete test2.txt"), 0, (sockaddr*)&client, sizeof(sockaddr_in));

                    char choice[BUFLEN] = {};
                    int choice_len;
                    int slen_choice = sizeof(sockaddr_in);
                    if ((choice_len = recvfrom(server_socket, choice, BUFLEN, 0, (sockaddr*)&client, &slen_choice)) == SOCKET_ERROR)
                    {
                        printf("recvfrom() failed with error code: %d", WSAGetLastError());
                        exit(EXIT_FAILURE);
                    }

                    if (strcmp(choice, "1") == 0) {
                        cout << "Client selected to delete test1.txt \n";
                        int status;
                        status = remove("test1.txt");
                        sendto(server_socket, "test1.txt is deleted", strlen("test1.txt is deleted"), 0, (sockaddr*)&client, sizeof(sockaddr_in));
                    }

                    else if (strcmp(choice, "2") == 0) {
                        cout << "Client selected to delete test2.txt \n";
                        int status;
                        status = remove("test2.txt");
                        sendto(server_socket, "test2.txt is deleted", strlen("test2.txt is deleted"), 0, (sockaddr*)&client, sizeof(sockaddr_in));
                    }

                    else {
                        sendto(server_socket, "You chose a wrong number \n", strlen("You chose a wrong number \n"), 0, (sockaddr*)&client, sizeof(sockaddr_in));
                        cout << "Client chose a wrong input  \n";
                    }
                }

                else {
                    sendto(server_socket, "You chose a wrong number \n", strlen("You chose a wrong number \n"), 0, (sockaddr*)&client, sizeof(sockaddr_in));
                    cout << "Client chose a wrong input  \n";
                }

                cin.getline(message, BUFLEN);
            }
        }

        cin.getline(message, BUFLEN);
    }

    closesocket(server_socket);
    WSACleanup();
}
