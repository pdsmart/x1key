/*
    Connect a PS / 2 keyboard to SHARP X1
    100 [ms] interval timer

    I am using it for the PS/2 reception timeout.


    Created July 22, 2014

    Kyoichi Sato http://kyoutan.jpn.org/

    There is no guarantee.
    The part created by Kyoichi Sato has no restrictions on its use. You can use it freely regardless of whether it is commercial or non-commercial.
    It means that you can copy, modify, distribute, or sell it without permission.
    No need to contact.
*/

#ifdef __cplusplus
    extern "C" {
#endif

#include "sfr_r8m12a.h"
#include "timer.h"
#include "ps2.h"
#include "iodefine.h"

volatile unsigned short TIMER = 0; // 100 [ms] timer

void timer_init(void)
{
    // TIMER RB2 initialization 100ms interval timer
    msttrb = 0; // Release standby
    trbcr = 0b00000100; // Stop counting
    trbmr = 0b01100100; // f64 division 16-bit timer mode
/*
 100 [ms] = 100000 [us] = f1  1843200
                          f2  921600
                          f4  460800
                          f8  230400
                          f16 115200
                          f32 57600
                          f64 28800 = 0x7080
*/
    // 100 [ms] = f64 0x7080
    trbpre = 0x80;   // Lower 8 bits
    trbpr = 0x70;    // Upper 8 bits
    trbie_trbir = 1; // Interrupt enabled
}

/*
void timer_start(void)
{
    tstart_trbcr = 1; // TIMER RB2 count start
}
*/

#pragma INTERRUPT INT_trb(vect=24)
void INT_trb(void)
{
    TIMER++;
    if (0xFF != PS2TIMER) PS2TIMER++; // Do not overflow for timeout processing

    while (trbif_trbir == 1) trbif_trbir = 0; // Clear interrupt flag
}

#ifdef __cplusplus
}
#endif
