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

template<typename T>
void cc::ConnectionService::send_message(int client_socket, T value) {
    send(client_socket, &value, sizeof(value), 0);
}

template<>
void cc::ConnectionService::send_message<string>(int client_socket, const string str) {
    const char *cstr = str.c_str();
    send(client_socket, cstr, sizeof(cstr), 0);
}

template<typename T>
T cc::ConnectionService::receive_message(int client_socket) {
    T value;
    recv(client_socket, &value, sizeof(value), 0);
    return value;
}

template<>
string cc::ConnectionService::receive_message<string>(int client_socket) {
    char str[MESSAGE_LEN];
    recv(client_socket, str, sizeof(str), 0);
    return string() + str;
}

void cc::ConnectionService::shutdown() {
    close(server_socket);
}

int cc::ConnectionService::accept_client() {
    struct sockaddr_in client;
    int client_socket;
    unsigned int len=sizeof(sockaddr_in);

    if((client_socket=accept(server_socket,(struct sockaddr *)&client,&len))==-1)
    {
        perror("accept error: ");
        exit(-1);
    }

    return client_socket;
}