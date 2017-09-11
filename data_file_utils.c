//
// Created by dave on 9/9/17.
//
#include <stdlib.h>
#include "data_file_utils.h"
#include "search.h"

#define MIN(A, B) (A) < (B) ? (A) : (B)

FILE *data_file;

// quicksort comparison function
int compare_values(const void *value1, const void *value2);

// search data access function
int value_at_file_position(long position, uint32_t *value);

int sort_and_write_values(FILE *file, uint32_t *values, size_t num_values){
    qsort(values, num_values, sizeof(uint32_t), compare_values);
    if (fwrite(values, sizeof(uint32_t), num_values, file) < num_values){
        return -1;
    }

    return 0;
}

void sort_values_in_memory(uint32_t *values, size_t num_values){
    qsort(values, num_values, sizeof(uint32_t), compare_values);
}

int compare_values(const void *value1, const void *value2){
    uint32_t uint_value1 = *((uint32_t *)value1);
    uint32_t uint_value2 = *((uint32_t *)value2);
    int ret = 1;

    if (uint_value1 == uint_value2){
        ret = 0;
    }
    else{
        if (uint_value1 < uint_value2){
            ret = -1;
        }
    }

    return ret;
}

int value_at_file_index(long index, uint32_t *value){
    long file_position = index * sizeof(uint32_t);
    uint32_t value_at_pos;

    int ret = fseek(data_file, file_position, SEEK_SET);
    if (ret < 0){
        return -1;
    }

    if (fread(&value_at_pos, sizeof(uint32_t), 1, data_file) < 1){
        return -1;
    }

    *value = value_at_pos;
    return 0;
}

long find_file_insertion_point(FILE *file, uint32_t value, long start_ind, long end_ind){
    long start_position = start_ind;
    long end_position = end_ind;
    data_file = file;

    bool search_result = search(value, &start_position, &end_position, value_at_file_index);

    return start_position;
}

int get_num_values_in_file(FILE *file){
    if (fseek(file, 0, SEEK_END) < 0){
        return -1;
    }

    int num_values = ftell(file);
    if (num_values > 0){
        num_values /= sizeof(uint32_t);
    }

    rewind(file);

    return num_values;
}


int insert_value_in_file(FILE *file, long insert_ind, uint32_t value, uint32_t *overwritten_value){
    uint32_t value_at_pos;

    // Read and save current value
    int ret = fseek(file, insert_ind * sizeof(uint32_t), SEEK_SET);
    if (ret < 0){
        return -1;
    }

    if (fread(&value_at_pos, sizeof(uint32_t), 1, file) < 1){
        return -1;
    }

    *overwritten_value = value_at_pos;

    // Back up to write replacement value
    ret = fseek(file, -sizeof(uint32_t), SEEK_CUR);
    if (ret < 0){
        return -1;
    }

    if (fwrite(&value, sizeof(uint32_t), 1, file) < 1){
        return -1;
    }

    printf("insert_value_in_file(): Wrote %u and saved %u\n", value, value_at_pos);

    return 0;
}

bool is_overlapping_merge(range_t *file_range, range_t *values_range){
    bool both_inside = (values_range->min_value >= file_range->min_value &&
            values_range->max_value <= file_range->max_value);
    bool right_inside = (values_range->max_value <= file_range->min_value);
    bool left_inside = (values_range->max_value <= file_range->max_value);
    bool right_outside = (values_range->min_value <= file_range->max_value);

    return (both_inside || right_inside || left_inside || right_outside);
}

int merge_values_into_file(FILE *file, uint32_t *values, size_t num_values){
    size_t num_file_values = get_num_values_in_file(file);
    if (num_file_values == 0){
        return -1;
    }

    size_t new_file_size = num_file_values + num_values;
    range_t value_range = {values[0], values[num_values -1]};
    range_t file_range;

    // Get the min and max values in the file, which is assumed to be sorted!
    if (fread(&(file_range.min_value), sizeof(uint32_t), 1, file) < 1){
        return -1;
    }

    if (fseek(file, -sizeof(uint32_t), SEEK_END) < 0){
        return -1;
    }

    if (fread(&(file_range.max_value), sizeof(uint32_t), 1, file) < 1){
        return -1;
    }

    rewind(file);

    if (is_overlapping_merge(&file_range, &value_range)){
        // The values to be merged overlap with the contents of the file
        // Find the insertion point in the file for the minimum of the
        // values
        long insert_index = find_file_insertion_point(file, values[0], 0, num_file_values - 1);

        // Insert the values one at a time, saving the overwritten values in the
        // values buffer and repeat until all values have been written, using the
        // values buffer as a circular buffer
        int copy_ind = 0;

        while (insert_index < num_file_values){
            int num_overlapping = num_file_values - insert_index;
            int num_to_insert = MIN(num_overlapping, num_values);
            int save_ind = 0;

            for (int i = 0; i < num_to_insert; i++){
                uint32_t value_copy = values[copy_ind];
                if (insert_value_in_file(file, insert_index++, value_copy, values + save_ind) < 0){
                    return -1;
                }

                if (i == num_values - 1 || values[i] > values[i + 1]) {
                    copy_ind = (copy_ind + 1) % num_values;
                    save_ind++;
                }
                else{
                    printf("merge_values_into_file() -- not advancing index because %u <= %u\n",
                           values[i], values[i+1]);
                }
            }
        }

        // Append the remainder to the file
        while (insert_index < new_file_size){
            if (fwrite(values + copy_ind, sizeof(uint32_t), 1, file) < 1){
                return -1;
            }
            printf("merge_values_into_file(): Wrote %u to end of file\n", values[copy_ind]);
            copy_ind = (copy_ind + 1) % num_values;
            insert_index++;
        }
    }
    else{
        // No overlap, just append the values to the end of the file
        if (fseek(file, 0, SEEK_END) < 0){
            return -1;
        }

        if (fwrite(values, sizeof(uint32_t), num_values, file) < 0){
            return -1;
        }
    }

    return 0;
}