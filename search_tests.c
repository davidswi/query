//
// Created by dave on 9/9/17.
//

#include <stdio.h>
#include <netinet/in.h>
#include "search_tests.h"
#include "search.h"

#define TEST_DATA_FILE "test.dat"

uint32_t positive_array[] = {1, 2, 4, 7, 10};
uint32_t negative_array[] = {1, 2, 4, 10};
uint32_t test_file_array[] = {2, 6, 11, 46, 96, 97, 127, 128};

FILE *data_file;

void setup_data_file(){
    data_file = fopen(TEST_DATA_FILE, "wb");
    if (data_file != NULL){
        for (int ind = 0; ind < 8; ind++){
            uint32_t big_endian_value = htonl(test_file_array[ind]);
            if (fwrite(&big_endian_value, sizeof(uint32_t), 1, data_file ) < 1){
                break;
            }
        }

        fclose(data_file);
    }
}

int data_access_positive_array(long index, uint32_t *value){
    if (index >= 0 && index <= 4){
        *value = positive_array[index];
        return 0;
    }
    else{
        return -1;
    }
}

int data_access_negative_array(long index, uint32_t *value){
    if (index >= 0 && index <= 3){
        *value = negative_array[index];
        return 0;
    }
    else{
        return -1;
    }
}

int data_access_at_file_position(long position, uint32_t *value){
    long file_position = position * sizeof(uint32_t);
    uint32_t value_at_pos;

    int ret = fseek(data_file, file_position, SEEK_SET);
    if (ret < 0){
        return -1;
    }

    if (fread(&value_at_pos, sizeof(uint32_t), 1, data_file) < 1){
        return -1;
    }

    *value = ntohl(value_at_pos);
    return 0;
}

bool test_search_array_found(){
    long start_ind = 0;
    long end_ind = 4;
    bool result = search(7, &start_ind, &end_ind, data_access_positive_array);

    if (!result || start_ind != 3){
        printf("ERROR: test_search_array_found() failed\n");
    }

    return result;
}

bool test_search_array_not_found(){
    long start_ind = 0;
    long end_ind = 3;
    bool result = search(7, &start_ind, &end_ind, data_access_negative_array);

    if (result || end_ind != 2){
        printf("ERROR: test_search_array_not_found() failed\n");
    }

    return !result;
}

bool test_search_file_found(){
    setup_data_file();
    long start_ind = 0;
    long end_ind = 7;
    bool result = false;

    data_file = fopen(TEST_DATA_FILE, "rb");
    if (data_file != NULL){
        result = search(96, &start_ind, &end_ind, data_access_at_file_position);
        if (!result || start_ind != 4){
            printf("ERROR: test_search_file_found() failed\n");
        }

        fclose(data_file);
    }

    return result;
}

bool test_search_file_not_found(){
    long start_ind = 0;
    long end_ind = 7;
    bool result = false;

    data_file = fopen(TEST_DATA_FILE, "rb");
    if (data_file != NULL){
        result = search(22, &start_ind, &end_ind, data_access_at_file_position);
        if (result || end_ind != 2){
            printf("ERROR: test_search_file_not_found() failed\n");
        }

        fclose(data_file);
    }

    return !result;
}
