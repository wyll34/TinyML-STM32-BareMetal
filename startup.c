#include <stdint.h>

extern uint32_t _data_start;
extern uint32_t _data_end;
extern uint32_t _data_load;
extern uint32_t _bss_start;
extern uint32_t _bss_end;
extern uint32_t _estack;

void Reset_Handler(void);
void Default_Handler(void);
int main(void);

__attribute__((section(".isr_vector")))
uint32_t vector_table[] = {
    (uint32_t)&_estack,
    (uint32_t)&Reset_Handler,
    (uint32_t)&Default_Handler,
    (uint32_t)&Default_Handler,
    (uint32_t)&Default_Handler,
    (uint32_t)&Default_Handler,
    (uint32_t)&Default_Handler,
    0, 0, 0, 0,
    (uint32_t)&Default_Handler,
    0, 0,
    (uint32_t)&Default_Handler,
    (uint32_t)&Default_Handler,
};

void Reset_Handler(void) {
    *((volatile uint32_t *)0xE000ED88) |= ((3UL << 20) | (3UL << 22));
    __asm__ volatile ("dsb \n isb");

    uint32_t *src = &_data_load;
    uint32_t *dst = &_data_start;
    while (dst < &_data_end) {
        *dst++ = *src++;
    }

    dst = &_bss_start;
    while (dst < &_bss_end) {
        *dst++ = 0;
    }

    main();
    while (1);
}

void Default_Handler(void) {
    *((volatile uint32_t *)0x40023830) |= (1 << 6);
    *((volatile uint32_t *)0x40021800) |= (1 << (14 * 2));
    *((volatile uint32_t *)0x40021818) = (1 << 14);
    while (1);
}

int _close(int file) { (void)file; return -1; }
int _lseek(int file, int ptr, int dir) { (void)file; (void)ptr; (void)dir; return 0; }
int _read(int file, char *ptr, int len) { (void)file; (void)ptr; (void)len; return 0; }
int _write(int file, char *ptr, int len) { (void)file; (void)ptr; return len; }