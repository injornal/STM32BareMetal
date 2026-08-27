MEMORY
{
  FLASH (rx): ORIGIN = 0x08000000, LENGTH = 64K
  SRAM (xrw): ORIGIN = 0x20000000, LENGTH = 20K
}
PROVIDE(_stack = ORIGIN(SRAM) + LENGTH(SRAM));
SECTIONS
{
    .text : {
	*(.vectors)
	*(.text*)
	. = ALIGN(4);
    } > FLASH
}
