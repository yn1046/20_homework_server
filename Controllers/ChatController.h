#pragma once

#include "../Services/ConnectionService.h"
#include "../Services/UserService.h"

#include <string>
#include <utility>

#include <thread>
#include <mutex>

using namespace std;

namespace cc {
    struct Session
    {
        int id;
        string name;
        int socket;
        thread th;
    };

    class ChatController {
    private:
        unsigned int seed;
        vector<Session> clients;
        mutex clients_mutex;

        ConnectionService *_connectionService;
        UserService *_userService;

    public:
        explicit ChatController(UserService *userService, ConnectionService *connectionService)
                : _userService(userService), _connectionService(connectionService) {
            initialize();
        };
        void initialize();
        void accept_connections();
        void handle_client(int client_socket, int id);
        void do_signup(int id);
        void do_login(int id);
        void expect_message(int id);
        void send_message(int id);
        void broadcast_message();
        static int get_action();
        static string gen_password(const string &password);
    };
} // namespace cc