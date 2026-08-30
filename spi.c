#include "spi.h"
#include "stm32f429_regs.h"

void spi5_cs_low(void) {
    GPIOC->BSRR = (1 << (2 + 16));
}

void spi5_cs_high(void) {
    GPIOC->BSRR = (1 << 2);
}

void spi5_dc_cmd(void) {
    GPIOD->BSRR = (1 << (13 + 16));
}

void spi5_dc_data(void) {
    GPIOD->BSRR = (1 << 13);
}

void spi5_init(void) {
    RCC->AHB1ENR |= 0x7F;
    RCC->APB2ENR |= (1 << 20);

    GPIOC->MODER &= ~(3 << (2 * 2));
    GPIOC->MODER |=  (1 << (2 * 2));
    GPIOC->OSPEEDR |= (3 << (2 * 2));

    GPIOD->MODER &= ~((3 << (12 * 2)) | (3 << (13 * 2)));
    GPIOD->MODER |=  ((1 << (12 * 2)) | (1 << (13 * 2)));
    GPIOD->OSPEEDR |= ((3 << (12 * 2)) | (3 << (13 * 2)));

    GPIOF->MODER &= ~((3 << (7 * 2)) | (3 << (9 * 2)));
    GPIOF->MODER |=  ((2 << (7 * 2)) | (2 << (9 * 2)));
    GPIOF->OSPEEDR |= ((3 << (7 * 2)) | (3 << (9 * 2)));
    GPIOF->AFR[0] &= ~(0xF << (7 * 4));
    GPIOF->AFR[0] |=  (5 << (7 * 4));
    GPIOF->AFR[1] &= ~(0xF << ((9 - 8) * 4));
    GPIOF->AFR[1] |=  (5 << ((9 - 8) * 4));

    spi5_cs_high();
    GPIOD->BSRR = (1 << 12);

    SPI5->CR1 = (1 << 2) | (3 << 3) | (1 << 6) | (1 << 8) | (1 << 9);
}

void spi5_write(uint8_t data) {
    while (!(SPI5->SR & (1 << 1)));
    *((volatile uint8_t *)&SPI5->DR) = data;
    while (!(SPI5->SR & (1 << 0)));
    (void)*((volatile uint8_t *)&SPI5->DR);
}

void spi5_write_buffer(const uint8_t *data, uint32_t length) {
    for (uint32_t i = 0; i < length; i++) {
        spi5_write(data[i]);
    }
}