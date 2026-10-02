# 设置 CMake 最低版本要求
cmake_minimum_required(VERSION 3.10)

# 设置项目名称
project(Compiler)

# 设置 C++ 标准为 C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 指定使用 Clang 编译器
if(WIN32)
    set(CMAKE_C_COMPILER "clang")
    set(CMAKE_CXX_COMPILER "clang++")
else()
    # 在非 Windows 系统上，通常也可以直接设置
    set(CMAKE_C_COMPILER "clang")
    set(CMAKE_CXX_COMPILER "clang++")
endif()

# 添加可执行文件
set(SOURCE_FILES 
	ComplierSysY.cpp
	ErrorHandler.cpp
	Interpreter.cpp
	Lexer.cpp
	Parser.cpp
	SymbolTableManager.cpp)
add_executable(Compiler ${SOURCE_FILES})