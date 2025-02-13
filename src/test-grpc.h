#pragma once

#include <vector>
#include <string>


#ifdef _WIN32
  #define TEST_GRPC_EXPORT __declspec(dllexport)
#else
  #define TEST_GRPC_EXPORT
#endif

TEST_GRPC_EXPORT void test_grpc();
TEST_GRPC_EXPORT void test_grpc_print_vector(const std::vector<std::string> &strings);
