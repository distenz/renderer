#include "../example/line.h"
#include "../example/triangle.h"
#include "../include/config.h"
#include "../include/geometry.h"
#include "../include/obj.h"
#include "../include/tga.h"
#include "./line.h"
#include "./triangle_rasterization.h"
#include <algorithm>
#include <cstdio>
#include <limits>

#define DEBUG true

void mesh(Model *model, const TGAColor &color, TGAImage &image);

Model *model = nullptr;

void mesh(Model *model, const TGAColor &color, TGAImage &image) {

  Vec3f light_dir(0, 0, -1);
  float *zbuffer = new float[width * height];
  for (int i = width * height; i--;
       zbuffer[i] = -std::numeric_limits<float>::max())
    ;

  int nFaces = model->nfaces();
  // int hw = image.get_width() / 2, hh = image.get_height() / 2;

  for (int i = 0; i < nFaces; i++) {
    std::vector<int> face = model->face(i);

    Vec3f vertex;
    Vec3f world_coords[3];
    Vec3f screen_coords[3];

    for (int j = 0; j < 3; j++) {
      vertex = model->vert(face[j]);
      world_coords[j] = vertex;
      screen_coords[j] =
          Vec3f(int((vertex.x + 1.) * width / 2. + .5),
                int((vertex.y + 1.) * height / 2. + .5), vertex.z);
    }

    Vec3f normal = (world_coords[2] - world_coords[0]) ^
                   (world_coords[1] - world_coords[0]);
    normal.normalize();
    float intensity = normal * light_dir;

    if (intensity > 0) {
      rasterize_triangle(
          image, zbuffer,
          TGAColor(intensity * 255, intensity * 255, intensity * 255, 255),
          screen_coords, width, height);
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

  const char *example_arg = argv[1];
  printf("argv[1]: %s\n", example_arg);
  if (example_arg == NULL) {
    example_arg = "0";
  }
  int example_id = example_arg[0] - '0';
  printf("example_id: %d\n", example_id);

  TGAImage image{width, height, TGAImage::RGB};

  switch (example_id) {
  case LINES:
    example_draw_lines(image);
    break;
  case RASTER:
    example_rasterize_triangles(image);
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
