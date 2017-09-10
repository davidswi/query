//
// Created by dave on 9/9/17.
//

#ifndef INTEGER_QUERY_DATA_FILE_UTILS_TESTS_H
#define INTEGER_QUERY_DATA_FILE_UTILS_TESTS_H

#include <stdbool.h>

bool test_sort_and_write_values();
bool test_find_file_insertion_point();
bool test_merge_values_into_file_overlapping_start();
bool test_merge_values_into_file_overlapping_middle();
bool test_merge_values_into_file_overlapping_end();
bool test_merge_values_into_file_nonoverlapping();

#endif //INTEGER_QUERY_DATA_FILE_UTILS_TESTS_H
