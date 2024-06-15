#include <stdio.h>
#include <stdlib.h>
#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui_c.h>
#include <omp.h>

typedef struct {
    int height;
    int width;
    int channels;
    unsigned char* data;
} Image;

typedef struct {
    int height;
    int width;
    float* data;
} Kernel;

Image* load_image(const char* filepath) {
    IplImage* img = cvLoadImage(filepath, CV_LOAD_IMAGE_COLOR);
    if (!img) {
        fprintf(stderr, "Failed to load image from path: %s\n", filepath);
        return NULL;
    }
    Image* image = (Image*)malloc(sizeof(Image));
    image->height = img->height;
    image->width = img->width;
    image->channels = img->nChannels;
    image->data = (unsigned char*)malloc(image->height * image->width * image->channels);
    memcpy(image->data, img->imageData, image->height * image->width * image->channels);
    cvReleaseImage(&img);
    return image;
}

void save_image(const char* filepath, const Image* image) {
    IplImage* img = cvCreateImage(cvSize(image->width, image->height), IPL_DEPTH_8U, image->channels);
    memcpy(img->imageData, image->data, image->height * image->width * image->channels);
    cvSaveImage(filepath, img, 0);
    cvReleaseImage(&img);
}

void free_image(Image* image) {
    if (image) {
        free(image->data);
        free(image);
    }
}

Image* convolution(const Image* image, const Kernel* kernel) {
    int image_height = image->height;
    int image_width = image->width;
    int image_channels = image->channels;
    int kernel_height = kernel->height;
    int kernel_width = kernel->width;
    int kernel_center_x = kernel_width / 2;
    int kernel_center_y = kernel_height / 2;

    Image* new_image = (Image*)malloc(sizeof(Image));
    new_image->height = image_height;
    new_image->width = image_width;
    new_image->channels = image_channels;
    new_image->data = (unsigned char*)calloc(image_height * image_width * image_channels, sizeof(unsigned char));

    #pragma omp parallel for collapse(3)
    for (int c = 0; c < image_channels; c++) {
        for (int x = 0; x < image_height; x++) {
            for (int y = 0; y < image_width; y++) {
                float sum = 0.0;
                for (int j = 0; j < kernel_height; j++) {
                    for (int k = 0; k < kernel_width; k++) {
                        int x_k = x + j - kernel_center_y;
                        int y_k = y + k - kernel_center_x;
                        if (x_k >= 0 && y_k >= 0 && x_k < image_height && y_k < image_width) {
                            sum += image->data[(x_k * image_width + y_k) * image_channels + c] * kernel->data[j * kernel_width + k];
                        }
                    }
                }
                new_image->data[(x * image_width + y) * image_channels + c] = (unsigned char)sum;
            }
        }
    }
    return new_image;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input_image> <output_image>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Load the image
    Image* image = load_image(argv[1]);
    if (!image) {
        return EXIT_FAILURE;
    }

    // Define a simple kernel (e.g., a 3x3 sharpen kernel)
    Kernel kernel = {3, 3, (float[9]){
        0, -1, 0,
       -1,  5, -1,
        0, -1, 0
    }};

    // Apply the kernel
    Image* result_image = convolution(image, &kernel);

    // Save the image
    save_image(argv[2], result_image);

    // Free resources
    free_image(image);
    free_image(result_image);

    return EXIT_SUCCESS;
}