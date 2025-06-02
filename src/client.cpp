#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <grpcpp/grpcpp.h>
#include "helloworld.pb.h"
#include "helloworld.grpc.pb.h"

//implement the client-side logic for the gRPC service
using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using helloworld::Greeter;
using helloworld::HelloReply;
using helloworld::HelloRequest;

class GreeterClient {
public:
    GreeterClient(std::shared_ptr<Channel> channel)
        : stub_(Greeter::NewStub(channel)) {
    }

    // Asynchronous method to send a greeting
    std::string SayHello(const std::string& user) {
        HelloRequest request;
        request.set_name(user);

        HelloReply reply;
        ClientContext context;

        Status status = stub_->SayHello(&context, request, &reply);

        if (status.ok()) {
            return reply.message();
        }
        else {
            std::cerr << "RPC failed: " << status.error_message() << std::endl;
            return "RPC failed";
        }
    }

    // Method to call SayHelloStreamReply and receive multiple responses
    void SayHelloStreamReply(const std::string& user) {
        HelloRequest request;
        request.set_name(user);

        ClientContext context;
        HelloReply reply;

        // Create a stream reader for the server's responses
        std::unique_ptr<grpc::ClientReader<HelloReply>> reader =
            stub_->SayHelloStreamReply(&context, request);

        // Read each response from the server
        while (reader->Read(&reply)) {
            std::cout << "Received: " << reply.message() << std::endl;
        }

        // Check the final status
        Status status = reader->Finish();
        if (!status.ok()) {
            std::cerr << "SayHelloStreamReply RPC failed: " << status.error_message() << std::endl;
        }
    }
    
    // Method to call SayHelloBidiStream with bidirectional streaming
    void SayHelloBidiStream() {
        ClientContext context;
        
        // Create a bidirectional stream
        std::shared_ptr<grpc::ClientReaderWriter<HelloRequest, HelloReply>> stream = 
            stub_->SayHelloBidiStream(&context);
        
        // Create a thread to send multiple requests
        std::thread writer([stream]() {
            std::vector<std::string> names = {"Alice", "Bob", "Charlie", "David", "Emma"};
            
            for (const auto& name : names) {
                HelloRequest request;
                request.set_name(name);
                
                if (stream->Write(request)) {
                    std::cout << "Sent request with name: " << name << std::endl;
                } else {
                    // Stream is broken
                    break;
                }
                
                // Small delay between requests
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }
            
            // Close the write direction of the stream
            stream->WritesDone();
        });
        
        // Read responses in the main thread
        HelloReply reply;
        while (stream->Read(&reply)) {
            std::cout << "Received response: " << reply.message() << std::endl;
        }
        
        // Wait for the sending thread to complete
        writer.join();
        
        // Check the final status
        Status status = stream->Finish();
        if (!status.ok()) {
            std::cerr << "SayHelloBidiStream RPC failed: " << status.error_message() << std::endl;
        }
    }

private:
    std::unique_ptr<Greeter::Stub> stub_;
};

// Main function to run the client
int main(int argc, char** argv) {
    GreeterClient client(grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials()));
    std::string user("world");

    // Call the regular SayHello method
    std::string reply = client.SayHello(user);
    std::cout << "Greeter received: " << reply << std::endl;

    // Call the streaming method
    std::cout << "Calling streaming RPC..." << std::endl;
    client.SayHelloStreamReply(user);
    
    // Call the bidirectional streaming method
    std::cout << "Calling bidirectional streaming RPC..." << std::endl;
    client.SayHelloBidiStream();

    return 0;
}