#include "../include/tga.h"
#include "../include/utils.h"

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

  int width = image.get_width();
  int height = image.get_height();

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
