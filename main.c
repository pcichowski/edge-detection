#include "conio.h"
#include "stdio.h"
#include <math.h>

#include <stdint.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb-master/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb-master/stb_image_write.h"

#include "edge_detection_algorithms.h"


#define LICZBA_MASEK 3



uint8_t non_maximum_suppression(uint8_t** gradient_strength, uint8_t** edge_angle, size_t x, size_t y) {
    uint8_t value = gradient_strength[y][x];
    struct Neighbours n = get_neighbours(gradient_strength, x, y);

    switch (edge_angle[y][x]) {
        case 0:
            if (value >= n.right && value >= n.left) return value;
            else return 0;
            break;
        case 45:
            if (value >= n.upright && value >= n.downleft) return value;
            else return 0;
            break;
        case 90:
            if (value >= n.up && value >= n.down) return value;
            else return 0;
            break;
        case 135:
            if (value >= n.downright && value >= n.upleft) return value;
            else return 0;
            break;

        default:
            break;
    }
}

uint8_t hysteresis(uint8_t** nms, uint8_t** edge_angle, size_t x, size_t y, uint8_t threshold_min, uint8_t threshold_max) {
    uint8_t value = nms[y][x];
    struct Neighbours n = get_neighbours(nms, x, y);

    if (value > threshold_max) {
        return 255;
    }
    else if (value < 0) {
        return 0;
    }
    else {
        switch (edge_angle[y][x]) {
        case 0:
            if (n.up > threshold_max || n.down > threshold_max) return 255;
            else return 0;
            break;
        case 45:
            if (n.upright > threshold_max || n.downleft > threshold_max) return 255;
            else return 0;
            break;
        case 90:
            if (n.right > threshold_max || n.left > threshold_max) return 255;
            else return 0;
            break;
        case 135:
            if (n.downright > threshold_max || n.upleft > threshold_max) return 255;
            else return 0;
            break;

        default:
            break;
        }
    }
    
}

uint8_t hysteresis_universal(uint8_t** nms, size_t x, size_t y, uint8_t threshold_min, uint8_t threshold_max) {
    uint8_t value = nms[y][x];
    struct Neighbours n = get_neighbours(nms, x, y);

    if (value > threshold_max) {
        return 255;
    }
    else if (value < 0) {
        return 0;
    }
    else {
        if (n.up > threshold_max || n.down > threshold_max || n.right > threshold_max || n.left > threshold_max) {
            return 255;
        }
        else {
            return 0;
        }
    }

}

uint8_t approximate_angle(double angle) {
    if (((angle < 22.5) && (angle > -22.5)) || (angle > 157.5) || (angle < -157.5)) return 0;
    else if (((angle > 22.5) && (angle < 67.5)) || (angle < -112.5) || (angle > -157.5)) return 45;
    else if (((angle > 67.5) && (angle < 112.5)) || (angle < -67.5) || (angle > -112.5)) return 90;
    else if (((angle > 112.5) && (angle < 157.5)) || (angle < -22.5) || (angle > -67.5)) return 135;
}
uint8_t** detect_edges(uint8_t** input, size_t width, size_t height, struct Mask mask, uint8_t threshold_min, uint8_t threshold_max) {
    mask.mask_radius = mask.mask_size / 2;
    mask.mask_radius = (size_t)(mask.mask_size / 2);

    printf("uzywana maska :\n");
    for (int i = 0; i < mask.mask_size; i++) {
        for (int j = 0; j < mask.mask_size; j++) {
            printf("%d ", mask.mask[i][j]);
        }
        printf("\n");
    }

    uint8_t** gradient_strength = malloc(height * sizeof(*gradient_strength));
    for (size_t i = 0; i < height; i++) {
        gradient_strength[i] = malloc(width * sizeof(gradient_strength[0]));
    }
    uint8_t** output = malloc(height * sizeof(*output));
    for (size_t i = 0; i < height; i++) {
        output[i] = malloc(width * sizeof(output[0]));
    }

    if (gradient_strength) {
        // sobel operator convolution
        // calculates edge value and angle for each pixel
        for (size_t i = mask.mask_radius; i < height - mask.mask_radius - 1; i++) {
            for (size_t j = mask.mask_radius; j < width - mask.mask_radius - 1; j++) {

                uint8_t pixel_value = calculate_mask(input, j, i, mask);

                gradient_strength[i][j] = pixel_value;

            }
        }

        //hysteresis
        for (size_t i = mask.mask_radius; i < height - mask.mask_radius - 1; i++) {
            for (size_t j = mask.mask_radius; j < width - mask.mask_radius - 1; j++) {

                //output[i][j] = hysteresis_universal(gradient_strength, j, i, threshold_min, threshold_max);
                output[i][j] = gradient_strength[i][j];
                //output[i][j] = nms[i][j];
                //output[i][j] = edge_angle[i][j];

            }
        }
    }

    return output;
}

uint8_t** detect_edges_canny(uint8_t** input, size_t width, size_t height, struct Mask gx, struct Mask gy, uint8_t threshold_min, uint8_t threshold_max) {

    gx.mask_radius = gx.mask_size / 2;
    gy.mask_radius = (size_t)(gy.mask_size / 2);

    printf("uzywana maska gx :\n");
    for (int i = 0; i < gx.mask_size; i++) {
        for (int j = 0; j < gx.mask_size; j++) {
            printf("%d ", gx.mask[i][j]);
        }
        printf("\n");
    }
    printf("uzywana maska gy :\n");
    for (int i = 0; i < gy.mask_size; i++) {
        for (int j = 0; j < gy.mask_size; j++) {
            printf("%d ", gy.mask[i][j]);
        }
        printf("\n");
    }

    uint8_t** gradient_strength = malloc(height * sizeof(*gradient_strength));
    for (size_t i = 0; i < height; i++) {
        gradient_strength[i] = malloc(width * sizeof(gradient_strength[0]));
    }
    uint8_t** edge_angle = malloc(height * sizeof(*edge_angle));
    for (size_t i = 0; i < height; i++) {
        edge_angle[i] = malloc(width * sizeof(edge_angle[0]));
    }
    uint8_t** nms = malloc(height * sizeof(*nms));
    for (size_t i = 0; i < height; i++) {
        nms[i] = malloc(width * sizeof(nms[0]));
    }
    uint8_t** output = malloc(height * sizeof(*output));
    for (size_t i = 0; i < height; i++) {
        output[i] = malloc(width * sizeof(output[0]));
    }

    if (gradient_strength) {
        // sobel operator convolution
        // calculates edge value and angle for each pixel
        for (size_t i = gx.mask_radius; i < height - gx.mask_radius - 1; i++) {
            for (size_t j = gx.mask_radius; j < width - gx.mask_radius - 1; j++) {
                
                uint8_t gx_value = calculate_mask(input, j, i, gx);
                uint8_t gy_value = calculate_mask(input, j, i, gy);

                gradient_strength[i][j] = (uint8_t)(sqrt( (double)gx_value * gx_value + (double)gy_value * gy_value));
                //output[i][j] = gradient_strength[i][j];
                    
                double angle_radians = atan2( (double)(gy_value) ,(double)gx_value);
                double angle_deg = angle_radians * 180.0 / 3.1415;
                
                edge_angle[i][j] = approximate_angle(angle_deg);
                
            }
        }

        stbi_write_jpg("output_gradientstr.jpg", width, height, 1, matrix_to_image(gradient_strength, width, height), width * channels);

        // non-maximum suppression
        for (size_t i = gx.mask_radius; i < height - gx.mask_radius - 1; i++) {
            for (size_t j = gx.mask_radius; j < width - gx.mask_radius - 1; j++) {

                nms[i][j] = non_maximum_suppression(gradient_strength, edge_angle, j, i);
                
            }
        }

        //hysteresis
        for (size_t i = gx.mask_radius; i < height - gx.mask_radius - 1; i++) {
            for (size_t j = gx.mask_radius; j < width - gx.mask_radius - 1; j++) {
                 
                output[i][j] = hysteresis(nms, edge_angle, j, i, threshold_min, threshold_max);
                //output[i][j] = gradient_strength[i][j];
                //output[i][j] = nms[i][j];
                //output[i][j] = edge_angle[i][j];

            }
        }


    }
    return output;
}

int main() {
    
    //struct Mask maska_1;
    //struct Mask maska_2;

    
    //struct Mask* maski = malloc(LICZBA_MASEK * sizeof(maski[0]));
    //create_masks(&maski);

    

    int width, height;
    uint8_t* image = stbi_load("./images/sample_lizard_medium.jpg", &width, &height, &channels, 3);

    uint8_t* gray = get_channel(convert_to_grayscale(image, width, height), width, height, 0);

    struct ImageMatrix image_mat;
    image_mat.width = width;
    image_mat.height = height;
    image_mat.matrix = image_to_matrix(gray, width, height);
    /* end prologue*/
    /* perform calculations on   image_mat */

    gaussian_blur(&image_mat);

    struct Masks mo = create_masks();
    struct Mask mask = mo.masks[1];

    calculate_gradient_strength(&image_mat, mask);

    /* start epilogue */
    uint8_t* output = matrix_to_image(&image_mat);
    stbi_write_jpg("output_test_medium.jpg", width, height, 1, output, width * channels);

    stbi_image_free(image);
	return 0;
}