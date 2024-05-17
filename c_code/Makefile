# Just run "make" to create ../src/mem_init.v

CFLAGS = -mno-save-restore -march=rv32i2p0 -mabi=ilp32 -nostartfiles -nostdlib -static -O1
LIBS = /usr/lib/gcc/riscv64-unknown-elf/10.2.0/rv32i/ilp32/libgcc.a

CC = riscv64-unknown-elf-gcc
OBJCOPY = riscv64-unknown-elf-objcopy
OBJDUMP = riscv64-unknown-elf-objdump

all: mem_init.v prog.hex conv_to_init

conv_to_init: conv_to_init.c
	gcc -o conv_to_init conv_to_init.c

mem_init.v: prog.bin conv_to_init
	rm -f ../src/mem_init.v
	./conv_to_init prog.bin > ../src/mem_init.v

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

countdown_timer.o: countdown_timer.c
	$(CC) $(CFLAGS) -c countdown_timer.c

uart.o: uart.c
	$(CC) $(CFLAGS) -c uart.c

leds.o: leds.c
	$(CC) $(CFLAGS) -c leds.c

prog.elf: startup.s main.o countdown_timer.o uart.o leds.o
	$(CC) $(CFLAGS) -Tlink_cmd.ld -o prog.elf startup.s main.o countdown_timer.o \
          uart.o leds.o $(LIBS)

prog.bin: prog.elf
	$(OBJCOPY) prog.elf -O binary prog.bin
#	$(OBJCOPY) -j .text prog -O binary prog.bin

prog.hex: prog.bin
	od -v -Ax -t x4 prog.bin > prog.hex

clean:
	rm -f prog.elf prog.hex prog.bin main.o conv_to_init countdown_timer.o uart.o leds.o

