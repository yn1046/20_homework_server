#include "ChatService.h"
#include <fstream>
#include <iomanip>

void ChatService::initialize() {
    ifstream test_exists(file_name);
    if (test_exists.good())
        return;

    ofstream json_file(file_name);
    if (json_file) {
        json j = vector<Message>();
        json_file << j;
    } else throw;
}

vector<Message> ChatService::get_messages() {
    ifstream json_file(file_name);
    if (json_file) {
        json j;
        json_file >> j;
        vector<Message> chat = j.get<vector<Message>>();

        return chat;
    } else throw;
}

vector<string> ChatService::get_formatted_messages(const User *user) {
    auto get_name = [&](User *compared) -> string {
        return user->id == compared->id ? "YOU" : compared->login;
    };
    auto messages = get_messages();
    vector<string> formatted;
    for (auto &message: messages) {
        if (message.is_personal) {
            auto[user1, user2] = message.between;
            if (user1.id == user->id || user2.id == user->id) {
                string caption;
                caption = "(PM) [" + get_name(&user1) + " to " + get_name(&user2) + "] " + message.text;
                formatted.push_back(caption);
            }
        } else {
            formatted.push_back("[" + message.sender + "] " + message.text);
        }
    }
    return formatted;
}

void ChatService::post_message(Message &message) {
    auto messages = get_messages();
    messages.push_back(message);

    ofstream json_file(file_name);
    if (json_file) {
        json j;
        j = messages;
        json_file << setw(4) << j << endl;
    } else throw;
}
