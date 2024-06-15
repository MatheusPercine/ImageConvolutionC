#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "convolution.h"
#include "image.h"
#include "kernel.h"

void usage() {
    printf("Usage: ./main <input_image> <output_image>\n");
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        usage();
        exit(EXIT_FAILURE);
    }

    // Load the image
    Image *image = load_image(argv[1]);
    if (!image) {
        fprintf(stderr, "Failed to load image from path: %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    printf("Image loaded successfully from the path: %s\n", argv[1]);
    printf("Image dimensions: %d x %d - %d channels\n", image->height, image->width, image->channels);

    printf("\nSelect the kernel you want to apply:\n");
    printf("1. Sharpen\n");
    printf("2. Edge Detection\n");
    printf("3. Emboss\n");
    printf("4. Gaussian\n");
    int kernel_choice;
    printf("Enter the kernel number: ");
    scanf("%d", &kernel_choice);

    Kernel2D *selected_kernel = NULL;
    int kernel_size;
    float sigma;

    switch (kernel_choice) {
        case 1:
            selected_kernel = create_kernel(sharpen_kernel, 3);
            break;
        case 2:
            printf("Enter the size of the kernel: ");
            scanf("%d", &kernel_size);
            if (kernel_size % 2 == 0) {
                fprintf(stderr, "Kernel size must be odd\n");
                exit(EXIT_FAILURE);
            }
            selected_kernel = create_kernel(edge_detection_kernel(kernel_size), kernel_size);
            break;
        case 3:
            selected_kernel = create_kernel(emboss_kernel, 3);
            break;
        case 4:
            printf("Enter the size of the kernel: ");
            scanf("%d", &kernel_size);
            printf("Enter the standard deviation of the Gaussian distribution: ");
            scanf("%f", &sigma);
            if (kernel_size % 2 == 0) {
                fprintf(stderr, "Kernel size must be odd\n");
                exit(EXIT_FAILURE);
            }
            selected_kernel = create_kernel(gaussian_kernel(kernel_size, sigma), kernel_size);
            break;
        default:
            fprintf(stderr, "Invalid kernel choice\n");
            exit(EXIT_FAILURE);
    }

    // Apply the selected kernel and measure the time
    clock_t start_time = clock();
    printf("Applying the kernel...\n");
    Image *result_image = apply_convolution(image, selected_kernel);
    clock_t end_time = clock();
    
    printf("Time taken to apply the kernel: %.2f seconds\n", (double)(end_time - start_time) / CLOCKS_PER_SEC);

    // Save the image
    if (!save_image(result_image, argv[2])) {
        fprintf(stderr, "Failed to save image to path: %s\n", argv[2]);
        exit(EXIT_FAILURE);
    }
    printf("Image saved successfully to the path: %s\n", argv[2]);

    // Display the image - this might be platform-specific and may need to be implemented accordingly
    // display_image(result_image);

    // Clean up
    free_image(image);
    free_image(result_image);
    free_kernel(selected_kernel);

    return 0;
}
