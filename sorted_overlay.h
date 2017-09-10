//
// Created by dave on 9/7/17.
//

#ifndef INTEGER_QUERY_SORTED_OVERLAY_H
#define INTEGER_QUERY_SORTED_OVERLAY_H

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

int sorted_overlay_init(size_t capacity);
int sorted_overlay_add(uint32_t value);
uint32_t sorted_overlay_find_nearest(uint32_t value);

#endif //INTEGER_QUERY_SORTED_OVERLAY_H
