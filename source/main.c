#include "conio.h"
#include "stdio.h"
#include <math.h>

#include <stdint.h>
#include <Windows.h>

#define STB_IMAGE_IMPLEMENTATION
#include "../stb-master/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../stb-master/stb_image_write.h"

#include "edge_detection_algorithms.h"


void executeForAllImages();

void detectEdges(const char*  inputPath, const char* outputPath);

int main() {
    
    
    executeForAllImages();
	return 0;
}

void detectEdges(const char* inputPath, const char* outputPath) {
    int width, height;
    uint8_t* image = stbi_load(inputPath, &width, &height, &channels, 3);

    if (image == NULL) {
        printf("cannot load image %s\n", inputPath);
        return;
    }

    uint8_t* gray = get_channel(convert_to_grayscale(image, width, height), width, height, 0);

    struct ImageMatrix image_mat;
    image_mat.width = width;
    image_mat.height = height;
    image_mat.matrix = image_to_matrix(gray, width, height);

    gaussian_blur(&image_mat);

    detect_edges(&image_mat, 180, 70);

    uint8_t* output = matrix_to_image(&image_mat);
    stbi_write_jpg(outputPath, width, height, 1, output, width * channels);

    stbi_image_free(image);

}

void executeForAllImages() {
    const wchar_t* inputDir = L".\\images\\input";
    const wchar_t* outputDir = L".\\images\\output";

    WIN32_FIND_DATA fdFile;
    HANDLE hFind = NULL;

    wchar_t inputPath[2048];
    wchar_t outputPath[2048];

    //Specify a file mask. *.* = We want everything! 
    wsprintf(inputPath, L"%s\\*.jpg", inputDir);

    if ((hFind = FindFirstFile(inputPath, &fdFile)) == INVALID_HANDLE_VALUE)
    {
        wprintf(L"Path not found: [%s]\n", inputDir);
        return;
    }
    wprintf(L"starting\n\n");
    do
    {
        //Find first file will always return "."
        //    and ".." as the first two directories. 
        if (wcscmp(fdFile.cFileName, L".") != 0
            && wcscmp(fdFile.cFileName, L"..") != 0)
        {
            //Build up our file path using the passed in 
            //  [sDir] and the file/foldername we just found: 
            wsprintf(inputPath, L"%s\\%s", inputDir, fdFile.cFileName);
            wsprintf(outputPath, L"%s\\%s", outputDir, fdFile.cFileName);

            //Is the entity a File or Folder? 
            if (fdFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                
            }
            else {
                wprintf(L"Detecting edges on file: %s\n", inputPath);

                char inputPathByte[2048];
                char outputPathByte[2048];

                wcstombs(inputPathByte, inputPath, 2048);
                wcstombs(outputPathByte, outputPath, 2048);
                
                detectEdges(inputPathByte, outputPathByte);

                wprintf(L"  done\n");
            }
        }
    } while (FindNextFile(hFind, &fdFile)); //Find the next file. 

    FindClose(hFind); //Always, Always, clean things up! 

    return;
}
