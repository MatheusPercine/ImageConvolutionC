#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <opencv2/opencv.hpp>

typedef struct {
    cv::Mat data;
    int height;
    int width;
    int channels;
    int pitch;
    const char* filepath;
    const char* name;
} Image;

//carregar imagem
Image* load_image(const char* filepath) {
    Image* img = (Image*)malloc(sizeof(Image));
    img->data = cv::imread(filepath, cv::IMREAD_COLOR);
    if (img->data.empty()) {
        fprintf(stderr, "Failed to load image from path: %s\n", filepath);
        free(img);
        return NULL;
    }
    img->height = img->data.rows;
    img->width = img->data.cols;
    img->channels = img->data.channels();
    img->filepath = filepath;
    img->name = strrchr(filepath, '/') ? strrchr(filepath, '/') + 1 : filepath;
    img->pitch = img->width;
    return img;
}

//criar uma imagem a partir de dados
Image* create_image_from_data(cv::Mat data) {
    Image* img = (Image*)malloc(sizeof(Image));
    img->data = data;
    img->height = data.rows;
    img->width = data.cols;
    img->channels = data.channels();
    img->filepath = NULL;
    img->name = "Image";
    img->pitch = img->width;
    return img;
}

//adicionar padding a imagem
void add_padding(Image* img, int padding_size_x, int padding_size_y) {
    cv::copyMakeBorder(img->data, img->data, padding_size_x, padding_size_x, padding_size_y, padding_size_y, cv::BORDER_REFLECT);
    img->pitch = img->width + 2 * padding_size_x;
}

//salvar imagem
void save_image(const Image* img, const char* output_path) {
    cv::imwrite(output_path, img->data);
}

//exibir imagem
void show_image(const Image* img) {
    cv::imshow(img->name, img->data);
    cv::waitKey(0);
    cv::destroyAllWindows();
}

// liberar memoria da imagem
void free_image(Image* img) {
    if (img) {
        free(img);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: ./main <input_image> <output_image>\n");
        return EXIT_FAILURE;
    }

    // Carrega imagem
    Image* image = load_image(argv[1]);
    if (!image) {
        return EXIT_FAILURE;
    }
,
    printf("Image loaded successfully from the path: %s\n", argv[1]);
    printf("Image dimensions: %d x %d - %d channels\n", image->height, image->width, image->channels);

    // teste
    add_padding(image, 10, 10);
    save_image(image, argv[2]);
    show_image(image);

    // Libera a memoria
    free_image(image);

    return EXIT_SUCCESS;
}