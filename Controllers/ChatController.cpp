#include "ChatController.h"
#include "../Enums/ActionTypes.h"
#include "../Helpers/strutil.h"
#include <botan/auto_rng.h>
#include <botan/bcrypt.h>
#include <iostream>

void cc::ChatController::initialize() {
    _connectionService->connect();
    accept_connections();
}

void cc::ChatController::accept_connections() {
    while (!isStopping) {
        int client_socket = _connectionService->accept_client();

        ++seed;
        thread t(&cc::ChatController::handle_client, this, client_socket, seed);
        lock_guard<mutex> guard(clients_mutex);
        Session session{seed, string("Anonymous"), false, client_socket, (move(t))};
        clients.insert({seed, move(session)});
    }

    for (auto&[_, client]: clients) {
        if (client.th.joinable())
            client.th.join();
    }

    _connectionService->shutdown();
    exit(EXIT_SUCCESS);
}


void ChatController::handle_client(int client_socket, uint32_t id) {
    auto action = _connectionService->receive_message<ACTION_TYPES>(client_socket);

    switch (action) {
        case ACTION_TYPES::LOGIN:
            do_login(id);
            break;
        case ACTION_TYPES::SIGNUP:
            do_signup(id);
            break;
        case ACTION_TYPES::QUIT:
        default:
            return do_disconnect(id);

    }

}


// WIP: should change cout to send
void cc::ChatController::do_signup(uint32_t id) {
    string login, password, pass_hash;

    do {
        login = _connectionService->receive_message<string>(clients[id].socket);
        cout << "Choose login" << endl;
        cin >> login;
        if (_userService->find_user(login)) {
            cout << "Login already in use" << endl;
        }
    } while (_userService->find_user(login));

    cout << "Choose password" << endl;
    password = _connectionService->receive_message<string>(clients[id].socket);
    pass_hash = gen_password(password);
    User user{.login = login, .password = pass_hash};
    User created_user = _userService->add_user(user);

    clients[id].online = true;

    expect_message(id);
}


// WIP: should change cout to send
void cc::ChatController::do_login(uint32_t id) {
    string login, password, pass_hash;
    do {
        login = _connectionService->receive_message<string>(clients[id].socket);
        if (!_userService->find_user(login)) {
            cout << "User not found" << endl;
        }
    } while (!_userService->find_user(login));
    User user = _userService->get_user(login);
    do {
        cout << "Enter password" << endl;
        password = _connectionService->receive_message<string>(clients[id].socket);
        if (!Botan::check_bcrypt(password, user.password)) {
            cout << "Bad password" << endl;
        }
    } while (!Botan::check_bcrypt(password, user.password));

    clients[id].online = true;

    expect_message(id);
}

string cc::ChatController::gen_password(const string &password) {
    Botan::AutoSeeded_RNG rng;
    int work_factor = 6;
    string pass_hash = Botan::generate_bcrypt(password, rng, work_factor);
    return pass_hash;
}

void cc::ChatController::do_disconnect(uint32_t id) {
    string name = clients[id].name;
    {
        lock_guard<mutex> lock(clients_mutex);
        _connectionService->close_client(clients[id].socket);
        clients[id].th.detach();
        clients.erase(id);
    }
    broadcast_alert("::: " + name + " has disconnected :::");
}

void cc::ChatController::expect_message(uint32_t id) {
    bool disconnecting = false;
    while (!disconnecting) {
        string message = trim(_connectionService->receive_message<string>(clients[id].socket));

        if (message.length() > 0) {
            if (message[0] == '@') {
                auto receiver_nick = message.substr(1, message.find(' ') - 1);
                if (_userService->find_user(receiver_nick)) {
                    auto receiver = find(begin(clients), end(clients),
                                         [&receiver_nick](Session &c) { return c.name == receiver_nick && c.online; });
                    if (receiver != clients.end()) {
                        auto text = message.substr(message.find(' ') + 1);
                        if (text.empty()) {
                            _connectionService->send_message<string>(clients[id].socket,
                                                                     "Incorrect format. Type /h for help.");
                        } else {
                            send_message(id, receiver->second.id, text);
                        }
                    } else {
                        _connectionService->send_message<string>(clients[id].socket,
                                                                 "User @" + receiver_nick + " ");
                    }

                } else {
                    _connectionService->send_message<string>(clients[id].socket,
                                                             "User @" + receiver_nick + " not found.");
                }
            } else if (message[0] == '/') {
                if (message[1] == 'h') {
                    const string help = "\nCOMMANDS:\n"
                                        "/h — show this help\n"
                                        "/q or /0 — quit chat\n"
                                        "SENDING PERSONAL MESSAGE:\n"
                                        "@<username> <message>\n"
                                        "example: @john hi!";

                    _connectionService->send_message<string>(clients[id].socket,
                                                             help);
                } else if (message[1] == 'q' || message[1] == '0') {
                    disconnecting = true;
                } else {
                    _connectionService->send_message<string>(clients[id].socket, "Unknown command. Type /h for help.");
                }
            } else {
                broadcast_message(id, message);
            }
        }
    }

    do_disconnect(id);
}

void cc::ChatController::send_message(uint32_t id, uint32_t to_id, const string &message) {
    _connectionService->send_message<string>(clients[id].socket,
                                             "(PM) [YOU to " + clients[to_id].name + "] " + message);
    _connectionService->send_message<string>(clients[to_id].socket,
                                             "(PM) [" + clients[id].name + " to YOU] " + message);
}

void ChatController::broadcast_message(uint32_t id, const string &message) {
    lock_guard<mutex> lock(clients_mutex);
    for (auto &[k, v]: clients) {
        if (k != id && v.online) {
            _connectionService->send_message<string>(v.socket, "[" + clients[id].name + "] " + message);
        }
    }
}

void ChatController::broadcast_alert(const string &text) {
    lock_guard<mutex> lock(clients_mutex);
    for (auto &[k, v]: clients) {
        _connectionService->send_message<string>(v.socket, text);
    }
}
