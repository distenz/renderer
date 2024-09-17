#include "../include/tga.h"
#include "../include/utils.h"
#include "./line.h"

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
