#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <err.h>
#include <sysexits.h>
#include <inttypes.h>
#include <string.h>

#include "bitmap.h"

struct bit_map *bit_map_init(uint64_t bits)
{
    struct bit_map *b = (struct bit_map *) malloc(sizeof(struct bit_map));
    if (b == NULL)
        err(1, "calloc error in bit_map_init().\n");

    b->num_bits = bits;
    b->num_ints = (bits + 32 - 1) / 32;
    b->bm = (uint32_t *) calloc(b->num_ints, sizeof(uint32_t));
    if (b->bm == NULL)
        err(1, "calloc error in bit_map_init().\n");

    return b;
}

struct bit_map *bit_map_load(char *file_name)
{
    FILE *f = fopen(file_name, "rb");
    if (f == NULL)
        err(1, "Could not open file '%s'.", file_name);

    struct bit_map *b = (struct bit_map *) malloc(sizeof(struct bit_map));
    if (b == NULL)
        err(1, "calloc error in bit_map_load().\n");

    size_t fr = fread(&(b->num_bits), sizeof(uint64_t), 1, f);
    check_file_read(file_name, f, 1, fr);
    fr = fread(&(b->num_ints), sizeof(uint32_t), 1, f);
    check_file_read(file_name, f, 1, fr);
    b->bm = (uint32_t *) calloc(b->num_ints, sizeof(uint32_t));
    if (b->bm == NULL)
        err(1, "calloc error in bit_map_load().\n");

    fr = fread(b->bm, sizeof(uint32_t), b->num_ints, f);
    check_file_read(file_name, f, b->num_ints, fr);

    fclose(f);

    return b;
}

void bit_map_store(struct bit_map *b, char *file_name)
{
    FILE *f = fopen(file_name, "wb");
    if (f == NULL)
        err(1, "Could not open file '%s'.", file_name);

    if (fwrite(&(b->num_bits), sizeof(uint64_t), 1, f) != 1)
        err(EX_IOERR,
            "Error writing number of bits to '%s'.",
            file_name);

    if (fwrite(&(b->num_ints), sizeof(uint32_t), 1, f) != 1)
        err(EX_IOERR,
            "Error writing number of ints to '%s'.",
            file_name);

    if (fwrite(b->bm, sizeof(uint32_t), b->num_ints, f) != b->num_ints)
        err(EX_IOERR,
            "Error writing number bitmap to '%s'.",
            file_name);
    fclose(f);
}

void bit_map_destroy(struct bit_map **b)
{
    free((*b)->bm);
    free(*b);
    *b = NULL;
}

void bit_map_set(struct bit_map *b, uint64_t i)
{
    while (i >= b->num_bits) {
        uint64_t new_bits = (b->num_bits)*2;
        uint32_t new_ints = (new_bits + 32 - 1) / 32;
        uint32_t *new_bm = (uint32_t *)calloc(new_ints, sizeof(uint32_t));
        if (new_bm== NULL)
            err(1, "calloc error in bit_map_set().\n");

        memcpy(new_bm, b->bm, (b->num_ints)*sizeof(uint32_t));

        free(b->bm);
        b->num_bits = new_bits;
        b->num_ints = new_ints;
        b->bm = new_bm;
    }


    b->bm[i/32] |= 1 << (31 - (i%32));
}

uint32_t bit_map_get(struct bit_map *b, uint64_t q)
{
    if (q > b->num_bits)
        return 0;

    return (( b->bm[q/32]) >> (31 - (q%32)) & 1);
}

void check_file_read(char *file_name, FILE *fp, size_t exp, size_t obs)
{
    if (exp != obs) {
        if (feof(fp))
            errx(EX_IOERR,
                 "Error reading file \"%s\": End of file",
                 file_name);
        err(EX_IOERR, "Error reading file \"%s\"", file_name);
    }
}
