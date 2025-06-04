#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <atomic>

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
class GreeterServiceImpl final : public Greeter::Service
{
public:
    // Implements the SayHello RPC method.
    Status SayHello(ServerContext *context, const HelloRequest *request,
                    HelloReply *reply) override
    {
        std::string prefix("Hello ");
        reply->set_message(prefix + request->name());
        std::cout << "SayHello called with name: " << request->name() << std::endl;
        return Status::OK;
    }

    // Implements the SayHelloStreamReply RPC method.
    Status SayHelloStreamReply(ServerContext *context, const HelloRequest *request,
                               ServerWriter<HelloReply> *writer) override
    {
        std::cout << "SayHelloStreamReply called with name: " << request->name() << std::endl;

        // Send multiple responses
        for (int i = 0; i < 5; i++)
        {
            HelloReply reply;
            reply.set_message("Hello " + request->name() + " response #" + std::to_string(i));
            writer->Write(reply);

            // Small delay between responses
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }

        return Status::OK;
    }

    // Implements the SayHelloBidiStream RPC method.
    Status SayHelloBidiStream(ServerContext *context,
                              ServerReaderWriter<HelloReply, HelloRequest> *stream) override
    {
        std::cout << "New bidirectional stream connection started" << std::endl;

        // 用于控制回复线程的标志
        std::atomic<bool> should_stop{false};
        std::string client_name;
        bool name_received = false;

        // 启动回复线程
        std::thread reply_thread([&]()
                                 {
            int counter = 0;
            while (!should_stop.load()) {
                if (name_received) {
                    HelloReply reply;
                    reply.set_message("Hello " + client_name + " - message #" + std::to_string(counter++));
                    
                    // 写入回复，如果失败则停止
                    if (!stream->Write(reply)) {
                        std::cout << "Failed to write reply, stopping thread for " << client_name << std::endl;
                        break;
                    }
                    
                    std::cout << "Sent reply #" << counter-1 << " to " << client_name << std::endl;
                }
                
                // 控制发送频率
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            }
            std::cout << "Reply thread stopped for " << client_name << std::endl; });

        // 读取客户端请求
        HelloRequest request;
        while (!should_stop.load())
        {
            if (stream->Read(&request))
            {
                if (!name_received)
                {
                    client_name = request.name();
                    name_received = true;
                    std::cout << "Started continuous replies for client: " << client_name << std::endl;
                }

                std::cout << "Received request from " << request.name() << ": " << request.name() << std::endl;

                // 可以根据请求内容做特殊处理
                if (request.name() == "stop")
                {
                    std::cout << "Received stop signal from " << client_name << std::endl;
                    // 停止回复线程
                    should_stop.store(true);
                    break;
                }
            }
            else
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        if (reply_thread.joinable())
        {
            reply_thread.join();
        }

        std::cout << "Bidirectional stream ended for " << client_name << std::endl;
        return Status::OK;
    }
};

void RunServer()
{
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

int main()
{
    RunServer();
    return 0;
}
