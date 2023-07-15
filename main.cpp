#include "tgaimage.h"
#include <cstdlib>
#include <iostream>

void drawLine(int,int,int,int,const TGAColor&,TGAImage&);

const TGAColor red{255,0,0,255};
const TGAColor green{0,255,0,255};

int main() {

    TGAImage image{100,100,TGAImage::RGB};

    drawLine(0,0,100,10,green,image);
    drawLine(0,0,20,100,green,image);

    drawLine(0,50,100,50,red,image);
    drawLine(50,0,50,100,red,image);

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

    int ranges[2] {x1 - x0, y1 - y0};
    int longerRange = ranges[(int)(ranges[1] > ranges[0])];

    for(int i = longerRange; i >= 0; i--) {

        int currentX,currentY;
        float progress = (float)i/longerRange;

        currentX = x0 + ranges[0] * progress;
        currentY = y0 + ranges[1] * progress;

        std::cout << currentX << ';' << currentY << '\n';

        image.set(currentX,currentY,color);
    }
}
