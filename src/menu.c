#include "menu.h"

#include <inttypes.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <ncurses.h>

#include "blocks.h"
#include "defs.h"

const char* testMenu[8] = {
    "option 1", "option 2", "option 3", "option 4",
    "option 5", "option 6", "option 7", "surprise motherfucker"};

void drawCursor(int i, int y, int x, char c) {
  move(y + i, x);
  addch(c);
}

void drawOptions(const char* const* options, uint8_t nOptions, int y, int x) {
  attron(COLOR_PAIR(34));
  int i;
  for (i = 0; i < nOptions; ++i) {
    move(y + i, x);
    addch(' ');
    int l = strlen(options[i]);
    printw(options[i]);
    attron(COLOR_PAIR(0));
    register unsigned char j;
    for (j = l; j < 64; ++j) { addch(' '); }
    attroff(COLOR_PAIR(0));
  }
  attroff(COLOR_PAIR(34));
}

uint8_t menu(
    const char* const* options, uint8_t nOptions, uint8_t paneSize, int y,
    int x, int i) {
  drawOptions(options, paneSize, y, x);
  int k = 0;
  int shift = min(i, nOptions - paneSize);
  attron(COLOR_PAIR(35));
  drawCursor(i - shift, y, x, '>');
  while (k != 10) {
    refresh();
    k = getch();
    drawCursor(i - shift, y, x, ' ');
    if (k == KEY_UP) {
      --i;
      if (i < 0) {
        i = nOptions - 1;
        shift = nOptions - paneSize;
        drawOptions(options + shift, paneSize, y, x);
      } else if (i < shift) {
        drawOptions(options + (--shift), paneSize, y, x);
      }
    } else if (k == KEY_DOWN) {
      ++i;
      if (i == nOptions) {
        shift = i = 0;
        drawOptions(options, paneSize, y, x);
      } else if (i >= shift + paneSize) {
        drawOptions(options + (++shift), paneSize, y, x);
      }
    }
    attron(COLOR_PAIR(35));
    drawCursor(i - shift, y, x, '>');
  }
  attroff(COLOR_PAIR(35));
  return i;
}
