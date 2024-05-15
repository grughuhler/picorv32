#ifndef COUNTDOWN_TIMER_H
#define COUNTDOWN_TIMER_H

/* Copyright 2024 Grug Huhler
 *
 * License: SPDX BSD-2-Clause.
 */

extern void cdt_write0(const unsigned char value);
extern void cdt_write1(const unsigned char value);
extern void cdt_write2(const unsigned char value);
extern void cdt_write3(const unsigned char value);

extern void cdt_write(const unsigned int value);
extern unsigned int cdt_read(void);
extern void cdt_delay(const unsigned int value);
#endif

