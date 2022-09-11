#include <fstream>
#include <nlohmann/json.hpp>
#include <iomanip>
#include "UserService.h"

User UserService::add_user(User &user) {
    auto users = get_users();
    ofstream json_file(file_name);
    json j;
    User new_user = move(user);
    if (!users.empty()) {
        int max_id = 0;
        for (auto &item: users) max_id = max(max_id, item.id);
        new_user.id = max_id + 1;
    } else {
        new_user.id = 1;
    }
    users.push_back(new_user);
    j = users;
    json_file << setw(4) << j << endl;
    return new_user;
}

User UserService::get_user(string &login) {
    if (!find_user(login)) throw;
    auto users = get_users();

    return *std::find_if(users.begin(), users.end(), [&](const User &item) {
        return item.login == login;
    });
}

bool UserService::find_user(string &login) {
    auto users = get_users();
    auto u = std::find_if(users.begin(), users.end(), [&](const User &item) {
        return item.login == login;
    });
    if (u != end(users)) {
        return true;
    }
    return false;
}

vector<User> UserService::get_users() {
    ifstream json_file(file_name);
    if (json_file) {
        json j;
        json_file >> j;
        vector<User> users = j.get<vector<User>>();

        return users;
    } else throw;
}

void UserService::initialize() {
    ifstream test_exists(file_name);
    if (test_exists.good()) return;

    ofstream json_file(file_name);
    if (json_file) {
        json j = vector<User>();
        json_file << j;
    } else throw;
}
