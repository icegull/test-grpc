cmake_minimum_required(VERSION 3.20)
project(grpc_example)

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED True)
add_definitions(-DUNICODE -D_UNICODE)

find_package(gRPC REQUIRED)

set(PROTO_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/helloworld.pb.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/helloworld.grpc.pb.cc
)

add_executable(rpc-server 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/server.cpp
    ${PROTO_SRC}
)

target_include_directories(rpc-server PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(rpc-server
    grpc::grpc
    protobuf::libprotobuf
)

add_executable(rpc-client 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/client.cpp
    ${PROTO_SRC}
)

target_include_directories(rpc-client PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(rpc-client
    grpc::grpc
    protobuf::libprotobuf
)


