//
// Created by dave on 9/9/17.
//

#ifndef INTEGER_QUERY_DATA_FILE_UTILS_H
#define INTEGER_QUERY_DATA_FILE_UTILS_H

#include <stdio.h>
#include <stdint.h>

typedef struct{
    uint32_t min_value;
    uint32_t max_value;
} range_t;

int get_num_values_in_file(FILE *file);
int sort_and_write_values(FILE *file, uint32_t *values, size_t num_values);
long find_file_insertion_point(FILE *file, uint32_t value, long start_ind, long end_ind);
int merge_values_into_file(FILE *file, uint32_t *values, size_t num_values);

#endif //INTEGER_QUERY_DATA_FILE_UTILS_H
