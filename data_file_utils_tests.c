//
// Created by dave on 9/9/17.
//

#include <stdio.h>
#include "data_file_utils_tests.h"
#include "data_file_utils.h"

#define SORTED_DATA_FILE "sorted.bin"
#define MERGED_DATA_FILE "merged.bin"

bool compare_written_to_expected(char *filename, uint32_t *expected, size_t length){
    bool result = false;

    FILE *data_file = fopen(filename, "rb");
    if (data_file != NULL) {
        int i;
        for (i = 0; i < length; i++){
            uint32_t next_;
            if (fread(&next_, sizeof(uint32_t), 1, data_file) != 1){
                break;
            }
            printf("compare_written_to_expected(): read %u, expecting %u\n", next_, expected[i]);
            if (next_ != expected[i]){
                break;
            }
        }

        result = (i == length);
    }

    return result;
}

bool test_sort_and_write_values(){
    uint32_t unsorted_data_array[] = {128, 97, 96, 46, 11, 2, 6, 127};
    uint32_t expected_data_array[] = {2, 6, 11, 46, 96, 97, 127, 128};
    int ret;
    bool result = false;

    FILE *data_file = fopen(SORTED_DATA_FILE, "wb");
    if (data_file != NULL) {
        if ((ret = sort_and_write_values(data_file, unsorted_data_array, 8)) < 0) {
            printf("ERROR: test_sort_and_write_values() failed\n");
        }
        fclose(data_file);
        if (ret == 0){
            result = compare_written_to_expected(SORTED_DATA_FILE, expected_data_array, 8);
            if (!result){
                printf("ERROR: test_sort_and_write_values() failed. Output file is not sorted in expected order!\n");
            }
        }
    }

    return result;
}

bool test_find_file_insertion_point(){
    bool result = false;

    FILE *data_file = fopen(SORTED_DATA_FILE, "rb");
    if (data_file != NULL) {
        long insert_ind = find_file_insertion_point(data_file, 27, 0, 7);
        if (insert_ind != 3){
            printf("ERROR: test_find_file_insertion_point() failed\n");
        }
        else{
            result = true;
        }
        fclose(data_file);
    }

    return result;
}

int write_data_file(char *file_name, uint32_t *data, size_t num_values){
    FILE *data_file = fopen(file_name, "wb");
    if (data_file == NULL){
        return -1;
    }

    if (fwrite(data, sizeof(uint32_t), num_values, data_file) < num_values){
        return -1;
    }

    fclose(data_file);

    return 0;
}

bool test_merge_values_into_file_overlapping_start(){
    uint32_t file_array[] = {4, 7, 10, 13, 18, 20};
    uint32_t merge_array[] = {1, 2, 3};
    uint32_t expected_array[] = {1, 2, 3, 4, 7, 10, 13, 18, 20};

    if (write_data_file("case1.bin", file_array, 6) < 0){
        printf("ERROR: test_merge_values_into_file_overlapping_start() failed writing data file.\n");
        return false;
    }

    FILE *data_file = fopen("case1.bin", "rb+");
    if (data_file == NULL){
        printf("ERROR: test_merge_values_into_file_overlapping_start() failed opening data file.\n");
        return false;
    }

    int result = merge_values_into_file(data_file, merge_array, 3);
    fclose(data_file);
    if (result < 0){
        printf("ERROR: test_merge_values_into_file_overlapping_start() failed merging s into file.\n");
        return false;
    }

    if (!compare_written_to_expected("case1.bin", expected_array, 9)){
        printf("ERROR: test_merge_values_into_file_overlapping_start() failed --- result doesn't match expected.\n");
        return false;
    }

    return true;
}

bool test_merge_values_into_file_overlapping_middle(){
    uint32_t file_array[] = {4, 5, 10, 12};
    uint32_t merge_array[] = {7, 8};
    uint32_t expected_array[] = {4, 5, 7, 8, 10, 12};

    if (write_data_file("case2.bin", file_array, 4) < 0){
        printf("ERROR: test_merge_values_into_file_overlapping_middle() failed creating data file.\n");
        return false;
    }

    FILE *data_file = fopen("case2.bin", "rb+");
    if (data_file == NULL){
        printf("ERROR: test_merge_values_into_file_overlapping_middle() failed opening data file.\n");
        return false;
    }

    int result = merge_values_into_file(data_file, merge_array, 2);
    fclose(data_file);
    if (result < 0){
        printf("ERROR: test_merge_values_into_file_overlapping_middle() failed merging data file.\n");
        return false;
    }

    if (!compare_written_to_expected("case2.bin", expected_array, 6)){
        printf("ERROR: test_merge_values_into_file_overlapping_middle() failed --- result doesn't match expected.\n");
        return false;
    }

    return true;
}

bool test_merge_values_into_file_overlapping_end(){
    uint32_t file_array[] = {10, 13, 22, 44, 45, 46};
    uint32_t merge_array[] = {43, 56, 98, 103, 112, 122};
    uint32_t expected_array[] = {10, 13, 22, 43, 44, 45, 46, 56, 98, 103, 112, 122};

    if (write_data_file("case3.bin", file_array, 6) < 0){
        printf("ERROR: test_merge_values_into_file_overlapping_end() failed creating data file.\n");
        return false;
    }

    FILE *data_file = fopen("case3.bin", "rb+");
    if (data_file == NULL){
        printf("ERROR: test_merge_values_into_file_overlapping_end() failed opening data file.\n");
        return false;
    }

    int result = merge_values_into_file(data_file, merge_array, 6);
    fclose(data_file);
    if (result < 0){
        printf("ERROR: test_merge_values_into_file_overlapping_end() failed merging data file.\n");
        return false;
    }

    if (!compare_written_to_expected("case3.bin", expected_array, 12)){
        printf("ERROR: test_merge_values_into_file_overlapping_end() failed -- result doesn't match expected.\n");
        return false;
    }

    return true;
}

bool test_merge_values_into_file_large(){
    uint32_t file_array[] = {2251,
    4209,
    4554,
    7580,
    8276,
    15323,
    26262,
    28318,
    42548,
    44230,
    45563,
    49613,
    50940,
    53749,
    59794,
    64724};
    uint32_t merge_array[] = {1309,
     3097,
     3750,
     7031,
     8545,
     9876,
     13289,
     16804,
     18504,
     19529,
     23769,
     29949,
     34108,
     46628,
     49221,
     56260
    };
    uint32_t expected_array[] = {
            1309,
            2251,
            3097,
            3750,
            4209,
            4554,
            7031,
            7580,
            8276,
            8545,
            9876,
            13289,
            15323,
            16804,
            18504,
            19529,
            23769,
            26262,
            28318,
            29949,
            34108,
            42548,
            44230,
            45563,
            46628,
            49221,
            49613,
            50940,
            53749,
            56260,
            59794,
            64724
    };

    if (write_data_file("case4.bin", file_array, 16) < 0){
        printf("ERROR: test_merge_values_into_file_large() failed creating data file.\n");
        return false;
    }

    FILE *data_file = fopen("case4.bin", "rb+");
    if (data_file == NULL){
        printf("ERROR: test_merge_values_into_file_large() failed opening data file.\n");
        return false;
    }

    int result = merge_values_into_file(data_file, merge_array, 16);
    fclose(data_file);
    if (result < 0){
        printf("ERROR: test_merge_values_into_file_large() failed merging data file.\n");
        return false;
    }

    if (!compare_written_to_expected("case4.bin", expected_array, 32)){
        printf("ERROR: test_merge_values_into_file_large() failed -- result doesn't match expected.\n");
        return false;
    }

    return true;
}


bool test_merge_values_into_file_nonoverlapping(){
    uint32_t file_array[] = {56, 58, 60, 64};
    uint32_t merge_array[] = {80, 83, 87, 95};
    uint32_t expected_array[] = {56, 58, 60, 64, 80, 83, 87, 95};

    if (write_data_file("case5.bin", file_array, 4) < 0){
        printf("ERROR: test_merge_values_into_file_nonoverlapping() failed creating data file.\n");
        return false;
    }

    FILE *data_file = fopen("case5.bin", "rb+");
    if (data_file == NULL){
        printf("ERROR: test_merge_values_into_file_nonoverlapping() failed opening data file.\n");
        return false;
    }

    int result = merge_values_into_file(data_file, merge_array, 4);
    fclose(data_file);
    if (result < 0){
        printf("ERROR: test_merge_values_into_file_nonoverlapping() failed merging data file.\n");
        return false;
    }

    if (!compare_written_to_expected("case5.bin", expected_array, 8)){
        printf("ERROR: test_merge_values_into_file_nonoverlapping() -- result doesn't match expected.\n");
        return false;
    }

    return true;
}
