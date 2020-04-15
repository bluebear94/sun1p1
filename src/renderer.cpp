#include "renderer.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#include <ncurses.h>

#include "defs.h"

void renderChunk(Chunk* chunk, TTPointer tt) {
  int i, j;
  for (i = 0; i < 32; ++i) {
    move(i + 1, 1);
    for (j = 0; j < 128; ++j) {
      int id = chunk->tiles[i][j];
      if (id) {
        Texture t = tt[id];
        attron(COLOR_PAIR(t.color));
        addch(t.repr);
        attroff(COLOR_PAIR(t.color));
      } else
        addch(' ');
    }
    move(34, 64);
    attron(COLOR_PAIR(7));
    printw("Chunk (%d, %d)", chunk->x, chunk->y);
    move(35, 64);
    printw("Difficulty: %d", chunk->difficulty);
    attroff(COLOR_PAIR(7));
  }
}

void renderPlayerStats(Entity* player) {
  move(34, 2);
  attron(COLOR_PAIR(3));
  printw(player->eh.name);
  attroff(COLOR_PAIR(3));
  move(35, 2);
  attron(COLOR_PAIR(1));
  printw("HP %d/%d ", player->eh.currHealth, player->eh.maxHealth);
  attroff(COLOR_PAIR(1));
  attron(COLOR_PAIR(6));
  printw("MP %d/%d", player->eh.currMagic, player->eh.maxMagic);
  attroff(COLOR_PAIR(6));
  attron(COLOR_PAIR(2));
  move(36, 2);
  printw(
      "XP %d/%d ", player->moreData.playerData.xp,
      player->moreData.playerData.nx);
  attroff(COLOR_PAIR(2));
}

void renderMobStats(Entity* mob) {
  move(34, 64);
  attron(COLOR_PAIR(3));
  printw(mob->eh.name);
  attroff(COLOR_PAIR(3));
  move(35, 64);
  attron(COLOR_PAIR(1));
  printw("HP %d/%d ", mob->eh.currHealth, mob->eh.maxHealth);
  attroff(COLOR_PAIR(1));
  attron(COLOR_PAIR(6));
  printw("MP %d/%d", mob->eh.currMagic, mob->eh.maxMagic);
  attroff(COLOR_PAIR(6));
}

void loadTextures(TTPointer tt) {
  FILE* is = fopen("assets/textures.txt", "r");
  int tn, bg, fg;
  char ch;
  while (!feof(is)) {
    fscanf(is, "%d: %d %d %c\n", &tn, &bg, &fg, &ch);
    printAndRefresh("%d: %d %d %c\n", tn, bg, fg, ch);
    tt[tn] = {.repr = ch, .color = (char) ((bg << 3) + fg)};
  }
  fclose(is);
}

void saveChunk(Chunk* chunk, const char* saveName) {
  char buff[64] = "saves/";
  snprintf(buff + 6, 64, "%s/chunks/%d.%d.bin", saveName, chunk->x, chunk->y);
  FILE* f = fopen(buff, "wb");
  fwrite(chunk, sizeof(Chunk), 1, f);
  fclose(f);
}

void loadChunk(Chunk* chunk, const char* saveName, int32_t x, int32_t y) {
  char buff[64] = "saves/";
  snprintf(buff + 6, 64, "%s/chunks/%d.%d.bin", saveName, x, y);
  FILE* f = fopen(buff, "rb");
  fread(chunk, sizeof(Chunk), 1, f);
  fclose(f);
}

void switchChunk(
    Chunk* c, int32_t x, int32_t y, const char* saveName, Entity* player,
    TTPointer(tt)) {
  saveChunk(c, saveName);
  loadChunk(c, saveName, x, y);
  clear();
  renderChunk(c, tt);
  renderPlayerStats(player);
}
