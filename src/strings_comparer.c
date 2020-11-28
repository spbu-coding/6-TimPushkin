#include <stdlib.h>
#include <stdio.h>

#include "message_handler.h"
#include "sortings.h"

#define SUPPORTED_SORT_TYPES {"bubble", "insertion", "merge", "quick", "radix"}
#define SUPPORTED_COMPARATORS {"asc", "des"}
#define SUPPORTED_FILE_FORMAT ".txt"
#define ESTIMATED_CL_ARGS_NUM 6

typedef void (*sort_func_t)(strings_array_t, array_size_t, comparator_func_t);

struct sort_options_t {
    array_size_t strs_num;
    const char* inp_filename;
    const char* otp_filename;
    sort_func_t sort_type;
    comparator_func_t comparator;
};

_Bool is_str_ending(const char* const str, const char* const ending) {
    const unsigned int str_len = strlen(str), ending_len = strlen(ending);
    if (str_len >= ending_len && strcmp(str + str_len - ending_len, ending) == 0) {
        return 1;
    } else {
        return 0;
    }
}

int asc_cmp(const char* str1, const char* str2) {
    return strcmp(str1, str2);
}

int des_cmp(const char* str1, const char* str2) {
    return -strcmp(str1, str2);
}

int parse_sort_options(struct sort_options_t* const options_container, const int* const argc, const char* const* const argv) {
    if (*argc != ESTIMATED_CL_ARGS_NUM) {
        message_handle("Unexpected number of command-line arguments\n", error);
        return -1;
    }
    options_container->strs_num = strtoul(argv[1], NULL, 10);
    if ((!options_container->strs_num && argv[1][0] != '0') || !('0' <= argv[1][0] && argv[1][0] <= '9')) {
        message_handle("The 1st command-line argument is invalid (must be a non-negative number)\n", error);
        return -1;
    }
    options_container->inp_filename = argv[2];
    if (!is_str_ending(options_container->inp_filename, SUPPORTED_FILE_FORMAT)) {
        message_handle("The 2nd command-line argument is invalid (must be a txt file)\n", error);
        return -1;
    }
    options_container->otp_filename = argv[3];
    if (!is_str_ending(options_container->otp_filename, SUPPORTED_FILE_FORMAT)) {
        message_handle("The 3rd command-line argument is invalid (must be a txt file)\n", error);
        return -1;
    }
    const char* supported_sort_types[5] = SUPPORTED_SORT_TYPES;
    if (strcmp(argv[4], supported_sort_types[0]) == 0) {
        options_container->sort_type = bubble;
    } else if (strcmp(argv[4], supported_sort_types[1]) == 0) {
        options_container->sort_type = insertion;
    } else if (strcmp(argv[4], supported_sort_types[2]) == 0) {
        options_container->sort_type = merge;
    } else if (strcmp(argv[4], supported_sort_types[3]) == 0) {
        options_container->sort_type = quick;
    } else if (strcmp(argv[4], supported_sort_types[4]) == 0) {
        options_container->sort_type = radix;
    } else {
        message_handle("The 4th command-line argument is invalid (must be one of the supported sort types)\n", error);
        return -1;
    }
    const char* supported_comparators[2] = SUPPORTED_COMPARATORS;
    if (strcmp(argv[5], supported_comparators[0]) == 0) {
        options_container->comparator = asc_cmp;
    } else if (strcmp(argv[5], supported_comparators[1]) == 0) {
        options_container->comparator = des_cmp;
    } else {
        message_handle("The 5th command-line argument is invalid (must be one of the supported comparators)\n", error);
        return -1;
    }
    return 0;
}

void dealloc_strs(strings_array_t strs_container, const array_size_t* const strs_num) {
    for (unsigned int i = 0; i < *strs_num; i++) {
        free(strs_container[i]);
    }
    free(strs_container);
}

int alloc_strs(strings_array_t* strs_container, const array_size_t* const strs_num) {
    *strs_container = (char**) malloc(*strs_num * sizeof(char*));
    if (*strs_container == NULL) {
        message_handle("Failed to allocate memory for strings to sort\n", error);
        return -1;
    }
    for (unsigned int i = 0; i < *strs_num; i++) {
        (*strs_container)[i] = (char*) malloc((MAX_INPUT_STRING_SIZE + 1) * sizeof(char));
        if ((*strs_container)[i] == NULL) {
            message_handle("Failed to allocate memory for one of the strings to sort\n", error);
            dealloc_strs(*strs_container, strs_num);
            return -1;
        }
    }
    return 0;
}

int read_strs(const char* const filename, strings_array_t strs_container, const array_size_t* const strs_num) {
    FILE* file_to_read = fopen(filename, "rb");
    if (file_to_read == NULL) {
        message_handle("Failed to open given input file\n", error);
        return -1;
    }
    for (unsigned int i = 0; i < *strs_num; i++) {
        if (fgets(strs_container[i], MAX_INPUT_STRING_SIZE + 1, file_to_read) == NULL) {
            message_handle("Failed to read given number of strings from the input file\n", error);
            if (fclose(file_to_read) != 0) {
                message_handle("Also failed to close the input file\n", error);
            }
            return -1;
        }
    }
    if (fclose(file_to_read) != 0) {
        message_handle("Failed to close the input file after reading\n", error);
        return -1;
    }
    const unsigned int last_char_ind = strlen(strs_container[*strs_num - 1]) - 1;
    if (strs_container[*strs_num - 1][last_char_ind] != '\n') {
        if (last_char_ind == MAX_INPUT_STRING_SIZE - 1) {
            message_handle("Unsupported strings given (the last string read does not contain LF, but has max length)\n", error);
            return -1;
        }
        strs_container[*strs_num - 1][last_char_ind + 1] = '\n';
        strs_container[*strs_num - 1][last_char_ind + 2] = '\0';
    }
    return 0;
}

int write_strs(const char* const filename, const strings_array_t strs_container, const array_size_t* const strs_num) {
    FILE* file_to_write = fopen(filename, "wb");
    if (file_to_write == NULL) {
        message_handle("Failed to open/create requested output file\n", error);
        return -1;
    }
    for (unsigned int i = 0; i < *strs_num; i++) {
        if (fputs(strs_container[i], file_to_write) == EOF) {
            message_handle("Failed to write given number of strings to the output file\n", error);
            if (fclose(file_to_write) != 0) {
                message_handle("Also failed to close the output file\n", error);
            }
            return -1;
        }
    }
    if (fclose(file_to_write) != 0) {
        message_handle("Failed to close the output file after writing\n", error);
        return -1;
    }
    return 0;
}

int main(const int argc, const char* const* const argv) {
    struct sort_options_t sort_options;
    if (parse_sort_options(&sort_options, &argc, argv) != 0) {
        return -1;
    }
    if (sort_options.strs_num > 0) {
        strings_array_t strs_to_sort = NULL;
        if (alloc_strs(&strs_to_sort, &sort_options.strs_num) != 0) {
            return -1;
        }
        if (read_strs(sort_options.inp_filename, strs_to_sort, &sort_options.strs_num) != 0) {
            dealloc_strs(strs_to_sort, &sort_options.strs_num);
            return -1;
        }
        sort_options.sort_type(strs_to_sort, sort_options.strs_num, sort_options.comparator);
        if (write_strs(sort_options.otp_filename, strs_to_sort, &sort_options.strs_num) != 0) {
            dealloc_strs(strs_to_sort, &sort_options.strs_num);
            return -1;
        }
        dealloc_strs(strs_to_sort, &sort_options.strs_num);
    }
    return 0;
}
