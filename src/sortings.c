#include <math.h>

#include "sortings.h"

#define SWAP_STRS(str1, str2) char* buf = str1; str1 = str2; str2 = buf;

#define ASCII_LEN 128

void bubble(strings_array_t strs_to_sort, array_size_t strs_num, comparator_func_t cmp) {
    for (unsigned int i = 0; i < strs_num; i++) {
        for (unsigned int j = 0; j < strs_num - 1 - i; j++) {
            if (cmp(strs_to_sort[j], strs_to_sort[j + 1]) > 0) {
                SWAP_STRS(strs_to_sort[j], strs_to_sort[j + 1])
            }
        }
    }
}

void insertion(strings_array_t strs_to_sort, array_size_t strs_num, comparator_func_t cmp) {
    for (unsigned int i = 1; i < strs_num; i++) {
        for (unsigned int j = i; j > 0 && cmp(strs_to_sort[j - 1], strs_to_sort[j]) > 0; j--) {
            SWAP_STRS(strs_to_sort[j - 1], strs_to_sort[j])
        }
    }
}

void merge(strings_array_t strs_to_sort, array_size_t strs_num, comparator_func_t cmp) {
    for (unsigned int parts_num = strs_num; parts_num > 1; parts_num = (unsigned int) ceil(parts_num / 2.0)) {
        size_t part_len = (size_t) ceil(strs_num / (double) parts_num);
        for (unsigned int i = 0; i < parts_num - 1; i += 2) {
            size_t len1 = part_len, len2 = (((i + 2) * part_len) <= strs_num) ? part_len : (strs_num - (i + 1) * part_len), buf_len = len1 + len2;
            unsigned int ind1 = i * part_len, ind2 = (i + 1) * part_len;
            char* buf[buf_len];
            while (len1 > 0 && len2 > 0) {
                if (cmp(strs_to_sort[ind1], strs_to_sort[ind2]) <= 0) {
                    buf[buf_len - (len1-- + len2)] = strs_to_sort[ind1++];
                } else {
                    buf[buf_len - (len1 + len2--)] = strs_to_sort[ind2++];
                }
            }
            if (len1 > 0) {
                memcpy(&buf[buf_len - len1], &strs_to_sort[ind1], len1 * sizeof(char*));
            } else {
                memcpy(&buf[buf_len - len2], &strs_to_sort[ind2], len2 * sizeof(char*));
            }
            memcpy(&strs_to_sort[i * part_len], buf, buf_len * sizeof(char*));
        }
    }
}

void quick_split(strings_array_t strs, unsigned int beg, const unsigned int end, comparator_func_t cmp) {
    while (beg < end) {
        // Sorting order: [beg, left - 1] -- less than pivot, [left, mid - 1] -- equal to pivot, [right, end - 1] -- greater than pivot
        if ((strs[beg] <= strs[(beg + end - 1) / 2] && strs[(beg + end - 1) / 2] <= strs[end - 1]) || (strs[end - 1] <= strs[(beg + end - 1) / 2] && strs[(beg + end - 1) / 2] <= strs[beg])) {
            SWAP_STRS(strs[beg], strs[(beg + end - 1) / 2])
        } else if ((strs[beg] <= strs[end - 1] && strs[end - 1] <= strs[(beg + end - 1) / 2]) || (strs[(beg + end - 1) / 2] <= strs[end - 1] && strs[end - 1] <= strs[beg])) {
            SWAP_STRS(strs[beg], strs[end - 1])
        }
        unsigned int left = beg, mid = beg + 1, right = end;
        for (unsigned int i = 0; i < end - beg - 1; i++) {
            const int cmp_result = cmp(strs[mid], strs[beg]);
            if (cmp_result < 0) {
                left++;
                SWAP_STRS(strs[mid], strs[left])
                mid++;
            } else if (cmp_result > 0) {
                right--;
                SWAP_STRS(strs[mid], strs[right])
            } else {
                mid++;
            }
        }
        SWAP_STRS(strs[beg], strs[left])
        quick_split(strs, beg, left, cmp);
        beg = right;
    }
}

void quick(strings_array_t strs_to_sort, array_size_t strs_num, comparator_func_t cmp) {
    quick_split(strs_to_sort, 0, strs_num, cmp);
}

void radix(strings_array_t strs_to_sort, array_size_t strs_num, comparator_func_t cmp) {
    const _Bool is_asc_order = (cmp("a", "b") < 0);
    size_t strs_len[strs_num], max_str_len = 0;
    for (unsigned int i = 0; i < strs_num; i++) {
        strs_len[i] = strlen(strs_to_sort[i]) - 1;
        if (strs_len[i] > max_str_len) {
            max_str_len = strs_len[i];
        }
    }
    for (int i = (int) max_str_len - 1; i >= 0; i--) {
        unsigned int char_counter[ASCII_LEN] = {0};
        for (unsigned int j = 0; j < strs_num; j++) {
            if ((int) strs_len[j] - 1 >= i) {
                char_counter[(unsigned int) strs_to_sort[j][i]]++;
            } else {
                char_counter[0]++;
            }
        }
        if (is_asc_order) {
            for (unsigned int j = 1; j < ASCII_LEN; j++) {
                char_counter[j] += char_counter[j - 1];
            }
        } else {
            for (int j = ASCII_LEN - 2; j >= 0; j--) {
                char_counter[j] += char_counter[j + 1];
            }
        }
        char* buf[strs_num];
        size_t buf_l[strs_num];
        for (int j = (int) strs_num - 1; j >= 0; j--) {
            if ((int) strs_len[j] - 1 >= i) {
                buf[(char_counter[(unsigned int) strs_to_sort[j][i]]) - 1] = strs_to_sort[j];
                buf_l[(char_counter[(unsigned int) strs_to_sort[j][i]]--) - 1] = strs_len[j];
            } else {
                buf[(char_counter[0]) - 1] = strs_to_sort[j];
                buf_l[(char_counter[0]--) - 1] = strs_len[j];
            }
        }
        memcpy(strs_to_sort, buf, strs_num * sizeof(char*));
        memcpy(strs_len, buf_l, strs_num * sizeof(size_t));
    }
}
