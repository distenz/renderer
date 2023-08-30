#include "geometry.h"
#include "model.h"
#include "tgaimage.h"
#include <algorithm>

#define ARTIFACT_NAME "artifact.tga"
#define DEBUG true

void mesh(Model* model,const TGAColor& color,TGAImage& image);

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

Vec3f barycentric(const Vec2i *pts, const Vec2i P) { 
    Vec3f u = Vec3f(pts[2].x -pts[0].x, pts[1].x-pts[0].x, pts[0].x-P.x)^Vec3f(pts[2].y-pts[0].y, pts[1].y-pts[0].y, pts[0].y-P.y);
    if (std::abs(u.z)<1) return Vec3f(-1,1,1);
    return Vec3f(1.-(u.x+u.y)/u.z, u.y/u.z, u.x/u.z); 
} 

void triangle2(TGAImage &image, const TGAColor &color, const Vec2i *pts) { 
    Vec2i boundingBoxMin(image.get_width()-1, image.get_height()-1),
          boundingBoxMax(0,0),
          imageBoundary(image.get_width()-1, image.get_width()-1);
    for(int i=0; i<3; i++) {
        boundingBoxMin.x = std::max(0, std::min(boundingBoxMin.x, pts[i].x));
        boundingBoxMin.y = std::max(0, std::min(boundingBoxMin.y, pts[i].y));

        boundingBoxMax.x = std::min(imageBoundary.x, std::max(boundingBoxMax.x, pts[i].x));
        boundingBoxMax.y = std::min(imageBoundary.y, std::max(boundingBoxMax.y, pts[i].y));
    }

    Vec2i iter;
    for(iter.x=boundingBoxMin.x; iter.x<boundingBoxMax.x; iter.x++) {
        for(iter.y=boundingBoxMin.y; iter.y<boundingBoxMax.y; iter.y++) {
            Vec3f barycentricP = barycentric(pts, iter); 
            if(barycentricP.x<0 || barycentricP.y<0 || barycentricP.z<0) continue;
            image.set(iter.x,iter.y, color);
        }
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

    model = new Model{"./obj/head.obj"};
    mesh(model, green, image);

    
//    Vec2i t0[3] = {Vec2i(10, 70),   Vec2i(50, 160),  Vec2i(70, 80)}; 
//    Vec2i t1[3] = {Vec2i(180, 50),  Vec2i(150, 1),   Vec2i(70, 180)}; 
//    Vec2i t2[3] = {Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180)}; 
//    triangle(image, red, t0[0].x, t0[0].y, t0[1].x, t0[1].y, t0[2].x, t0[2].y); 
//    triangle(image, white, t1[0].x, t1[0].y, t1[1].x, t1[1].y, t1[2].x, t1[2].y); 
//    triangle(image, green, t2[0].x, t2[0].y, t2[1].x, t2[1].y, t2[2].x, t2[2].y); 

//    triangle2(image, red, t0);
//    triangle2(image, white, t1);
//    triangle2(image, green, t2);

    // 

    image.flip_vertically();
    image.write_tga_file(ARTIFACT_NAME);

    delete model;

    return 0;
}

void mesh(Model* model,const TGAColor& color, TGAImage& image) {

    Vec3f light_dir(0, 0, -1);

    int nFaces = model->nfaces();
    int hw = image.get_width() / 2,
        hh = image.get_height() / 2;

    for (int i=0; i < nFaces;i++) {
        std::vector<int> face = model->face(i);

        Vec2i screen[3];
        Vec3f world[3];

        for(int j=0;j<3;j++) {
            Vec3f vertex = model->vert(face[j]);
            screen[j] = Vec2i(
                    (vertex.x+1)*hw,
                    (vertex.y+1)*hh
                    );
            world[j] = vertex;
        }
        
        Vec3f normal = (world[2]-world[0])^(world[1]-world[0]);
        normal.normalize();
        float intensity = normal*light_dir;

        if (intensity>0) {
            triangle2(image, TGAColor(intensity*255, intensity*255, intensity*255, 255), screen);
        }
    }
}

