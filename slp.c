#include <inttypes.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defs.h"

/*

#_____: pushes number on stack
D     : duplicates entry
T     : transposes 2 entries
+*-/% : Exactly what you think
R     : it's RANDOM
abcd/
efgh  : attributes
ijkl/
mnop  : more
P     : like T#00256*T/, but doesn't cause overflow
qrst  : damage
u     : poison
v     : heal
w     : MP heal

*/

void renderInflictedDamage(char* s, int isPlayer, int col) {
  move(23, 96 - (isPlayer << 6));
  attron(COLOR_PAIR(col));
  printw(s);
  attroff(COLOR_PAIR(col));
}

void damage(EntityHeader* opp, int64_t amount) {
  opp->currHealth = min(opp->maxHealth, opp->currHealth - amount);
}

void slpRun(
    Spell spell, Stack* stack, EContext context, int playerIsAttacking) {
  char buff[32];
  for (; *spell; ++spell) {
    char in = *spell;
    switch (in) {
    case '#':
      push(
          stack, 10000 * spell[1] + 1000 * spell[2] + 100 * spell[3] +
                     10 * spell[4] + spell[5] - 533328);
      spell += 5;
      break;
    case 'D': push(stack, top(stack)); break;
    case 'T': {
      int64_t a = pop(stack);
      int64_t b = pop(stack);
      push(stack, a);
      push(stack, b);
      break;
    }
    case '+': {
      int64_t a = pop(stack);
      int64_t b = pop(stack);
      push(stack, a + b);
      break;
    }
    case '-': {
      int64_t a = pop(stack);
      int64_t b = pop(stack);
      push(stack, b - a);
      break;
    }
    case '*': {
      int64_t a = pop(stack);
      int64_t b = pop(stack);
      push(stack, a * b);
      break;
    }
    case '/': {
      int64_t a = pop(stack);
      int64_t b = pop(stack);
      push(stack, b / a);
      break;
    }
    case '%': {
      int64_t a = pop(stack);
      int64_t b = pop(stack);
      push(stack, b % a);
      break;
    }
    case 'R': push(stack, rand()); break;
    case 'P': {
      int64_t a = pop(stack);
      int64_t b = pop(stack);
      push(stack, 256.0 * b / a);
      break;
    }
    case '&': {
      int64_t a = pop(stack);
      int64_t b = pop(stack);
      push(stack, a && b);
      break;
    }
    case '|': {
      int64_t a = pop(stack);
      int64_t b = pop(stack);
      push(stack, a || b);
      break;
    }
    case '^': {
      int64_t a = pop(stack);
      int64_t b = pop(stack);
      push(stack, !a != !b);
      break;
    }
    case '!': {
      push(stack, !pop(stack));
      break;
    }
    case '=': {
      int64_t a = pop(stack);
      int64_t b = pop(stack);
      push(stack, a == b);
      break;
    }
    case '<': {
      int64_t a = pop(stack);
      int64_t b = pop(stack);
      push(stack, a < b);
      break;
    }
    case '>': {
      int64_t a = pop(stack);
      int64_t b = pop(stack);
      push(stack, a > b);
      break;
    }
    default: {
      if (in >= 'a' && in <= 'h') {
        char d = in - 'a';
        push(stack, context[d & 4]->eh.attrs[d & 3]);
      } else if (in >= 'i' && in <= 'p') {
        char d = in - 'i';
        push(stack, ((uint64_t*) context[d & 4])[d & 3]);
      } else if (in >= 'q' && in <= 'u') {
        char d = in - 'q';
        int64_t dmg = pop(stack);
        EntityHeader* opp = (EntityHeader*) (context + 1);
        if (in != 'u') {
          int64_t amt = (dmg * opp->elemAmp[d]) >> 8;
          damage(opp, amt);
          sprintf(buff, "-%lli", amt);
          renderInflictedDamage(buff, playerIsAttacking, 1);
        } else {
          int64_t amt = (dmg * opp->elemAmp[2]) >> 8;
          opp->poisonAmount += amt;
          sprintf(buff, "psn %lli", amt);
          renderInflictedDamage(buff, playerIsAttacking, 2);
        }
      } else if (in == 'v' || in == 'w') {
        int64_t heal = pop(stack);
        EntityHeader* pl = (EntityHeader*) context;
        if (in == 'v') {
          sprintf(buff, "+%lli", heal);
          pl->currHealth = min(pl->maxHealth, pl->currHealth + heal);
          renderInflictedDamage(buff, playerIsAttacking, 5);
        } else {
          sprintf(buff, "m+%lli", heal);
          pl->currMagic = min(pl->maxMagic, pl->currMagic + heal);
          renderInflictedDamage(buff, playerIsAttacking, 6);
        }
      }
    }
    }
  }
}

void loadSpellTable() {
  extern SpellTable spells;
  FILE* is = fopen("assets/spells.txt", "r");
  uint16_t cs = 0;
  char buffer[256];
  while (!feof(is)) {
    fgets(buffer, 256, is);
    switch (*buffer) {
    case '#': cs = atoi(buffer + 1); break;
    case 'm': spells[cs].mCost = atoll(buffer + 1); break;
    case 'a': spells[cs].animType = atoi(buffer + 1); break;
    case 'n':
      strncpy(spells[cs].name, buffer + 1, 59);
      spells[cs].name[59] = 0;
      break;
    case ' ':
      strncpy(spells[cs].inst, buffer + 1, 118);
      spells[cs].inst[118] = 0;
    }
  }
  fclose(is);
}
