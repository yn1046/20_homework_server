#pragma once

#include <string>
#include <utility>

using namespace std;
using json = nlohmann::json;

namespace cc {
    struct Message {
        string sender;
        string text;
        bool is_personal = false;
        pair<User, User> between;
    };

    inline void to_json(json &j, const cc::Message &m) {
        j = json{{"sender",      m.sender},
                 {"text",        m.text},
                 {"is_personal", m.is_personal},
                 {"between",     m.between}};
    }

    inline void from_json(const json &j, cc::Message &m) {
        j.at("sender").get_to(m.sender);
        j.at("text").get_to(m.text);
        j.at("is_personal").get_to(m.is_personal);
        j.at("between").get_to(m.between);
    }
}