cmake_minimum_required(VERSION 3.7)
project(integer_query)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES main.c sorted_overlay.h sorted_overlay.c data_file_utils.h data_file_utils.c search.h search.c search_tests.c search_tests.h data_file_utils_tests.h data_file_utils_tests.c)
add_executable(integer_query ${SOURCE_FILES})