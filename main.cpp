#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <cstdint>
#include <algorithm>
#include <unordered_map>
#include "bucket_sort.h"

using namespace std;

template<class T>
void print_mas(T* mas, int N) {
    for (int i = 0; i < N; i++) {
        std::cout << mas[i] << " ";
    }
    std::cout << std::endl;
}

float float_rand(float min, float max)
{
    float scale = rand() / (float)RAND_MAX; /* [0, 1.0] */
    return min + scale * (max - min);      /* [min, max] */
}

int main()
{
    srand((unsigned int)time(NULL));

    int size = 1025;
    float* m = (float*)malloc(sizeof(float) * size);
    float* res = (float*)malloc(sizeof(float) * size);
    if (m == 0 or res == 0) {
        return 0;
    }

    for (int i = 0; i < size; i++) {
        m[i] = 0.0;
    }
    m[0] = 100.0;

    for (int i = 0; i < size; i++) {
        printf("%f ", m[i]);
    }
    printf("\n");
    
    bucket_sort(m, res, size);

    for (int i = 0; i < size; i++) {
        printf("%f ", res[i]);
    }
    printf("\n");

    free(res);
    free(m);
    return 0;
}