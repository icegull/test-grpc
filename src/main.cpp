#include "test-grpc.h"
#include <vector>
#include <string>

int main() {
    test_grpc();

    std::vector<std::string> vec;
    vec.push_back("test_package");

    test_grpc_print_vector(vec);
}
