#ifndef STM32F429_REGS_H
#define STM32F429_REGS_H

#include <stdint.h>

/* --- Peripheral Base Addresses (from RM0090 Table 2) --- */
#define PERIPH_BASE       0x40000000UL

#define APB1PERIPH_BASE   (PERIPH_BASE + 0x00000000UL) /* 0x40000000 */
#define AHB1PERIPH_BASE   (PERIPH_BASE + 0x00020000UL) /* 0x40020000 */
#define APB2PERIPH_BASE   (PERIPH_BASE + 0x00010000UL) /* 0x40010000 */

/* GPIO Ports */
#define GPIOA_BASE        (AHB1PERIPH_BASE + 0x0000UL) /* 0x40020000 */
#define GPIOC_BASE        (AHB1PERIPH_BASE + 0x0800UL) /* 0x40020800 */
#define GPIOD_BASE        (AHB1PERIPH_BASE + 0x0C00UL) /* 0x40020C00 */
#define GPIOF_BASE        (AHB1PERIPH_BASE + 0x1400UL) /* 0x40021400 */
#define GPIOG_BASE        (AHB1PERIPH_BASE + 0x1800UL) /* 0x40021800 */

/* Clock Control */
#define RCC_BASE          (AHB1PERIPH_BASE + 0x3800UL) /* 0x40023800 */

/* Direct Memory Access */
#define DMA2_BASE         (AHB1PERIPH_BASE + 0x6400UL) /* 0x40026400 */

/* SPI5 */
#define SPI5_BASE         (APB2PERIPH_BASE + 0x5000UL) /* 0x40015000 */

/* I2C3 (STMPE811 Touch) */
#define I2C3_BASE         (APB1PERIPH_BASE + 0x5C00UL) /* 0x40005C00 */

/* I2C Register Layout */
typedef struct {
    volatile uint32_t CR1;      // 0x00
    volatile uint32_t CR2;      // 0x04
    volatile uint32_t OAR1;     // 0x08
    volatile uint32_t OAR2;     // 0x0C
    volatile uint32_t DR;       // 0x10
    volatile uint32_t SR1;      // 0x14
    volatile uint32_t SR2;      // 0x18
    volatile uint32_t CCR;      // 0x1C
    volatile uint32_t TRISE;    // 0x20
    volatile uint32_t FLTR;     // 0x24
} I2C_TypeDef;

#define I2C3    ((I2C_TypeDef *) I2C3_BASE)



/* GPIO Register Layout */
typedef struct {
    volatile uint32_t MODER;    /* Offset 0x00 */
    volatile uint32_t OTYPER;   /* Offset 0x04 */
    volatile uint32_t OSPEEDR;  /* Offset 0x08 */
    volatile uint32_t PUPDR;    /* Offset 0x0C */
    volatile uint32_t IDR;      /* Offset 0x10 */
    volatile uint32_t ODR;      /* Offset 0x14 */
    volatile uint32_t BSRR;     /* Offset 0x18 */
    volatile uint32_t LCKR;     /* Offset 0x1C */
    volatile uint32_t AFR[2];   /* Offset 0x20: AFR[0]=AFRL, AFR[1]=AFRH */
} GPIO_TypeDef;

/* Peripheral Pointer Casts */
#define GPIOA ((GPIO_TypeDef *) GPIOA_BASE)
#define GPIOC ((GPIO_TypeDef *) GPIOC_BASE)
#define GPIOD ((GPIO_TypeDef *) GPIOD_BASE)
#define GPIOF ((GPIO_TypeDef *) GPIOF_BASE)
#define GPIOG ((GPIO_TypeDef *) GPIOG_BASE)


#define RCC     ((RCC_TypeDef *) RCC_BASE)
#define DMA2    ((DMA_TypeDef *) DMA2_BASE)
#define SPI5    ((SPI_TypeDef *) SPI5_BASE) 

/* RCC Register Layout */
typedef struct {
    volatile uint32_t CR;       // 0x00 : Clock control register
    volatile uint32_t PLLCFGR;  // 0x04 : PLL configuration register
    volatile uint32_t CFGR;     // 0x08 : Clock configuration register
    volatile uint32_t CIR;      // 0x0C : Clock interrupt register
    volatile uint32_t AHB1RSTR; // 0x10 : AHB1 peripheral reset register
    volatile uint32_t AHB2RSTR; // 0x14 : AHB2 peripheral reset register
    volatile uint32_t AHB3RSTR; // 0x18 : AHB3 peripheral reset register
    uint32_t      RESERVED0;  // 0x1C : Reserved
    volatile uint32_t APB1RSTR; // 0x20 : APB1 peripheral reset register
    volatile uint32_t APB2RSTR; // 0x24 : APB2 peripheral reset register
    uint32_t      RESERVED1;  // 0x28 : Reserved
    uint32_t      RESERVED2;  // 0x2C : Reserved
    volatile uint32_t AHB1ENR;  // 0x30 : AHB1 peripheral clock enable register
    volatile uint32_t AHB2ENR;  // 0x34 : AHB2 peripheral clock enable register
    volatile uint32_t AHB3ENR;  // 0x38 : AHB3 peripheral clock enable register
    uint32_t      RESERVED3;  // 0x3C : Reserved
    volatile uint32_t APB1ENR;  // 0x40 : APB1 peripheral clock enable register
    volatile uint32_t APB2ENR;  // 0x44 : APB2 peripheral clock enable register
    uint32_t      RESERVED4;  // 0x48 : Reserved
    uint32_t      RESERVED5;  // 0x4C : Reserved
    volatile uint32_t AHB1LPENR;// 0x50 : AHB1 peripheral clock enable in low power mode register
    volatile uint32_t AHB2LPENR;// 0x54 : AHB2 peripheral clock enable in low power mode register
    volatile uint32_t AHB3LPENR;// 0x58 : AHB3 peripheral clock enable in low power mode register
    uint32_t      RESERVED6;  // 0x5C : Reserved
    volatile uint32_t APB1LPENR;// 0x60 : APB1 peripheral clock enable in low power mode register
    volatile uint32_t APB2LPENR;// 0x64 : APB2 peripheral clock enable in low power mode register
    uint32_t      RESERVED7;  // 0x68 : Reserved
    uint32_t      RESERVED8;  // 0x6C : Reserved
    volatile uint32_t BDCR;     // 0x70 : Backup domain control register
    volatile uint32_t CSR;      // 0x74 : Clock control & status register
    uint32_t      RESERVED9[2]; // 0x78-0x7C : Reserved
    volatile uint32_t SSCGR;    // 0x80 : Spread-spectrum clock generation register
    volatile uint32_t PLLI2SCFGR; // 0x84 : PLLI2S configuration register
    volatile uint32_t PLLSAICFGR; // 0x88 : PLLSAI configuration register
    volatile uint32_t DCKCFGR;  // 0x8C : Dedicated Clock Configuration Register
    volatile uint32_t CKGATENR; // 0x90 : Clocks Gating Tender Register
    volatile uint32_t DCKCFGR2; // 0x94 : Dedicated Clock Configuration Register 2
} RCC_TypeDef;

/* DMA Register Layout (Simplified for stream access) */
typedef struct {
    volatile uint32_t CR;       // 0x00 : Stream control register
    volatile uint32_t NDTR;     // 0x04 : Stream number of data register
    volatile uint32_t PAR;      // 0x08 : Stream peripheral address register
    volatile uint32_t M0AR;     // 0x0C : Stream memory 0 address register
    volatile uint32_t M1AR;     // 0x10 : Stream memory 1 address register
    volatile uint32_t FCR;      // 0x14 : Stream FIFO control register
} DMA_Stream_TypeDef;

typedef struct {
    uint32_t      RESERVED0[4];     // 0x00-0x0C : Reserved
    volatile uint32_t S[8];       // 0x10-0x2F : Stream control register array (S0-S7)
    uint32_t      RESERVED1[8];     // 0x30-0x4F : Reserved
    volatile uint32_t SxCR[8];      // 0x50-0x6F : Stream control register (S0-S7, duplicate/alias)
    volatile uint32_t SxNDTR[8];    // 0x70-0x8F : Stream number of data register (S0-S7, duplicate/alias)
    volatile uint32_t SxPAR[8];     // 0x90-0xAF : Stream peripheral address register (S0-S7, duplicate/alias)
    volatile uint32_t SxM0AR[8];    // 0xB0-0xCF : Stream memory 0 address register (S0-S7, duplicate/alias)
    volatile uint32_t SxM1AR[8];    // 0xD0-0xEF : Stream memory 1 address register (S0-S7, duplicate/alias)
    volatile uint32_t SxFCR[8];     // 0xF0-0x10F: Stream FIFO control register (S0-S7, duplicate/alias)
    uint32_t      RESERVED2[8];     // 0x110-0x12F: Reserved
    volatile uint32_t LISR;       // 0x130 : DMA low interrupt status register
    volatile uint32_t HISR;       // 0x134 : DMA high interrupt status register
    volatile uint32_t LIFCR;      // 0x138 : DMA low interrupt flag clear register
    volatile uint32_t HIFCR;      // 0x13C : DMA high interrupt flag clear register
} DMA_TypeDef;

/* SPI Register Layout (Simplified for full control) */
typedef struct {
    volatile uint32_t CR1;        // 0x00 : Control register 1
    volatile uint32_t CR2;        // 0x04 : Control register 2
    volatile uint32_t SR;         // 0x08 : Status register
    volatile uint32_t DR;         // 0x0C : Data register
    volatile uint32_t CRCPR;      // 0x10 : CRC polynomial register
    volatile uint32_t RXCRCR;     // 0x14 : Receive CRC register
    volatile uint32_t TXCRCR;     // 0x18 : Transmit CRC register
    volatile uint32_t I2SCFGR;    // 0x1C : I2S configuration register
    volatile uint32_t I2SPR;      // 0x20 : I2S prescaler register
} SPI_TypeDef;

#endif