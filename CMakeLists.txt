cmake_minimum_required(VERSION 3.20)
project(20_homework_server)

set(CMAKE_CXX_STANDARD 20)

include(cmake-build-debug/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(20_homework_server main.cpp Models/User.h Services/UserService.cpp Services/UserService.h Services/ConnectionService.cpp Services/ConnectionService.h Controllers/ChatController.cpp Controllers/ChatController.h Enums/ActionTypes.h)

target_link_libraries(20_homework_server ${CONAN_LIBS})