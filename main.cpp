#include "geometry.h"
#include "model.h"
#include "tgaimage.h"
#include <algorithm>
#include <limits>

#define ARTIFACT_NAME "artifact.tga"
#define DEBUG true

void mesh(Model *model, const TGAColor &color, TGAImage &image);

const TGAColor white{255, 255, 255, 255};
const TGAColor red{255, 0, 0, 255};
const TGAColor green{0, 255, 0, 255};
const TGAColor blue{0, 0, 255, 255};
const int width{800};
const int height{800};

Model *model = nullptr;

void swapInt(int *a, int *b) {
  int t = *a;
  *a = *b;
  *b = t;
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
void line(TGAImage &image, const TGAColor &color, int x1, int y1, int x2,
          int y2) {

  int dx = x2 - x1;
  int dy = y2 - y1;

  // m = dy/dx, mx = dy/dx*x, mx = dy*x/dx
  // abs(dy*x) > abs(dx)
  // therefore we do not nead to convert to float
  // flooring with ints will suffice

  // m will be dy/dx
  // Therefore dx can not be 0, or we would divide by 0
  if (dx != 0) {

    int c = y1 - dy * x1 / dx;

    if (x2 < x1)
      swapInt(&x1, &x2);

    for (int x = x1; x < x2; x++) {
      if (0 <= x && x < width) {
        int y = dy * x / dx + c;
        int error = dy * (x + 1) / dx + c;
        if (error < y)
          swapInt(&y, &error);
        for (int p = y; p <= error; p++) {
          if (0 <= p && p < height) {
            image.set(x, p, color);
          }
        }
      }
    }

  } else {
    // if dx would have been 0, then our line is vertical
    int x = x1;
    if (0 <= x && x < width) {
      if (y2 < y1)
        swapInt(&y1, &y2);
      for (int y = y1; y < y2; y++) {
        if (0 <= y && y < height) {
          image.set(x, y, color);
        }
      }
    }
  }
}
void triangle(TGAImage &image, const TGAColor &color, int x0, int y0, int x1,
              int y1, int x2, int y2) {

  //    line(image, color, x0, y0, x1, y1);
  //    line(image, color, x2, y2, x0, y0);
  //    line(image, color, x1, y1, x2, y2);

  if (x0 > x1) {
    swapInt(&y0, &y1);
    swapInt(&x0, &x1);
  }
  if (x1 > x2) {
    swapInt(&y1, &y2);
    swapInt(&x1, &x2);
  }
  if (x0 > x1) {
    swapInt(&y0, &y1);
    swapInt(&x0, &x1);
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
  for (int x = x0; x < x2; x++) {
    int rly1;
    // A->C
    // rly0
    // y = dy/dx*x+c
    int rly0 = bdy * x / bdx + bc;
    if (x < x1) {
      // A->B
      // rly1
      rly1 = cdy * x / cdx + cc;
    } else {
      // B->C
      // rly1
      rly1 = ady * x / adx + ac;
    }

    line(image, color, x, rly0, x, rly1);
  }
}

Vec3f barycentric(const Vec3f *pts, const Vec3f P) {
  Vec3f u = Vec3f(pts[2].x - pts[0].x, pts[1].x - pts[0].x, pts[0].x - P.x) ^
            Vec3f(pts[2].y - pts[0].y, pts[1].y - pts[0].y, pts[0].y - P.y);
  if (std::abs(u.z) < 1)
    return Vec3f(-1, 1, 1);
  return Vec3f(1. - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
}

void triangle2(TGAImage &image, float *zbuffer, const TGAColor &color,
               const Vec3f *pts) {
  Vec2f boundingBoxMin(std::numeric_limits<float>::max(),
                       std::numeric_limits<float>::max()),
      boundingBoxMax(-std::numeric_limits<float>::max(),
                     -std::numeric_limits<float>::max()),
      imageBoundary(image.get_width() - 1, image.get_width() - 1);
  for (int i = 0; i < 3; i++) {
    boundingBoxMin.x = std::max(0.f, std::min(boundingBoxMin.x, pts[i].x));
    boundingBoxMin.y = std::max(0.f, std::min(boundingBoxMin.y, pts[i].y));

    boundingBoxMax.x =
        std::min(imageBoundary.x, std::max(boundingBoxMax.x, pts[i].x));
    boundingBoxMax.y =
        std::min(imageBoundary.y, std::max(boundingBoxMax.y, pts[i].y));
  }

  Vec3f iter;
  for (iter.x = boundingBoxMin.x; iter.x < boundingBoxMax.x; iter.x++) {
    for (iter.y = boundingBoxMin.y; iter.y < boundingBoxMax.y; iter.y++) {
      Vec3f barycentricP = barycentric(pts, iter);
      if (barycentricP.x < 0 || barycentricP.y < 0 || barycentricP.z < 0)
        continue;
      iter.z = 0;
      for (int i = 0; i < 3; i++) {
        iter.z += pts[i].z * barycentricP.z;
      }
      if (zbuffer[int(iter.x + iter.y * width)] < iter.z) {
        zbuffer[int(iter.x + iter.y * width)] = iter.z;
        image.set(iter.x, iter.y, color);
      }
    }
  }
}
Vec3f world2screen(Vec3f v) {
  return Vec3f(int((v.x + 1.) * width / 2. + .5),
               int((v.y + 1.) * height / 2. + .5), v.z);
}
void mesh(Model *model, const TGAColor &color, TGAImage &image) {

  Vec3f light_dir(0, 0, -1);
  float *zbuffer = new float[width * height];
  for (int i = width * height; i--;
       zbuffer[i] = -std::numeric_limits<float>::max())
    ;

  int nFaces = model->nfaces();
  int hw = image.get_width() / 2, hh = image.get_height() / 2;

  for (int i = 0; i < nFaces; i++) {
    std::vector<int> face = model->face(i);

    Vec3f vertex;
    Vec3f world_coords[3];
    Vec3f screen_coords[3];

    for (int j = 0; j < 3; j++) {
      vertex = model->vert(face[j]);
      world_coords[j] = vertex;
      screen_coords[j] = world2screen(vertex);
    }

    Vec3f normal = (world_coords[2] - world_coords[0]) ^
                   (world_coords[1] - world_coords[0]);
    normal.normalize();
    float intensity = normal * light_dir;

    if (intensity > 0) {
      triangle2(
          image, zbuffer,
          TGAColor(intensity * 255, intensity * 255, intensity * 255, 255),
          screen_coords);
    }
  }
}
void rasterize(Vec2i p0, Vec2i p1, TGAImage &image, TGAColor color,
               int ybuffer[]) {
  if (p0.x > p1.x) {
    std::swap(p0, p1);
  }
  for (int x = p0.x; x <= p1.x; x++) {
    float t = (x - p0.x) / (float)(p1.x - p0.x);
    int y = p0.y * (1. - t) + p1.y * t;
    if (ybuffer[x] < y) {
      ybuffer[x] = y;
      image.set(x, 0, color);
    }
  }
}

void exampleLines(TGAImage &image) {

  line(image, red, 0, 0, width, height);
  line(image, green, 0, height, width, 0);
  // vertical and horizontal
  line(image, blue, 0, height / 2, width, height / 2);
  line(image, blue, width / 2, 0, width / 2, height);
}

void exampleRaster(TGAImage &image) {
  Vec3f t0[3] = {Vec3f(10, 70, 0), Vec3f(50, 160, 0), Vec3f(70, 80, 0)};
  Vec3f t1[3] = {Vec3f(180, 5, 00), Vec3f(150, 1, 0), Vec3f(70, 180, 0)};
  Vec3f t2[3] = {Vec3f(180, 1, 050), Vec3f(120, 160, 0), Vec3f(130, 180, 0)};
  float *zb = new float[width * height];

  triangle2(image, zb, red, t0);
  triangle2(image, zb, white, t1);
  triangle2(image, zb, green, t2);
}
void exampleMesh(TGAImage &image) {
  model = new Model{"./obj/head.obj"};
  mesh(model, green, image);
}
void exampleYBuffer1(TGAImage &image) {
  // scene "2d mesh"
  line(image, red, 20, 34, 744, 400);
  line(image, green, 120, 434, 444, 400);
  line(image, blue, 330, 463, 594, 200);

  // screen line
  line(image, white, 10, 10, 790, 10);
}
void exampleYBuffer2(TGAImage &image) {
  int ybuffer[width];
  for (int i = 0; i < width; i++) {
    ybuffer[i] = std::numeric_limits<int>::min();
  }
  rasterize(Vec2i(20, 34), Vec2i(744, 400), image, red, ybuffer);
  rasterize(Vec2i(120, 434), Vec2i(444, 400), image, green, ybuffer);
  rasterize(Vec2i(330, 463), Vec2i(594, 200), image, blue, ybuffer);
}

enum Examples {
  LINES = 0,
  RASTER = 1,
  MESH = 2,
  YBUFFER = 3,
};

int main(int argc, char *argv[]) {

  long eg = (long)argv[2];
  TGAImage image{width, height, TGAImage::RGB};

  switch (eg) {
  case LINES:
    exampleLines(image);
    break;
  case RASTER:
    exampleRaster(image);
    break;
  case MESH:
    exampleMesh(image);
    break;
  case YBUFFER:
    exampleYBuffer2(image);
    break;
  default:
    exampleMesh(image);
  }

  image.flip_vertically();
  image.write_tga_file(ARTIFACT_NAME);

  delete model;

  return 0;
}
