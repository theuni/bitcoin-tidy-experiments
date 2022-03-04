cmake_minimum_required(VERSION 3.9)

project(bitcoin-tidy VERSION 1.0.0 DESCRIPTION "clang-tidy checks for Bitcoin Core")

include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS False)

add_compile_options(-fno-rtti)
add_compile_options(-fno-exceptions)

add_library(bitcoin-tidy SHARED bitcoin-tidy.cpp EarlyExitTidyModule.cpp )

install(TARGETS bitcoin-tidy LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
