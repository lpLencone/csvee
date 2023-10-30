#ifndef CSV_H_
#define CSV_H_

#include "common.h"

struct _CSV_Column {
    const char *id;
    const char **data; // Dynamic array of `const char *` type
    size_t count;
    size_t capacity;
};

typedef struct {
    const char *filename;
    da(struct _CSV_Column) columns;
    size_t row_count;
} CSV;

/// @brief create CSV struture with given variadic column names
/// @param filename csv filename
/// @param ... optional columns (there must be at least one; the last argument must be NULL)
/// @return On failure, NULL is returned. Otherwise, a pointer to a heap-allocated CSV structure.
CSV *csv_create(const char *filename, ...);

/// @brief Insert values into the CSV structure in the order they're given
/// @param csv csv structure
/// @param ... the values to insert into the CSV 
/// @return On failure, non-zero value
int csv_insert(CSV *csv, ...);

/// @brief Display the whole CSV structure
/// @param csv csv structure
void csv_display(CSV *csv);

/// @brief Destroy the CSV structure and free all the memory related
/// @param csv csv structure
void csv_destroy(CSV *csv);

/// @brief Return descriptive error string of the last failed operation
const char *csv_strerror(void);

#endif // CSV_H_

