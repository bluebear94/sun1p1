#include <inttypes.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "defs.h"

// i luv expression chaining

void push(Stack* stack, int64_t value) {
  stack->vals[stack->size++] = value;
}
int64_t pop(Stack* stack) {
  if (--stack->size) return stack->vals[stack->size];
  return -1;
}
int64_t top(Stack* stack) {
  if (stack->size != 1) return stack->vals[stack->size - 1];
  return -1;
}
void initStack(Stack* stack) {
  memset(stack, 0, sizeof(Stack));
}
Stack* allocStack() {
  Stack* s = malloc(sizeof(Stack));
  initStack(s);
  return s;
}
