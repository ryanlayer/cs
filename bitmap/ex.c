
#include <stdio.h>
#include <stdlib.h>

#include "bitmap.h"

int main(int argc, char **argv)
{
    uint64_t starting_size = 8;

    struct bit_map *b = bit_map_init(starting_size);

    printf("b[0]: %d\n", bit_map_get(b, 0));

    printf("b[0] = 1\n");
    bit_map_set(b, 0);

    printf("b[0]: %d\n", bit_map_get(b, 0));

    printf("b[1000] = 1\n");
    bit_map_set(b, 1000);

    printf("b[999]: %d\n", bit_map_get(b, 999));
    printf("b[1000]: %d\n", bit_map_get(b, 1000));
    printf("b[9000]: %d\n", bit_map_get(b, 9000));

    bit_map_store(b, "bitmap.data");

    bit_map_destroy(&b);

    struct bit_map *d = bit_map_load("bitmap.data");

    printf("d[0]: %d\n", bit_map_get(d, 0));
    printf("d[999]: %d\n", bit_map_get(d, 999));
    printf("d[1000]: %d\n", bit_map_get(d, 1000));
    printf("d[9000]: %d\n", bit_map_get(d, 9000));

    bit_map_destroy(&d);
}
