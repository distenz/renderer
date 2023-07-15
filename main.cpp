#include "tgaimage.h"
#include <cmath>
#include <cstdlib>
#include <iostream>

void drawLine(int,int,int,int,const TGAColor&,TGAImage&);

const TGAColor red{255,0,0,255};
const TGAColor green{0,255,0,255};

int main() {

    TGAImage image{1000,1000,TGAImage::RGB};

    drawLine(0,0,1000,100,green,image);
    drawLine(0,0,200,1000,green,image);

    drawLine(0,500,1000,500,red,image);
    drawLine(500,0,500,1000,red,image);

    image.set(0,0, red);
    image.set(0,999, red);
    image.set(999,0,red);
    image.set(999,999, red);

    image.flip_vertically();
    image.write_tga_file("image.tga");

    return 0;
}

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
        
        //std::cout << x << ';' << y << '\n';

        image.set(x,y,color);

        error += steepness;
        if (error >= 0.5) {
            ++value;
            error = 0.0;
        }

        //std::cout << error << '\n';
    }
}
