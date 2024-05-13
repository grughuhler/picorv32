/* Copyright 2024 Grug Huhler.  License SPDX BSD-2-Clause.

   This program shows counting on the LEDs of the Tang Nano
   9K FPGA development board.

   Function foo lets one see stores of different size and alignment by
   looking at the wstrb signals from the pico32rv.
*/

void foo(void)
{
  volatile int *ip = (volatile int *) 0x244;
  volatile short *sp0 = (volatile short *) 0x250;
  volatile short *sp1 = (volatile short *) 0x252;
  volatile char *cp0 = (volatile char *) 0x250;
  volatile char *cp1 = (volatile char *) 0x251;
  volatile char *cp2 = (volatile char *) 0x252;
  volatile char *cp3 = (volatile char *) 0x253;
  
  *ip = 6;
  *sp0 = 6;
  *sp1 = 6;
  *cp0 = 6;
  *cp1 = 6;
  *cp2 = 6;
  *cp3 = 6;
}

void waste_time (int n)
{
  // An additional iteration takes about 0.556 uSec @ 27MHz
  while (n-- > 0)
    asm("nop");
}

void set_leds(unsigned char val)
{
  volatile unsigned char *p = (volatile unsigned char *) 0x80000000;
  *p = val;
}

unsigned char get_leds(void)
{
  volatile unsigned char *p = (volatile unsigned char *) 0x80000000;
  return (*p);
}

int main()
{
  unsigned char v;
  foo();

  set_leds(0);
  while (1) {
    v = get_leds();
    set_leds(v+1);
    waste_time(900000);
  }
  
  return 0;
}

