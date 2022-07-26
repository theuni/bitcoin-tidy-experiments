cmake_minimum_required(VERSION 3.9)

project(bitcoin-tidy-experiments VERSION 1.0.0 DESCRIPTION "Experimental clang-tidy checks for Bitcoin Core")

include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS False)

add_compile_options(-fno-rtti)
add_compile_options(-fno-exceptions)

add_library(bitcoin-tidy-experiments SHARED bitcoin-tidy.cpp EarlyExitTidyModule.cpp ExportMainCheck.cpp InitListCheck.cpp LogPrintfCheck.cpp NoADLCheck.cpp)

install(TARGETS bitcoin-tidy-experiments LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
