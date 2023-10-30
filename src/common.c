#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "common.h"

char *read_to_string(const char *filename)
{
    FILE *fp = fopen(filename, "r");
    panic_if(fp == NULL);
    
    char *contents = NULL;
    char buf[1024];
    size_t bytes = 0;
    size_t total = 0;
    while ((bytes = fread(buf, 1, 1024, fp)) != 0) {
        total += bytes;
        contents = realloc(contents, total);
        memcpy(contents + (total - bytes), buf, bytes);
    }
    return contents;
}

