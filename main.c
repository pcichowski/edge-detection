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


int main() { 

    int width, height;
    uint8_t* image = stbi_load("./images/sample_domek.jpg", &width, &height, &channels, 3);

    uint8_t* gray = get_channel(convert_to_grayscale(image, width, height), width, height, 0);

    struct ImageMatrix image_mat;
    image_mat.width = width;
    image_mat.height = height;
    image_mat.matrix = image_to_matrix(gray, width, height);

    gaussian_blur(&image_mat);


    detect_edges(&image_mat);

    uint8_t* output = matrix_to_image(&image_mat);
    stbi_write_jpg("output_domek.jpg", width, height, 1, output, width * channels);

    stbi_image_free(image);
	return 0;
}