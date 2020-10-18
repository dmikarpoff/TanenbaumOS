cmake_minimum_required(VERSION 3.16)

project(tiny_shell)

find_package(Boost REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} PUBLIC
		Boost::boost
	)
