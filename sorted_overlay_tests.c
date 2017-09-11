//
// Created by dave on 9/10/17.
//

#include <stdio.h>
#include "sorted_overlay_tests.h"
#include "sorted_overlay.h"

bool test_sorted_overlay_find_nearest_in_memory(){
    uint32_t input_values[] = {128, 97, 96, 46, 11, 2, 6, 127};
    uint32_t test_cases[] = {0, 255, 90, 4, 16};
    uint32_t expected_results[] = {2, 128, 96, 2, 11};

    if (sorted_overlay_init(8) < 0){
        printf("ERROR: test_sorted_overlay_find_nearest_in_memory() failed initializing.\n");
        return false;
    }

    for (int i = 0; i < 8; i++){
        if (sorted_overlay_add(input_values[i]) < 0){
            printf("ERROR: test_sorted_overlay_find_nearest_in_memory() failed adding values.\n");
            return false;
        }
    }

    for (int j = 0; j < 5; j++){
        uint32_t nearest = sorted_overlay_find_nearest(test_cases[j]);
        if (nearest != expected_results[j]){

        }
    }

    return true;
}

bool test_sorted_overlay_find_nearest_in_file(){

    return false;
}
