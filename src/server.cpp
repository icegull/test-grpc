#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#include "helloworld.pb.h"
#include "helloworld.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerReaderWriter;
using grpc::ServerWriter;
using grpc::Status;
using helloworld::Greeter;
using helloworld::HelloReply;
using helloworld::HelloRequest;

// Logic and data behind the server's behavior.
class GreeterServiceImpl final : public Greeter::Service {
public:
    // Implements the SayHello RPC method.
    Status SayHello(ServerContext* context, const HelloRequest* request,
                    HelloReply* reply) override {
        std::string prefix("Hello ");
        reply->set_message(prefix + request->name());
        std::cout << "SayHello called with name: " << request->name() << std::endl;
        return Status::OK;
    }

    // Implements the SayHelloStreamReply RPC method.
    Status SayHelloStreamReply(ServerContext* context, const HelloRequest* request,
                              ServerWriter<HelloReply>* writer) override {
        std::cout << "SayHelloStreamReply called with name: " << request->name() << std::endl;
        
        // Send multiple responses
        for (int i = 0; i < 5; i++) {
            HelloReply reply;
            reply.set_message("Hello " + request->name() + " response #" + std::to_string(i));
            writer->Write(reply);
            
            // Small delay between responses
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
        
        return Status::OK;
    }

    // Implements the SayHelloBidiStream RPC method.
    Status SayHelloBidiStream(ServerContext* context,
                             ServerReaderWriter<HelloReply, HelloRequest>* stream) override {
        HelloRequest request;
        
        // Read requests from the client stream
        while (stream->Read(&request)) {
            std::cout << "Received request with name: " << request.name() << std::endl;
            
            // Send a reply
            HelloReply reply;
            reply.set_message("Hello " + request.name() + " (bidirectional)");
            stream->Write(reply);
        }
        
        return Status::OK;
    }
};

void RunServer() {
    std::string server_address("0.0.0.0:50051");
    GreeterServiceImpl service;

    grpc::EnableDefaultHealthCheckService(true);
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();
    
    ServerBuilder builder;
    // Listen on the given address without any authentication mechanism
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    // Register "service" as the instance through which we'll communicate with clients
    builder.RegisterService(&service);
    
    // Finally assemble the server
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;

    // Wait for the server to shutdown
    server->Wait();
}

int main() {
    RunServer();
    return 0;
}
