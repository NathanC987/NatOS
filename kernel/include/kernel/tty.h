#ifndef _KERNEL_TTY_H
#define _KERNEL_TTY_H

#include <stdint.h>

void ssfn_setup(uint32_t start_x, uint32_t start_y);
void terminal_clear(uint32_t start_y);
void terminal_initialize(uint32_t fg, uint32_t bg);
void terminal_putchar(char c);
void terminal_writestring(const char *s);

#endif
