#pragma once

#include "../Services/ConnectionService.h"
#include "../Services/UserService.h"

#include <string>
#include <utility>

#include <thread>
#include <mutex>
#include <unordered_map>

using namespace std;

namespace cc {
    struct Session {
        uint32_t id;
        string name;
        bool online = false;
        int socket;
        thread th;
    };

    class ChatController {
    private:
        unsigned int seed;
        unordered_map<uint32_t, Session> clients;
        mutex clients_mutex;
        bool isStopping = false;

        ConnectionService *_connectionService;
        UserService *_userService;

    public:
        explicit ChatController(UserService *userService, ConnectionService *connectionService)
                : _userService(userService), _connectionService(connectionService) {
            initialize();
        };
        void initialize();
        void accept_connections();
        void handle_client(int client_socket, uint32_t id);
        void do_signup(uint32_t id);
        void do_login(uint32_t id);
        void do_disconnect(uint32_t id);
        void expect_message(uint32_t id);
        void send_message(uint32_t id, uint32_t to_id, const string &message);
        void broadcast_message(uint32_t id, const string &message);
        void broadcast_alert(const string &text);
        static string gen_password(const string &password);
    };
} // namespace cc