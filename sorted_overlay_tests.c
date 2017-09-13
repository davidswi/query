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
            printf("ERROR: test_sorted_overlay_find_nearest_in_memory() failed -- expected %u, got %u\n",
                   expected_results[j], nearest);
            return false;
        }
    }

    return true;
}

bool test_sorted_overlay_find_nearest_in_file(){
    uint32_t input_values[] = {44230,
            15323,
            28318,
            50940,
            59794,
            49613,
            8276,
            42548,
            4209,
            4554,
            2251,
            53749,
            64724,
            7580,
            45563,
            26262,
            49221,
            13289,
            19529,
            3750,
            16804,
            29949,
            46628,
            18504,
            9876,
            56260,
            8545,
            23769,
            3097,
            34108,
            7031,
            1309};
    uint32_t test_cases[] = {16384, 1280, 10000, 60000, 25867};
    uint32_t expected_results[] = {16804, 1309, 9876, 59794, 26262};

    if (sorted_overlay_init(32) < 0){
        printf("ERROR: test_sorted_overlay_find_nearest_in_file() failed initializing.\n");
        return false;
    }

    for (int i = 0; i < 32; i++){
        if (sorted_overlay_add(input_values[i]) < 0){
            printf("ERROR: test_sorted_overlay_find_nearest_in_file() failed adding values.\n");
            return false;
        }
    }

    for (int j = 0; j < 5; j++){
        uint32_t nearest = sorted_overlay_find_nearest(test_cases[j]);
        if (nearest != expected_results[j]) {
            printf("ERROR: test_sorted_overlay_find_nearest_in_file() failed -- expected %u, got %u\n",
                   expected_results[j], nearest);
            return false;
        }
    }

    return true;
}
