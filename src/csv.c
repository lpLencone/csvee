#include "csv.h"

#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define CSV_INIT_COLUMN_CAPACITY 8;

CSV *csv_with_columns(size_t ncols, const char **col_names)
{
    assert(ncols > 0);

    CSV *csv = calloc(1, sizeof(CSV));
    assert(csv != NULL);

    struct _CSV_Column csv_col = {0};
    for (size_t i = 0; i < ncols; i++) {
        csv_col.id = strdup(col_names[i]);
        da_append(&csv->columns, &csv_col);
    }

    return csv;
}

CSV *csv_create(size_t ncols, ...)
{
    da(const char *) cols = {0};

    va_list ap;
    va_start(ap, ncols);
    for (size_t i = 0; i < ncols; i++) {
        const char *col_name = va_arg(ap, const char *);
        da_append(&cols, &col_name);
    }
    va_end(ap);

    CSV *csv = csv_with_columns(ncols, cols.data);

    da_end(&cols);
    return csv;
}

CSV *csv_from_cstr(const char *const cstr)
{
    char *cstr_dup = strdup(cstr);
    assert(cstr_dup != NULL);

    CSV *csv = NULL;
    char *body = NULL;
    const char *line_delim = "\n";
    const char *data_delim = ",";
    char *saveptr = NULL;

    { // Create CSV with columns
        char *header = strtok_r(cstr_dup, line_delim, &body);
        size_t ncols = 0;
        da(const char *) cols = {0};
        char *col_name = strtok_r(header, data_delim, &saveptr);
        while (col_name != NULL) {
            ncols++;
            da_append(&cols, &col_name);
            col_name = strtok_r(saveptr, data_delim, &saveptr);
        }
        csv = csv_with_columns(ncols, cols.data);
        da_end(&cols);
    }

    { // Insert data into CSV
        char *nextline = NULL;
        char *line = strtok_r(body, line_delim, &nextline);
        // `strdup` is necessary to use the strsep function
        // `strtok` won't work instead because I need the zero-length strings where there are adjecent delimiter bytes.
        while (line != NULL) {
            line = strdup(line);
            assert(line != NULL);

            // Since `strsep` modifies where the string pointer points to, it is necessary to save the address of the start of the allocated duplicated string to free it later.
            char *save_line_ptr = line;
            da(const char *) data = {0};

            char *datum = strsep(&line, data_delim);
            while (datum != NULL) {
                da_append(&data, &datum);
                datum = strsep(&line, data_delim);
            }
            csv_append_array(csv, data.data);
            da_end(&data);
            free(save_line_ptr);

            line = strtok_r(nextline, line_delim, &nextline);
        }
    }
    
    free(cstr_dup);
    return csv;
}

void csv_destroy(CSV *const csv)
{
    for (size_t i = 0; i < csv->columns.count; i++) {
        struct _CSV_Column *col = &csv->columns.data[i];
        free(col->id);
        if (col->data != NULL) {
            for (size_t j = 0; j < col->count; j++) {
                free(col->data[j]);
            }
            da_end(col);
        }
    }
    da_end(&csv->columns);
    free(csv);
}

void csv_insert_array(CSV *const csv, const char *const data[csv->columns.count], size_t row)
{
    for (size_t i = 0; i < csv->columns.count; i++) {
        struct _CSV_Column *col = &csv->columns.data[i];
        if (col->capacity == 0) {
            col->capacity = CSV_INIT_COLUMN_CAPACITY;
            col->data = malloc(col->capacity * sizeof(char *));
        }

        if (col->count + 1 < col->capacity) {
            col->capacity *= 2;
            col->data = realloc(col->data, col->capacity * sizeof(char *));
            assert(col->data != NULL);
        }

        memmove(col->data + row + 1, col->data + row, sizeof(char *));

        if (data[i] != NULL) {
            col->data[row] = strdup(data[i]);
            assert(col->data[row] != NULL);
        }
        col->count++;
    }
    csv->row_count++;
}

void csv_get_row(const CSV *const csv, size_t row, const char *values[csv->columns.count])
{
    assert(row < csv->row_count);
    for (size_t i = 0; i < csv->columns.count; i++) {
        values[i] = csv->columns.data[i].data[row];
    }
}

void csv_get_row_array(const CSV *const csv, size_t row, size_t ncols, const char *const col_names[ncols], const char *values[ncols])
{
    assert(row < csv->row_count && ncols < csv->columns.count);

    for (size_t i = 0; i < ncols; i++) {
        struct _CSV_Column *col = NULL;
        for (size_t j = 0; j < csv->columns.count; j++) {
            if (strcmp(csv->columns.data[j].id, col_names[i]) == 0) {
                col = &csv->columns.data[j];
            }
        }
        assert(col != NULL && "Column name not present in CSV");
        values[i] = col->data[row];
    }
}

void csv_edit_row_array(const CSV *const csv, size_t row, size_t ncols, const char *const col_names[ncols], const char *values[ncols])
{
    assert(row < csv->row_count && ncols < csv->columns.count);

    for (size_t i = 0; i < ncols; i++) {
        struct _CSV_Column *col = NULL;
        for (size_t j = 0; j < csv->columns.count; j++) {
            if (strcmp(csv->columns.data[j].id, col_names[i]) == 0) {
                col = &csv->columns.data[j];
            }
        }
        assert(col != NULL && "Column not found in CSV");
        free(col->data[row]);
        col->data[row] = strdup(values[i]);
        assert(col->data[row] != NULL);
    }
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
            if (data != NULL) {
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









