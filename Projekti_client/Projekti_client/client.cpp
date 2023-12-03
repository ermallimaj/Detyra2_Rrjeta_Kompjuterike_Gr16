#include <iostream>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable : 4996)

#define BUFLEN 512

int main()
{
	WSADATA wsa;
	printf("Initialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code: %d", WSAGetLastError());
		return 1;
	}
	printf("Initialised.\n");

	SOCKET client_socket;
	if ((client_socket = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
	{
		printf("Could not create socket: %d", WSAGetLastError());
		return 1;
	}
	printf("Socket created.\n");

	sockaddr_in server_addr;

	char serverIP[15];
	printf("Enter server IP address: ");
	fgets(serverIP, sizeof(serverIP), stdin);
	serverIP[strcspn(serverIP, "\n")] = '\0';

	int serverPort;
	printf("Enter server port: ");
	scanf("%d", &serverPort);

	getchar();

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(serverPort);
	server_addr.sin_addr.s_addr = inet_addr(serverIP);

	while (true)
	{
		char command[BUFLEN];
		printf("Enter message: ");
		fgets(command, BUFLEN, stdin);
		command[strcspn(command, "\n")] = '\0';

		if (sendto(client_socket, command, strlen(command), 0, (sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR)
		{
			printf("sendto() failed with error code: %d", WSAGetLastError());
			return 1;
		}

		char serverResponse[BUFLEN];
		int serverResponseLength = recvfrom(client_socket, serverResponse, BUFLEN, 0, NULL, NULL);
		if (serverResponseLength == SOCKET_ERROR)
		{
			printf("recvfrom() failed with error code: %d", WSAGetLastError());
			return 1;
		}

		serverResponse[serverResponseLength] = '\0';
		printf("Server says: %s\n", serverResponse);
	}

	closesocket(client_socket);
	WSACleanup();
	return 0;
}