#include "combat.h"

#include <inttypes.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <ncurses.h>

#include "defs.h"
#include "menu.h"

const char* battleMenu[4] = {"Attack", "Magic", "Item", "Flee"};

const char* spellMenuByRank[33] = {
    "Rank 1",  "Rank 2",  "Rank 3",  "Rank 4",  "Rank 5",  "Rank 6",  "Rank 7",
    "Rank 8",  "Rank 9",  "Rank 10", "Rank 11", "Rank 12", "Rank 13", "Rank 14",
    "Rank 15", "Rank 16", "Rank 17", "Rank 18", "Rank 19", "Rank 20", "Rank 21",
    "Rank 22", "Rank 23", "Rank 24", "Rank 25", "Rank 26", "Rank 27", "Rank 28",
    "Rank 29", "Rank 30", "Rank 31", "Rank 32", "Back"};

int isSpellKnown(uint64_t* spellsKnown, uint16_t id) {
  return spellsKnown[id >> 6] & (1LL << (id & 63));
}

void createSpellMenu(Entity* player) {
  uint64_t* spellsKnown = player->eh.spellsKnown;
  for (int i = 0; i < 1024; ++i) {
    snprintf(
        spellMenuArray[i], 32,
        isSpellKnown(spellsKnown, i) ? "%s (%" PRIu64 ")" : "???",
        spells[i].name, spells[i].mCost);
    spellMenu[i] = spellMenuArray[i];
  }
}

int selectSpellFromRank(int rank) {
  return menu(spellMenu + (rank << 5), 32, 8, 40, 1, 0) + (rank << 5);
}

int selectSpellRaw() {
  int rank = menu(spellMenuByRank, 33, 8, 40, 1, 0);
  return (rank != 32) ? selectSpellFromRank(rank) : -1;
}

int selectSpell(Entity* player) {
  int spellId;
  extern SpellTable spells;
  do {
    spellId = selectSpellRaw();
  } while (spellId != -1 && !(isSpellKnown(player->eh.spellsKnown, spellId) &&
                              player->eh.currMagic >= spells[spellId].mCost));
  return spellId;
}

void renderField(Entity* mob) {
  move(33, 36);
  addch('X');
  int i, j;
  for (i = 0; i < 8; ++i) {
    move(i + 25, 92);
    for (j = 0; j < 16; ++j) { addch(mob->moreData.mobData.repr[i][j]); }
  }
}

int64_t evalPoly(uint16_t* coeffs, int16_t x, uint8_t n) {
  int64_t y = 0;
  for (int i = n - 1; i >= 0; --i) { y = y * x + coeffs[i]; }
  return y;
}

void mobFactory(Entity* mob, uint16_t difficulty, MobDNA* dnas) {
  memcpy(mob->moreData.mobData.repr, dnas, 128);
  int i;
  for (i = 0; i < 16; ++i) {
    int sn = dnas->spells[i];
    mob->eh.spellsKnown[sn & 0x3C0] |= (1 << (sn & 0x3F));
  }
  mob->eh.currHealth = mob->eh.maxHealth =
      evalPoly(dnas->mhCoeffs, difficulty, 4);
  mob->eh.currMagic = mob->eh.maxMagic =
      evalPoly(dnas->mmCoeffs, difficulty, 4);
  memcpy(mob->eh.attrs, dnas->attrs, 16);
  memcpy(mob->eh.elemAmp, dnas->elemAmp, 8);
  mob->eh.gold =
      (uint64_t) * (dnas->gCoeffs) + (uint64_t) difficulty * dnas->gCoeffs[1];
  strncpy(mob->eh.name, dnas->name, 29);
  mob->eh.name[29] = 0;
}

void createMob(Entity* mob, uint16_t difficulty, uint16_t id) {
  char fname[32] = "assets/mobdna/";
  char* s = fname + 13;
  char buffer[256];
  for (; id; id >>= 4) { // Bitfuckery
    int d = id & 15;
    *(++s) = d + ((d > 9) ? '7' : '0');
  }
  *(++s) = 0;
  fname[31] = 0;
  MobDNA* md = (MobDNA*) malloc(sizeof(MobDNA));
  FILE* f = fopen(fname, "r");
  while (!feof(f)) {
    fgets(buffer, 256, f);
    char* nl = buffer + strlen(buffer);
    if (*nl == '\n') *nl = 0;
    nl = buffer + 1;
    switch (*nl) {
    case 'S': {
      int i;
      for (i = 0; i < 16 && *nl; ++i) {
        char* l = strchr(nl, ' ');
        *l = 0;
        md->spells[i] = atoi(nl);
      }
      break;
    }
    case 'H': {
      sscanf(
          nl, "%hu %hu %hu %hu", md->mhCoeffs, md->mhCoeffs + 1,
          md->mhCoeffs + 2, md->mhCoeffs + 3);
      break;
    }
    case 'M': {
      sscanf(
          nl, "%hu %hu %hu %hu", md->mmCoeffs, md->mmCoeffs + 1,
          md->mmCoeffs + 2, md->mmCoeffs + 3);
      break;
    }
    case 'A': {
      sscanf(
          nl, "%u %u %u %u", md->attrs, md->attrs + 1, md->attrs + 2,
          md->attrs + 3);
      break;
    }
    case 'E': {
      sscanf(
          nl, "%hi %hi %hi %hi", md->elemAmp, md->elemAmp + 1, md->elemAmp + 2,
          md->elemAmp + 3);
      break;
    }
    case 'G': {
      sscanf(nl, "%u %u", md->gCoeffs, md->gCoeffs + 1);
      break;
    }
    case 'N': {
      strncpy(md->name, nl, 29);
      md->name[29] = 0;
      break;
    }
    case 'R': {
      memcpy(md->repr, nl, 128);
    }
    }
  }
  mobFactory(mob, difficulty, md);
  free(md);
}
