# TCP example
This repository is an example of how you can set up a TCP connection between a C server and a Python client. It is intended for use in project 2 in the course 02322 Machine oriented programming at DTU.
During this project, students may choose to implement their GUI in C using SDL or with another language communicating with the C backend over TCP. If you choose to use another language for your frontend, this example shows how you can set up communication between the server and client.

Students can choose to clone this repository and build their backend with it as a template, but you can also build your backend and then add the TCP setup once you're ready to add a frontend by copying from this example repository.

Students are highly encouraged to read the explanation of the code below and work to understand how it works.

### Running with CLion
1. clone the project using `git clone git@github.com:nordbobirk/02322-tcp-example.git` with SSH or `git clone https://github.com/nordbobirk/02322-tcp-example.git` with HTTPS.
2. open the project in CLion and if prompted click "Trust project"
3. add a run configuration by clicking "Add Configuration" > "Edit Configurations" > "Add new" > "C/C++ file" in the top right
4. select `server/server.c` as the source file and click "OK"
5. click the "Run" icon (green right-facing arrow) in the top right. The server should now start and the CLion terminal should open in the bottom of your screen showing "Initializing server... Awaiting client connection..."
6. use another terminal session or your favorite Python IDE to open and run the Python client. The simplest option is to run it from a terminal with `python3 python-client/client.py`. When starting the Python client, it should automatically connect to the server and prompt "Write a message for the server: ". You can now send messages to the server

### OS
This has been tested on Linux, macOS and Windows using CLion as described above.

### Questions and feedback
If you have any questions about the code or find any issues with it, please contact me via DTU email (s245427) or speak to me or one of the other TA's during the lab sessions.

### Explanation
This example consists of two parts: a server implemented in C which is analogous to your C backend, and a client implemented in Python which is analogous to your GUI implemented in another language than C (you can choose any language, Python is just used in this example for simplicity).

#### The server
The C server is responsible for setting up a TCP connection and handling communication with a single client at a time.

The server starts running in `main()`, where a TcpServer struct is created to hold the two sockets: one for listening (`server_socket`) and one for communicating with a connected client (`client_socket`).

##### Initialization
``` c
if (!tcp_server_init(&server)) {
    printf("Error initializing server!\n");
    return 1;
}
```
During initialization, the server:

Calls `INIT_NETWORK()` — this only matters on Windows, where Winsock must be started manually using `WSAStartup()`.

Creates a socket with
```c
server->server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
```
Here `AF_INET` means IPv4, `SOCK_STREAM` specifies a TCP connection, and `IPPROTO_TCP` confirms we’re using TCP.

Binds the socket to an address and port using `bind()`. The example uses
```c
addr.sin_addr.s_addr = INADDR_ANY;
addr.sin_port = htons(PORT);
```
`INADDR_ANY` means the server accepts messages on any available network interface (localhost, Wi-Fi, etc.), and `htons()` converts the port number (12345) to network byte order, ensuring compatibility across systems.

Starts listening for clients with `listen()`. This tells the OS to queue incoming connection requests up to a system-defined limit (here `SOMAXCONN`).

At this point, the server is ready to accept a connection.

##### Waiting for a client
```c
tcp_server_wait_for_client(&server);
```
This function blocks (pauses execution) until a client attempts to connect. It calls `accept()` on the listening socket. When a connection is accepted:

A new socket (`client_socket`) is created specifically for communicating with that client.

The original `server_socket` continues to exist — it could accept more connections in a more advanced version, but this example keeps it simple and handles one client only.

The `is_connected` flag is set to 1.

##### Receiving and sending data
After a client connects, the server enters a loop like this:

```c
int bytes_received = tcp_server_receive(&server, buffer, BUFFER_SIZE);
...
tcp_server_send(&server, "This is a response!\n");
```
`tcp_server_receive()` calls the system function `recv()`, which reads data sent by the client and stores it in buffer. It handles three cases:

1. A positive number: received that many bytes.
2. 0: the client has disconnected.
3. A negative number: there was an error (e.g., network failure).

If data is received, the server prints it to the terminal.

`tcp_server_send()` then uses `send()` to send back a response over the same `client_socket`. This simple example always sends the same response, but you could modify it to process the message and send different data instead.

##### Closing the connection
When the client disconnects or the server is stopped, the `tcp_server_close()` function runs:

```c
CLOSESOCKET(server->client_socket);
CLOSESOCKET(server->server_socket);
CLEANUP_NETWORK();
```
On Windows, this also calls `WSACleanup()` to properly close the Winsock library.
After this, all network resources are released.

##### Summary of the data flow
The server creates and binds a listening socket (`server_socket`).

It waits for a client connection (`accept()`).

Once connected, it communicates using client_socket.

Messages are exchanged using `recv()` and `send()`.

When done, both sockets are closed.

In short, the TCP server manages a reliable, ordered byte stream between itself and a connected client — wrapping up the necessary system calls behind the helper functions defined in this project.

#### The client
The first part of the client is
```
serverName = "localhost"
serverPort = 12345
clientSocket = socket(AF_INET, SOCK_STREAM)
clientSocket.connect((serverName,serverPort))
```
which just creates a TCP socket on localhost on port `12345` and connects to it.

Then, it takes input from the standard input in a loop and sends it into the socket, receiving a response and printing it to the standard output (if the input is not `disconnect`):

```
running = True

while running:
    msg = input("Write a message for the server: ")
    if msg.lower() == "disconnect":
        running = False
        print("Disconnecting from server!")
    else:
        clientSocket.send(msg.encode())
        response = clientSocket.recv(1024)
        print("Response from the server: ", response.decode()) 

clientSocket.close()
```
