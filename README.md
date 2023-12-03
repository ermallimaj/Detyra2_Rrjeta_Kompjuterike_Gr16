# Computer Networks Project- Programming with sockets

## UDP server implemented in C++ 

### Project description:
**Server**

Define variables containing the port number (the port number should be arbitrary) and the IP address.

Be able to listen for incoming connections from all members of the group.

Accept requests from devices that send requests (each group member must execute at least one request on the server).

Read messages sent by clients.

Provide full access to at least one client to access folders/content/files on the server.

**Client**

Establish a socket connection with the server.

One of the devices (clients) should have the privileges of write(), read(), execute().

Other clients should only have read() permission.

Establish a connection with the server by specifying the correct port and IP address.

Define server socket correctly and ensure that the connection does not fail.

Read responses returned by the server.

Send messages to the server in text form.

Have full access to folders/content on the server.

## Overview

This repository contains the code for a simple UDP server implemented in C++ for a computer networks project. The server communicates with clients over UDP and supports various operations, including a math game, file reading, file appending, file writing, and code execution.


## Project Structure

- **server.cpp:** Contains the main server logic.
- **server_executable.cpp:** Temporary file for user code execution.
- **client.cpp:** Client-side code to interact with the server.

## Libraries Used

- **Winsock2.h:** Socket programming library for Windows.
- **iostream, fstream, string:** Standard C++ libraries for I/O and string handling.
- **unordered_set, unordered_map:** C++ Standard Template Library (STL) containers for managing client states.

## Compilation and Execution

1. Compile the code using an appropriate compiler (g++ for Windows).
2. Run the compiled code executable on a Windows machine.
3. Clients can connect to the server using the provided IP address and port.

## Features

1. **Math Game:** Clients can initiate a math game where the server generates math problems for them to solve.

2. **File Operations:**
   - Read: Clients can request to read the content of a file on the server.
   - Append: Clients with appropriate permissions can append content to a file on the server.
   - Write: Clients with appropriate permissions can write content to a file on the server.

3. **Code Execution:**
   - Clients can send C++ code snippets to the server.
   - The server compiles and executes the code, providing the results back to the clients.

## Notes

- Ensure that the server is reachable from client machines over the network.
- The project is designed to run on a Windows machine due to the use of Winsock.

### Contributors:
Ermal Limaj
Erza Berbatovci
Erza Merovci
Ferdeze Nurkasa
   
Client-server communication and features shown with photos:
<img width="525" alt="wre" src="https://github.com/ermallimaj/Detyra2_Rrjeta_Kompjuterike_Gr16/assets/116812415/48c736ed-cf6b-405d-87ff-cc2293c42e9d">

<img width="525" alt="exe" src="https://github.com/ermallimaj/Detyra2_Rrjeta_Kompjuterike_Gr16/assets/116812415/aa897b4a-c56f-477c-96f3-4d6cc0720947">

<img width="531" alt="game" src="https://github.com/ermallimaj/Detyra2_Rrjeta_Kompjuterike_Gr16/assets/116812415/11427dfb-d927-46ac-bcbf-1f85ddca8067">



