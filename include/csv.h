#ifndef CSVEE_CSV_H_
#define CSVEE_CSV_H_

#include "common.h"

#include <stdint.h>

struct _CSV_Column {
    char *id;
    char **data;
    size_t count;
    size_t capacity;
};

typedef struct {
    da(struct _CSV_Column) columns;
    size_t row_count;
} CSV;

CSV *csv_with_columns(size_t ncols, const char **col_names) NONNULL;

CSV *csv_create(size_t ncols, ...);

void csv_destroy(CSV *const csv) NONNULL;

CSV *csv_from_cstr(const char *const cstr) NONNULL;

void csv_insert_array(CSV *const csv, const char *const data[csv->columns.count], size_t row) NONNULL;
#define csv_append_array(csv, data) csv_insert_array(csv, data, (csv)->row_count)

void csv_edit_row(const CSV *const csv, size_t row, const char *values[csv->columns.count]) NONNULL;

void csv_edit_row_array(const CSV *const csv, size_t row, size_t ncols, const char *const col_names[ncols], const char *values[ncols]) NONNULL;

void csv_get_row(const CSV *const csv, size_t row, const char *values[csv->columns.count]) NONNULL;

void csv_get_row_array(const CSV *const csv, size_t row, size_t ncols, const char *const col_names[ncols], const char *values[ncols]) NONNULL;

void csv_fprint(const CSV *const csv, FILE *const context) NONNULL;
#define csv_print(csv) csv_fprint(csv, stdout)
#define csv_save csv_fprint

#endif // CSVEE_CSV_H_

