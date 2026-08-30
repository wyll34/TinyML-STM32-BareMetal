#ifndef SPI_H
#define SPI_H

#include <stdint.h>

void spi5_init(void);
void spi5_cs_low(void);
void spi5_cs_high(void);
void spi5_dc_cmd(void);
void spi5_dc_data(void);
void spi5_write(uint8_t data);
void spi5_write_buffer(const uint8_t *data, uint32_t length);

#endif