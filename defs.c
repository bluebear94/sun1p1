#include <ncurses.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "defs.h"

void createNewSave(char* name) {
  char dirname[64] = "saves/";
  strncpy(dirname + 6, name, 47);
  dirname[63] = 0;
  mkdir(dirname);
  int l = strlen(dirname);
  strcpy(dirname + l, "/world.bin");
  FILE* f = fopen(dirname, "wb");
  
  fclose(f);
}
