#ifndef __BITMAP_H__
#define __BITMAP_H__

#include <stdint.h>

// BITMAP
struct bit_map
{
    uint64_t num_bits;
    uint32_t num_ints;
    uint32_t *bm;
};

struct bit_map *bit_map_init(uint64_t bits);
struct bit_map *bit_map_load(char *file_name);
void bit_map_store(struct bit_map *b, char *file_name);
void bit_map_destroy(struct bit_map **b);
void bit_map_set(struct bit_map *b, uint64_t i);
uint32_t bit_map_get(struct bit_map *b, uint64_t q);

void check_file_read(char *file_name, FILE *fp, size_t exp, size_t obs);

#endif
