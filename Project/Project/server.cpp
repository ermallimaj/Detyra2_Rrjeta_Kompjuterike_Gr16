#include <iostream>
#include <fstream>
#include <winsock2.h>
#include <string>
#include <unordered_set>
#include <unordered_map>

#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable : 4996)

#define BUFLEN 512

using namespace std;
int generatePort(int min, int max) {
	srand(static_cast<unsigned int>(time(0)));
	return rand() % (max - min + 1) + min;
}

string compileAndExecuteCode(const string& userCode) {
#ifdef _WIN32
	string compileCommand = "g++ -o server_executable server_executable.cpp";
	string executeCommand = ".\\server_executable";
#else
	string compileCommand = "g++ -o server_executable server_executable.cpp";
	string executeCommand = "./server_executable";
#endif

	ofstream codeFile("server_executable.cpp");
	codeFile << "#include <iostream>" << endl;
	codeFile << "int main() {" << endl;
	codeFile << " " << userCode << endl;
	codeFile << " return 0;" << endl;
	codeFile << "}" << endl;
	codeFile.close();

	int compileResult = system(compileCommand.c_str());

	if (compileResult != 0) {
		cerr << "Error compiling code." << endl;
		return "Error compiling code.";
	}

	FILE* pipe = _popen(executeCommand.c_str(), "r");

	if (pipe) {
		char output[4096];
		string result;

		while (fgets(output, sizeof(output), pipe) != nullptr) {
			result += output;
		}

		_pclose(pipe);
		return result;
	}
	else {
		cerr << "Error executing code." << endl;
		return "Error executing code.";
	}
}

void writeFile(const string& filename, const string& content)
{
	ofstream file(filename, ios::out | ios::binary);
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

void appendFile(const string& filename, const string& content)
{
	ofstream file(filename, ios::out | ios::app | ios::binary);
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

int main()
{

	system("title UDP Server");

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
	}
	printf("Socket created.\n");

	sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr("192.168.0.105");
	server.sin_port = generatePort(1000, 9999);

	if (bind(server_socket, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
	{
		printf("Bind failed with error code: %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	puts("Bind done.");
	printf("Server is running on IP %s and port %d\n", inet_ntoa(server.sin_addr), ntohs(server.sin_port));

	unordered_set<string> firstClient;
	unordered_map<string, sockaddr_in> clientAddresses;

	while (true)
	{
		printf("Waiting for data...");
		fflush(stdout);
		char message[BUFLEN] = {};

		sockaddr_in clientAddr;
		int slen = sizeof(clientAddr);
		int message_len;
		if ((message_len = recvfrom(server_socket, message, BUFLEN, 0, (sockaddr*)&clientAddr, &slen)) == SOCKET_ERROR)
		{
			printf("recvfrom() failed with error code: %d", WSAGetLastError());
			exit(0);
		}

		printf("\nReceived packet from %s:%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

		string clientKey = string(inet_ntoa(clientAddr.sin_addr)) + ":" + to_string(ntohs(clientAddr.sin_port));

		if (strcmp(message, "play game") == 0) {
			if (firstClient.empty() || firstClient.count(clientKey) > 0) {

				int num1 = rand() % 10 + 1;
				int num2 = rand() % 10 + 1;
				char operationList[] = { '+', '-', '*', '/', '%' };
				int numberOfOperations = sizeof(operationList) / sizeof(operationList[0]);
				int randomOperationIndex = rand() % numberOfOperations;
				char operation = operationList[randomOperationIndex];

				string mathProblem = to_string(num1) + " " + operation + " " + to_string(num2);

				string response = "Math problem generated: " + mathProblem + ". Try to solve! You have 5 guesses";
				if (sendto(server_socket, response.c_str(), response.size(), 0, (sockaddr*)&clientAddr, sizeof(sockaddr_in)) == SOCKET_ERROR) {
					printf("sendto() failed with error code: %d", WSAGetLastError());
					return 3;
				}

				int i = 5;
				while (i > 0) {
					i--;

					char solutionMessage[BUFLEN];
					int solutionMessageLength = recvfrom(server_socket, solutionMessage, BUFLEN, 0, (sockaddr*)&clientAddr, &slen);
					if (solutionMessageLength == SOCKET_ERROR) {
						printf("recvfrom() failed with error code: %d", WSAGetLastError());
						exit(0);
					}

					solutionMessage[solutionMessageLength] = '\0';

					double clientSolution = atof(solutionMessage);
					double correctSolution;

					switch (operation) {
					case '+':
						correctSolution = num1 + num2;
						break;
					case '-':
						correctSolution = num1 - num2;
						break;
					case '*':
						correctSolution = num1 * num2;
						break;
					case '/':
						correctSolution = static_cast<double>(num1) / num2;
						break;
					case '%':
						correctSolution = num1 % num2;
						break;
					default:
						correctSolution = 0;
						break;
					}

					if (clientSolution == correctSolution) {
						string successMessage = "Congratulations! You solved the math problem!";
						if (sendto(server_socket, successMessage.c_str(), successMessage.size(), 0, (sockaddr*)&clientAddr, sizeof(sockaddr_in)) == SOCKET_ERROR) {
							printf("sendto() failed with error code: %d", WSAGetLastError());
							return 3;
						}

						printf("Client solved the math problem!\n");
						break;
					}
					else if (i != 0) {
						string feedbackMessage = "Incorrect solution. Try again! Remaining guesses: " + to_string(i);
						if (sendto(server_socket, feedbackMessage.c_str(), feedbackMessage.size(), 0, (sockaddr*)&clientAddr, sizeof(sockaddr_in)) == SOCKET_ERROR) {
							printf("sendto() failed with error code: %d", WSAGetLastError());
							return 3;
						}
					}

					if (i == 0) {
						string errorMessage = "You lost the game! The correct solution was: " + to_string(correctSolution);
						if (sendto(server_socket, errorMessage.c_str(), errorMessage.size(), 0, (sockaddr*)&clientAddr, sizeof(sockaddr_in)) == SOCKET_ERROR) {
							printf("sendto() failed with error code: %d", WSAGetLastError());
							return 3;
						}

						printf("Client didn't solve the math problem\n");
					}
				}
				firstClient.insert(clientKey);

			}
			else {
				string errorMsg = "Math Game not allowed for guests.";
				if (sendto(server_socket, errorMsg.c_str(), errorMsg.size(), 0, (sockaddr*)&clientAddr, sizeof(sockaddr_in)) == SOCKET_ERROR) {
					printf("sendto() failed with error code: %d", WSAGetLastError());
					return 3;
				}
			}
		}
		else if (strncmp(message, "read f: ", 8) == 0)
		{
			string filename = message + 8;

			ifstream file(filename, ios::binary);
			if (!file.is_open())
			{
				cout << "Error opening file: " << filename << endl;

				string errorMsg = "Error opening file: " + filename;
				if (sendto(server_socket, errorMsg.c_str(), errorMsg.size(), 0, (sockaddr*)&clientAddr, sizeof(sockaddr_in)) == SOCKET_ERROR)
				{
					printf("sendto() failed with error code: %d", WSAGetLastError());
					return 3;
				}

				continue;
			}

			string fileContent((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
			file.close();

			if (sendto(server_socket, fileContent.c_str(), fileContent.size(), 0, (sockaddr*)&clientAddr, sizeof(sockaddr_in)) == SOCKET_ERROR)
			{
				printf("sendto() failed with error code: %d", WSAGetLastError());
				return 3;
			}
		}
		else if (strncmp(message, "apnd f: ", 8) == 0)
		{
			if (firstClient.empty() || firstClient.count(clientKey) > 0)
			{
				string fileAppendCommand = message + 8;
				size_t pos = fileAppendCommand.find(' ');
				if (pos != string::npos)
				{
					string filename = fileAppendCommand.substr(0, pos);
					string content = fileAppendCommand.substr(pos + 1);

					appendFile(filename, content);

					string successMsg = "Content appended to the file: " + filename + " successfully";
					if (sendto(server_socket, successMsg.c_str(), successMsg.size(), 0, (sockaddr*)&clientAddr, sizeof(sockaddr_in)) == SOCKET_ERROR)
					{
						printf("sendto() failed with error code: %d", WSAGetLastError());
						return 3;
					}

					firstClient.insert(clientKey);
				}
			}
			else
			{
				string errorMsg = "File appending command is not allowed for guests.";
				if (sendto(server_socket, errorMsg.c_str(), errorMsg.size(), 0, (sockaddr*)&clientAddr, sizeof(sockaddr_in)) == SOCKET_ERROR)
				{
					printf("sendto() failed with error code: %d", WSAGetLastError());
					return 3;
				}
			}
		}
		else if (strncmp(message, "wrte f: ", 8) == 0)
		{
			if (firstClient.empty() || firstClient.count(clientKey) > 0)
			{
				string fileWriteCommand = message + 8;
				size_t pos = fileWriteCommand.find(' ');
				if (pos != string::npos)
				{
					string filename = fileWriteCommand.substr(0, pos);
					string content = fileWriteCommand.substr(pos + 1);

					writeFile(filename, content);

					string successMsg = "Content written to the file: " + filename + " successfully";
					if (sendto(server_socket, successMsg.c_str(), successMsg.size(), 0, (sockaddr*)&clientAddr, sizeof(sockaddr_in)) == SOCKET_ERROR)
					{
						printf("sendto() failed with error code: %d", WSAGetLastError());
						return 3;
					}

					firstClient.insert(clientKey);
				}
			}
			else
			{
				string errorMsg = "Writing in a file is not allowed for guests.";
				if (sendto(server_socket, errorMsg.c_str(), errorMsg.size(), 0, (sockaddr*)&clientAddr, sizeof(sockaddr_in)) == SOCKET_ERROR)
				{
					printf("sendto() failed with error code: %d", WSAGetLastError());
					return 3;
				}
			}
		}
		else if (strncmp(message, "execute code: ", 14) == 0) {
			if (firstClient.empty() || firstClient.count(clientKey) > 0) {
				string codeToExecute = message + 14;

				writeFile("server_executable.cpp", codeToExecute);

				string executionResult = compileAndExecuteCode(codeToExecute);
				if (sendto(server_socket, executionResult.c_str(), executionResult.size(), 0, (sockaddr*)&clientAddr, sizeof(sockaddr_in)) == SOCKET_ERROR) {
					printf("sendto() failed with error code: %d", WSAGetLastError());
					return 3;
				}

				remove("server_executable");

				firstClient.insert(clientKey);
			}
			else {
				string errorMsg = "Code execution not allowed for guests.";
				if (sendto(server_socket, errorMsg.c_str(), errorMsg.size(), 0, (sockaddr*)&clientAddr, sizeof(sockaddr_in)) == SOCKET_ERROR) {
					printf("sendto() failed with error code: %d", WSAGetLastError());
					return 3;
				}
			}
		}
		else
		{
			printf("Client says: %s\n", message);

			char serverMessage[BUFLEN];
			printf("Enter message to client: ");
			cin.getline(serverMessage, BUFLEN);

			if (sendto(server_socket, serverMessage, strlen(serverMessage), 0, (sockaddr*)&clientAddr, sizeof(sockaddr_in)) == SOCKET_ERROR)
			{
				printf("sendto() failed with error code: %d", WSAGetLastError());
				return 3;
			}
		}
	}

	closesocket(server_socket);
	WSACleanup();
	return 0;
}