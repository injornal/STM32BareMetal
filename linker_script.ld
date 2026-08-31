MEMORY
{
    FLASH (rx): ORIGIN = 0x08000000, LENGTH = 64K
    SRAM (xrw): ORIGIN = 0x20000000, LENGTH = 20K
}

PROVIDE(_stack = ORIGIN(SRAM) + LENGTH(SRAM));

SECTIONS
{
    .isr_vector :
    {
        . = ALIGN(4);
        KEEP(*(.isr_vector))
        . = ALIGN(4);
    } > FLASH

    .text :
    {
        *(.text)
        *(.text.*)
        . = ALIGN(4);
    } > FLASH

    .rodata :
    {
        *(.rodata)
        *(.rodata.*)
        . = ALIGN(4);
    } > FLASH

    _la_data = LOADADDR(.data);

    .data :
    {
        _sdata = .;
        *(.data)
        *(.data.*)
        . = ALIGN(4);
        _edata = .;
    } > SRAM AT > FLASH

    .bss :
    {
        _sbss = .;
        __bss_start__ = _sbss;
        *(.bss)
        *(.bss.*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
        __bss_end__ = _ebss;
    } > SRAM

    .heap :
    {
        _sheap = .;
        . = . + 0x1000; /* 4KB mean heap size */
        . = ALIGN(4);
        _eheap = .;
        end = _eheap;
    } > SRAM
}

