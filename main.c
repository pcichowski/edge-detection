#include "conio.h"
#include "stdio.h"
#include <math.h>

#include <stdint.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb-master/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb-master/stb_image_write.h"

size_t channels;

struct ImageMatrix {
    uint8_t** matrix;
    size_t width;
    size_t height;
    size_t offset_x;
    size_t offset_y;
};

struct ImageArray {
    uint8_t* array;
    size_t width;
};

struct Mask {
    int** mask;
    size_t mask_size;
    size_t mask_radius;
};

struct Neighbours {
    uint8_t up;
    uint8_t down;
    uint8_t right;
    uint8_t left;

    uint8_t upright;
    uint8_t upleft;
    uint8_t downright;
    uint8_t downleft;
};

// converts 1xN image matrix to grayscale
uint8_t* convert_to_grayscale(uint8_t* input, size_t width, size_t height) {
    uint8_t* output = malloc(width * height * channels);

    if (output) {
        for (size_t i = 0; i < width * height * channels; i += channels) {

            uint8_t pixel_value = 0;
            // to convert to grayscale, each pixel's value is R/3 + G/3 + B/3
            pixel_value = (uint8_t)((double)input[i] * 0.2126 + (double)input[i + 1] * 0.7152 + input[i + 2] * 0.0722);

            output[i] = pixel_value;
            output[i + 1] = pixel_value;
            output[i + 2] = pixel_value;
        }
    }

    return output;
}

// copies 1xN image matrix to another
uint8_t* copy_image(uint8_t* input, size_t width, size_t height) {
    uint8_t* output = malloc(width * height * channels);
    
    if (output) {
        for (size_t i = 0; i < width * height * channels; ++i) {
            output[i] = input[i];
        }
    }

    return output;
}

// extracts chosen channel from 1xN image matrix
uint8_t* get_channel(uint8_t* input, size_t width, size_t height, size_t channel_number) {

    uint8_t* output = malloc(width * height);

    if (output) {
        for (size_t i = 0; i < width * height * channels; i += channels) {
            output[i / channels] = input[i + channel_number];
        }
    }

    return output;
}



// converts 1xN image matrix to NxN
uint8_t** image_to_matrix(uint8_t* input, size_t width, size_t height) {

    uint8_t** image = malloc(height * sizeof(*image));
    for (size_t i = 0; i < height; i++) {
        image[i] = malloc(width * sizeof(image[0]));
    }

    if (image) {
        for (size_t i = 0; i < height; ++i) {
            for (size_t j = 0; j < width; ++j) {
                image[i][j] = input[j + i * width];
            }
        }
    }

    return image;
}

//converts NxN image matrix to 1xN
uint8_t* matrix_to_image(uint8_t** input, size_t width, size_t height) {

    uint8_t* output = malloc(width * height * sizeof(output[0]));

    if (output) {
        for (size_t i = 0; i < height; ++i) {
            for (size_t j = 0; j < width; ++j) {
                output[j + i * width] = input[i][j];
            }
        }
    }

    return output;
}

struct Neighbours get_neighbours(uint8_t** matrix, size_t x, size_t y) {
    struct Neighbours n;

    n.up = matrix[y - 1][x];
    n.down = matrix[y + 1][x];
    n.right = matrix[y][x + 1];
    n.left = matrix[y][x - 1];
    n.upright = matrix[y - 1][x + 1];
    n.upleft = matrix[y - 1][x - 1];
    n.downright = matrix[y + 1][x + 1];
    n.downleft = matrix[y + 1][x - 1];

    return n;
}

uint8_t calculate_mask(uint8_t** input, size_t x, size_t y, struct Mask mask) {
    mask.mask_radius = (size_t)(mask.mask_size / 2);
    size_t sum = 0;
    for (size_t i = y - mask.mask_radius; i <= y + mask.mask_radius; ++i) {
        for (size_t j = x - mask.mask_radius; j <= x + mask.mask_radius; ++j) {

            int image_val = input[i][j];
           
            int mask_val = mask.mask[i - y + mask.mask_radius][j - x + mask.mask_radius];
            //printf("%d %d\n", image_val, mask_val);

            sum += image_val * mask_val;
        }
    }
    //printf("%d\n\n", sum);
    if (sum < 0) sum = 0;
    if (sum > 255) sum = 255;
    return (uint8_t)sum;
}

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

    size_t width, height;
    uint8_t* image = stbi_load("sample_park.jpg", &width, &height, &channels, 3);

    if (image) {
        //printf("first %d pixels\n", 10);
        //printf("width = %d, height = %d, num of channels = %d \n", width, height, channels);
        for (size_t i = 0; i < 10 * channels; i++) {
            //printf("%x%s", image[i], ((i + 1) % channels) ? " " : "\n");
        }
        printf("\n");
    }

    struct Mask maska_1;
    struct Mask maska_2;

    // maska
    int** mask1 = malloc(3 * sizeof(*mask1));
    for (size_t i = 0; i < 3; i++) {
        mask1[i] = malloc(3 * sizeof(mask1[0]));
    }
    mask1[0][0] = -1; mask1[0][1] = -2; mask1[0][2] = -1;
    mask1[1][0] = 0;  mask1[1][1] = 0; mask1[1][2] = 0;
    mask1[2][0] = 1; mask1[2][1] = 2; mask1[2][2] = 1;

    maska_1.mask = mask1;
    maska_1.mask_size = 3;

    int** mask2 = malloc(3 * sizeof(*mask2));
    for (size_t i = 0; i < 3; i++) {
        mask2[i] = malloc(3 * sizeof(mask2[0]));
    }
    mask2[0][0] = -1; mask2[0][1] = 0; mask2[0][2] = 1;
    mask2[1][0] = -2;  mask2[1][1] = 0; mask2[1][2] = 2;
    mask2[2][0] = -1; mask2[2][1] = 0; mask2[2][2] = 1;

    maska_2.mask = mask2;
    maska_2.mask_size = 3;

    struct Mask maska_3;
    int** mask3 = malloc(3 * sizeof(*mask3));
    for (size_t i = 0; i < 3; i++) {
        mask3[i] = malloc(3 * sizeof(mask3[0]));
    }
    mask3[0][0] = 0; mask3[0][1] = 1; mask3[0][2] = 2;
    mask3[1][0] = -1;  mask3[1][1] = 0; mask3[1][2] = 1;
    mask3[2][0] = -2; mask3[2][1] = -1; mask3[2][2] = 0;

    maska_3.mask = mask3;
    maska_3.mask_size = 3;

    

    uint8_t* gray = get_channel(convert_to_grayscale(image, width, height), width, height, 0); // get first channel of image in grayscale


    //uint8_t** test = image_to_matrix(image, width, height);

    //uint8_t** edges = detect_edges_canny(image_to_matrix(gray, width, height), width, height, maska_2, maska_1, 60, 180);
    uint8_t** edges = detect_edges(image_to_matrix(gray, width, height), width, height, maska_3, 60, 180);

    uint8_t* output = matrix_to_image(edges, width, height);

    stbi_write_jpg("output_test.jpg", width, height, 1, output, width * channels);


    stbi_image_free(image);

	return 0;
}