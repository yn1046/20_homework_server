#pragma once

#include "../Services/ChatService.h"
#include "../Services/UserService.h"
#include <string>
#include <utility>

using namespace std;

namespace cc {
    class ChatController {
    private:
        const User *_user = nullptr;
        UserService *_userService;
        ChatService *_chatService;

    public:
        explicit ChatController(UserService *userService, ChatService *chatService)
                : _userService(userService), _chatService(chatService) {
            initialize();
        };
        void initialize();
        void do_signup();
        void do_login();
        void enter_chat(const User &user);
        void show_messages();
        void do_input();
        static void show_help();
        static int get_action();
        static string gen_password(const string &password);
    };
} // namespace cc