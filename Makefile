all: compile load

compile: 
	arm-none-eabi-gcc src/main.c startup.c syscalls.c -T linker_script.ld -o bin/blink.elf -mcpu=cortex-m3 -mthumb -specs=nano.specs -specs=nosys.specs -Wall -Wextra -O0 -g3

load: 
	openocd -f interface/stlink.cfg -f target/stm32f1x.cfg -c "program bin/blink.elf verify reset exit"

