cmake_minimum_required(VERSION 3.20)
project(20_homework_client)

set(CMAKE_CXX_STANDARD 20)

include(cmake-build-debug/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(20_homework_client main.cpp Models/User.h Services/UserService.cpp Services/UserService.h Controllers/ChatController.cpp Controllers/ChatController.h Enums/ActionTypes.h Models/Message.h Services/ChatService.cpp Services/ChatService.h Helpers/strutil.h Services/ConnectionService.cpp Services/ConnectionService.h)

target_link_libraries(20_homework_client ${CONAN_LIBS})