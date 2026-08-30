#include "stmpe811.h"
#include "stm32f429_regs.h"

#define STMPE811_ADDR 0x82

static void delay_us(volatile uint32_t us) {
    while (us--) {
        volatile uint32_t count = 35;
        while (count--) { __asm__("nop"); }
    }
}

static void delay_ms(volatile uint32_t ms) {
    while (ms--) {
        volatile uint32_t count = 56000;
        while (count--) { __asm__("nop"); }
    }
}

static void i2c3_init(void) {
    RCC->AHB1ENR |= (1 << 0) | (1 << 2);
    RCC->APB1ENR |= (1 << 23);

    GPIOA->MODER &= ~(3 << (8 * 2));
    GPIOA->MODER |=  (2 << (8 * 2));
    GPIOA->OTYPER |= (1 << 8);
    GPIOA->PUPDR  |= (1 << (8 * 2));
    GPIOA->OSPEEDR |= (3 << (8 * 2));
    GPIOA->AFR[1] &= ~(0xF << ((8 - 8) * 4));
    GPIOA->AFR[1] |=  (4 << ((8 - 8) * 4));

    GPIOC->MODER &= ~(3 << (9 * 2));
    GPIOC->MODER |=  (2 << (9 * 2));
    GPIOC->OTYPER |= (1 << 9);
    GPIOC->PUPDR  |= (1 << (9 * 2));
    GPIOC->OSPEEDR |= (3 << (9 * 2));
    GPIOC->AFR[1] &= ~(0xF << ((9 - 8) * 4));
    GPIOC->AFR[1] |=  (4 << ((9 - 8) * 4));

    I2C3->CR1 |= (1 << 15);
    delay_us(100);
    I2C3->CR1 &= ~(1 << 15);

    I2C3->CR2 = 42;
    I2C3->CCR = 210;
    I2C3->TRISE = 43;
    I2C3->CR1 |= (1 << 0);
}

static uint8_t i2c3_start(uint8_t addr_rw) {
    I2C3->CR1 |= (1 << 8);
    uint32_t timeout = 50000;
    while (!(I2C3->SR1 & (1 << 0))) {
        if (--timeout == 0) return 0;
    }

    I2C3->DR = addr_rw;
    timeout = 50000;
    while (!(I2C3->SR1 & (1 << 1))) {
        if (I2C3->SR1 & (1 << 10)) {
            I2C3->SR1 &= ~(1 << 10);
            I2C3->CR1 |= (1 << 9);
            return 0;
        }
        if (--timeout == 0) return 0;
    }
    (void)I2C3->SR1;
    (void)I2C3->SR2;
    return 1;
}

static void i2c3_stop(void) {
    I2C3->CR1 |= (1 << 9);
}

static uint8_t i2c3_write_byte(uint8_t data) {
    uint32_t timeout = 50000;
    while (!(I2C3->SR1 & (1 << 7))) {
        if (--timeout == 0) return 0;
    }
    I2C3->DR = data;
    timeout = 50000;
    while (!(I2C3->SR1 & (1 << 7))) {
        if (--timeout == 0) return 0;
    }
    return 1;
}

static uint8_t i2c3_read_nack(void) {
    I2C3->CR1 &= ~(1 << 10);
    uint32_t timeout = 50000;
    while (!(I2C3->SR1 & (1 << 6))) {
        if (--timeout == 0) return 0;
    }
    i2c3_stop();
    return (uint8_t)I2C3->DR;
}

static void stmpe811_write_reg(uint8_t reg, uint8_t val) {
    if (!i2c3_start(STMPE811_ADDR | 0x00)) return;
    i2c3_write_byte(reg);
    i2c3_write_byte(val);
    i2c3_stop();
}

static uint8_t stmpe811_read_reg(uint8_t reg) {
    if (!i2c3_start(STMPE811_ADDR | 0x00)) return 0;
    i2c3_write_byte(reg);
    if (!i2c3_start(STMPE811_ADDR | 0x01)) return 0;
    return i2c3_read_nack();
}

uint8_t stmpe811_init(void) {
    i2c3_init();
    delay_ms(10);

    stmpe811_write_reg(0x03, 0x01);
    delay_ms(10);

    uint8_t id_h = stmpe811_read_reg(0x00);
    uint8_t id_l = stmpe811_read_reg(0x01);
    if (id_h != 0x08 || id_l != 0x11) {
        return 0;
    }

    stmpe811_write_reg(0x04, 0x0C);
    stmpe811_write_reg(0x0A, 0x04);
    stmpe811_write_reg(0x20, 0x49);
    delay_ms(2);
    stmpe811_write_reg(0x21, 0x01);
    stmpe811_write_reg(0x41, 0x9A);
    stmpe811_write_reg(0x4A, 0x01);
    stmpe811_write_reg(0x4B, 0x01);
    stmpe811_write_reg(0x4B, 0x00);
    stmpe811_write_reg(0x56, 0x07);
    stmpe811_write_reg(0x58, 0x01);
    stmpe811_write_reg(0x40, 0x01);
    stmpe811_write_reg(0x0B, 0xFF);
    return 1;
}

uint8_t stmpe811_read_touch(uint16_t *x, uint16_t *y) {
    uint8_t ctrl = stmpe811_read_reg(0x40);
    if (!(ctrl & 0x80)) {
        return 0;
    }

    uint8_t d[4];
    for (int i = 0; i < 4; i++) {
        d[i] = stmpe811_read_reg(0xD7);
    }

    uint16_t raw_x = ((uint16_t)d[0] << 4) | (d[1] >> 4);
    uint16_t raw_y = ((uint16_t)(d[1] & 0x0F) << 8) | d[2];

    stmpe811_write_reg(0x4B, 0x01);
    stmpe811_write_reg(0x4B, 0x00);

    if (raw_x < 220) raw_x = 220;
    if (raw_x > 3850) raw_x = 3850;
    if (raw_y < 200) raw_y = 200;
    if (raw_y > 3800) raw_y = 3800;

    *x = (uint16_t)(((uint32_t)(raw_x - 220) * 240) / 3630);
    *y = (uint16_t)(((uint32_t)(raw_y - 200) * 320) / 3600);

    if (*x >= 240) *x = 239;
    if (*y >= 320) *y = 319;

    return 1;
}