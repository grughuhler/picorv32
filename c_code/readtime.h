/* Copyright 2024 Grug Huhler.  License SPDX BSD-2-Clause.

   The picorv32 core implements several counters and instructions to
   access them.  These are part of the risc-v specification.  Functions
   in this file use two of them.
*/

#ifndef _READTIME_H
#define _READTIME_H

typedef union {
  struct {
    unsigned int time_low;
    unsigned int time_high;
  } s;
  unsigned long long time_val;
} time_ll_t;

static inline time_ll_t readtime_ll(void)
{
  unsigned int tmp;
  time_ll_t t;

  do {
    asm volatile("rdtimeh %0" : "=r" (t.s.time_high));
    asm volatile("rdtime %0" : "=r" (t.s.time_low));
    asm volatile("rdtimeh %0" : "=r" (tmp));
  } while (tmp != t.s.time_high);
  
  return t;
}

static inline unsigned int readtime(void)
{
  unsigned int time_low;

  asm volatile("rdtime %0" : "=r" (time_low));
  return time_low;

}

#endif
