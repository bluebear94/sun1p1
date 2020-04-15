#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <ncurses.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "defs.h"
#include "gen.h"
#include "menu.h"
#include "renderer.h"

int main(void) {
  printf("starting...\n");
  getch();
  initscr();
  printAndRefresh("Screen initialized...\n");
  int y, x;
  getmaxyx(stdscr, y, x);
  if (y < MIN_HEIGHT || x < MIN_WIDTH) {
    printAndRefresh(
        "Required: %d by %d; found: %d by %d.\nAborting...\n", MIN_WIDTH,
        MIN_HEIGHT, x, y);
    getch();
    echo();
    endwin();
    exit(1);
  } else {
    printAndRefresh("Found: %d by %d.\nThis will suffice.\n", x, y);
  }
  noecho();
  keypad(stdscr, 1);
  // halfdelay(1);
  if (!has_colors()) {
    printAndRefresh(
        "What are you doing with a terminal that doesn't support color?!\n");
    getch();
    echo();
    endwin();
    exit(1);
  } else {
    printAndRefresh("Good to know that color is supported.\n");
  }
  start_color();
  int i, j;
  for (i = 0; i < 8; ++i)
    for (j = 0; j < 8; ++j) init_pair((i << 3) + j, j, i);
  printAndRefresh("All systems go.\n");
  getch();
  clear();
  srand(time(NULL));
  printAndRefresh("Allocating memory...\n");
  refresh();
  Chunk* c = malloc(16384);
  TTPointer tt = malloc(sizeof(TextureTable));
  printAndRefresh("Loading textures...\n");
  loadTextures(tt);
  printAndRefresh("Generating terrain...\n");
  genChunk(c, -2, 2, 5426933);
  clear();
  renderChunk(c, tt);
  refresh();
  getch();
  free(c);
  free(tt);
  menu(testMenu, 8, 4, 40, 1, 0);
  printw("Nothing here!\n");
  mkdir("saves", 0777);
  refresh();
  echo();
  endwin();
  return 0;
}
