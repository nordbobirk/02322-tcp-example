#include <stdio.h>
#include <string.h>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    typedef SOCKET socket_t;
    #define CLOSESOCKET closesocket
    #define INIT_NETWORK() \
        WSADATA wsa_data; \
        if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) { \
            printf("WSAStartup failed\n"); \
            return 0; \
        }
    #define CLEANUP_NETWORK() WSACleanup()
    #define INVALID_SOCKET INVALID_SOCKET
    #define SOCKET_ERROR SOCKET_ERROR
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    typedef int socket_t;
    #define CLOSESOCKET close
    #define INIT_NETWORK()
    #define CLEANUP_NETWORK()
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR   -1
#endif

#define PORT 12345
#define BUFFER_SIZE 2048

typedef struct {
    socket_t server_socket;
    socket_t client_socket;
    int is_connected;
} TcpServer;

int tcp_server_init(TcpServer *server);

int tcp_server_wait_for_client(TcpServer *server);

int tcp_server_receive(TcpServer *server, char *buffer, const int buffer_size);

int tcp_server_send(const TcpServer *server, const char *message);

void tcp_server_close(TcpServer *server);

int main(void) {
    TcpServer server;

    printf("Initializing server...\n");
    if (!tcp_server_init(&server)) {
        printf("Error initializing server!\n");
        return 1;
    }

    printf("Awaiting client connection...\n");
    if (!tcp_server_wait_for_client(&server)) {
        printf("Error establishing connection with client!\n");
        tcp_server_close(&server);
        return 1;
    }

    printf("Client connected!\n");

    while (1) {
        char buffer[BUFFER_SIZE];
        int bytes_received = tcp_server_receive(&server, buffer, BUFFER_SIZE);

        if (((&server)->is_connected) == 0) {
            // connection lost, stop running
            break;
        }

        buffer[bytes_received] = '\0'; // add null terminator
        printf("Received from client: %s\n", buffer);

        tcp_server_send(&server, "This is a response!\n");
    }

    tcp_server_close(&server);
    return 0;
}

int tcp_server_init(TcpServer *server) {
    INIT_NETWORK();

    struct sockaddr_in addr;
    server->server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server->server_socket == INVALID_SOCKET) {
        printf("Socket creation failed\n");
        CLEANUP_NETWORK();
        return 0;
    }

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    if (bind(server->server_socket, (struct sockaddr *) &addr, sizeof(addr)) == SOCKET_ERROR) {
        printf("Bind failed\n");
        CLOSESOCKET(server->server_socket);
        CLEANUP_NETWORK();
        return 0;
    }

    if (listen(server->server_socket, SOMAXCONN) == SOCKET_ERROR) {
        printf("Listen failed\n");
        CLOSESOCKET(server->server_socket);
        CLEANUP_NETWORK();
        return 0;
    }

    server->is_connected = 0;
    return 1;
}

int tcp_server_wait_for_client(TcpServer *server) {
    server->client_socket = accept(server->server_socket, NULL, NULL);
    if (server->client_socket == INVALID_SOCKET) {
        printf("Accept failed\n");
        return 0;
    }
    server->is_connected = 1;
    printf("Client connection established\n");
    return 1;
}

int tcp_server_receive(TcpServer *server, char *buffer, const int buffer_size) {
    if (!server->is_connected) return -1;

    int bytes_received = recv(server->client_socket, buffer, buffer_size - 1, 0);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';
    } else if (bytes_received == 0) {
        printf("Client disconnected.\n");
        server->is_connected = 0;
    } else {
        printf("Receive failed.\n");
        server->is_connected = 0;
    }
    return bytes_received;
}

int tcp_server_send(const TcpServer *server, const char *message) {
    if (!server->is_connected) return 0;
    return send(server->client_socket, message, (int) strlen(message), 0);
}

void tcp_server_close(TcpServer *server) {
    printf("Closing TCP server!\n");
    if (server->is_connected) {
        CLOSESOCKET(server->client_socket);
        server->is_connected = 0;
    }
    CLOSESOCKET(server->server_socket);
    CLEANUP_NETWORK();
}
