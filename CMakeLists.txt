cmake_minimum_required(VERSION 3.15)
project(test-grpc CXX)

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED True)
add_definitions(-DUNICODE -D_UNICODE)

find_package(gRPC REQUIRED)

add_executable("rpc-server" src/server.cpp)
target_link_libraries("rpc-server" grpc::grpc)

add_executable("rpc-client" src/client.cpp)
target_link_libraries("rpc-client" grpc::grpc)


