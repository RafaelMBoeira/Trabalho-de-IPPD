enum operation_type {
    GRAYSCALE = 1,
    FLIP_H = 2,
    FLIP_V = 3,
    BINARIZE = 4,
    INVERT = 5,
    ROTATION = 6
};

struct image_data {
    int width;
    int height;
    int channels;
    opaque pixels<>;
};

struct image_request {
    int arg;
    operation_type op;
    image_data img;
};

program RPC_PROG {           
    version RPC_VERS {   
        image_data process_image(image_request) = 1;
    } = 1;                 
} = 0x20000001;    