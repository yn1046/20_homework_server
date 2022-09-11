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
    while(true)
    {
        int client_socket = _connectionService->accept_client();

        seed++;
        thread t(handle_client, client_socket, seed);
        lock_guard<mutex> guard(clients_mutex);
        clients.push_back({seed, string("Anonymous"),client_socket,(move(t))});
    }

    for(int i=0; i<clients.size(); i++)
    {
        if(clients[i].th.joinable())
            clients[i].th.join();
    }

    close(server_socket);
    return 0;
}

int cc::ChatController::get_action() {
    int action = 0;
    do {
        cout << (action < 0 || action > 2 ? "Incorrect input"
                                          : "Welcome to ConsoleChat!")
             << endl;
        cout << "Please select an action:" << endl;
        cout << "1 - login" << endl;
        cout << "2 - sign up" << endl;
        cout << "0 - exit" << endl;

        cin >> action;
    } while (action < 0 || action > 2);

    return action;
}

void cc::ChatController::do_signup() {
    string login, password, pass_hash;
    do {
        cout << "Choose login" << endl;
        cin >> login;
        if (_userService->find_user(login)) {
            cout << "Login already in use" << endl;
        }
    } while (_userService->find_user(login));
    cout << "Choose password" << endl;
    cin >> password;
    pass_hash = gen_password(password);
    User user{.login = login, .password = pass_hash};
    User created_user = _userService->add_user(user);

    enter_chat(created_user);
}

void cc::ChatController::do_login() {
    string login, password, pass_hash;
    do {
        cout << "Enter login" << endl;
        cin >> login;
        if (!_userService->find_user(login)) {
            cout << "User not found" << endl;
        }
    } while (!_userService->find_user(login));
    User user = _userService->get_user(login);
    do {
        cout << "Enter password" << endl;
        cin >> password;
        if (!Botan::check_bcrypt(password, user.password)) {
            cout << "Bad password" << endl;
        }
    } while (!Botan::check_bcrypt(password, user.password));

    enter_chat(user);
}

string cc::ChatController::gen_password(const string &password) {
    Botan::AutoSeeded_RNG rng;
    int work_factor = 6;
    string pass_hash = Botan::generate_bcrypt(password, rng, work_factor);
    return pass_hash;
}

void cc::ChatController::do_input() {
    string input;
    getline(cin, input);
    input = trim(input);
    if (input.length() > 0) {
        if (input[0] == '@') {
            auto receiver_nick = input.substr(1, input.find(' ') - 1);
            if (_userService->find_user(receiver_nick)) {
                auto receiver = _userService->get_user(receiver_nick);
                auto text = input.substr(input.find(' ') + 1);
                if (text.empty()) {
                    cout << "Incorrect format. Type /h for help." << endl;
                } else {
                    Message message{.sender = _user->login,
                            .text = text,
                            .is_personal=true,
                            .between={*_user, receiver}};
                    _chatService->post_message(message);
                }
            } else {
                cout << "User @" + receiver_nick + " not found." << endl;
            }
        } else if (input[0] == '/') {
            if (input[1] == 'h') {
                show_help();
            } else if (input[1] == 'q' || input[1] == '0') {
                exit(0);
            } else {
                cout << "Unknown command. Type /h for help." << endl;
            }
        } else {
            Message message{.sender = _user->login, .text = input};
            _chatService->post_message(message);
        }
        show_messages();
    }
    do_input();
}
