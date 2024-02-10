cmake_minimum_required(VERSION 3.8)
add_executable(server simpleServer.cpp)
target_link_libraries(server PUBLIC oatpp)