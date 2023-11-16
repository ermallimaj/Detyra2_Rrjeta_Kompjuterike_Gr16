#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>

using namespace std;

int main() {

	int serverPort = 12345;
	const char* serverIP = "127.0.0.1";

	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	if (sockfd == -1) {
		cout << "Gabim ne krijimin e soketit" << endl;
		return 1;
	}
	//test

	return 0;
}
