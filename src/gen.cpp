#include "gen.h"

#include <inttypes.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <algorithm>

#include <ncurses.h>

#include "blocks.h"
#include "defs.h"

int interpolate(int a, int b, double x) {
  double f = (1 - cos(x * PI)) * .5;
  printAndRefresh("%d %d %f %f\n", a, b, x, f);
  return a * (1 - f) + b * f;
}

Block cover(int32_t height) {
  return height > 3 ? GRASS : SAND;
}

void genChunk(Chunk* c, int32_t rx, int32_t ry, uint64_t seed) {
  int heights[63]; // 0, 1, 3, 7, 15, 31
  int tailHeights[6];
  int i = 0, j = 0;
  for (i = 0; i < 6; ++i) {
    int start = (1 << i) - 1;
    for (j = 0; j < start + 1; ++j) {
      int nx = (rx << 7) + (128 >> i) * j;
      srand(seed ^ (nx * nx * 23 + i * 43));
      heights[j + start] = rand() & 127;
    }
    int nx = (rx + 1) << 7;
    srand(seed ^ (nx * nx * 23 + i * 43));
    tailHeights[i] = rand() & 127;
  }
  for (i = 0; i < 128; ++i) {
    int height = 0;
    for (j = 0; j < 6; ++j) {
      int interval = (128 >> j);
      int start = (1 << j) - 1;
      int k = i - start;
      int index = k / interval;
      double x = (double) (k - index * interval) / interval;
      int a = heights[start + index];
      int b = (index == start) ? tailHeights[j] : heights[start + index + 1];
      height += interpolate(a, b, x) >> j;
    }
    int sa = height - (ry << 5);
    int startAt = (sa >= 32) ? 31 : sa;
    for (j = 0; j <= startAt; ++j) {
      int dirtDepth = 2 + (rand() & 3);
      c->tiles[31 - j][i] = (j == sa) ? cover(j + (ry << 5)) :
                                        (j >= sa - dirtDepth) ? DIRT : STONE;
    }
    for (j = startAt + 1; j < 32; ++j) {
      c->tiles[31 - j][i] = j + (ry << 5) > 0 ? AIR : WATER;
    }
    if (!(i && rand() & 3)) {
      c->element =
          sa >= 128 ? E_FIRE : sa < -16 ? E_AIR : ry < 0 ? E_WATER : E_EARTH;
    }
  }
  c->x = rx;
  c->y = ry;
  c->difficulty =
      std::max(1, (int) sqrt((rx * rx + ry * ry + (rand() & 8) - 4) / 8.0 + 1));
  srand(time(NULL));
}
