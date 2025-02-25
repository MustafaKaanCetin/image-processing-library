#ifndef PREPROCESSING_H
#define PREPROCESSING_H

#include <vector>

struct Image {
    unsigned int width;
    unsigned int height;
    int channels;
    std::vector<unsigned char> image;
};

class PreProcessing {
public:
    PreProcessing() = default;
    ~PreProcessing() = default;

    static Image get_image_from_file(const char* filename);
    static Image rgb_to_gray(const Image &image);
    static Image resize(const Image &image, int new_width, int new_height);
    static Image process(const char* filename);
    static void save(const char* filename, const Image &image, int quality = 100);
};

#endif //PREPROCESSING_H
