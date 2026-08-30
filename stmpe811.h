#ifndef STMPE811_H
#define STMPE811_H

#include <stdint.h>

uint8_t stmpe811_init(void);
uint8_t stmpe811_read_touch(uint16_t *x, uint16_t *y);

#endif