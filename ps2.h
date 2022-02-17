/*
    Connect a PS / 2 keyboard to SHARP X1
    PS / 2 keyboard reception processing

    Created July 22, 2014

    Kyoichi Sato http://kyoutan.jpn.org/

    There is no guarantee.
    The part created by Kyoichi Sato has no restrictions on its use. You can use it freely regardless of whether it is commercial or non-commercial.
    It means that you can copy, modify, distribute, or sell it without permission.
    No need to contact.
*/

#ifndef PS2_H
#define PS2_H

#ifdef __cplusplus
    extern "C" {
#endif


#define PS2TIMEOUT  30          // PS2 timeout 30 * 100 [ms] = 3 [s]
#define PS2BUFFSIZE 0x10

extern volatile unsigned short PS2TIMER; // PS2 receive timeout timer
// extern volatile unsigned char PS2BUFF [PS2BUFFSIZE]; // PS2 receive buffer
// extern volatile unsigned char PS2RPOS; // PS2 read position
// extern volatile unsigned char PS2WPOS; // PS2 write position

void          ps2key_init(void);
unsigned char ps2size(void);
void          ps2clear(void);
unsigned char ps2read(void);
unsigned char ps2get(void);

#ifdef __cplusplus
}
#endif
#endif // PS2_H
