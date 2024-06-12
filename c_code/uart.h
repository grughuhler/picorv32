/* Copyright 2024 Grug Huhler.  License SPDX BSD-2-Clause.
*/

#ifndef _UART_H
#define UART_H

extern void uart_set_div(unsigned int div);
extern void uart_print_hex(unsigned int val);
extern char uart_getchar(void);
extern void uart_putchar(char ch);
extern void uart_puts(char *s);
extern unsigned int uart_gets(char *buf, unsigned int buf_len);
extern unsigned int uart_get_hex(void);

#endif
