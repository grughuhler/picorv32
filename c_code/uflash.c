/* Copyright 2024 Grug Huhler.  License SPDX BSD-2-Clause.
 *
 * UFLASH on Tang Nano 9K has 38 pages of 2048 bytes each.  Erase a page
 * by doing a byte-sized write (of any value) to the page.
 *
 * Writes to the UFLASH are accomplished by 32-bit writes to the address
 * to be written.
 *
 * Reads may be 8, 16, or 32-bits wide.
 */

#include "uflash.h"

void erase_page_uflash(unsigned int addr)
{
  volatile unsigned char *p = (volatile unsigned char *) addr;
  
  *p = 0;
}
