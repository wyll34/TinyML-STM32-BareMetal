CROSS_COMPILE ?= arm-none-eabi-
CC      = $(CROSS_COMPILE)gcc
OBJCOPY = $(CROSS_COMPILE)objcopy
SIZE    = $(CROSS_COMPILE)size

TARGET  = main
SRCS    = main.c startup.c spi.c ili9341.c stmpe811.c
LDSCRIPT = stm32f429zi.ld

MCUFLAGS = -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard
CFLAGS   = $(MCUFLAGS) -Wall -Wextra -O2 -g --specs=nosys.specs --specs=nano.specs
LDFLAGS  = $(MCUFLAGS) -T $(LDSCRIPT) -Wl,-Map=$(TARGET).map -Wl,--gc-sections

.PHONY: all clean size

all: $(TARGET).elf $(TARGET).hex size

$(TARGET).elf: $(SRCS) $(LDSCRIPT)
	$(CC) $(CFLAGS) $(SRCS) $(LDFLAGS) -o $@

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex $< $@

size: $(TARGET).elf
	$(SIZE) $<

clean:
	rm -f $(TARGET).elf $(TARGET).hex $(TARGET).map *.o