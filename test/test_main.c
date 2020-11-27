#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

// cmocka requires some headers above
#include <cmocka.h>

int __real_main(int, char**);

char* OUTPUT_FILENAME = "output.txt";
int SUCCESS_CODE = 0;
int FAILURE_CODE = -1;

size_t DEFAULT_ARGS_SIZE = 6;
#define MAX_INPUT_STRING_SIZE 1000


// Compare data in OUTPUT_FILENAME and expected.
// Assumes that there are <= 10 string and each size is <= 1000
// Hope no errors will arise :)
int cmp_file_and_str(const char* expected) {
    FILE* f = fopen(OUTPUT_FILENAME, "r");

    // use any non 0 as failed status
    int result = 1;

    char str[MAX_INPUT_STRING_SIZE];
    char all_data[MAX_INPUT_STRING_SIZE * 10] = "";

    if (f != NULL) {
        while (fgets(str, MAX_INPUT_STRING_SIZE, f) != NULL) {
            strcat(all_data, str);
        }
        // Commented code below can be useful for debug
        // fprintf(stderr, "Got: %s\n", all_data);
        // fprintf(stderr, "Exp: %s\n", expected);
        result = strcmp(all_data, expected);
        fclose(f);
    }
    return result;
}


int test_real_main_vs_expected(
    char* args[], size_t args_size,
    int expected_code, const char* expected_str)
{
    int res = __real_main(args_size, args);
    assert_int_equal(res, expected_code);
    if (res == 0) {
        return cmp_file_and_str(expected_str);
    } else {
        return 0;
    }
}


// WRONG INPUT TESTS GROUP

static void test_non_existing_algorithm(void** state) {
    char* args[] = {
        "test_non_existing_algorithm",
        "42",
        "5_string_inp_asc.txt",
        OUTPUT_FILENAME,
        "log_sort",
        "asc"
    };
    assert_int_equal(0, test_real_main_vs_expected(args, DEFAULT_ARGS_SIZE, FAILURE_CODE, NULL));
    return;
}


// CORNER CASES TESTS GROUP

static void test_read_less_strings_than_in_input(void** state) {
    char* args[] = {
        "test_read_less_strings_than_in_input",
        "2",
        "5_string_inp_des.txt",
        OUTPUT_FILENAME,
        "bubble",
        "asc"
    };
    char* expected_data = "string data 4\nstring data 5\n";
    assert_int_equal(0, 
        test_real_main_vs_expected(args, DEFAULT_ARGS_SIZE, SUCCESS_CODE, expected_data));

    return;
}


// SORT 5 STRINGS TESTS GROUP

char* from_1_to_5 = 
        "string data 1\n"
        "string data 2\n"
        "string data 3\n"
        "string data 4\n"
        "string data 5\n";

char* from_5_to_1 = 
        "string data 5\n"
        "string data 4\n"
        "string data 3\n"
        "string data 2\n"
        "string data 1\n";


void run_sortings_with_different_correct_params(char* args[]) {
    // Run with ascending comparator
    args[5] = "asc";
    args[2] = "5_string_inp_asc.txt";
    assert_int_equal(0, 
        test_real_main_vs_expected(args, DEFAULT_ARGS_SIZE, SUCCESS_CODE, from_1_to_5));

    args[2] = "5_string_inp_des.txt";
    assert_int_equal(0, 
        test_real_main_vs_expected(args, DEFAULT_ARGS_SIZE, SUCCESS_CODE, from_1_to_5));

    // Run with descending comparator
    args[5] = "des";
    args[2] = "5_string_inp_asc.txt";
    assert_int_equal(0, 
        test_real_main_vs_expected(args, DEFAULT_ARGS_SIZE, SUCCESS_CODE, from_5_to_1));

    args[2] = "5_string_inp_des.txt";
    assert_int_equal(0, 
        test_real_main_vs_expected(args, DEFAULT_ARGS_SIZE, SUCCESS_CODE, from_5_to_1));
}


static void test_5_string_quick(void** state) {
    char* args[] = {
        "test_5_string_quick",
        "5",
        "5_string_inp_asc.txt",
        OUTPUT_FILENAME,
        "quick",
        "asc"
    };
    run_sortings_with_different_correct_params(args);

    return;
}


static void test_5_string_radix(void** state) {
    char* args[] = {
        "test_5_string_radix",
        "5",
        "5_string_inp_asc.txt",
        OUTPUT_FILENAME,
        "radix",
        "asc"
    };
    run_sortings_with_different_correct_params(args);

    return;
}


int __wrap_main() {
    const struct CMUnitTest incorrect_input_tests[] = {
        cmocka_unit_test(test_non_existing_algorithm),
    };

    const struct CMUnitTest sortings_tests[] = {
        cmocka_unit_test(test_5_string_quick),
        cmocka_unit_test(test_5_string_radix),
        cmocka_unit_test(test_read_less_strings_than_in_input),
    };

    return 
        cmocka_run_group_tests(incorrect_input_tests, NULL, NULL) +
        cmocka_run_group_tests(sortings_tests, NULL, NULL);
}
