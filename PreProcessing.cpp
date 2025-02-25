#include "PreProcessing.h"
#include <iostream>
#include <cstdio>
#include <cstddef>
#include <jpeglib.h>
#include <cmath>

Image PreProcessing::get_image_from_file(const char* filename) {
    Image image{};

    FILE *file = fopen(filename, "rb");
    if (!file) {
        std::cerr<<"Couldn't read the image"<<std::endl;
        return image;
    }

    jpeg_decompress_struct cinfo{};
    jpeg_error_mgr jerr{};
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, file);
    jpeg_read_header(&cinfo, TRUE);
    jpeg_start_decompress(&cinfo);

    image.width = cinfo.output_width;
    image.height = cinfo.output_height;
    image.channels = cinfo.output_components;
    image.image.resize(image.width*image.height*image.channels);

    JSAMPROW row_pointer[1];
    while (cinfo.output_scanline < image.height) {
        row_pointer[0] = &image.image[cinfo.output_scanline*image.width*image.channels];
        jpeg_read_scanlines(&cinfo, row_pointer, 1);
    }

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(file);
    return image;
}

Image PreProcessing::rgb_to_gray(const Image &image) {
    Image gs_img{};
    if (!image.width) {
        return gs_img;
    }

    gs_img.width = image.width;
    gs_img.height = image.height;
    gs_img.channels = 1;
    gs_img.image.resize(image.image.size()/3);

    for (int i = 0; i < image.image.size()/3; i++) {
        const double c_linear = 0.2126 * image.image.at(3*i) + 0.7152 * image.image.at(3*i+1) + 0.0722 * image.image.at(3*2);
        const unsigned int val = floor(c_linear);
        gs_img.image.at(i) = val;
    }

    return gs_img;
}

Image PreProcessing::resize(const Image &image, const int new_width, const int new_height) {
    Image rszd_img{};
    if (!image.width) {
        return rszd_img;
    }

    const float x_rat = static_cast<float>(image.width) / static_cast<float>(new_width);
    const float y_rat = static_cast<float>(image.height) / static_cast<float>(new_height);

    rszd_img.width = new_width;
    rszd_img.height = new_height;
    rszd_img.channels = image.channels;
    rszd_img.image.resize(new_height*new_width*rszd_img.channels);

    for (int x = 0; x < new_width; x++) {
        for (int y = 0; y < new_height; y++) {
            const int old_x = static_cast<int>(static_cast<float>(x) * x_rat);
            const int old_y = static_cast<int>(static_cast<float>(y) * y_rat);
            const int old_pos = static_cast<int>(old_y * image.width + old_x) * image.channels;
            const int new_pos = (y * new_width + x) * image.channels;

            for (int c = 0; c < image.channels; c++) {
                rszd_img.image[new_pos + c] = image.image[old_pos + c];
            }
        }
    }

    return rszd_img;
}

Image PreProcessing::process(const char* filename) {
    const Image img = get_image_from_file(filename);
    Image gs_img = rgb_to_gray(img);
    Image resized = resize(gs_img, 64, 128);
    return resized;
}

void PreProcessing::save(const char *filename, const Image &image, int quality) {
    FILE *file = fopen(filename, "wb");

    jpeg_compress_struct cinfo{};
    jpeg_error_mgr jerr{};
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
    jpeg_stdio_dest(&cinfo, file);

    cinfo.image_height = image.height;
    cinfo.image_width = image.width;
    cinfo.input_components = image.channels;
    cinfo.in_color_space = (image.channels == 1) ? JCS_GRAYSCALE : JCS_RGB;
    cinfo.data_precision = 8;

    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, quality, TRUE);

    jpeg_start_compress(&cinfo, TRUE);
    JSAMPROW row_pointer[1];

    while (cinfo.next_scanline < cinfo.image_height) {
        row_pointer[0] = const_cast<unsigned char *>(&image.image[cinfo.next_scanline*cinfo.image_width*cinfo.input_components]);
        jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }

    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);
    fclose(file);
}



