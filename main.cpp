#include <iostream>
#include <nlohmann/json.hpp>
#include <fstream>
#include <iomanip>
#include "Controllers/ChatController.h"
#include "Services/UserService.h"
#include "Services/ConnectionService.h"

using namespace std;
using namespace cc;

int main() {
    new ChatController(new UserService, new ConnectionService);

    return 0;
}
