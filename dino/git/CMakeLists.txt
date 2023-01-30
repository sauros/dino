cmake_minimum_required(VERSION 3.5)
project (git)

set(CMAKE_CXX_STANDARD 20)

add_library(git_lib SHARED package.cpp)
set_target_properties(git_lib PROPERTIES PREFIX "" SUFFIX ".lib" OUTPUT_NAME "git")
