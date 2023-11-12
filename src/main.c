#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "csv.h"

void test0(void)
{
    const char *col_names[] = {"abcdef", "column", "deeznuts", "bruh"};
    CSV *csv = csv_with_columns(4, col_names);
    csv_print(csv);
    csv_destroy(csv);
}

void test1(void)
{
    CSV *csv = csv_create(3, "bacdef", "column", "deeznuts");
    csv_print(csv);
    csv_destroy(csv);
}

void test2(void)
{
    CSV *csv = csv_create(3, "bacdef", "column", "deeznuts");

    const char *data[] = {"1", "2", "0"};
    csv_append_array(csv, data); 
    csv_print(csv);
    csv_destroy(csv);
}

void test3(void)
{
    CSV *csv = csv_create(4, "bacdef", "column", "deeznuts", "faggot");
    const char *data[] = {"pajeeet", "sober", "aphorititties", "lul"};
    csv_append_array(csv, data);
    data[2] = "sabe muito";
    csv_append_array(csv, data);
    FILE *context = fopen("bin/filename.csv", "w");
    csv_save(csv, context);
    fclose(context);
    csv_destroy(csv);
}

void test4(void)
{
    char *const cstr = read_to_cstr("bin/filename.csv");
    CSV *csv = csv_from_cstr(cstr);
    csv_print(csv);
    csv_destroy(csv);
    free(cstr);
}

void test5(void)
{
    char *const cstr = read_to_cstr("bin/filename.csv");
    CSV *csv = csv_from_cstr(cstr);

    const char *values[4] = {};
    csv_get_row(csv, 1, values);
    for (size_t i = 0; i < 4; i++) {
        printf("%s\n", values[i]);
    }

    csv_destroy(csv);
    free(cstr);
}

void test6(void)
{
    char *const cstr = read_to_cstr("bin/filename.csv");
    CSV *csv = csv_from_cstr(cstr);

    const char *col_names[] = {"column", "faggot", "bacdef"};
    const char *values[3] = {};
    csv_get_row_array(csv, 1, 3, col_names, values);
    for (size_t i = 0; i < 3; i++) {
        printf("%s : %s\n", col_names[i], values[i]);
    }

    csv_destroy(csv);
    free(cstr);
}

void test7(void)
{
    CSV *csv = csv_create(4, "bacdef", "column", "deeznuts", "faggot");
    const char *data[] = {"pajeeet", "sober", "aphorititties", "lul"};
    csv_append_array(csv, data);
    data[2] = "sabe muito";
    csv_insert_array(csv, data, 0);
    csv_print(csv);
    csv_destroy(csv);
}

void test8(void)
{
    char *const cstr = read_to_cstr("bin/filename.csv");
    CSV *csv = csv_from_cstr(cstr);

    // const char *col_names[] = {"column", "faggot", "bacdef"};
    // const char *values[] = {"majeet", "esphing", "marcone"};
    // csv_edit_row_array(csv, 1, 3, col_names, values);

    csv_edit_row_array(csv, 1, 3, 
            (const char *[3]){"column", "faggot", "bacdef"}, 
            (const char *[3]){"majeet", "esphing", "marcone"});

    csv_print(csv);

    csv_destroy(csv);
    free(cstr);
}

int main(void)
{
    test8();
    return 0;
}
