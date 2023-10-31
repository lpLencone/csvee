#ifndef CSVEE_CSV_H_
#define CSVEE_CSV_H_

#include "common.h"
#include <stdint.h>

struct _CSV_Column {
    const char *id;
    const char **data;
    size_t count;
    size_t capacity;
};

typedef struct {
    da(struct _CSV_Column) columns;
    size_t row_count;
} CSV;

/**
 * Create a new CSV structure.
 *
 * This function dynamically allocates memory for a new CSV structure, initializes
 * its properties, and sets up the column headers. The caller is responsible for
 * freeing the allocated memory when it's no longer needed using `csv_destroy`.
 *
 * @param column_count The number of columns in the CSV.
 * @param ... Variable number of column names (as const char* arguments).
 *
 * @return A pointer to the newly created CSV structure, or NULL on failure.
 *   Check the global variable `csv_status` for any error information.
 */
CSV *csv_create(size_t columns_count, ...);
void csv_destroy(CSV *const csv);

/**
 * Append data to a CSV structure using a bitmask.
 *
 * This function appends data to a row in the CSV structure, selectively based on a bitmask.
 * It allows you to insert data for specific columns, identified by the set bits in the mask.
 *
 * @param csv A pointer to the CSV structure.
 * @param mask A bitmask where each bit represents a column (1 for append, 0 for skip). Note that on skip NULL is appended.
 * @param ... Variable number of values (as const char* arguments) to be appended.
 *
 * @return `true` on successful append, `false` if an error occurs.
 */
#define csv_append_row(csv, ...) csv_append_mask(csv, 0xFFFFFFFF, __VA_ARGS__)
bool csv_append_mask(CSV *const csv, uint32_t mask, ...);

/**
 * Retrieve data from specific columns of a row in a CSV structure using a bitmask.
 *
 * This function allows you to retrieve data from specific columns of a particular row in a CSV structure
 * based on a bitmask that defines which columns to include.
 *
 * @param csv A pointer to the CSV structure.
 * @param row The index of the row from which to retrieve data.
 * @param mask A bitmask where each bit represents a column to retrieve (1 for retrieve, 0 for skip).
 * @param values An array of pointers to store the retrieved values.
 *               The order of values in the array corresponds to the set bits in the mask.
 *               The caller must provide an array of valid addresses to store the values.
 *
 * @return `true` on successful data retrieval, `false` if an error occurs.
 */
bool csv_get_row_mask(const CSV *const csv, size_t row, uint32_t mask, const char **const values);

/**
 * Print the contents of a CSV structure into "context". The contents include headers and the data rows. The data are separated by comma.
 *
 * @param csv A pointer to the CSV structure to be displayed.
 *
 * @param context A file pointer to the context in which to print the data.
 */
void csv_fprint(const CSV *const csv, FILE *const context);
#define csv_print(csv) csv_fprint(csv, stdout)
#define csv_save csv_fprint

const char *csv_strerror(void);

#endif // CSVEE_CSV_H_

