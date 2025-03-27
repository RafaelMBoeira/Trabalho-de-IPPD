#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"
#include <stdio.h>
#include <stdlib.h>
#include "rpc_interface.h"

int main(int argc, char *argv[]) {
    CLIENT *clnt;
    image_request request;
    image_data *output;
    int width, height, channels;

    if (argc != 3){
        printf("Formatação de Entrada Incorreta!\nFormato Correto: ./{executável} {caminho da imagem} {operação}\n");
        return 1;
    }

    printf("Conectando...\n");
    clnt = clnt_create("localhost", RPC_PROG, RPC_VERS, "tcp");
    if (clnt == NULL) {
        printf("Falha na Conexão.\n");
        return 1;
    }
    printf("Conectado.\n");

    printf("Carregando Imagem\n");
    unsigned char *image = stbi_load(argv[1], &width, &height, &channels, 0);
    if (!image) {
        fprintf(stderr, "Falha ao Carregar a Imagem.\n");
        return 1;
    }
    printf("Imagem Carregada\n");

    request.img.width = width;
    request.img.height = height;
    request.img.channels = channels;
    request.img.pixels.pixels_len = width * height * channels;
    request.img.pixels.pixels_val = image;

    printf("Dimensões: %dx%dx%d\n", width, height, channels);

    if (strcmp(argv[2], "gray") == 0) {
        printf("Operação selecionada: grayscale\n");
        request.op = GRAYSCALE;
    } else if (strcmp(argv[2], "flip_h") == 0) {
        printf("Operação selecionada: Inversão Horizontal.\n");
        request.op = FLIP_H;
    } else if (strcmp(argv[2], "flip_v") == 0) {
        printf("Operação selecionada: Inversão Vertical.\n");
        request.op = FLIP_V;
    } else if (strcmp(argv[2], "bin") == 0) {
        printf("Operação selecionada: Binarização.\n");
        printf("Escolha o Limiar (0 - 255): ");
        scanf("%d", &request.arg);
        request.op = BINARIZE;
    } else if (strcmp(argv[2], "invert") == 0){
        printf("Operação selecionada: Inversão de Cores.\n");
        request.op = INVERT;
    } else if (strcmp(argv[2], "rotate") == 0){
        printf("Operação selecionada: Rotação.\n");
        request.op = ROTATION;
    } else {
        fprintf(stderr, "Operação Não Encontrada: %s.\n", argv[2]);
        return 1;
    }

    output = process_image_1(&request, clnt);
    if (output == NULL) {
        printf("Erro durante a realização da operação.\n");
        free(image);
        clnt_destroy(clnt);
        return 1;
    }

    printf("Salvando...\n");
    if (stbi_write_png("output.png", output->width, output->height, output->channels, output->pixels.pixels_val, output->width * output->channels))
        printf("Salvo.\n");
    else
        printf("Falha ao Salvar\n");

    free(image);
    clnt_destroy(clnt);
    return 0;
}