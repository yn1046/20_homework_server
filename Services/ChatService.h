#pragma once

#include <string>
#include <vector>
#include "../Models/User.h"
#include "../Models/Message.h"

using namespace std;
using namespace cc;
using json = nlohmann::json;

namespace cc {
    class ChatService {
        const string file_name = "chat.json";
    public:
        ChatService() {
            initialize();
        }

        void initialize();
        vector<Message> get_messages();
        vector<string> get_formatted_messages(const User *user);
        void post_message(Message &message);
    };
}