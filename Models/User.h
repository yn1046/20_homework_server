#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

namespace cc {
    struct User {
        int id;
        string login;
        string password;
    };

    inline void to_json(json &j, const cc::User &u) {
        j = json{{"id",       u.id},
                 {"login",    u.login},
                 {"password", u.password}};
    }

    inline void from_json(const json &j, cc::User &u) {
        j.at("id").get_to(u.id);
        j.at("login").get_to(u.login);
        j.at("password").get_to(u.password);
    }
}