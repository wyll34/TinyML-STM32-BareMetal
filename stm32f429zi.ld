MEMORY 
{ 
    FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 2M
    RAM (rwx) : ORIGIN = 0x20000000, LENGTH = 128K
}
SECTIONS
{
   /* Code and Read-Only Data in Flash */
   .text :
   {
      KEEP(*(.isr_vector)) /* Interrupt Vector Table */
      *(.text*) /* Code */
      *(.rodata*) /* Read-only data */
      . = ALIGN(4);
   } > FLASH

   /* Initialized Data in RAM */
   .data :
   {
      _data_start = .;         /* RAM address where startup begins copying TO */
      *(.data*)
      . = ALIGN(4);
      _data_end = .;           /* RAM address where startup stops copying */
   } > RAM AT> FLASH
   _data_load = LOADADDR(.data); /* FLASH address where initial values are stored (copy FROM) */

   /* Uninitialized Data in RAM */
   .bss :
   {
      _bss_start = .;          /* RAM address where startup begins zeroing */
      *(.bss*)
      . = ALIGN(4);
      _bss_end = .;            /* RAM address where startup stops zeroing */
   } > RAM

   /* Stack and Heap */
   _estack = ORIGIN(RAM) + LENGTH(RAM);
   _Min_Heap_Size = 0x100; /* Minimum heap size */
   _Min_Stack_Size = 0x400; /* Minimum stack size */

}