#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void print_matrix(float** matrix, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            printf("%f ", matrix[i][j]);
        }
        printf("\n");
    }
}

float** allocate_matrix(int size) {
    float** matrix = (float**)malloc(size * sizeof(float*));
    for (int i = 0; i < size; i++) {
        matrix[i] = (float*)malloc(size * sizeof(float));
    }
    return matrix;
}

void free_matrix(float** matrix, int size) {
    for (int i = 0; i < size; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

float** gaussian_kernel(int size, float sigma) {
    float** kernel = allocate_matrix(size);
    float sum = 0.0;
    int half_size = size / 2;

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            float x = i - half_size;
            float y = j - half_size;
            kernel[i][j] = exp(-(x*x + y*y) / (2 * sigma * sigma));
            sum += kernel[i][j];
        }
    }

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            kernel[i][j] /= sum;
        }
    }

    return kernel;
}

float** edge_detection_kernel(int size) {
    if (size % 2 == 0) {
        fprintf(stderr, "Size of the kernel must be an odd number\n");
        exit(EXIT_FAILURE);
    }

    float** kernel = allocate_matrix(size);
    int center = size / 2;

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            kernel[i][j] = -1;
        }
    }

    kernel[center][center] = size * size - 1;

    return kernel;
}

float** create_kernel_from_array(float* array, int size) {
    float** kernel = allocate_matrix(size);
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            kernel[i][j] = array[i * size + j];
        }
    }
    return kernel;
}

typedef struct {
    float** kernel;
    int height;
    int width;
    int center_x;
    int center_y;
} Kernel2D;

Kernel2D* create_kernel2D(float** kernel, int size) {
    Kernel2D* k = (Kernel2D*)malloc(sizeof(Kernel2D));
    k->kernel = kernel;
    k->height = size;
    k->width = size;
    k->center_x = size / 2;
    k->center_y = size / 2;
    return k;
}

int main() {
    printf("Testing the kernel module\n");

    printf("Gaussian Kernel:\n");
    float** gauss_kernel = gaussian_kernel(5, 1.0);
    print_matrix(gauss_kernel, 5);
    free_matrix(gauss_kernel, 5);

    printf("Edge Detection Kernel:\n");
    float** edge_kernel = edge_detection_kernel(3);
    print_matrix(edge_kernel, 3);
    free_matrix(edge_kernel, 3);

    printf("Sharpen Kernel:\n");
    float sharpen_array[3 * 3] = {0, -1, 0, -1, 5, -1, 0, -1, 0};
    float** sharpen_kernel = create_kernel_from_array(sharpen_array, 3);
    print_matrix(sharpen_kernel, 3);
    free_matrix(sharpen_kernel, 3);

    printf("Emboss Kernel:\n");
    float emboss_array[3 * 3] = {-1, 0, 0, 0, 0, 0, 0, 0, 1};
    float** emboss_kernel = create_kernel_from_array(emboss_array, 3);
    print_matrix(emboss_kernel, 3);
    free_matrix(emboss_kernel, 3);

    printf("Identity Kernel:\n");
    float identity_array[3 * 3] = {0, 0, 0, 0, 1, 0, 0, 0, 0};
    float** identity_kernel = create_kernel_from_array(identity_array, 3);
    print_matrix(identity_kernel, 3);
    free_matrix(identity_kernel, 3);

    return 0;
}