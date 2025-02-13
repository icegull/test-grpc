cmake_minimum_required(VERSION 3.15)
project(test-grpc CXX)



add_executable(test-grpc src/test-grpc.cpp src/main.cpp)



install(TARGETS test-grpc DESTINATION "."
        RUNTIME DESTINATION bin
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        )
