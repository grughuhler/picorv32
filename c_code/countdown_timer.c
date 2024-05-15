/* Copyright 2024 Grug Huhler
 *
 * License: SPDX BSD-2-Clause.
 */

#include "countdown_timer.h"

#define CDT_COUNTER ((volatile unsigned int *) 0x80000010)
#define CDT_COUNTER_B0 ((volatile unsigned char *) 0x80000010)
#define CDT_COUNTER_B1 ((volatile unsigned char *) 0x80000011)
#define CDT_COUNTER_B2 ((volatile unsigned char *) 0x80000012)
#define CDT_COUNTER_B3 ((volatile unsigned char *) 0x80000013)

void cdt_write0(const unsigned char value)
{
  *CDT_COUNTER_B0 = value;
}

void cdt_write1(const unsigned char value)
{
  *CDT_COUNTER_B1 = value;
}

void cdt_write2(const unsigned char value)
{
  *CDT_COUNTER_B2 = value;
}

void cdt_write3(const unsigned char value)
{
  *CDT_COUNTER_B3 = value;
}

void cdt_write(const unsigned int value)
{
  *CDT_COUNTER = value;
}

unsigned int cdt_read(void)
{
  return *CDT_COUNTER;
}

void cdt_delay(const unsigned int value)
{
  cdt_write(value);
  while (*CDT_COUNTER) {}
}
