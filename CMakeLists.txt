cmake_minimum_required(VERSION 3.15)
project(MyProject CXX)

set(CMAKE_CXX_STANDARD 20)

enable_testing()

add_executable(basic_test testing/gpt-tests.cpp)
add_executable(diff_test testing/diff-test.cpp)
add_test(NAME basic_tests COMMAND basic_test)
add_test(NAME difference_tests COMMAND diff_test)


target_include_directories(basic_test PRIVATE .)
target_include_directories(diff_test PRIVATE .)
if (SANITIZE_ADDRESS)
    add_compile_options(-fsanitize=address -g)
    add_link_options(-fsanitize=address)
endif()
