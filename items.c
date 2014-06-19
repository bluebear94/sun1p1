#include <ncurses.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include "defs.h"

/*

0x0000 - 2FFF: consumable
0x3000 - 3FFF: other
0x4000 - 4FFF: hat
0x5000 - 5FFF: robe
0x6000 - 6FFF: sword
0x7000 - 7FFF: ring

*/

void dispInvMenu(EContext context, Stack stack, int allowEquips) {
  extern char* invMenu[64];
  int pick;
  Entity* player = *context;
  loadInvMenu(player);
  do {
    pick = menu(invMenu, 64, 32, 1, 1, pick);
    uint16_t qty = player->moreData.playerData.items[pick].qty;
    if (qty) {
      uint16_t id = player->moreData.playerData.items[pick].id;
      if (id < 0x3000) consumeItem(id, pick, stack, context);
      else if (allowEquips && (id & 0x4000)) equipItem(id, pick, stack, context);
    }
  } while (pick);
}

void updateMenuEntry(Entity* player, int slot, int id, int qty) {
  extern Item items[0x8000];
  extern char* invMenu[64];
  extern char invMenuArray[64][64];
  if (qty) sprintf(invMenuArray[slot], (id & 0x4000) ? "%s" : "%s (%d)", items[id].name, qty);
}

void loadInvMenu(Entity* player) {
  extern char* invMenu[64];
  extern char invMenuArray[64][64];
  *invMenu = "Back";
  int i;
  for (i = 1; i < 64; ++i) {
    invMenu[i] = invMenuArray + i;
    updateMenuEntry(player, i, player->moreData.playerData.items[i].id, player->moreData.playerData.items[i].qty);
  }
}

void consumeItem(uint16_t id, uint8_t slot, Stack stack, EContext context) {
  extern Item items[0x8000];
  Item* item = items + id;
  slpRun(item->data.consumableInstructions, stack, context, 1);
  (*context)->moreData.playerData.currWt -= item->weight;
  updateMenuEntry(*context, slot, id, --(*context)->moreData.playerData.items[slot].qty); // Yay C!!!
}

void equipItem(uint16_t id, uint8_t slot, Stack stack, EContext context) {
  extern Item items[0x8000];
  int eqType = (id >> 13) - 2;
  uint16_t* oldId = (*context)->moreData.playerData.equipped + eqType;
  uint32_t* playerAttrs = (*context)->eh.attrs;
  uint32_t* oldAttrBoosts = items[*oldId].data.equipmentData.attrs;
  uint32_t* newAttrBoosts = items[id].data.equipmentData.attrs;
  int i;
  for (i = 0; i < 4; ++i) {
    playerAttrs[i] += newAttrBoosts[i] - oldAttrBoosts[i];
  }
  uint64_t* playerHM = (uint64_t*) *context;
  uint64_t* oldHM = (uint64_t*) (oldAttrBoosts + 4);
  uint64_t* newHM = (uint64_t*) (newAttrBoosts + 4);
  for (i = 0; i < 2; ++i) {
    playerHM[i] += newHM[i] - oldHM[i];
  }
  uint16_t t = *oldId;
  *oldId = (*context)->moreData.playerData.items[slot].id;
  (*context)->moreData.playerData.items[slot].id = t;
  updateMenuEntry(*context, slot, id, 1);
}

void loadItemTable() {
  extern Item items[0x8000];
  FILE* is = fopen("assets/items.txt", "r");
  uint16_t cs = 0;
  char buffer[256];
  while (!feof(is)) {
    fgets(buffer, 256, is);
    switch (*buffer) {
      case '#':
        cs = atoi(buffer + 1);
        break;
      case 'w':
        items[cs].weight = atoi(buffer + 1);
        break;
      case 'a': {
          uint32_t* attrs = items[cs].data.equipmentData.attrs;
          sscanf(buffer + 1, "%lu %lu %lu %lu", attrs, attrs + 1, attrs + 2, attrs + 3);
          break;
        }
      case 'h': {
          uint64_t* hm = &(items[cs].data.equipmentData.maxHealthBonus);
          sscanf(buffer + 1, "%llu %llu", hm, hm + 1);
          break;
        }
      case 'n':
        strncpy(items[cs].name, buffer + 1, 29);
        items[cs].name[29] = 0;
        break;
      case ' ':
        strncpy(items[cs].data.consumableInstructions, buffer + 1, 118);
        items[cs].data.consumableInstructions[118] = 0;
    }
  }
  fclose(is);
}
