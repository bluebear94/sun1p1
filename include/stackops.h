#ifndef STACKOPS__H
#define STACKOPS__H

#include <stdint.h>

#include "defs.h"

void push(Stack* stack, int64_t value);
int64_t pop(Stack* stack);
int64_t top(Stack* stack);
void initStack(Stack* stack);
Stack* allocStack();

#endif
