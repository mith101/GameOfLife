cmake_minimum_required(VERSION 3.16)
list(APPEND CMAKE_MODULE_PATH "~/local/share/cmake")

set(CMAKE_C_COMPILER /usr/bin/gcc-10)
set(CMAKE_CXX_COMPILER /usr/bin/g++-10)

project(GameOfLife)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS False)

add_compile_options(-g -Wall -Wextra -Wpedantic -Werror)

find_package(Threads REQUIRED)

include(Units)

add_unit(Cell)
add_unit(GameOfLife PRIVATE Threads::Threads)

include(InstallAndPackage)
