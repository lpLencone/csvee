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

#define csv_logic_err(message) csv_status_set(CSV_Status_Logic_Err, message)
#define csv_libc_err() csv_status_set(CSV_Status_Libc_Err, strerror(errno))

CSV *csv_create(size_t column_count, ...)
{
    CSV *csv = malloc(sizeof(CSV));
    if (csv == NULL) {
        csv_libc_err();
        return NULL;
    }
    csv->row_count = 0;

    size_t count = 0;
    va_list ap;
    va_start(ap, column_count);
    while (count < column_count) {
        const char *column_id = va_arg(ap, const char *);
        struct _CSV_Column csv_col = {0};
        csv_col.id = column_id;
        da_append(&csv->columns, &csv_col);
        count++;
    }
    va_end(ap);

    if (csv_status.c != CSV_Status_Ok) {
        if (csv) {
            free(csv);
            return NULL;
        }
    }
    return csv;
}

void csv_destroy(CSV *const csv)
{
    da_end(&csv->columns);
    free(csv);
}

bool csv_append_mask(CSV *const csv, uint32_t mask, ...)
{
    size_t count = 0;

    va_list ap;
    va_start(ap, mask);
    while (count < csv->columns.count) {
        const char *value = NULL;
        struct _CSV_Column *csv_col = &csv->columns.data[count];
        if ((mask >> count) & 1) {
            value = va_arg(ap, const char *);
        }
        da_append(csv_col, &value);
        count++;
    }
    va_end(ap);

    csv->row_count++;
    return true;
}

bool csv_get_row_mask(const CSV *const csv, size_t row, uint32_t mask, const char **values)
{
    if (values == NULL) {
        csv_logic_err("The \"values\" parameter must contain a valid address to store the values");
        return false;
    }

    if (row >= csv->row_count) {
        csv_logic_err("Row index exceeds number of rows in CSV");
        return false;
    }

    if (bit_min_amount(mask) > csv->columns.count) {
        csv_logic_err("The number of bits in \"mask\" is greater than the number of columns");
        return false;
    }

    for (size_t i = 0; i < bit_count_high(mask); i++) {
        values[i] = csv->columns.data[i].data[row];
    }

    return true;
}

void csv_fprint(const CSV *const csv, FILE *const context)
{
    size_t i;
    for (i = 0; i + 1 < csv->columns.count; i++) {
        fprintf(context, "%s,", csv->columns.data[i].id);
    }
    fprintf(context, "%s", csv->columns.data[i].id);
    fputc('\n', context);

    for (size_t i = 0; i < csv->row_count; i++) {
        size_t j;
        const char *data = NULL;

        for (j = 0; j + 1 < csv->columns.count; j++) {
            data = csv->columns.data[j].data[i];
            if (data) {
                fprintf(context, "%s", data);
            }
            fprintf(context, ",");
        }
        data = csv->columns.data[j].data[i];
        if (data) {
            fprintf(context, "%s", data);
        }
        fputc('\n', context);
    }
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









