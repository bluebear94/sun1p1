#ifndef RENDERER__H
#define RENDERER__H

#include "defs.h"

void renderChunk(Chunk* chunk, TTPointer tt);
void renderPlayerStats(Entity* player);
void renderMobStats(Entity* mob);
void loadTextures(TTPointer tt);
void saveChunk(Chunk* chunk, const char* saveName);
void loadChunk(Chunk* chunk, const char* saveName, int32_t x, int32_t y);
void switchChunk(
    Chunk* c, int32_t x, int32_t y, const char* saveName, Entity* player,
    TTPointer(tt));

#endif
