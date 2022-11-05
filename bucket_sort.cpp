#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "bucket_sort.h"

void odd_even_sort_cpu(float* arr, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size / 2 + 1; j++) {
            int ai = 2 * j + (i % 2);
            int bi = 2 * j + (i % 2) + 1;

            if ((ai < size) and (bi < size) and (arr[ai] > arr[bi])) {
                float t = arr[ai];
                arr[ai] = arr[bi];
                arr[bi] = t;
            }
        }
    }
}

void odd_even_lsort_cpu(float* res, int* pocket, int pocket_N) {
    for (int bdx = 0; bdx < pocket_N; bdx++) {
        int N = pocket[bdx + 1] - pocket[bdx];
        if (N <= 1024) {
            int start_elem = pocket[bdx];
            odd_even_sort_cpu(res + start_elem, N);
        }
        else {
            continue;
        }
    }
}

void keys_init_cpu(float* mas, int* keys, float min, float max, int N, int n_split) {
    for (int i = 0; i < N; i++) {
        keys[i] = (mas[i] - min) / (max - min) * n_split * 0.9999999;
    }
}

void histogram_cpu(int* mas, int* res, int mas_n) {
    for (int i = 0; i < mas_n; i++) {
        res[mas[i]]++;
    }
}

void scan_cpu(int* hist, int* mscan, int N) {
    for (int i = 0; i < N; i++) {
        if (i == 0) {
            mscan[i] = 0;
        }
        else {
            mscan[i] = mscan[i - 1] + hist[i - 1];
        }
    }
}

void group_cpu(float* mas, float* res, int* hist, int* mscan, float min, float max, int N, int n_split) {
    for (int i = 0; i < N; i++) {
        int pocket = (mas[i] - min) / (max - min) * n_split * 0.9999999;
        int hist_value = hist[pocket]--;
        int mscan_value = mscan[pocket];
        res[hist_value + mscan_value - 1] = mas[i];
    }
}

void bucket_sort(float* mas, float* res, int N) {
    if (N <= 1024) {
        memcpy(res, mas, sizeof(float) * N);
        odd_even_sort_cpu(res, N);
        return;
    }

    float min = mas[0], max = mas[0];

    for (int i = 0; i < N; i++) {
        if (min > mas[i]) {
            min = mas[i];
        }
        if (max < mas[i]) {
            max = mas[i];
        }
    }

    if (fabs(min - max) < 0.00000001) {
        memcpy(res, mas, sizeof(float) * N);
        return;
    }

    int split_size = 16;
    int n_split = (N - 1) / split_size + 1;

    int* keys = (int*)malloc(sizeof(int) * N);
    if (keys == 0) exit(0);

    keys_init_cpu(mas, keys, min, max, N, n_split);
    //print_mas(keys, N);

    int* hist = (int*)malloc(sizeof(int) * n_split);
    if (hist == 0) exit(0);
    memset(hist, 0, sizeof(int) * n_split);

    histogram_cpu(keys, hist, N);
    //print_mas(hist, n_split);

    int* mscan = (int*)malloc(sizeof(int) * (n_split + 1));
    if (mscan == 0) exit(0);

    scan_cpu(hist, mscan, n_split);
    mscan[n_split] = hist[n_split - 1] + mscan[n_split - 1];
    //print_mas(mscan, n_split + 1);

    group_cpu(mas, res, hist, mscan, min, max, N, n_split);

    /*printf("============================================================\n");
    print_mas(hist, n_split);
    print_mas(res, N);*/

    int pocket_size = 1024;
    int iter = 0;
    for (int i = 0; i <= n_split; i++) {
        if (mscan[i] - mscan[iter] > pocket_size) {
            if (iter + 1 == i) {
                iter++;

                float* recres = (float*)malloc(sizeof(float) * (mscan[i] - mscan[i - 1]));
                if (recres == 0) exit(0);
                // printf("RECURSION...\n");
                bucket_sort(res + mscan[i - 1], recres, mscan[i] - mscan[i - 1]);
                // printf("EXIT FROM RECURSION\n");
                memcpy(res + mscan[i - 1], recres, sizeof(float) * (mscan[i] - mscan[i - 1]));
                free(recres);

            }
            else {
                mscan[iter + 1] = mscan[i - 1];
                iter++;

                if (mscan[i] - mscan[iter] > pocket_size) {
                    float* recres = (float*)malloc(sizeof(float) * (mscan[i] - mscan[iter]));
                    if (recres == 0) exit(0);
                    // printf("RECURSION...\n");
                    bucket_sort(res + mscan[iter], recres, mscan[i] - mscan[iter]);
                    // printf("EXIT FROM RECURSION\n");
                    memcpy(res + mscan[iter], recres, sizeof(float) * (mscan[i] - mscan[iter]));
                    free(recres);
                }
            }
        }
    }
    int pocket_N = iter + 2;
    mscan[iter + 1] = mscan[n_split];

    odd_even_lsort_cpu(res, mscan, pocket_N);

    free(mscan);
    free(keys);
    free(hist);
}