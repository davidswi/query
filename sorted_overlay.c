//
// Created by dave on 9/7/17.
//

#include <stdio.h>
#include "sorted_overlay.h"
#include "data_file_utils.h"
#include "search.h"

#define ABS(X) (X) < 0 ? -(X) : (X)

#define SORTED_OVERLAY_BYTES        32768
#define SORTED_OVERLAY_CAPACITY     SORTED_OVERLAY_BYTES / sizeof(uint32_t)
#define SORTED_OVERLAY_DATA_FILE    "sorted_overlay.dat"
#define SORTED_DATA_FILE            "sorted.dat"

typedef struct{
    uint8_t overlay_num;
    range_t range;
} overlay_index_t;

// The sorted_overlay file format consists of one or more persisted overlays containing up
// to SORTED_OVERLAY_CAPACITY integers in sorted order. The sorted_overlay module accesses
// the file through an overlay index it allocates on the heap at initialization time.
// Once the in-memory overlay array is filled up or the last input value is read, the overlay
// is sorted and written to the overlay file. The index is updated with the min and max values
// from the sorted overlay and the in-memory array index is reset for the next chunk of values,
// or the file is closed, if all the values have been read. Once all values have been read,
// the index is sorted such that overlapping sorted segment indices come before non-overlapping
// indices. Then the index is used to create a new fully sorted data file by merging overlapping
// overlays into the new file and appending non-overlapping ones. As the fully sorted segments
// are completed, the overlay index is updated with the correct [min, max] ranges for the
// overlays so that queries for the nearest value can be executed by consulting the index to
// find the containing overlay in log(num_overlays) time, loading the overlay into memory and
// finding the nearest value by binary search of the in-memory overlay.

uint32_t in_memory_overlay[SORTED_OVERLAY_CAPACITY];
uint16_t in_memory_length;

// Pointer to dynamically allocated overlay index array
overlay_index_t *overlay_index = NULL;
// Index of current overlay in index array
uint8_t curr_overlay;
// Upper limit on overlay index array
uint8_t total_overlays;

// Helper functions to create fully sorted values file
void sort_overlay_index();
int create_sorted_values_file();

// Query helper functions
uint32_t closest_value(uint32_t target, range_t *range);
uint8_t find_sorted_overlay_index(uint32_t target);

void sort_overlay_index(){

}

int create_sorted_values_file(){
    return 0;
}

uint32_t closest_value(uint32_t target, range_t *range){
    int dist_to_lower = ABS(target - range->min_value);
    int dist_to_upper = ABS(target - range->max_value);

    if (dist_to_lower <= dist_to_upper){
        return range->min_value;
    }
    else{
        return range->max_value;
    }
}

int sorted_overlay_init(size_t capacity){
    return 0;
}

int sorted_overlay_add(uint32_t value){
    return 0;
}

uint32_t sorted_overlay_find_nearest(uint32_t value){
    return 0;
}
