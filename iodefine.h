/*
    Connect a PS / 2 keyboard to SHARP X1
    Definition of I / O pins, etc.

    Created July 22, 2014

    Kyoichi Sato http://kyoutan.jpn.org/

    There is no guarantee.
    The part created by Kyoichi Sato has no restrictions on its use. You can use it freely regardless of whether it is commercial or non-commercial.
    It means that you can copy, modify, distribute, or sell it without permission.
    No need to contact.
*/

#ifndef IODEFINE_H
#define IODEFINE_H

#ifdef __cplusplus
    extern "C" {
#endif

// LED
#define LED            p1_0

// UART
#define TXD            p1_4
#define RXD            p1_5

// PS/2 Interface.
#define PS2_DATA_IN    p3_3
#define PS2_DATA_OUT   p4_2
#define PS2_CLK_IN     p4_5
#define PS2_CLK_OUT    p1_1
#define PS2DATA        p3_3
#define X1KEYOUT       p3_7

// X1 Keyboard interface.
#define X1DATA         p3_7

#define TRUE           1
#define FALSE          0
#define ON             1
#define OFF            0
#define NULL           0

#define DI() asm ("FCLR I") // Interrupt disabled
#define EI() asm ("FSET I") // Interrupt enabled

// Types missing in the Renesas C compiler headers.
typedef unsigned char uint8_t;

#ifdef __cplusplus
}
#endif
#endif // IODEFINE_H
