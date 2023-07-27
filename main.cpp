#include "geometry.h"
#include "model.h"
#include "tgaimage.h"
#include <cstdio>
#include <fstream>
#include <iostream>

#define ARTIFACT_NAME "artifact.tga"
#define DEBUG true

void drawTriangle(Vec2i p1, Vec2i p2, Vec2i p3, const TGAColor& color, TGAImage&);
void drawFilledTriangle(Vec2i p1, Vec2i p2, Vec2i p3, const TGAColor& color, TGAImage&);
void drawLine(int x0,int y0,int x1,int y1,const TGAColor& color,TGAImage& image);
void drawWireMesh(Model* model,const TGAColor& color,TGAImage& image);

const TGAColor white{255,255,255,255};
const TGAColor red{255,0,0,255};
const TGAColor green{0,255,0,255};
const TGAColor blue{0,0,255,255};
const int width {800};
const int height {800};

Model* model = nullptr;

void swapInt(int* a, int* b) {
    int t= *a;
    *a=*b;
    *b=t;
}

// Draws a line 
//
// Where the line is represented as the following equation:
// y = mx+c
//
// y1 = mx1+c  
// y2 = mx2+c
//
// y1-mx1 = c  
// y2     = mx2+c
//
// y1-mx1 = c  
// y2     = mx2+(y1-mx1)
//
// y1-mx1 = c  
// y2     = m(x2-x1)+y1
//
// y1-mx1 = c  
// y2-y1  = m(x2-x1)  
//
//
// y1-mx1           = c  
// (y2-y1)/(x2-x1)  = m  
void line(TGAImage& image, const TGAColor& color, int x1, int y1, int x2, int y2) {
    
    int dx = x2 - x1;
    int dy = y2 - y1;

    // m = dy/dx, mx = dy/dx*x, mx = dy*x/dx
    // abs(dy*x) > abs(dx)
    // therefore we do not nead to convert to float
    // flooring with ints will suffice

    // m will be dy/dx
    // Therefore dx can not be 0, or we would divide by 0
    if (dx!=0) {

        int c = y1 - dy * x1 / dx;

        if(x2 < x1) swapInt(&x1, &x2);

        for (int x=x1; x<=x2; x++) {
            if (0 <= x && x < width) {
                int y = dy*x/dx+c; 
                int error = dy*(x+1)/dx+c; 
                if (error < y) swapInt(&y,&error);
                for (int p=y; p<=error;p++) {
                    if (0 <= p && p < height) {
                        image.set(x,p,color);
                    }
                }
            }
        }

    } else {
        // if dx would have been 0, then our line is vertical
        int x = x1;
        if (0 <= x && x < width ) {
            if (y2 < y1) swapInt(&y1, &y2);
            for (int y=y1; y<=y2; y++) {
                if (0 <= y && y < height) {
                    image.set(x,y,color);
                }
            }
        }
    }
}

int main() {

    TGAImage image{width,height,TGAImage::RGB};

    // 
    line(image, red, 0, 0, width, height);
    line(image, green, 0, height, width, 0);
    // vertical and horizontal
    line(image, blue, 0, height/2, width, height/2);
    line(image, blue, width/2, 0, width/2, height);

    //model = new Model{"./obj/head.obj"};
    //drawWireMesh(model, white, image);

    // 

    //image.flip_vertically();
    image.write_tga_file(ARTIFACT_NAME);

    delete model;

    return 0;
}

void drawFilledTriangle(Vec2i p1, Vec2i p2, Vec2i p3, const TGAColor& color, TGAImage& image) {}

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
