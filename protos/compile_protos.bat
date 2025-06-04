"C:\Users\x.wang\Downloads\grpc-submodule\Release\protobuf\bin\protoc.exe" ^
--cpp_out=..\src ^
--proto_path=.\ ^
helloworld.proto

"C:\Users\x.wang\Downloads\grpc-submodule\Release\protobuf\bin\protoc.exe" ^
 --grpc_out=..\src ^
 --plugin=protoc-gen-grpc="C:\Users\x.wang\Downloads\grpc-submodule\Release\grpc\bin\grpc_cpp_plugin.exe" ^
 --proto_path=.\ ^
helloworld.proto