#include "geometry.h"
#include "model.h"
#include "tgaimage.h"
#include <cstdlib>
#include <iostream>
#include <vector>

#define ARTIFACT_NAME "artifact.tga"
#define DEBUG true

void drawTriangle(Vec2i p1, Vec2i p2, Vec2i p3, const TGAColor& color, TGAImage&);
void drawLine(int x0,int y0,int x1,int y1,const TGAColor& color,TGAImage& image);
void drawWireMesh(Model* model,const TGAColor& color,TGAImage& image);

const TGAColor white{255,255,255,255};
const TGAColor red{255,0,0,255};
const TGAColor green{0,255,0,255};
const int width {800};
const int height {800};

Model* model = nullptr;

int main() {

    TGAImage image{width,height,TGAImage::RGB};

    Vec2i t0[3] = {Vec2i(10, 70),   Vec2i(50, 160),  Vec2i(70, 80)}; 
    Vec2i t1[3] = {Vec2i(180, 50),  Vec2i(150, 1),   Vec2i(70, 180)}; 
    Vec2i t2[3] = {Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180)}; 
    drawTriangle(t0[0], t0[1], t0[2], red, image); 
    drawTriangle(t1[0], t1[1], t1[2], white, image); 
    drawTriangle(t2[0], t2[1], t2[2], green, image);

    //model = new Model{"./obj/head.obj"};
    //drawWireMesh(model, white, image);

    // 

    image.flip_vertically();
    image.write_tga_file(ARTIFACT_NAME);

    delete model;

    return 0;
}

void drawTriangle(Vec2i p1, Vec2i p2, Vec2i p3, const TGAColor& color, TGAImage& image) {

    drawLine(p1.x, p1.y, p2.x, p2.y, color, image);
    drawLine(p2.x, p2.y, p3.x, p3.y, color, image);
    drawLine(p1.x, p1.y, p3.x, p3.y, color, image);
}

void drawWireMesh(Model* model,const TGAColor& color, TGAImage& image) {


    int nFaces = model->nfaces();

    for (int i=0; i < nFaces;i++) {
        std::vector<int> face = model->face(i);

        for(int k=0;k<3;k++) {
            Vec3f v0 = model->vert(face[k]);
            Vec3f v1 = model->vert(face[(k+1)%3]);
            Vec3f v2 = model->vert(face[(k+2)%3]);

            int x0 = (v0.x+1) * width/2;
            int y0 = (v0.y+1) * height/2;
            int x1 = (v1.x+1) * width/2;
            int y1 = (v1.y+1) * height/2;

            drawLine(x0, y0, x1, y1, color, image);
        }
    }
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
