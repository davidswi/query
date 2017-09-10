//
// Created by dave on 9/9/17.
//

#include "search.h"

bool search(uint32_t target, long *start_ind, long *end_ind, data_access_fn_t access_fn){
    long lower_ind = *start_ind;
    long upper_ind = *end_ind;
    bool found = false;

    while (upper_ind >= lower_ind){
        long midpoint_ind = lower_ind + ((upper_ind - lower_ind) >> 1);

        uint32_t value_at_midpoint;
        if (access_fn(midpoint_ind, &value_at_midpoint) < 0){
            break;
        }

        if (value_at_midpoint == target){
            // Return found index on lower index
            lower_ind = midpoint_ind;
            found = true;
            break;
        }
        else {
            if (value_at_midpoint > target){
                upper_ind = midpoint_ind - 1;
            }
            else{
                lower_ind = midpoint_ind + 1;
            }
        }
    }

    *start_ind = lower_ind;
    *end_ind = upper_ind;

    return found;
}
