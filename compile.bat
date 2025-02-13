@ECHO ON

set BASEDIR=%~dp0
PUSHD %BASEDIR%

RMDIR /Q /S build

conan build . --output-folder=build --build=missing -s build_type=Debug
conan build . --output-folder=build --build=missing -s build_type=Release
