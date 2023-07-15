#include "tgaimage.h"
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <valarray>

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

    int start[2] {x0, y0};
    int deltas[2] {x1 - x0, y1 - y0};

    bool isRotated {deltas[1] > deltas[0]};

    int stepAxis = (int)isRotated;
    int stepDelta {deltas[(int)stepAxis]};

    int valueAxis = (int)(!stepAxis);
    int valueDelta = deltas[(int)valueAxis];

    float steepness = (float)valueDelta/stepDelta;
    float error = 0;
    int value = start[valueAxis];

    for(int i = 0; i <= stepDelta; i++) {

        int x = !isRotated * i + isRotated * value;
        int y = isRotated * i + !isRotated * value; 
        
        std::cout << x << ';' << y << '\n';

        image.set(x,y,color);

        error += steepness;
        if (error >= 0.5) {
            ++value;
            error = 0.0;
        }

        std::cout << error << '\n';
    }
}
