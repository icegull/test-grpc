@echo off
set BASEDIR=D:\Project\3rd\test-grpc\
PUSHD %BASEDIR%

REM 首先确保生成protobuf文件
cd src
call compile_protos.bat
cd ..

REM 清理build目录
RMDIR /Q /S build

REM 使用conan构建项目
conan build . --output-folder=build\release --build=missing -s build_type=Release

POPD
