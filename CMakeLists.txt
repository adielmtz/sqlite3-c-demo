cmake_minimum_required(VERSION 3.22)
project(TestSQLite3Program C)

set(CMAKE_C_STANDARD 11)

add_executable(TestSQLite3Program main.c)

find_package(SQLite3)
include_directories(${SQLite3_INCLUDE_DIRS})
target_link_libraries(TestSQLite3Program ${SQLite3_LIBRARIES})
