#pragma once

#include "edge_detection_auxiliary.h"
#include "edge_detection_masks.h"

long long mean;
long long standard_deviation;

/// <summary>
/// function calculates 3x3 matrix mask with 3x3 bitmap matrix
/// </summary>
/// <param name="bitmap matrix"></param>
/// <param name="x"></param>
/// <param name="y"></param>
/// <param name="mask"></param>
/// <returns></returns>
uint8_t calculate_mask(uint8_t** input, size_t x, size_t y, struct Mask mask) {
    mask.mask_radius = (size_t)(mask.mask_size / 2);
    long long sum = 0;
    
    for (size_t i = y - mask.mask_radius; i <= y + mask.mask_radius; ++i) {
        for (size_t j = x - mask.mask_radius; j <= x + mask.mask_radius; ++j) {
        }
    }

    for (size_t i = y - mask.mask_radius; i <= y + mask.mask_radius; ++i) {
        for (size_t j = x - mask.mask_radius; j <= x + mask.mask_radius; ++j) {

            uint8_t image_val = input[i][j];
            
            int mask_val = mask.mask[i - y + mask.mask_radius][j - x + mask.mask_radius];

            sum += (long long)image_val * mask_val;

        }
    }

    if (sum < 0) sum = 0;
    if (sum > 255) sum = 255;

    return (uint8_t)sum;
}

uint8_t calculate_gaussian_mask(uint8_t** input, size_t x, size_t y, struct Mask mask) {
    mask.mask_radius = (size_t)(mask.mask_size / 2);
    size_t sum = 0;

    for (size_t i = y - mask.mask_radius; i <= y + mask.mask_radius; ++i) {
        for (size_t j = x - mask.mask_radius; j <= x + mask.mask_radius; ++j) {

            uint8_t image_val = input[i][j];

            int mask_val = mask.mask[i - y + mask.mask_radius][j - x + mask.mask_radius];

            sum += (long long)image_val * mask_val;
        }
    }

    sum /= 159;

    if (sum < 0) sum = 0;
    if (sum > 255) sum = 255;
    return (uint8_t)sum;
}

/// <summary>
/// performs convolution with given mask
/// </summary>
/// <param name="mat"></param>
/// <param name="mask"></param>
void calculate_gradient_strength(struct ImageMatrix* mat, struct Mask mask) {
    uint8_t** gradient_strength = malloc(mat->height * sizeof(*gradient_strength));
    for (size_t i = 0; i < mat->height; i++) {
        gradient_strength[i] = malloc(mat->width * sizeof(gradient_strength[0]));
    }

    for (size_t i = mask.mask_radius; i < mat->height - mask.mask_radius - 1; i++) {
        for (size_t j = mask.mask_radius; j < mat->width - mask.mask_radius - 1; j++) {

            uint8_t pixel_value = calculate_mask(mat->matrix, j, i, mask);

            gradient_strength[i][j] = pixel_value;

        }
    }

    mat->matrix = gradient_strength;
}

/// <summary>
/// performs convolution with gaussian blur matrix 5x5
/// </summary>
/// <param name="mat"></param>
void gaussian_blur(struct ImageMatrix* mat) {
    struct Masks mo = create_masks();
    struct Mask mask = mo.masks[0];



    if (mat->width > 0 && mat->height > 0) {
        uint8_t** gaussian_blur = malloc(mat->height * sizeof(*gaussian_blur));
        for (size_t i = 0; i < mat->height; i++) {
            gaussian_blur[i] = malloc(mat->width * sizeof(gaussian_blur[0]));
        }

        for (size_t i = mask.mask_radius; i < mat->height - mask.mask_radius; ++i) {
            for (size_t j = mask.mask_radius; j < mat->width - mask.mask_radius; ++j) {

                uint8_t pixel_value = calculate_gaussian_mask(mat->matrix, j, i, mask);

                gaussian_blur[i][j] = pixel_value;

            }
        }

        mat->matrix = gaussian_blur;
    }
}

/// <summary>
/// performs convolution with horizontal sobel mask
/// </summary>
/// <param name="mat"></param>
void sobel_horizontal(struct ImageMatrix* mat) {
    struct Masks mo = create_masks();
    struct Mask mask = mo.masks[2];

    uint8_t** gradient_strength = malloc(mat->height * sizeof(*gradient_strength));
    for (size_t i = 0; i < mat->height; i++) {
        gradient_strength[i] = malloc(mat->width * sizeof(gradient_strength[0]));
    }

    for (size_t i = mask.mask_radius; i < mat->height - mask.mask_radius - 1; i++) {
        for (size_t j = mask.mask_radius; j < mat->width - mask.mask_radius - 1; j++) {

            uint8_t pixel_value = calculate_mask(mat->matrix, j, i, mask);

            gradient_strength[i][j] = pixel_value;

        }
    }

    mat->matrix = gradient_strength;
}

/// <summary>
/// performs convolution with vertical sobel mask
/// </summary>
/// <param name="mat"></param>
void sobel_vertical(struct ImageMatrix* mat) {
    struct Masks mo = create_masks();
    struct Mask mask = mo.masks[1];

    uint8_t** gradient_strength = malloc(mat->height * sizeof(*gradient_strength));
    for (size_t i = 0; i < mat->height; i++) {
        gradient_strength[i] = malloc(mat->width * sizeof(gradient_strength[0]));
    }

    for (size_t i = mask.mask_radius; i < mat->height - mask.mask_radius - 1; i++) {
        for (size_t j = mask.mask_radius; j < mat->width - mask.mask_radius - 1; j++) {

            uint8_t pixel_value = calculate_mask(mat->matrix, j, i, mask);

            gradient_strength[i][j] = pixel_value;

        }
    }

    mat->matrix = gradient_strength;
}

/// <summary>
/// peforms non maximum suppresion on given bitmap matrix
/// </summary>
/// <param name="mat"></param>
/// <param name="edge_angle"></param>
void non_maximum_suppression(struct ImageMatrix* mat, int** edge_angle) {
    for (size_t i = 1; i < mat->height - 1; i++) {
        for (size_t j = 1; j < mat->width - 1; j++) {

            uint8_t pixel_value = mat->matrix[i][j];

            struct Neighbours n = get_neighbours(mat->matrix, j, i);

            switch (edge_angle[i][j]) {
            case 0:
                if (pixel_value < n.right && pixel_value < n.left) {
                    mat->matrix[i][j] = 0;
                }
                break;
            case 45:
                if (pixel_value >= n.upright && pixel_value >= n.downleft) {
                    mat->matrix[i][j] = 0;
                }
                break;
            case 90:
                if (pixel_value >= n.up && pixel_value >= n.down) {
                    mat->matrix[i][j] = 0;
                }
                break;
            case 135:
                if (pixel_value >= n.downright && pixel_value >= n.upleft) {
                    mat->matrix[i][j] = 0;
                }
                break;
            default:
                break;
            }
        }
    }
    
}

void hysteresis(struct ImageMatrix* mat, int** edge_angle, uint8_t threshold_high, uint8_t threshold_low) {
    for (size_t i = 1; i < mat->height - 1; ++i) {
        for (size_t j = 1; j < mat->width - 1; ++j) {
            uint8_t pixel_value = mat->matrix[i][j];

            struct Neighbours n = get_neighbours(mat->matrix, j, i);

            if (pixel_value > threshold_high) {
                mat->matrix[i][j] = 255;
            }
            else if (pixel_value < threshold_low) {
                mat->matrix[i][j] = 0;
            }
            else if (pixel_value < 0) {
                mat->matrix[i][j] = 0;
            }/*
            else {
                if (n.left >= threshold_high ||
                    n.right >= threshold_high ||
                    n.down >= threshold_high ||
                    n.up >= threshold_high ||
                    n.upleft >= threshold_high ||
                    n.upright >= threshold_high ||
                    n.downleft >= threshold_high ||
                    n.downright >= threshold_high) {
                    mat->matrix[i][j] = 255;
                }
                else {
                    mat->matrix[i][j] = 0;
                }
            }*/
        }
    }

}


/// <summary>
/// combines all operations to detect edges using cany algorithm
/// </summary>
/// <param name="mat">Image matrix to detect edges on</param>
/// <param name="thresholdUpper">Upper threshold of hysteresis</param>
/// <param name="thresholdLower">Lower threshold of hysteresis</param>
void detect_edges(struct ImageMatrix* mat, int thresholdUpper, int thresholdLower) {

    uint8_t** output = malloc(mat->height * sizeof(*output));
    for (size_t i = 0; i < mat->height; i++) {
        output[i] = malloc(mat->width * sizeof(output[0]));
    }

    int** edge_angle = malloc(mat->height * sizeof(*edge_angle));
    for (size_t i = 0; i < mat->height; i++) {
        edge_angle[i] = malloc(mat->width * sizeof(edge_angle[0]));
    }

    struct ImageMatrix gradient_vertical = copy_matrix(mat);
    struct ImageMatrix gradient_horizontal = copy_matrix(mat);

    sobel_vertical(&gradient_vertical);
    sobel_horizontal(&gradient_horizontal);

    long long pixel_value_sum = 0;
    long long variance = 0;

    for (size_t i = 0; i < mat->height; ++i) {
        for (size_t j = 0; j < mat->width; ++j) {
            uint8_t gx_value = gradient_horizontal.matrix[i][j];
            uint8_t gy_value = gradient_vertical.matrix[i][j];

            double value = sqrt((double)gx_value * gx_value + (double)gy_value * gy_value);
            if (value > 255) {
                value = 255;
            }

            mat->matrix[i][j] = (uint8_t)value;

            pixel_value_sum += (long long)value;

            double angle_radians = atan2((double)(gy_value), (double)gx_value);
            double angle_deg = angle_radians * 180.0 / 3.1415;

            if (angle_deg < 0) {
                angle_deg += 360;
            }
            if (angle_deg > 255) {
                angle_deg = 255;
            }

            angle_deg = fmod(angle_deg, 360.0);

            edge_angle[i][j] = approximate_angle(angle_deg);
        }
    }

    mean = (long long)pixel_value_sum / (mat->width * mat->height);

    for (size_t i = 0; i < mat->height; ++i) {
        for (size_t j = 0; j < mat->width; ++j) {
            long long difference = mat->matrix[i][j] - mean;
            variance += difference * difference;
        }
    }
    standard_deviation = (long long)sqrt(variance / (mat->width * mat->height));

    //non_maximum_suppression(mat, edge_angle);

    hysteresis(mat, edge_angle, thresholdUpper, thresholdLower);
    

}