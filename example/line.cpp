#include "../src/line.h"
#include "../include/config.h"
#include "../include/tga.h"

void example_draw_lines(TGAImage &image) {
  line(image, red, 0, 0, width, height);
  line(image, green, 0, height, width, 0);
  // vertical and horizontal
  line(image, blue, 0, height / 2, width, height / 2);
  line(image, blue, width / 2, 0, width / 2, height);
}
