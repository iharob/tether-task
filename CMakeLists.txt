cmake_minimum_required(VERSION 4.0)

find_package(cmake-bare REQUIRED PATHS node_modules/cmake-bare)

find_path(LIBUV_INCLUDE_DIR NAMES uv.h)
find_library(LIBUV_LIBRARY NAMES uv libuv)

project(bare_addon C)

add_bare_module(bare_addon)

include_directories(src/include)

file(GLOB SOURCES "src/sources/*")
file(GLOB HEADERS "src/include/*")

target_sources(${bare_addon} PRIVATE binding.c ${SOURCES} ${HEADERS})
set_target_properties(${bare_addon} PROPERTIES LINKER_LANGUAGE C)
