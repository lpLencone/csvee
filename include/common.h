#ifndef CSV_COMMON_H_
#define CSV_COMMON_H_

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define eprint(fmt, ...) fprintf(stderr, fmt __VA_OPT__(,) __VA_ARGS__)
#define panic() \
    do { \
        eprint("%s:%d in %s: %s\n", __FILE__, __LINE__, __func__, strerror(errno)); \
        exit(1); \
    } while (0)
#define panic_if(cond) do { if (cond) panic(); } while (0)

char *read_to_string(const char *filename);

#ifndef DA_INIT_CAPACITY
#  define DA_INIT_CAPACITY  8
#endif // DA_INIT_CAPACITY

#define TYPESIZE(da) sizeof(*(da)->data)

#define da(t) \
    struct { \
        size_t count; \
        size_t capacity; \
        t *data; \
    } 

#define da_insert_n(da, d, n, at) \
    do { \
        assert((da) != NULL && d != NULL); \
        assert(at <= (da)->count); \
        \
        if ((da)->capacity == 0) { \
            (da)->capacity = DA_INIT_CAPACITY; \
            (da)->data = calloc(DA_INIT_CAPACITY, TYPESIZE(da)); \
        } \
        \
        while ((da)->capacity < (da)->count + n) { \
            (da)->capacity *= 2; \
            (da)->data = realloc((da)->data, (da)->capacity * TYPESIZE(da)); \
            assert((da)->data != NULL); \
        } \
        \
        memmove( \
            (da)->data + (at + n), \
            (da)->data + at, \
            ((da)->count - at) * TYPESIZE(da) \
        ); \
        memcpy((da)->data + at, d, (n * TYPESIZE(da))); \
        \
        (da)->count += n; \
    } while (0)

#define da_insert(da, d, at) da_insert_n(da, d, 1, at)
#define da_append_n(da, d, n) da_insert_n(da, d, n, (da)->count)
#define da_append(da, d) da_append_n(da, d, 1)

#define da_end(da) \
    do { \
        assert(da != NULL && (da)->data != NULL); \
        free((da)->data); \
        (da)->data = NULL; \
        (da)->count = 0; \
        (da)->capacity = 0; \
    } while (0)

#endif // CSV_COMMON_H_
