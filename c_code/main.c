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

static inline unsigned int readtime(void)
{
  unsigned int val;
  unsigned long long jj;
  asm volatile("rdtime %0" : "=r" (val));
  return val;

}

void waste_time (int n)
{
  volatile int i;
  // An additional iteration takes about 0.556 uSec @ 27MHz
  while (n-- > 0) {i;}
    //    asm("nop");
}

int mem_test (void)
{
  int i, test, errors;
  unsigned long val, val_read;

  errors = 0;
  for (test = 0; test < sizeof(test_vals)/sizeof(test_vals[0]); test++) {

    for (i = 0; i < MEMSIZE; i++) mem[i] = test_vals[test];

    for (i = 0; i < MEMSIZE; i++) {
      val_read = mem[i];
      if (val_read != test_vals[test]) {
	uart_print_hex(val_read);
	uart_puts(" != ");
	uart_print_hex(test_vals[test]);
	uart_puts(", i = ");
	uart_print_hex(i);
	uart_puts("\r\n");
	errors += 1;
      }
    }
  }

  for (i = 0; i < MEMSIZE; i++) mem[i] = i + (i << 17);

  for (i = 0; i < MEMSIZE; i++) {
    val_read = mem[i];
    if (val_read != i + (i << 17)) {
      uart_print_hex(val_read);
      uart_puts(" != ");
      uart_print_hex(i + (i << 17));
      uart_puts(", i = ");
      uart_print_hex(i);
      uart_puts("\r\n");
      errors += 1;
    }
  }

  return errors;
}

int main()
{
  unsigned char v, ch;
  unsigned int i;

  cdt_write3(0x1); // addr 0x80000013
  cdt_write2(0x0); // addr 0x80000012
  cdt_write1(0x1); // addr 0x80000011
  cdt_write0(0x1); // addr 0x80000010
  cdt_write(0x01000000);
  cdt_write(0x00010000);
  cdt_write(0x00000100);
  cdt_write(0x00000001);

  uart_set_div(234); // 27000000/115200

  set_leds(6);

  uart_puts("\r\ntime is ");
  uart_print_hex(readtime());
  uart_puts(", mem_test = ");
  uart_print_hex(mem_test());
  uart_puts(", time is ");
  uart_print_hex(readtime());
  uart_puts("\r\n");

  cdt_write(0xde000000);
  uart_puts("CDT = ");
  uart_print_hex(cdt_read());
  uart_puts(" and then ");
  waste_time(1000);
  uart_print_hex(cdt_read());
  uart_puts("\r\n");

  cdt_write(0x7654);
  uart_puts("CDT = ");
  uart_print_hex(cdt_read());
  uart_puts(" and then ");
  waste_time(1);
  uart_print_hex(cdt_read());
  uart_puts(" and then ");
  waste_time(1);
  uart_print_hex(cdt_read());
  uart_puts("\r\n");
  
  uart_puts("Get 4 chars: ");
  for (i = 0; i < 4; i++) {
    ch = uart_getchar();
    uart_putchar(':');
    uart_putchar(ch);
    uart_putchar(':');
  }

  uart_puts("\r\n");

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

    cdt_delay(27000000/2);
    i += 1;
  }
  
  return 0;
}
