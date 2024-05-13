CFLAGS = -mno-save-restore -march=rv32i2p0 -mabi=ilp32 -nostartfiles -nostdlib -Ttext=0 -O1
LIBS = /usr/lib/gcc/riscv64-unknown-elf/10.2.0/rv32i/ilp32/libgcc.a

CC = riscv64-unknown-elf-gcc
OBJCOPY = riscv64-unknown-elf-objcopy
OBJDUMP = riscv64-unknown-elf-objdump

all: mem_init.v prog_hex conv_to_init

conv_to_init: conv_to_init.c
	gcc -o conv_to_init conv_to_init.c

mem_init.v: prog_bin conv_to_init
	rm -f ../src/mem_init.v
	./conv_to_init prog_bin > ../src/mem_init.v

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

prog: startup.s main.o
	$(CC) $(CFLAGS) -o prog startup.s main.c $(LIBS)

prog_bin: prog
	$(OBJCOPY) -j .text prog -O binary prog_bin

prog_hex: prog_bin
	od -t x4 -v prog_bin > prog_hex

clean:
	rm -f prog prog_hex prog_bin main.o conv_to_init

