all: compile load

compile: 
	arm-none-eabi-gcc src/main.c startup.c -T linker_script.ld -o bin/blink.elf -mcpu=cortex-m3 -mthumb -nostdlib

load: 
	openocd -f interface/stlink.cfg -f target/stm32f1x.cfg -c "program bin/blink.elf verify reset exit"

