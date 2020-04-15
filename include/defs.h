#ifndef DEFS__H
#define DEFS__H

#define DEBUG

#include <inttypes.h>

#define MIN_WIDTH 132
#define MIN_HEIGHT 50
#define JUMP_HEIGHT 4
#define PI 3.14159265
#define stRun(spell, stack, context) slpRun(spell->inst, stack, context)
#ifdef DEBUG
#  define printAndRefresh(...) \
    printw(__VA_ARGS__); \
    refresh();
#else
#  define printAndRefresh(...)
#endif

#define max(a, b) \
  ({ \
    __typeof__(a) _a = (a); \
    __typeof__(b) _b = (b); \
    _a > _b ? _a : _b; \
  })
#define min(a, b) \
  ({ \
    __typeof__(a) _a = (a); \
    __typeof__(b) _b = (b); \
    _a < _b ? _a : _b; \
  })

typedef int64_t Health;
typedef Health Magic;
typedef Magic Gold;
typedef char* Spell;
typedef struct Stack {
  int64_t size;
  int64_t vals[2047];
} Stack; // That's 16 KiB

enum { STRENGTH, DEFENSE, AGILITY, INTELLECT } Attributes;
enum { E_FIRE, E_WATER, E_EARTH, E_AIR } Elements;
enum { HAT, ROBE, SWORD, RING } Equipment;

typedef struct EntityHeader {
  Health currHealth; //   8 bytes
  Health maxHealth; //  16 bytes
  Magic currMagic; //  24 bytes
  Magic maxMagic; //  32 bytes
  Gold gold; //  40 bytes
  uint64_t spellsKnown[16]; // 168 bytes (1024 spells)
  uint16_t id; // 170 bytes
  char name[30]; // 200 bytes
  uint32_t attrs[4]; // 216 bytes
  Health poisonAmount; // 224 bytes
  int16_t elemAmp[4]; // 232 bytes (256 = no increase or decrease)
  char pad[24]; // 256 bytes
} EntityHeader;

typedef struct Entity {
  EntityHeader eh;
  union {
    struct {
      uint16_t level;
      uint64_t xp;
      uint64_t nx;
      uint64_t seed;
      int32_t roomX;
      int32_t roomY;
      uint8_t x;
      uint8_t y;
      uint8_t jumped;
      uint16_t equipped[4];
      uint32_t currWt;
      struct {
        uint16_t id;
        uint16_t qty;
      } items[63];
      char pad[594];
    } playerData;
    struct {
      char repr[8][16];
      char pad[639];
    } mobData;
  } moreData; // 767 bytes
  char isPlayer;
} Entity; // 1024 bytes

typedef Entity* EContext[2]; // attacker, THEN defender

Entity player;

typedef struct STEntry {
  int64_t mCost;
  char name[68];
  char inst[179];
  char animType;
} STEntry;
typedef STEntry SpellTable[1024];

SpellTable spells;

#define NUM_TEXTURES 1024
#define cForeground(color) (color & 7)
#define cBackground(color) (color >> 3)

typedef struct Texture {
  char repr;
  char color;
} Texture;
typedef Texture TextureTable[NUM_TEXTURES];
typedef Texture* TTPointer;

typedef uint16_t Block;
typedef struct Chunk {
  Block tiles[32][128]; // 8192 bytes
  uint16_t candidates[15]; // 8222 bytes
  uint16_t difficulty; // 8224 bytes
  int32_t x;
  int32_t y; // 8232 bytes
  uint8_t element; // 8233 bytes
} Chunk;

typedef struct Item {
  char name[30];
  uint16_t weight;
  union {
    struct {
      uint32_t attrs[4];
      uint64_t maxHealthBonus;
      uint64_t maxMagicBonus;
    } equipmentData;
    char consumableInstructions[32];
  } data;
} Item;

Item items[0x8000];

typedef struct MobDNA {
  char repr[8][16];
  uint16_t spells[16];
  uint16_t mhCoeffs[4];
  uint16_t mmCoeffs[4];
  uint32_t attrs[4];
  int16_t elemAmp[4];
  uint32_t gCoeffs[2];
  char name[30];
  char pad[18];
} MobDNA;

char spellMenuArray[1024][32];
const char* spellMenu[1024];
char invMenuArray[64][64];
const char* invMenu[64];

#endif
