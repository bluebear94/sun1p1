#ifndef ITEMS__H
#define ITEMS__H

#include <stdint.h>

#include "defs.h"

void consumeItem(uint16_t id, uint8_t slot, Stack* stack, EContext context);
void equipItem(uint16_t id, uint8_t slot, Stack* stack, EContext context);
void loadInvMenu(Entity* player);

#endif