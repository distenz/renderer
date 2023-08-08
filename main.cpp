#include "geometry.h"
#include "model.h"
#include "tgaimage.h"

#define ARTIFACT_NAME "artifact.tga"
#define DEBUG true

void drawWireMesh(Model* model,const TGAColor& color,TGAImage& image);

const TGAColor white{255,255,255,255};
const TGAColor red{255,0,0,255};
const TGAColor green{0,255,0,255};
const TGAColor blue{0,0,255,255};
const int width {200};
const int height {200};

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

        for (int x=x1; x<x2; x++) {
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
            for (int y=y1; y<y2; y++) {
                if (0 <= y && y < height) {
                    image.set(x,y,color);
                }
            }
        }
    }
}
void triangle(TGAImage &image, const TGAColor &color, int x0, int y0, int x1, int y1, int x2, int y2) { 

//    line(image, color, x0, y0, x1, y1);
//    line(image, color, x2, y2, x0, y0);
//    line(image, color, x1, y1, x2, y2);

    if(x0>x1) {
        swapInt(&y0,&y1);
        swapInt(&x0,&x1);
    }
    if(x1>x2) {
        swapInt(&y1,&y2);
        swapInt(&x1,&x2);
    }
    if(x0>x1) {
        swapInt(&y0,&y1);
        swapInt(&x0,&x1);
    }

    // A____b___C
    //  \      /
    //  c\    /a
    //    \  /
    //     \/
    //      B
    // A(x0,y0)
    // B(x1,y1)
    // C(x2,y2)
    //

    // slope and offsetf
    // for line c
    int cdy = y1 - y0;
    int cdx = x1 - x0;
    int cc = y0 - cdy * x0 / cdx;
    // for ling b
    int bdy = y2 - y0;
    int bdx = x2 - x0;
    int bc = y0 - bdy * x0 / bdx;
    // for line a
    int ady = y2 - y1;
    int adx = x2 - x1;
    int ac = y1 - ady * x1 / adx;
    for(int x=x0; x<x2; x++) {
        int rly1;
        // A->C
        // rly0
        // y = dy/dx*x+c
        int rly0 = bdy*x/bdx+bc;
        if (x<x1) {
            // A->B
            // rly1
            rly1 = cdy*x/cdx+cc;
        } else {
            // B->C
            // rly1
            rly1 = ady*x/adx+ac;
        }
        
        line(image, color, x, rly0, x, rly1);
    }
}

int main() {

    TGAImage image{width,height,TGAImage::RGB};

    // 
//    line(image, red, 0, 0, width, height);
//    line(image, green, 0, height, width, 0);
//    // vertical and horizontal
//    line(image, blue, 0, height/2, width, height/2);
//    line(image, blue, width/2, 0, width/2, height);

//    model = new Model{"./obj/head.obj"};
//    drawWireMesh(model, green, image);

    
    Vec2i t0[3] = {Vec2i(10, 70),   Vec2i(50, 160),  Vec2i(70, 80)}; 
    Vec2i t1[3] = {Vec2i(180, 50),  Vec2i(150, 1),   Vec2i(70, 180)}; 
    Vec2i t2[3] = {Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180)}; 
    triangle(image, red, t0[0].x, t0[0].y, t0[1].x, t0[1].y, t0[2].x, t0[2].y); 
    triangle(image, white, t1[0].x, t1[0].y, t1[1].x, t1[1].y, t1[2].x, t1[2].y); 
    triangle(image, green, t2[0].x, t2[0].y, t2[1].x, t2[1].y, t2[2].x, t2[2].y); 

    // 

    image.flip_vertically();
    image.write_tga_file(ARTIFACT_NAME);

    delete model;

    return 0;
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

            line(image, color, x0, y0, x1, y1);
        }
    }
}

