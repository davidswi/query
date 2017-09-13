//
// Created by dave on 9/7/17.
//

#include <stdio.h>
#include <stdlib.h>
#include "sorted_overlay.h"
#include "data_file_utils.h"
#include "search.h"

#define MAX_CAPACITY                1 << 20

#define UNIT_TESTS

#ifdef UNIT_TESTS
#define SORTED_OVERLAY_BYTES        64
#else
#define SORTED_OVERLAY_BYTES        32768
#endif // UNIT_TESTS

#define SORTED_OVERLAY_CAPACITY     SORTED_OVERLAY_BYTES / sizeof(uint32_t)
#define SORTED_OVERLAY_DATA_FILE    "sorted_overlay.dat"
#define SORTED_DATA_FILE            "sorted.dat"

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

size_t total_values;
size_t max_values;

// Pointer to dynamically allocated overlay range LUT
range_t *overlay_lookup_table;
// Upper limit on overlay LUT
uint8_t total_overlays;

FILE *sorted_overlay_file = NULL;
FILE *sorted_file = NULL;

// Helper functions to create fully sorted values file
int create_sorted_file_index();
int create_sorted_values_file();

// Query helper functions
uint32_t closest_value(uint32_t target, range_t *range);
uint8_t find_sorted_overlay_index(uint32_t target);

long sorted_file_position(uint8_t overlay_index, uint32_t offset_index){
    // Start of overlay in terms of values is zero-based index * SORTED_OVERLAY_CAPACITY
    long value_index = overlay_index * SORTED_OVERLAY_CAPACITY;
    // Now add offset
    value_index += offset_index;

    // Multiply by size of uint32_t to get file position in bytes
    return value_index * sizeof(uint32_t);
}

void sorted_overlay_dump_file(char *filename){
    FILE *file = fopen(filename, "rb");
    if (file == NULL){
        printf("ERROR: Failed to open %s\n", filename);
    }

    for (int i = 0; i < total_values; i++){
        uint32_t value;
        if (fread(&value, sizeof(uint32_t), 1, file) < 1){
            printf("ERROR: Failed to read value from file!\n");
            break;
        }
        printf("value (%d) = %u\n", i, value);
    }

    fclose(file);
}

void sorted_overlay_dump_lookup_table(){

}

void sorted_overlay_dump(){
    sorted_overlay_dump_file(SORTED_OVERLAY_DATA_FILE);
    sorted_overlay_dump_file(SORTED_DATA_FILE);
    sorted_overlay_dump_lookup_table();
}

int create_sorted_file_index(){
    long file_position;
    uint8_t overlay_ind;
    range_t overlay_range;

    for (overlay_ind = 0; overlay_ind < total_overlays; overlay_ind++){
        // Read the minimum value for the current overlay from the sorted file
        // minimum values occur at 0, SORTED_OVERLAY_CAPACITY * 4, etc.
        file_position = sorted_file_position(overlay_ind, 0);
        if (fseek(sorted_file, file_position, SEEK_SET) < 0){
            return -1;
        }

        if (fread(&(overlay_range.min_value), sizeof(uint32_t), 1, sorted_file) < 1){
            return -1;
        }

        uint32_t last_value_offset;
        if (overlay_ind == total_overlays - 1){
            last_value_offset = (total_values % SORTED_OVERLAY_CAPACITY) - 1;
        }
        else{
            last_value_offset = SORTED_OVERLAY_CAPACITY - 1;
        }
        file_position = sorted_file_position(overlay_ind, last_value_offset);
        if (fseek(sorted_file, file_position, SEEK_SET) < 0){
            return -1;
        }

        if (fread(&(overlay_range.max_value), sizeof(uint32_t), 1, sorted_file) < 1){
            return -1;
        }

        overlay_lookup_table[overlay_ind].min_value = overlay_range.min_value;
        overlay_lookup_table[overlay_ind].max_value = overlay_range.max_value;
    }

    return 0;
}

int create_sorted_values_file(){
    int ret = 0;

    sorted_overlay_file = fopen(SORTED_OVERLAY_DATA_FILE, "rb");
    if (sorted_overlay_file == NULL){
        return -1;
    }

    sorted_file = fopen(SORTED_DATA_FILE, "wb");
    if (sorted_file == NULL){
        return  -1;
    }

    for (uint8_t overlay_ind = 0; overlay_ind < total_overlays; overlay_ind++){
        size_t overlay_length;
        div_t overlay_div = div(total_values, SORTED_OVERLAY_CAPACITY);

        if (overlay_ind < total_overlays - 1){
            if (overlay_div.rem == 0) {
                overlay_length = SORTED_OVERLAY_CAPACITY;
            }
            else{
                overlay_length = overlay_div.rem;
            }
        }
        else{
            overlay_length = SORTED_OVERLAY_CAPACITY;
        }

        if (fread(in_memory_overlay, sizeof(uint32_t), overlay_length, sorted_overlay_file) != overlay_length){
            ret = -1;
            goto done;
        }

        if (overlay_ind == 0){
            // Write the first overlay to the sorted values file
            if (fwrite(in_memory_overlay, sizeof(uint32_t), overlay_length, sorted_file) != overlay_length){
                ret = -1;
                goto done;
            }
        }
        else{
            sorted_file = freopen(SORTED_DATA_FILE, "rb+", sorted_file);
            if (sorted_file == NULL){
                return -1;
            }

            if (merge_values_into_file(sorted_file, in_memory_overlay, overlay_length) < 0){
                ret = -1;
                goto done;
            }
        }
    }

    // Now build the sorted file index
    ret = create_sorted_file_index();

done:
    fclose(sorted_file);
    sorted_file = NULL;

    return ret;
}

uint32_t closest_value(uint32_t target, range_t *range){
    int dist_to_lower = abs(target - range->min_value);
    int dist_to_upper = abs(target - range->max_value);

    if (dist_to_lower <= dist_to_upper){
        return range->min_value;
    }
    else{
        return range->max_value;
    }
}

int sorted_overlay_init(size_t capacity){
    if (capacity > MAX_CAPACITY){
        return -1;
    }

    // Compute the number of overlays in the file
    if (capacity <= SORTED_OVERLAY_CAPACITY){
        total_overlays = 1;
    }
    else{
        div_t overlay_div = div(capacity, SORTED_OVERLAY_CAPACITY);
        total_overlays = overlay_div.quot;
        if (overlay_div.rem > 0){
            total_overlays++;
        }

        // Attempt to allocate the LUT
        overlay_lookup_table = (range_t *)malloc(total_overlays * sizeof(range_t));
        if (overlay_lookup_table == NULL){
            return -1;
        }

        // Create the empty sorted overlay file
        sorted_overlay_file = fopen(SORTED_OVERLAY_DATA_FILE, "wb");
        if (sorted_overlay_file == NULL){
            return -1;
        }
    }

    in_memory_length = 0;
    total_values = 0;
    max_values = capacity;

    return 0;
}

void sorted_overlay_deinit(){
    if (total_overlays > 1){
        free(overlay_lookup_table);
    }
}

int sorted_overlay_add(uint32_t value){
    int ret = 0;

    in_memory_overlay[in_memory_length] = value;
    in_memory_length++;
    total_values++;

    bool overlay_complete = (in_memory_length == SORTED_OVERLAY_CAPACITY || total_values == max_values);

    if (overlay_complete && total_overlays > 1){
        ret = sort_and_write_values(sorted_overlay_file, in_memory_overlay, in_memory_length);
        in_memory_length = 0;
        if (total_values == max_values){
            fclose(sorted_overlay_file);
            ret = create_sorted_values_file();
        }
    }
    else{
        sort_values_in_memory(in_memory_overlay, in_memory_length);
    }

    return ret;
}

uint8_t find_sorted_overlay_index(uint32_t target){
    // Do a binary search of the overlay index to find the overlay
    // that contains the closest value to the target
    uint8_t lower_ind = 0;
    uint8_t upper_ind = total_overlays - 1;
    uint8_t midpoint;

    while (upper_ind >= lower_ind){
        midpoint = lower_ind + ((upper_ind - lower_ind) >> 1);
        if (target >= overlay_lookup_table[midpoint].min_value &&
                target <= overlay_lookup_table[midpoint].max_value){
            break;
        }
        else{
            if (target < overlay_lookup_table[midpoint].min_value){
                upper_ind = midpoint - 1;
            }
            else{
                lower_ind = midpoint + 1;
            }
        }
    }

    return midpoint;
}

int data_access_in_memory_overlay(long index, uint32_t *value){
    if (index >= 0 && index <= in_memory_length){
        *value = in_memory_overlay[index];
        return 0;
    }

    return -1;
}

bool is_valid_index(long index){
    return (index >= 0 && index < in_memory_length);
}

uint32_t sorted_overlay_find_nearest(uint32_t value){
    uint32_t nearest;
    long start_ind = 0;
    long end_ind;

    if (total_overlays > 1){
        // Find and load the overlay bracketing the value
        uint8_t containing_overlay_ind = find_sorted_overlay_index(value);
        long file_pos = sorted_file_position(containing_overlay_ind, 0);
        if (containing_overlay_ind == total_overlays - 1){
            in_memory_length = total_values % SORTED_OVERLAY_CAPACITY;
        }
        else{
            in_memory_length = SORTED_OVERLAY_CAPACITY;
        }

        if (sorted_file == NULL){
            sorted_file = fopen(SORTED_DATA_FILE, "rb");
            if (sorted_file == NULL){
                return UINT32_MAX;
            }
        }


        if (fread(in_memory_overlay, in_memory_length, sizeof(uint32_t), sorted_file) != in_memory_length){
            return UINT32_MAX;
        }
    }

    end_ind = in_memory_length - 1;
    if (search(value, &start_ind, &end_ind, data_access_in_memory_overlay)){
        nearest = value;
    }
    else{
        if (is_valid_index(start_ind) && is_valid_index(end_ind)){
            range_t nearest_in_search = {in_memory_overlay[end_ind], in_memory_overlay[start_ind]};
            nearest = closest_value(value, &nearest_in_search);
        }
        else{
            if (is_valid_index(start_ind)){
                nearest = in_memory_overlay[start_ind];
            }
            else{
                nearest = in_memory_overlay[end_ind];
            }
        }
    }

    return nearest;
}
