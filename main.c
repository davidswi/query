#define UNIT_TESTS

#ifdef UNIT_TESTS

#include <stdio.h>
#include "search_tests.h"
#include "data_file_utils_tests.h"
#include "sorted_overlay_tests.h"

int main(){
    int passing_test_count = 0;
    int failing_test_count = 0;

    if (test_search_array_found()){
        printf("Test test_search_array_found() passed\n");
        passing_test_count++;
    }
    else{
        failing_test_count++;
    }

    if (test_search_array_not_found()){
        printf("Test test_search_array_not_found() passed\n");
        passing_test_count++;
    }
    else{
        failing_test_count++;
    }

    if (test_search_file_found()){
        printf("Test test_search_file_found() passed\n");
        passing_test_count++;
    }
    else{
        failing_test_count++;
    }

    if (test_search_file_not_found()){
        printf("Test test_search_file_not_found() passed\n");
        passing_test_count++;
    }
    else{
        failing_test_count++;
    }

    if (test_sort_and_write_values()){
        printf("Test test_sort_and_write_values() passed\n");
        passing_test_count++;
    }
    else{
        failing_test_count++;
    }

    if (test_find_file_insertion_point()){
        printf("Test test_find_file_insertion_point() passed\n");
        passing_test_count++;
    }
    else{
        failing_test_count++;
    }

    if (test_merge_values_into_file_overlapping_start()){
        printf("Test test_merge_values_into_file_overlapping_start() passed\n");
        passing_test_count++;
    }
    else{
        failing_test_count++;
    }

    if (test_merge_values_into_file_overlapping_middle()){
        printf("Test test_merge_values_into_file_overlapping_middle() passed\n");
        passing_test_count++;
    }
    else{
        failing_test_count++;
    }

    if (test_merge_values_into_file_overlapping_end()){
        printf("Test test_merge_values_into_file_overlapping_end() passed\n");
        passing_test_count++;
    }
    else{
        failing_test_count++;
    }

    if (test_merge_values_into_file_nonoverlapping()){
        printf("Test test_merge_values_into_file_nonoverlapping() passed\n");
        passing_test_count++;
    }
    else{
        failing_test_count++;
    }

    if (test_sorted_overlay_find_nearest_in_memory()){
        printf("Test test_sorted_overlay_find_nearest_in_memory() passed\n");
        passing_test_count++;
    }
    else{
        failing_test_count++;
    }

    if (test_sorted_overlay_find_nearest_in_file()){
        printf("Test test_sorted_overlay_find_nearest_in_file() passed\n");
        passing_test_count++;
    }
    else{
        failing_test_count++;
    }


    printf("Tests completed with %d passed, %d failed.\n", passing_test_count,
           failing_test_count);
    return failing_test_count;
}

#else // UNIT_TESTS

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <netinet/in.h>
#include "sorted_overlay.h"
#include "data_file_utils.h"

#define MAX_RANDOM_VALUE        (1 << 25) - 1
#define IN_MEMORY_SORTED_VALUES 32768 / 4
#define NUM_OVERLAYS            (MAX_RANDOM_VALUE >> 3) / IN_MEMORY_SORTED_VALUES
#define MAX_TEST_CASES          10000

// Data input
int read_data_file(FILE *data_file){
    int ret = 0;

    long num_values = get_num_values_in_file(data_file);
    if (sorted_overlay_init(num_values) < 0){
        ret = -1;
    }

    while (ret == 0 && !feof(data_file)){
        uint32_t big_endian_value;
        size_t bytes_read = fread(&big_endian_value, sizeof(uint32_t), 1, data_file);
        if (bytes_read != sizeof(uint32_t)){
            ret = -1;
        }
        else{
            uint32_t host_format_value = ntohl(big_endian_value);
            if (sorted_overlay_add(host_format_value) < 0){
                ret -1;
            }
        }
    }

    fclose(data_file);

    return ret;
}

int main(int argc, char **argv){
    if (argc != 2){
        printf("Usage: query <filename>, where filename is a file containing values in the expected format.\n");
        return -1;
    }

    FILE *data_file = fopen(argv[1], "rb");
    if (data_file == NULL){
        printf("ERROR: Failed to open data file. Check that file exists.\n");
        return -1;
    }

    if (read_data_file(data_file) < 0){
        printf("ERROR: Failed to read data file. Check file integrity.\n");
        return -1;
    }

    printf("Reading number of test cases...\n");
    char *test_case_count = NULL;
    if (getline(&test_case_count, NULL, stdin) == -1){
        printf("ERROR: Test case count not specified.\n");
        return -1;
    }

    int total_tests = atoi(test_case_count);
    if (total_tests < 0 || total_tests > MAX_TEST_CASES){
        printf("ERROR: %d is an invalid test case count (1-10000 allowed)\n", total_tests);
        return -1;
    }

    char *query_integer;
    while (test_case_count > 0 && getline(&query_integer, NULL, stdin) != -1){
        uint32_t target_value = atoi(query_integer);
        uint32_t closest_value = sorted_overlay_find_nearest(target_value);
        printf("%d\n", closest_value);
    }

    return 0;
}
#endif // UNIT_TESTS