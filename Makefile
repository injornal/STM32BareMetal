Q := @
ifneq ($(V),)
Q :=
endif

SRC := $(wildcard src/*.c) $(wildcard src/**/*.c)

LINKER_SCRIPT := linker_script.ld

all: clean build load
	$(Q)echo "All tasks complete."

build: bin/blink.elf
	$(Q)echo "Build complete.\n"

bin/blink.elf: $(SRC) $(LINKER_SCRIPT)
	$(Q)echo "Building project..."
	$(Q)arm-none-eabi-gcc -Iinclude $(SRC) -T $(LINKER_SCRIPT) -o bin/blink.elf \
		-mcpu=cortex-m3 \
		-mthumb \
		-specs=nano.specs \
		-specs=nosys.specs \
		-Wall -Wextra -O0 -g3

load: 
	$(Q)echo "Loading project to target..."
	$(Q)openocd -f interface/stlink.cfg -f target/stm32f1x.cfg -c "program bin/blink.elf verify reset exit"
	$(Q)echo "Load complete.\n"

clean: 
	$(Q)echo "Cleaning project..."
	$(Q)rm -f bin/blink.elf
	$(Q)echo "Clean complete.\n"


.PHONY: all build load clean

