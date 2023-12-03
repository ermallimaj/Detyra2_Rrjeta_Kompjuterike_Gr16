# Computer Networks Project- Programming with sockets

## UDP server implemented in C++

This repository contains the code for a simple UDP server implemented in C++ for a computer networks project. The server communicates with clients over UDP and supports various operations, including a math game, file reading, file appending, file writing, and code execution.

### Project description:
Server

Define variables containing the port number (the port number should be arbitrary) and the IP address.

Be able to listen for incoming connections from all members of the group.

Accept requests from devices that send requests (each group member must execute at least one request on the server).

Read messages sent by clients.

Provide full access to at least one client to access folders/content/files on the server.

Client

Establish a socket connection with the server.

One of the devices (clients) should have the privileges of write(), read(), execute().

Other clients should only have read() permission.

Establish a connection with the server by specifying the correct port and IP address.

Define server socket correctly and ensure that the connection does not fail.

Read responses returned by the server.

Send messages to the server in text form.

Have full access to folders/content on the server.

### Features:
#### Math Game: The server picks two random numbers (1-10) and a random operator (+, -, *, /, %).
Clients receive a math problem like "3 + 7" and try to solve it.
The server checks the client's answer and provides feedback.
Clients have a few guesses before the correct solution is revealed.

#### File Operations:

1. Read File: Clients can request to read the content of a specified file.
2. Append to File: Clients with appropriate privileges can append content to a file.
3. Write to File: Clients with appropriate privileges can write new content to a file.
4. Code Execution: Clients with the necessary privileges can send C++ code snippets to the server for compilation and execution. The server sends back the result of the code execution.

### Contributors:
1. Ermal Limaj
2. Erza Berbatovci
3. Erza Merovci
4. Ferdeze Nurkasa
   
Client-server communication and features shown with photos:
<img width="525" alt="wre" src="https://github.com/ermallimaj/Detyra2_Rrjeta_Kompjuterike_Gr16/assets/116812415/48c736ed-cf6b-405d-87ff-cc2293c42e9d">

<img width="525" alt="exe" src="https://github.com/ermallimaj/Detyra2_Rrjeta_Kompjuterike_Gr16/assets/116812415/aa897b4a-c56f-477c-96f3-4d6cc0720947">

<img width="531" alt="game" src="https://github.com/ermallimaj/Detyra2_Rrjeta_Kompjuterike_Gr16/assets/116812415/11427dfb-d927-46ac-bcbf-1f85ddca8067">



