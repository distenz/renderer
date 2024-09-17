#include "./triangle_rasterization.h"
#include "../include/geometry.h"
#include "../include/tga.h"

Vec3f barycentric(const Vec3f *pts, const Vec3f P) {
  Vec3f u = Vec3f(pts[2].x - pts[0].x, pts[1].x - pts[0].x, pts[0].x - P.x) ^
            Vec3f(pts[2].y - pts[0].y, pts[1].y - pts[0].y, pts[0].y - P.y);
  if (std::abs(u.z) < 1)
    return Vec3f(-1, 1, 1);
  return Vec3f(1. - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
}

void rasterize_triangle(TGAImage &image, float *zbuffer, const TGAColor &color,
                        const Vec3f *pts, int width, int height) {
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
