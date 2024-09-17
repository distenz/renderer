#ifndef TRIANGLE_RASTERIAZTION_H
#define TRIANGLE_RASTERIAZTION_H

#include "../include/geometry.h"
#include "../include/tga.h"

void rasterize_triangle(TGAImage &image, float *zbuffer, const TGAColor &color,
                        const Vec3f *pts, int width, int height);

#endif // TRIANGLE_RASTERIZATION_H
