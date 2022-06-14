#include <stdio.h>

float* get_element(float* array, int w, int i, int j) {

    float* loc = array + w*j + i;

    return loc;
}

float calculate_hma(float* array, int w, int h, int i, int j) {

    float rose[] = {0, 0, 0, 0, 0, 0, 0, 0}; // N, NE, E, SE, S, SW, W, NW

    if (i != (w-1) && j != 0) {
        rose[1] = *get_element(array, w, j+1, i+1);
    }
    if (i != (w-1) && j != (h-1)) {
        rose[3] = *get_element(array, w, j-1, i+1);
    }
    if (i != 0 && j != 0) {
        rose[5] = *get_element(array, w, j-1, i-1);
    }
    if (i != 0 && j != 0) {
        rose[7] = *get_element(array, w, j+1, i-1);
    }

    if (i == 0) {
        rose[6] = 0;
        rose[2] = *get_element(array, w, j, i+1);
    } else if (i == (w-1)) {
        rose[2] = 0;
        rose[6] = *get_element(array, w, j, i-1);
    } else {
        // lefts and rights have values!
        rose[6] = *get_element(array, w, j, i-1);
        rose[2] = *get_element(array, w, j, i+1);
    }
    if (j == 0) {
        rose[0] = 0;
        rose[4] = *get_element(array, w, j-1, i);
    } else if (j == (h-1)) {
        rose[4] = 0;
        rose[0] = *get_element(array, w, j+1, i);
    } else {
        rose[0] = *get_element(array, w, j+1, i);
        rose[4] = *get_element(array, w, j-1, i);
    }

    float hma = *get_element(array, w, j, i);
    for (int i = 0; i < 8; i++) {
        hma += rose[i];
    }
    return 0.111 * hma;
}

void get_hma(float* array, int w, int h, int* found_x, int* found_y) {

    float max = -1;
    for (int i = 0; i < w; i++) {
        for (int j = 0; j < h; j++) {

            float hma = calculate_hma(array, w, h, i, j);
            if (hma > max) {
                max = hma;
                *found_x = i;
                *found_y = j;
            }

        }
    }
}

int main(int argc, char** argv) {
    if (1 != argc) {
        return -1;
    }

    float array[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    int w = 3;
    int h = 3;
    int x = 0;
    int y = 0;
    int* found_x = &x;
    int* found_y = &y;
    get_hma(array, w, h, found_x, found_y);

    printf("(%d, %d)\n", x, y);

    return 1;
}