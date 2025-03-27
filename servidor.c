#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "rpc_interface.h"
#include <omp.h>
#include <math.h>

image_data* to_grayscale(image_data* img) {
    static image_data output;
    int img_size = img->width * img->height * img->channels;

    output.width = img->width;
    output.height = img->height;
    output.channels = img->channels;
    output.pixels.pixels_len = img_size;
    output.pixels.pixels_val = (unsigned char*) malloc(img_size);

    if (!output.pixels.pixels_val || img->channels < 3) 
        return NULL;

    clock_t start = clock();
    #pragma omp parallel for schedule(static, (int) fmax(1, (img_size * 0.2)))
        for (int i = 0; i < img_size; i += img->channels) {
            int gray = (img->pixels.pixels_val[i] + img->pixels.pixels_val[i + 1] + img->pixels.pixels_val[i + 2]) / 3;
            
            output.pixels.pixels_val[i] = gray;
            output.pixels.pixels_val[i + 1] = gray;
            output.pixels.pixels_val[i + 2] = gray;
        }    
    clock_t end = clock();
    printf("Operação Executada em %fs\n", ((double)(end - start))/CLOCKS_PER_SEC);

    return &output;
}

image_data* invert(image_data* img) {
    static image_data output;
    int img_size = img->width * img->height * img->channels;

    output.width = img->width;
    output.height = img->height;
    output.channels = img->channels;
    output.pixels.pixels_len = img_size;
    output.pixels.pixels_val = (unsigned char*) malloc(img_size);

    if (!output.pixels.pixels_val) 
        return NULL;

    clock_t start = clock();
    #pragma omp for schedule(static, (int) fmax(1, (img_size * 0.2)))
        for (int i = 0; i < img->width * img->height * img->channels; i++) 
            output.pixels.pixels_val[i] = (unsigned char) (255 - (int) img->pixels.pixels_val[i]);
    clock_t end = clock();
    printf("Operação Executada em %fs\n", ((double)(end - start))/CLOCKS_PER_SEC);

    return &output;
}

image_data* binarize(image_data* img, int threshold) {
    static image_data output;
    int img_size = img->width * img->height * img->channels;

    output.width = img->width;
    output.height = img->height;
    output.channels = img->channels;
    output.pixels.pixels_len = img_size;
    output.pixels.pixels_val = (unsigned char*) malloc(img_size);

    if (!output.pixels.pixels_val) 
        return NULL;

    clock_t start = clock();
    #pragma omp parallel for schedule(dynamic, (int) fmax(1, (img_size * 0.2)))
        for (int i = 0; i<img_size; i++)
            if (img->pixels.pixels_val[i] >= threshold)
                output.pixels.pixels_val[i] = (unsigned char) 255;
            else
                output.pixels.pixels_val[i] = (unsigned char) 0;
    clock_t end = clock();
    printf("Operação Executada em %fs\n", ((double)(end - start))/CLOCKS_PER_SEC);

    return &output;
}

image_data* horizontal_flip(image_data* img) {
    static image_data output;
    int img_size = img->width * img->height * img->channels;
    int row_size = img->width*img->channels;

    output.width = img->width;
    output.height = img->height;
    output.channels = img->channels;
    output.pixels.pixels_len = img_size;
    output.pixels.pixels_val = (unsigned char*) malloc(img_size);

    if (!output.pixels.pixels_val) 
        return NULL;

    clock_t start = clock();
    #pragma omp parallel for collapse(2) schedule(static, (int) fmax(1, (img_size * 0.2)))
        for (int i = 0; i < img->height; i++) {
            for (int j = 0; j <= img->width / 2; j++) {
                if ((img->width % 2 == 1 && j == img->width / 2) || (j < img->width / 2)) {
                    int left_index = (i * row_size) + j * img->channels;
                    int right_index = (i * row_size) + (img->width - 1 - j) * img->channels;

                    for (int k = 0; k < img->channels; k++) {    
                        output.pixels.pixels_val[left_index + k] = img->pixels.pixels_val[right_index + k];
                        output.pixels.pixels_val[right_index + k] = img->pixels.pixels_val[left_index + k];
                    }
                }
            }
        }

    clock_t end = clock();
    printf("Operação Executada em %fs\n", ((double)(end - start))/CLOCKS_PER_SEC);

    return &output;
}

image_data* vertical_flip(image_data* img) {
    static image_data output;
    int img_size = img->width * img->height * img->channels;

    output.width = img->width;
    output.height = img->height;
    output.channels = img->channels;
    output.pixels.pixels_len = img_size;
    output.pixels.pixels_val = (unsigned char*) malloc(img_size);

    if (!output.pixels.pixels_val) 
        return NULL;
    
    clock_t start = clock();
    #pragma omp parallel for collapse(2) schedule(dynamic, (int) fmax(1, (img_size * 0.2)))
        for (int i = 0; i <= img->height / 2; i++) {
            for (int j = 0; j < img->width * img->channels; j++) {
                if ((img->height % 2 == 1 && i == img->height / 2) || (i < img->height / 2)){
                    int top_index = i * img->width * img->channels + j;
                    int bottom_index = (img->height - 1 - i) * img->width * img->channels + j;
                    
                    output.pixels.pixels_val[top_index] = img->pixels.pixels_val[bottom_index];
                    output.pixels.pixels_val[bottom_index] = img->pixels.pixels_val[top_index];
                }
            }
        }
    clock_t end = clock();
    printf("Operação Executada em %fs\n", ((double)(end - start))/CLOCKS_PER_SEC);
    
    return &output;
}

image_data* clockwise_rotation(image_data* img){
    static image_data output;
    int img_size = img->width * img->height * img->channels;

    output.width = img->height;
    output.height = img->width;
    output.channels = img->channels;
    output.pixels.pixels_len = img_size;
    output.pixels.pixels_val = (unsigned char*) malloc(img_size);

    if (!output.pixels.pixels_val) 
        return NULL;
    
    clock_t start = clock();
    #pragma omp parallel for collapse(3) schedule(dynamic, (int) fmax(1, (img_size * 0.2)))
        for (int i = 0; i < img->height; i++) {
            for (int j = 0; j < img->width; j++) {
                for (int k = 0; k < img->channels; k++) {
                    output.pixels.pixels_val[((j * img->height) + (img->height - 1 - i)) * img->channels + k] = img->pixels.pixels_val[(i * img->width + j) * img->channels + k];
                }
            }
        }
    clock_t end = clock();
    printf("Operação Executada em %fs\n", ((double)(end - start))/CLOCKS_PER_SEC);
    
    return &output;
}

image_data* process_image_1_svc(image_request* request, struct svc_req* rqstp) {
    static image_data* output = NULL;

    int avaliable_threads = omp_get_num_threads();
    if (avaliable_threads > 2)
        omp_set_num_threads(avaliable_threads - 2);
    else
        omp_set_num_threads(avaliable_threads);

    switch (request->op) {
        case GRAYSCALE:
            printf("Convertendo Imagem para Grayscale...\n");
            output = to_grayscale(&request->img);
            break;
        case FLIP_H:
            printf("Invertendo Horizontalmente...\n");
            output = horizontal_flip(&request->img); 
            break;
        case FLIP_V:
            printf("Invertendo Verticalmente...\n");
            output = vertical_flip(&request->img);
            break;
        case BINARIZE:
            printf("Binarizando a Imagem...\n");
            output = binarize(to_grayscale(&request->img), request->arg);
            break;
        case INVERT:
            printf("Invertendo as Cores da Imagem...\n");
            output = invert(&request->img);
            break;
        case ROTATION:
            printf("Rotacionando Imagem...\n");
            output = clockwise_rotation(&request->img);
            break;
        default:
            fprintf(stderr, "Operação Desconhecida\n");
            return NULL;
    }
    if (output == NULL){
        printf("Erro durante a realização da operação.\n");
    }
    printf("---------------------------------------------------------------\n");
    return output;
}
