cmake_minimum_required(VERSION 3.12)
project(Raytracing)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)

find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

include_directories(Libraries/include)
add_library(glad src/glad.c)

file(GLOB game_src
     "src/*.h"
     "src/*.cpp"
     "src/*/*.cpp"
     "src/*/*.h"
)

add_executable(game ${game_src})
target_link_libraries(game glfw glad glm)