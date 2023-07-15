#include "tgaimage.h"
#include <cstdlib>
#include <iostream>

void drawLine(int,int,int,int,const TGAColor&,TGAImage&);

const TGAColor red{255,0,0,255};
const TGAColor green{0,255,0,255};

int main() {

    TGAImage image{100,100,TGAImage::RGB};

    drawLine(0,-100,100,100,green,image);

    image.set(0,0, red);
    image.set(0,99, red);
    image.set(99,0,red);
    image.set(99,99, red);

    image.flip_vertically();
    image.write_tga_file("image.tga");

    return 0;
}

/**
 * Primitive line drawing algo
 */
void drawLine(int x0, int y0, int x1, int y1, const TGAColor& color, TGAImage& image) {

    int rangeX {x1 - x0};
    int rangeY {y1 - y0};

    for(int xi = rangeX; xi >= 0; xi--) {
        int x,y;
        double progress;

        x = x0 + xi;
        progress = (double)x/rangeX;
        y = y0 + rangeY * progress;

        std::cout << x << ';' << y << '\n';

        image.set(x,y,color);
    }
}
