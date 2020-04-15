#ifndef MENU__H
#define MENU__H

extern const char* testMenu[8];

uint8_t menu(
    const char* const* options, uint8_t nOptions, uint8_t paneSize, int y,
    int x, int i);

#endif
