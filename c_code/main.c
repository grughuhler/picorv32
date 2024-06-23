/* Copyright 2024 Grug Huhler.  License SPDX BSD-2-Clause.

   This is a test program for the simple SoC based on the PicoRV32 core.
   It can be used on either the Tang Nano 9K or the Tang Nano 20K, but
   only the former contains user flash.
*/

#include "leds.h"
#include "uart.h"
#include "countdown_timer.h"
#include "uflash.h"
#include "xorshift32.h"
#include "readtime.h"

#define MEMSIZE 512
unsigned int mem[MEMSIZE];
unsigned int test_vals[] = {0, 0xffffffff, 0xaaaaaaaa, 0x55555555, 0xdeadbeef};

/* A simple memory test.  Delete this and also array mem
   above to free much of the SRAM for other things
*/

int mem_test (void)
{
  int i, test, errors;
  unsigned int val, val_read;

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

void endian_test(void)
{
  volatile unsigned int test_loc = 0;
  volatile unsigned int *addr = &test_loc;
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

/* la_functions are useful for looking at the bus using a logic
   analyzer.
*/

void la_wtest(void)
{
  unsigned int v;
  volatile unsigned int *ip = (volatile unsigned int *) &v;
  volatile unsigned short *sp = (volatile unsigned short *) &v;
  volatile unsigned char *cp = (volatile unsigned char *) &v;

  *ip = 0x03020100;  // addr 0x00

  *sp = 0x0302;      // addr 0x00
  *(sp+1) = 0x0100;  // addr 0x02

  *cp = 0x03;        // addr 0x00
  *(cp+1) = 0x02;    // addr 0x01
  *(cp+2) = 0x01;    // addr 0x02
  *(cp+3) = 0x00;    // addr 0x03
}


void la_rtest(void)
{
  unsigned int v;
  volatile unsigned int *ip = (volatile unsigned int *) &v;
  volatile unsigned short *sp = (volatile unsigned short *) &v;
  volatile unsigned char *cp = (volatile unsigned char *) &v;

  *ip = 0x03020100;  // addr 0x00

  *ip;     // addr 0x00

  *sp;     // addr 0x00
  *(sp+1); // addr 0x02

  *cp;     // addr 0x00
  *(cp+1); // addr 0x01
  *(cp+2); // addr 0x02
  *(cp+3); // addr 0x03
}


void countdown_timer_test(void)
{
  unsigned int val;
  unsigned int test_errors = 0;
  
  // If register is little-endian, write to 0x80000013 should set
  // the MSB,  Does it?

  cdt_wbyte3(0xff);
  val = cdt_read();
  if ((val == 0xff000000) || (val < 0xfe000000)) test_errors = 1;

  // Write zero to most significant half-word.
  cdt_whalf2(0);
  val = cdt_read();
  if (val > 0xffff) test_errors |= 2;

  uart_puts("Countdown timer test ");
  if (test_errors) {
    uart_puts("FAILED, mask = ");
    uart_print_hex(test_errors);
    uart_puts("\r\n");
  } else {
    uart_puts("PASSED\r\n");
  }
}

void cycle_delay(unsigned int cycles)
{
  uart_puts("delay ");
  uart_print_hex(cycles);
  uart_puts(" cycles\r\n");
  cdt_delay(cycles);
  uart_puts("done\r\n");
}

void read_led(void)
{
  unsigned char v;
  
  v = get_leds();
  uart_puts("LED = ");
  uart_print_hex(v);
  uart_puts("\r\n");
}

void incr_led(void)
{
  unsigned char v;
  
  v = get_leds();
  set_leds(v+1);
}

void set_led(unsigned int value)
{
  set_leds(value);
}

void memory_test(void)
{
  if (mem_test())
    uart_puts("memory test FAILED.\r\n");
  else
    uart_puts("memory test PASSED.\r\n");
}

void read_clock(void)
{
  uart_puts("time is ");
  uart_print_hex(readtime());
  uart_puts("\r\n");
}

void read_clock_ll(void)
{
  time_ll_t t;

  t = readtime_ll();
  uart_puts("time is ");
  uart_print_hex(t.s.time_high);
  uart_putchar(':');
  uart_print_hex(t.s.time_low);
  uart_puts("\r\n");
}

void erase_all_flash(void)
{
  volatile unsigned char *p = (volatile unsigned char *) 0x20000;
  time_ll_t start, end;
  unsigned int diff;
  int i;

  start = readtime_ll();
  for (i = 0; i < 38; i++) {
    *p = 0;
    p += 2048;
    uart_print_hex(i);
    uart_puts("\r\n");
  }
  end = readtime_ll();

  diff = end.time_val - start.time_val;
  uart_puts("done in ");
  uart_print_hex(diff);
  uart_puts(" cycles\r\n");
}

void write_all_flash(void)
{
  volatile unsigned int *p = (volatile unsigned int *) 0x20000;
  unsigned int state = 1;
  time_ll_t start, end;
  unsigned int diff;
  int i;

  start = readtime_ll();
  for (i = 0; i < 19456; i++) {
    *p++ = xorshift32(&state);
  }
  end = readtime_ll();

  diff = end.time_val - start.time_val;
  uart_puts("done in ");
  uart_print_hex(diff);
  uart_puts(" cycles\r\n");
}

void check_all_flash(void)
{
  volatile unsigned int *p = (volatile unsigned int *) 0x20000;
  unsigned int state = 1;
  int i, j, errors = 0;

  for (i = 0; i < 304; i++) {
    uart_print_hex(0x20000 + (i << 8)); /* 0x20000 + 4*64*i */
    uart_putchar(' ');
    for (j = 0; j < 64; j++) {
      if (*p++ != xorshift32(&state)) {
	errors += 1;
	uart_putchar('x');
      } else {
	uart_putchar('.');
      }
    }
    uart_puts("\r\n");
  }
  
  uart_puts("errors = ");
  uart_print_hex(errors);
  uart_puts("\r\n");
}

void read2_flash(unsigned int addr)
{
  volatile unsigned int *p1 = (volatile unsigned int *) addr;
  volatile unsigned int *p2 = (volatile unsigned int *) (addr + 4);
  unsigned int v1, v2;

  v1 = *p1;
  v2 = *p2;

  uart_print_hex(v1);
  uart_putchar(' ');
  uart_print_hex(v2);
  uart_puts("\r\n");
}


void help(void)
{
  uart_puts("ct            : test countdown timer\r\n");
  uart_puts("dc cycles     : delay for cycles\r\n");
  uart_puts("cf            : check that uflash values match wf\r\n");
  uart_puts("ef addr       : erase page of uflash\r\n");
  uart_puts("af            : erase all uflash\r\n");
  uart_puts("wf            : write all uflash\r\n");
  uart_puts("et            : endian test\r\n");
  uart_puts("rl            : read LEDs\r\n");
  uart_puts("il            : increment LEDs\r\n");
  uart_puts("sl value      : set LEDs to value\r\n");
  uart_puts("mt            : memory test\r\n");
  uart_puts("rc            : read clock low\r\n");
  uart_puts("rd            : read clock hi:low\r\n");
  uart_puts("he            : print help\r\n");
  uart_puts("rb addr       : read byte\r\n");
  uart_puts("rh addr       : read half word\r\n");
  uart_puts("rw addr       : read word\r\n");
  uart_puts("wb addr value : write byte\r\n");
  uart_puts("wh addr value : write half word\r\n");
  uart_puts("ww addr value : write word\r\n");
  uart_puts("   all numbers are hex\r\n");
}

void read_byte(unsigned int addr)
{
  volatile unsigned char *p = (volatile unsigned char *) addr;

  uart_print_hex(*p);
  uart_puts("\r\n");
}

void read_half(unsigned int addr)
{
  volatile unsigned short *p = (volatile unsigned short *) addr;

  uart_print_hex(*p);
  uart_puts("\r\n");
}

void read_word(unsigned int addr)
{
  volatile unsigned int *p = (volatile unsigned int *) addr;

  uart_print_hex(*p);
  uart_puts("\r\n");
}

void write_byte(unsigned int addr, unsigned int value)
{
  volatile unsigned char *p = (volatile unsigned char *) addr;

  *p = value;
}

void write_half(unsigned int addr, unsigned int value)
{
  volatile unsigned short *p = (volatile unsigned short *) addr;

  *p = value;
}

void write_word(unsigned int addr, unsigned int value)
{
  volatile unsigned int *p = (volatile unsigned int *) addr;

  *p = value;
}

/* struct command lists available commands.  See function help.
 * Each function takes one or two arguments.  The table below
 * contains a pointer to the function to call for each command.
 */

struct command {
  char *cmd_string;
  int num_args;
  union {
    void (*func0)(void);
    void (*func1)(unsigned int val);
    void (*func2)(unsigned int val1, unsigned int val2);
  } u;
} commands[] = {
  {"ct", 0, .u.func0=countdown_timer_test},
  {"dc", 1, .u.func1=cycle_delay}, // cycles
  {"2f", 1, .u.func1=read2_flash}, // addr
  {"ef", 1, .u.func1=erase_page_uflash}, // addr
  {"cf", 0, .u.func0=check_all_flash},
  {"af", 0, .u.func0=erase_all_flash},
  {"wf", 0, .u.func0=write_all_flash},
  {"et", 0, .u.func0=endian_test},
  {"rl", 0, .u.func0=read_led},
  {"il", 0, .u.func0=incr_led},
  {"sl", 1, .u.func1=set_led},   // val
  {"mt", 0, .u.func0=memory_test},
  {"rc", 0, .u.func0=read_clock},
  {"rd", 0, .u.func0=read_clock_ll},
  {"he", 0, .u.func0=help},
  {"rb", 1, .u.func1=read_byte}, // addr
  {"rh", 1, .u.func1=read_half}, // addr
  {"rw", 1, .u.func1=read_word}, // addr
  {"wb", 2, .u.func2=write_byte}, // addr, val
  {"wh", 2, .u.func2=write_half}, // addr, val
  {"ww", 2, .u.func2=write_word} // addr, val
};


void eat_spaces(char **buf, unsigned int *len)
{
  while (len > 0) {
    if (**buf == ' ') {
      *buf += 1;
      *len -= 1;
    } else
      break;
  }
}

/* Returns 1 if a number found, else 0.  Number is in *v */

unsigned int get_hex(char **buf, unsigned int *len, unsigned int *v)
{
  int valid = 0;
  int keep_going;
  char ch;

  keep_going = 1;

  *v = 0;
  while (keep_going && (*len > 0)) {

    ch = **buf;
    *buf += 1;
    *len -= 1;

    if ((ch >= '0') && (ch <= '9')) {
      *v = 16*(*v) + (ch - '0');
      valid = 1;
    } else if ((ch >= 'a') && (ch <= 'f')) {
      *v = 16*(*v) + (ch - 'a' + 10);
      valid = 1;
    } else if ((ch >= 'A') && (ch <= 'F')) {
      *v = 16*(*v) + (ch - 'A' + 10);
      valid = 1;
    } else {
      keep_going = 0;
    }
  }

  return valid;
}


void parse(char *buf, unsigned int len)
{
  int i, cmd_not_ok;
  unsigned int val1, val2;

  cmd_not_ok = 1;
  eat_spaces(&buf, &len);
  if (len < 2) goto err;

  for (i = 0; i < sizeof(commands)/sizeof(commands[0]); i++)
    if ((buf[0] == commands[i].cmd_string[0]) && (buf[1] == commands[i].cmd_string[1])) {
      buf += 2;
      len -= 2;
      switch (commands[i].num_args) {
      case 0:
	commands[i].u.func0();
	cmd_not_ok = 0;
	break;
      case 1:
	eat_spaces(&buf, &len);
	if (get_hex(&buf, &len, &val1)) {
	  commands[i].u.func1(val1);
	  cmd_not_ok = 0;
	}
	break;
      case 2:
	eat_spaces(&buf, &len);
	if (get_hex(&buf, &len, &val1)) {
	  eat_spaces(&buf, &len);
	  if (get_hex(&buf, &len, &val2)) {
	    commands[i].u.func2(val1, val2);
	    cmd_not_ok = 0;
	  }
	}
	break;
      default:
	break;
      }
      break;
    }

 err:
  if (cmd_not_ok)
    uart_puts("illegal command, he for help\r\n");
}

#define BUFLEN 64

int main()
{
  char buf[BUFLEN];
  unsigned int len;
  
  set_leds(6);
  la_wtest(); /* Could delete these.  They produce transactions that are */
  la_rtest(); /* nice to view on a logic analyzer. */

  uart_set_div(CLK_FREQ/115200.0 + 0.5);
  
  uart_puts("\r\nStarting, CLK_FREQ: 0x");
  uart_print_hex(CLK_FREQ);
  uart_puts("\r\nenter he for a list of commands\r\n");

  while (1) {
    len = uart_gets(buf, BUFLEN);
    if (len != 0) parse(buf, len);
  }

  return 0;
}
