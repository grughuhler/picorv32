/* Copyright 2024 Grug Huhler.  License SPDX BSD-2-Clause.

   This program shows counting on the LEDs of the Tang Nano
   9K FPGA development board.

   Function foo lets one see stores of different size and alignment by
   looking at the wstrb signals from the pico32rv.
*/

#include "leds.h"
#include "uart.h"
#include "countdown_timer.h"

#define MEMSIZE 1000
unsigned long mem[MEMSIZE];
unsigned long test_vals[] = {0, 0xffffffff, 0xaaaaaaaa, 0x55555555, 0xdeadbeef};


/* A simple memory test.  Delete this and also array mem
   above to free much of the SRAM for other things
*/

int mem_test (void)
{
  int i, test, errors;
  unsigned long val, val_read;

  errors = 0;
  for (test = 0; test < sizeof(test_vals)/sizeof(test_vals[0]); test++) {

    for (i = 0; i < MEMSIZE; i++) mem[i] = test_vals[test];

    for (i = 0; i < MEMSIZE; i++) {
      val_read = mem[i];
      if (val_read != test_vals[test]) errors += 1;
    }
  }

  for (i = 0; i < MEMSIZE; i++) mem[i] = i + (i << 17);

  for (i = 0; i < MEMSIZE; i++) {
    val_read = mem[i];
    if (val_read != i + (i << 17)) errors += 1;
  }

  return(errors);
}

/* The picorv32 core implements several counters and
   instructions to access them.  These are part of the
   risc-v specification.  Function readtime uses one
   of them.
*/

static inline unsigned int readtime(void)
{
  unsigned int val;
  unsigned long long jj;
  asm volatile("rdtime %0" : "=r" (val));
  return val;

}

void endian_test(volatile unsigned int *addr)
{
  volatile unsigned char *cp0, *cp3;
  char byte0, byte3;
  unsigned int i, ok;

  cp0 = (volatile unsigned char *) addr;
  cp3 = cp0 + 3;
  *addr = 0x44332211;
  byte0 = *cp0;
  byte3 = *cp3;
  *cp3 = 0xab;
  i = *addr;

  ok = (byte0 == 0x11) && (byte3 == 0x44) && (i == 0xab332211);
  uart_puts("\r\nEndian test: at ");
  uart_print_hex((unsigned int) addr);
  uart_puts(", byte0: ");
  uart_print_hex((unsigned int) byte0);
  uart_puts(", byte3: ");
  uart_print_hex((unsigned int) byte3);
  uart_puts(",\r\n     word: ");
  uart_print_hex(i);
  if (ok)
    uart_puts(" [PASSED]\r\n");
  else
    uart_puts(" [FAILED]\r\n");
}

void uart_rx_test(void)
{
  char buf[5];
  int i;
  
  uart_puts("Type 4 characters (they will echo): ");
  for (i = 0; i < 4; i++) {
    buf[i] = uart_getchar();
    uart_putchar(buf[i]);
  }
  buf[4] = 0;
  uart_puts("\r\nUART read: ");
  uart_puts(buf);
  uart_puts("\r\n");
}


int main()
{
  int i;
  unsigned char v, ch;

  set_leds(6);

  /* These are interesting to see on the logic analyzer */
  cdt_write3(0x1); // addr 0x80000013
  cdt_write2(0x0); // addr 0x80000012
  cdt_write1(0x1); // addr 0x80000011
  cdt_write0(0x1); // addr 0x80000010
  cdt_write(0x01000000);
  cdt_write(0x00010000);
  cdt_write(0x00000100);
  cdt_write(0x00000001);

  uart_set_div(234); // 27000000/115200
  endian_test((volatile unsigned int *)&i);

  /* Run the mem_test */
  if (mem_test())
    uart_puts("memory test FAILED.\r\n");
  else
    uart_puts("memory test PASSED.\r\n");

  /* Play with the CDT */
  cdt_write(0xde000000);
  uart_puts("CDT = ");
  uart_print_hex(cdt_read());
  uart_puts(" and then ");
  uart_print_hex(cdt_read());
  uart_puts("\r\n");

  cdt_write(0xfeee);
  uart_puts("CDT = ");
  uart_print_hex(cdt_read());
  uart_puts(" and then ");
  uart_print_hex(cdt_read());
  uart_puts(" and then ");
  uart_print_hex(cdt_read());
  uart_puts("\r\n");
  
  /* Test UART input */
  uart_rx_test();
  
  uart_puts("\r\nPress a key to start LED counting and lots of prints:\r\n");
  (void) uart_getchar();

  /* Print stuff over and over and have the LED count,
     both writing and reading the LED.
  */

  i = 0;
  while (1) {
    uart_puts("\r\nLoop ");
    uart_print_hex(i);
    uart_puts("\r\n");
    v = get_leds();
    set_leds(v+1);
    
    uart_puts("Four score and seven years ago our fathers brought forth upon this\r\n"
	      "continent, a new nation, conceived in Liberty, and dedicated to the\r\n"
	      "proposition that all men are created equal.  Now we are engaged in a\r\n"
	      "great civil war, testing whether that nation, or any nation so\r\n"
	      "conceived and so dedicated, can long endure. We are met on a great\r\n"
	      "battle-field of that war. We have come to dedicate a portion of that\r\n"
	      "field, as a final resting place for those who here gave their lives that\r\n"
	      "that nation might live. It is altogether fitting and proper that we\r\n"
	      "should do this.\r\n");
    
    uart_puts("time is ");
    uart_print_hex(readtime());
    uart_puts("\r\n");

    cdt_delay(27000000);
    i += 1;
  }
  
  return 0;
}
