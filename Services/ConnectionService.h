#pragma once

#include <iostream>
#include <string>
#include <vector>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <arpa/inet.h>
#include <unistd.h>

#include <signal.h>
#include <string.h>

using namespace std;

#define MESSAGE_LEN 1024
#define PORT 10000

namespace cc {
    class ConnectionService {
        const int server_socket;
    public:
        ConnectionService() : server_socket{socket(AF_INET, SOCK_STREAM, 0)} {}

        void connect();
        int accept_client();

        template<typename T>
        void send_message(int client_socket, T value);

        template<typename T>
        T receive_message(int client_socket);

        void shutdown();
    };
}