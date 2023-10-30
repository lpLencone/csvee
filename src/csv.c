#include "csv.h"

#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef da(const char *) CSV_Column;

struct {
    enum {
        CSV_Status_Ok = 0,
        CSV_Status_Logic_Err,
        CSV_Status_Libc_Err,
    } c;
    const char *msg;
    size_t line;
} csv_status;

#define csv_status_set(code, message) \
    do { \
        csv_status.c = code; \
        csv_status.msg = message; \
        csv_status.line = __LINE__; \
    } while (0)

CSV *csv_create(const char *filename, ...)
{
    if (access(filename, F_OK) == 0) {
        csv_status_set(CSV_Status_Logic_Err, "Provided filename is unavailable");
        return NULL;
    }

    CSV *csv = malloc(sizeof(CSV));
    if (csv == NULL) {
        csv_status_set(CSV_Status_Libc_Err, strerror(errno));
        return NULL;
    }
    csv->filename = filename;
    csv->row_count = 0;

    size_t count = 0;
    va_list ap;
    va_start(ap, filename);
    const char *column_id;
    while ((column_id = va_arg(ap, const char *)) != NULL) {
        struct _CSV_Column csv_col = {0};
        csv_col.id = column_id;
        da_append(&csv->columns, &csv_col); // gotta remove this assertion deal
        count++;
    }
    va_end(ap);

    if (count == 0) {
        csv_status_set(CSV_Status_Logic_Err, "There must be at least one column");
    }

    if (csv_status.c != CSV_Status_Ok) {
        if (csv) {
            free(csv);
            return NULL;
        }
    }
    return csv;
}

int csv_insert(CSV *csv, ...)
{
    size_t count = 0;
    const char *value = NULL;

    va_list ap;
    va_start(ap, csv);
    while (count < csv->columns.count) {
        value = va_arg(ap, const char *);
        if (count < csv->columns.count) {
            struct _CSV_Column *csv_col = &csv->columns.data[count];
            da_append(csv_col, &value);
        }
        
        count++;
    }
    va_end(ap);

    csv->row_count++;

    if (count != csv->columns.count) {
        csv_status_set(CSV_Status_Logic_Err, "Number of items mismatches number of columns");
        return 1;
    }

    return 0;
}

void csv_display(CSV *csv)
{
    for (size_t i = 0; i < csv->columns.count; i++) {
        struct _CSV_Column csv_col = csv->columns.data[i];
        printf("%s,", csv_col.id);
    }
    putchar('\n');

    for (size_t i = 0; i < csv->row_count; i++) {
        for (size_t j = 0; j < csv->columns.count; j++) {
            struct _CSV_Column csv_col = csv->columns.data[j];
            printf("%s,", csv_col.data[i]);
        }
        putchar('\n');
    }
}

void csv_destroy(CSV *csv)
{
    da_end(&csv->columns);
    free(csv);
}

const char *csv_strerror(void)
{
    switch (csv_status.c) {
        case CSV_Status_Ok:
            return "No error occurred";
        case CSV_Status_Libc_Err:
            return strerror(errno);
        case CSV_Status_Logic_Err:
            return csv_status.msg;
        default:
            assert(false && "unreachable");
    }
}
