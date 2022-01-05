#pragma once

size_t channels = 3;
#define LICZBA_MASEK 4

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
    size_t height;
    size_t length;
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
    uint8_t* output = malloc(width * height * channels * sizeof(output[0]));

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

/*// converts 1xN image matrix to grayscale
void convert_to_grayscale(struct ImageArray* image) {

    if (image->width > 0 && image->height > 0) {
        for (size_t i = 0; i < image->width * image->height * channels; i += channels) {

            uint8_t pixel_value = 0;
            // to convert to grayscale, each pixel's value is R/3 + G/3 + B/3
            pixel_value = (uint8_t)((double)image->array[i] * 0.2126 + (double)image->array[i + 1] * 0.7152 + image->array[i + 2] * 0.0722);

            image->array[i] = pixel_value;
            image->array[i + 1] = pixel_value;
            image->array[i + 2] = pixel_value;
        }
    }
}*/

// copies 1xN image matrix to another
struct ImageArray copy_image(struct ImageArray* image) {
    struct ImageArray output_image;
    if (image->width > 0 && image->height > 0) {
        
        output_image.width = image->width;
        output_image.height = image->height;
        uint8_t* output = malloc(image->width * image->height * channels);
        output_image.array = output;
        if (output) {
            for (size_t i = 0; i < image->width * image->height * channels; ++i) {
                output[i] = image->array[i];
            }
        }
    }
    return output_image;
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

    if (width > 0 && height > 0) {
        uint8_t** image = malloc(height * sizeof(*image));
        for (size_t i = 0; i < height; i++) {
            if (image) {
                image[i] = malloc(width * sizeof(image[0]));
            }
        }

        if (image) {
            for (size_t i = 0; i < height; ++i) {
                if (image[i]) {
                    for (size_t j = 0; j < width; ++j) {
                        image[i][j] = input[j + i * width];
                    }
                }
            }
        }

        return image;
    }

    return 0; //error
}

//converts NxN image matrix to 1xN
uint8_t* matrix_to_image(struct ImageMatrix* mat) {

    if (mat->width > 0 && mat->height > 0) {
        uint8_t* output = malloc(mat->width * mat->height * sizeof(output[0]));

        if (output) {
            for (size_t i = 0; i < mat->height; ++i) {
                for (size_t j = 0; j < mat->width; ++j) {
                    output[j + i * mat->width] = mat->matrix[i][j];
                }
            }
        }

        return output;
    }

    return 0; //error
}

