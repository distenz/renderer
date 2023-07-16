#include "tgaimage.h"
#include <cstdlib>

#define ARTIFACT_NAME "artifact.tga"
void drawLine(int,int,int,int,const TGAColor&,TGAImage&);

const TGAColor red{255,0,0,255};
const TGAColor green{0,255,0,255};

int main() {

    TGAImage image{100,100,TGAImage::RGB};

    drawLine(0,99,99,0,green,image);
    drawLine(0,0,99,40,green,image);
    drawLine(25,33,99,66,green,image);

    drawLine(0,50,99,50,red,image);
    drawLine(50,0,50,99,red,image);

    image.set(0,0, red);
    image.set(0,99, red);
    image.set(99,0,red);
    image.set(99,99, red);

    image.flip_vertically();
    image.write_tga_file(ARTIFACT_NAME);

    return 0;
}

void drawLine(int x0, int y0, int x1, int y1, const TGAColor& color, TGAImage& image) {


    bool isRotated {false};

    if (std::abs(x0-x1)<std::abs(y0-y1)) {
        std::swap(x0, y0);
        std::swap(x1, y1);
        isRotated = true;
    }

    if (x0>x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    
    int dx {x1-x0};
    int dy {y1-y0};
    int dt {std::abs(dy)};
    int error {0};
    int y {y0};
    bool m = y1 > y0;

    for (int x = x0; x <= x1; x++) {

        error += dt;
        if (error > dx) {
            y += m ? 1 : -1;
            error -= dx;
        }
        
        if (isRotated) {
            image.set(y,x,color);
        } else {
            image.set(x,y,color);
        }

    }
}
