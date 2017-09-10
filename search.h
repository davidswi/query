//
// Created by dave on 9/9/17.
//

#ifndef INTEGER_QUERY_SEARCH_H
#define INTEGER_QUERY_SEARCH_H

#include <stdint.h>
#include <stdbool.h>

// Callback function typedef for accessing *sorted* data to be searched.
// If value cannot be accessed, callback must return -1, otherwise
// it must provide the value in the passed in pointer and return 0
typedef int (*data_access_fn_t)(long index, uint32_t *value);

// search returns true if target found, false otherwise. Searching occurs
// within the interval [start_ind, end_ind] until target is found or entire
// search interval has been covered. start_ind and end_ind are updated with
// the final search position. The data access callback must return the
// data value at the index specified from a sorted data set.
bool search(uint32_t target, long *start_ind, long *end_ind, data_access_fn_t access_fn);

#endif //INTEGER_QUERY_SEARCH_H
