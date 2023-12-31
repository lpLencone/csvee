#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "common.h"

char *read_to_cstr(const char *filename)
{
    char *contents = NULL;
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        goto finally;
    }
    
    char buf[1024];
    size_t bytes = 0;
    size_t total = 0;
    while ((bytes = fread(buf, 1, 1024, fp)) != 0) {
        total += bytes;
        contents = realloc(contents, total + 1);
        if (contents == NULL) {
            goto finally;
        }
        memcpy(contents + (total - bytes), buf, bytes);
    }

    if (ferror(fp)) {
        free(contents);
        contents = NULL;
    }

finally:
    if (fp != NULL) {
        fclose(fp);
    }
    contents[total] = '\0';
    return contents;
}

size_t bit_min_amount(uint32_t bits)
{
    size_t count;
    for (count = 0; (bits >> count) > 0; count++)
        continue;
    return count;
}

size_t bit_count_high(uint32_t bits)
{
    size_t count = 0;
    for (size_t i = 0; (bits >> i) > 0; i++) {
        if ((bits >> i) & 0b1) {
            count++;
        }
    }
    return count;
}

uint32_t powu32(uint32_t a, uint32_t b)
{
    uint32_t res = 1;
    for (uint32_t i = 0; i < b; i++) {
        res *= a;
    }
    return res;
}
