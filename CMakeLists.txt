cmake_minimum_required(VERSION 3.16)
project(GameEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include all source files
file(GLOB_RECURSE SRC_FILES
    src/*.cpp
)

add_executable(GameEngine ${SRC_FILES})

# Include Headers
target_include_directories(GameEngine PRIVATE
    src
    core
    rendering
    vendor
    vendor/glad/include
)

# Link glad (GL loader) and X11
target_link_libraries(GameEngine
    dl
    GL
    X11
)
