#include "tgaimage.h"

const TGAColor red = TGAColor(255,0,0,255);
int main() {

    TGAImage image(100,100,TGAImage::RGB);
    image.set(50, 75, red); 
    image.flip_vertically();
    image.write_tga_file("image.tga");

    return 0;
}
