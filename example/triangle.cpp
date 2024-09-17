#include "../include/config.h"
#include "../include/tga.h"
#include "../src/triangle_rasterization.h"

void example_rasterize_triangles(TGAImage &image) {

  Vec3f t0[3] = {Vec3f(10, 70, 0), Vec3f(50, 160, 0), Vec3f(70, 80, 0)};
  Vec3f t1[3] = {Vec3f(180, 5, 00), Vec3f(150, 1, 0), Vec3f(70, 180, 0)};
  Vec3f t2[3] = {Vec3f(180, 1, 050), Vec3f(120, 160, 0), Vec3f(130, 180, 0)};
  float *zb = new float[width * height];

  rasterize_triangle(image, zb, red, t0, width, height);
  rasterize_triangle(image, zb, white, t1, width, height);
  rasterize_triangle(image, zb, green, t2, width, height);
}
