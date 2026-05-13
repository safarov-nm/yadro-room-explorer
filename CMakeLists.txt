cmake_minimum_required(VERSION 3.10)
project(RoomExplorer)

set(CMAKE_CXX_STANDARD 17)

add_executable(room_explorer
    src/main.cpp
    src/game.cpp
)

target_include_directories(room_explorer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)