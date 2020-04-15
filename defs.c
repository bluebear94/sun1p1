#include "defs.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ncurses.h>
#include <sys/stat.h>
#include <sys/types.h>


void createNewSave(char* name) {
  char dirname[64] = "saves/";
  strncpy(dirname + 6, name, 47);
  dirname[63] = 0;
  mkdir(dirname, 0644);
  int l = strlen(dirname);
  strcpy(dirname + l, "/world.bin");
  FILE* f = fopen(dirname, "wb");

  fclose(f);
}
