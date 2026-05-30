cmake_minimum_required(VERSION 3.14)
project(fifo LANGUAGES C CXX)

add_library(fifo src/fifo.c)
target_include_directories(fifo PUBLIC include)

enable_testing()
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.14.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(fifo_test tests/test.cpp)
target_link_libraries(fifo_test PRIVATE fifo GTest::gtest)

include(GoogleTest)
gtest_discover_tests(fifo_test)