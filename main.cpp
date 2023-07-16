#include "geometry.h"
#include "model.h"
#include "tgaimage.h"
#include <cstdlib>
#include <iostream>
#include <vector>

#define ARTIFACT_NAME "artifact.tga"
#define DEBUG true

void drawLine(int,int,int,int,const TGAColor&,TGAImage&);

const TGAColor red{255,0,0,255};
const TGAColor green{0,255,0,255};
const int width {800};
const int height {800};

Model* model = nullptr;

int main() {

    TGAImage image{width,height,TGAImage::RGB};
    
    model = new Model{"./obj/head.obj"};

    int nFaces = model->nfaces();

    for (int i=0; i < nFaces;i++) {
        std::vector<int> face = model->face(i);

#if DEBUG
        for(int j : face)
            std::cout << j << ' ';
        std::cout << '\n';
#endif

        for(int k=0;k<3;k++) {
            Vec3f v0 = model->vert(face[k]);
            Vec3f v1 = model->vert(face[(k+1)%3]);

            int x0 = (v0.x+1) * width/2;
            int y0 = (v0.y+1) * height/2;
            int x1 = (v1.x+1) * width/2;
            int y1 = (v1.y+1) * height/2;

#if DEBUG
        std::cout << x0 << ';' << y0 << ' ' << x1 << ';' << y1 << '\n';
#endif
        
            drawLine(x0, y0, x1, y1, green, image);
        }
    }

    // 

    image.flip_vertically();
    image.write_tga_file(ARTIFACT_NAME);

    delete model;

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
