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

#define csvcc(result) \
    do { \
        if (!result) { \
            retcode = 1; \
            eprint("%d: CSV Error: %s\n", __LINE__, csv_strerror()); \
            goto finally; \
        } \
    } while (0)

int test0(void)
{
    int retcode = 0;

    CSV *csv = csv_create(3, "column0", "column1", "column2");
    csvcp(csv);

    csvcc(csv_append_row(csv, "value0", "value1", "value2checc"));
    csvcc(csv_append_row(csv, "value0deez", "value1", "value2"));
    csvcc(csv_append_row(csv, "value0", "value1nuts", "value2"));

    csv_print(csv);

finally:
    if (csv) {
        csv_destroy(csv);
    }
    return retcode;
}

int test1(void)
{
    int retcode = 0;

    CSV *csv = csv_create(4, "abcdef", "column", "deeznuts", "bruh");
    csvcp(csv);

    csvcc(csv_append_mask(csv, 0b1111, "shacks", "snacks", "sfacks", "monkeys"));
    csvcc(csv_append_mask(csv, 0b1011, "abadu", "lemao", "marks"));
    csvcc(csv_append_mask(csv, 0b1110, "seenoo", "estrid", "falukt"));

    csv_print(csv);

finally:
    if (csv) {
        csv_destroy(csv);
    }
    return retcode;
}

int test2(void)
{
    int retcode = 0;

    CSV *csv = csv_create(4, "abcdef", "column", "deeznuts", "bruh");
    csvcp(csv);

    csvcc(csv_append_mask(csv, 0b1111, "shacks", "snacks", "sfacks", "monkeys"));
    csvcc(csv_append_mask(csv, 0b1011, "abadu", "lemao", "marks"));
    csvcc(csv_append_mask(csv, 0b1110, "seenoo", "estrid", "falukt"));

    uint32_t mask = 0b1010;
    const char *values[2];
    csv_get_row_mask(csv, 1, mask, values);
    csvcp(values);
    for (size_t i = 0; i < bit_count_high(mask); i++) {
        printf("%s,", values[i]);
    }
    putchar('\n');

finally:
    if (csv) {
        csv_destroy(csv);
    }
    return retcode;
}

int test3(void)
{
    int retcode = 0;
    FILE *context = NULL;

    CSV *csv = csv_create(4, "abcdef", "column", "deeznuts", "bruh");
    csvcp(csv);

    csvcc(csv_append_mask(csv, 0b1111, "shacks", "snacks", "sfacks", "monkeys"));
    csvcc(csv_append_mask(csv, 0b1011, "abadu", "lemao", "marks"));
    csvcc(csv_append_mask(csv, 0b1110, "seenoo", "estrid", "falukt"));

    context = fopen("filename.csv", "w");
    if (context != NULL) {
        csv_save(csv, context);
    }

finally:
    if (csv) {
        csv_destroy(csv);
    }
    if (context) {
        fclose(context);
    }
    return retcode;
}

int main(void)
{
    return test3();
}
