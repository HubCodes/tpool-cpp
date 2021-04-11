cmake_minimum_required(VERSION 3.17)
project(tpool_cpp)

set(CMAKE_CXX_STANDARD 14)

add_executable(tpool_cpp main.cpp)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(tpool_cpp PRIVATE Threads::Threads)
