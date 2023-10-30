#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "csv.h"

#define csvcp(p) \
    do { \
        if (p == NULL) { \
            retcode = 1; \
            eprint("%d: CSV Error: %s\n", __LINE__, csv_strerror()); \
            goto finally; \
        } \
    } while (0)

#define csvcc(err) \
    do { \
        if (err) { \
            retcode = 1; \
            eprint("%d: CSV Error: %s\n", __LINE__, csv_strerror()); \
            goto finally; \
        } \
    } while (0)

int test0(void)
{
    int retcode = 0;

    CSV *csv = csv_create("filename.csv", "column0", "column1", "column2");
    csvcp(csv);

    csvcc(csv_insert(csv, "value0", "value1", "value2checc"));
    csvcc(csv_insert(csv, "value0deez", "value1", "value2"));
    csvcc(csv_insert(csv, "value0", "value1nuts", "value2"));

    csv_display(csv);

finally:
    if (csv) {
        csv_destroy(csv);
    }
    return retcode;
}

int main(void)
{
    return test0();
}
