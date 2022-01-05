#pragma once

#include "edge_detection_auxiliary.h"

struct Masks {
	struct Mask masks[4];
};

struct Masks create_masks() {
    struct Masks write_to;

    int** mask_gaussian = malloc(5 * sizeof(*mask_gaussian));
    if (mask_gaussian) {
        for (size_t i = 0; i < 5; i++) {
            mask_gaussian[i] = malloc(5 * sizeof(mask_gaussian[0]));
        }

        mask_gaussian[0][0] = 2; mask_gaussian[0][1] = 4; mask_gaussian[0][2] = 5; mask_gaussian[0][3] = 4; mask_gaussian[0][4] = 2;
        mask_gaussian[1][0] = 4; mask_gaussian[1][1] = 9; mask_gaussian[1][2] = 12; mask_gaussian[1][3] = 9; mask_gaussian[1][4] = 4;
        mask_gaussian[2][0] = 5; mask_gaussian[2][1] = 12; mask_gaussian[2][2] = 15; mask_gaussian[2][3] = 12; mask_gaussian[2][4] = 5;
        mask_gaussian[3][0] = 4; mask_gaussian[3][1] = 9; mask_gaussian[3][2] = 12; mask_gaussian[3][3] = 9; mask_gaussian[3][4] = 4;
        mask_gaussian[4][0] = 2; mask_gaussian[4][1] = 4; mask_gaussian[4][2] = 5; mask_gaussian[4][3] = 4; mask_gaussian[4][4] = 2;

        write_to.masks[1].mask = mask_gaussian;
        write_to.masks[1].mask_size = 5;
    }

    int** mask1 = malloc(3 * sizeof(*mask1));
    if (mask1) {
        for (size_t i = 0; i < 3; i++) {
            mask1[i] = malloc(3 * sizeof(mask1[0]));
        }

        mask1[0][0] = -1; mask1[0][1] = -2; mask1[0][2] = -1;
        mask1[1][0] = 0;  mask1[1][1] = 0; mask1[1][2] = 0;
        mask1[2][0] = 1; mask1[2][1] = 2; mask1[2][2] = 1;

        write_to.masks[1].mask = mask1;
        write_to.masks[1].mask_size = 3;
    }


    int** mask2 = malloc(3 * sizeof(*mask2));
    if (mask2) {
        for (size_t i = 0; i < 3; i++) {
            mask2[i] = malloc(3 * sizeof(mask2[0]));
        }
        mask2[0][0] = -1; mask2[0][1] = 0; mask2[0][2] = 1;
        mask2[1][0] = -2;  mask2[1][1] = 0; mask2[1][2] = 2;
        mask2[2][0] = -1; mask2[2][1] = 0; mask2[2][2] = 1;

        write_to.masks[2].mask = mask2;
        write_to.masks[2].mask_size = 3;
    }


    int** mask3 = malloc(3 * sizeof(*mask3));
    if (mask3) {
        for (size_t i = 0; i < 3; i++) {
            mask3[i] = malloc(3 * sizeof(mask3[0]));
        }
        mask3[0][0] = 0; mask3[0][1] = 1; mask3[0][2] = 2;
        mask3[1][0] = -1;  mask3[1][1] = 0; mask3[1][2] = 1;
        mask3[2][0] = -2; mask3[2][1] = -1; mask3[2][2] = 0;

        write_to.masks[3].mask = mask3;
        write_to.masks[3].mask_size = 3;
    }

    return write_to;
}