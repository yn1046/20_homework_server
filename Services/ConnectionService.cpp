#include "ConnectionService.h"

void cc::ConnectionService::connect() {
    if (server_socket == -1) {
        perror("socket: ");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = INADDR_ANY;
    bzero(&server.sin_zero, 0);

    if ((bind(server_socket, (struct sockaddr *) &server, sizeof(struct sockaddr_in))) == -1) {
        perror("bind error: ");
        exit(-1);
    }

    if ((listen(server_socket, 8)) == -1) {
        perror("listen error: ");
        exit(-1);
    }

}

void cc::ConnectionService::shutdown() {
    close(server_socket);
}

int cc::ConnectionService::accept_client() {
    struct sockaddr_in client;
    int client_socket;
    unsigned int len = sizeof(sockaddr_in);

    if ((client_socket = accept(server_socket, (struct sockaddr *) &client, &len)) == -1) {
        perror("accept error: ");
        exit(-1);
    }

    return client_socket;
}

void cc::ConnectionService::close_client(int client_socket) {
    close(client_socket);
}

void cc::ConnectionService::send_message_string(int client_socket, const string &str) {
    const char *cstr = str.c_str();
    send(client_socket, cstr, MESSAGE_LEN, 0);
}

string cc::ConnectionService::receive_message_string(int client_socket) {
    char str[MESSAGE_LEN];
    recv(client_socket, str, MESSAGE_LEN, 0);
    return string() + str;
}
