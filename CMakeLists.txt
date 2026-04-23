cmake_minimum_required(VERSION 3.15)
project(MyProject CXX)

set(CMAKE_CXX_STANDARD 20)

enable_testing()

add_executable(test_run testing/gpt-tests.cpp)
add_test(NAME all_tests COMMAND test_run)
target_include_directories(test_run PRIVATE .)
if (SANITIZE_ADDRESS)
    add_compile_options(-fsanitize=address -g)
    add_link_options(-fsanitize=address)
endif()
