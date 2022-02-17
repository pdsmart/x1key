/*
    Connect a PS / 2 keyboard to SHARP X1
    X1 keyboard transmission process
    .
    I referred to the keyboard transmission format specifications of X1 Center (http://www.x1center.org/).
    I don't have the actual X1 keyboard, so it was very helpful.
    The material of X1 Center says, "If you have something you can use, feel free to use it."
    .
    
    Created July 22, 2014
    .
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
#include "x1key.h"
#include "iodefine.h"

unsigned short SEND_DATA; // X1 transmission data

void X1_send(unsigned short data)
{
    while (0 != tcstf_trjcr); // Wait until it finishes if sending

    SEND_DATA = data;

    /* TIMER RJ */
    trjioc = 0b00000011; /* Start output from TRJO H, no TRJIO toggle output */
    trjmr = 0b00000001; /* Pulse output mode, no frequency division */
    trjcr = 0b00000100; /* Stop counting Output pin initialization */
    pmh3 = 0b10000000; // P3_7 TRJO X1KEYOUT to TRJO
    trjie_trjir=1; /* TIMER RJ interrupt enabled */
    trj=TRJ250us; // 250us set (250us followed by L header) 250us has no meaning. May be shorter
    tstart_trjcr = 1; // Timer start
}

void x1key_init(void)
{
    X1KEYOUT = 1;

    /* TIMER RJ initialization */
    msttrj = 0; // Release standby
    trjcr = 0b00000100; /* Stop counting */
    trjioc = 0b00000011; /* Start output from TRJO H, no TRJIO toggle output */
    trjmr = 0b00000001; /* Pulse output mode, no frequency division */
    trjcr = 0b00000100; /* Stop counting Output pin initialization */
}

// TIMER RJ Underflow interrupt
// With the timer function, every time the down counter underflows
// The output of the output pin (TRJO) is inverted
#pragma INTERRUPT INT_trj (vect = 22)
void INT_trj(void)
{
    static unsigned char count = 0;

    if (0 == (count & 1)) // even L period
    {// 0 2 4 6 8 10 12 14 16 18 20 22 24 26 28 30 32 34 36
        switch (count)
        {
            case 0: // header L
                trj = TRJ1000us; // 1000us
                break;

            case 36: // stop End
                while (1 == tcstf_trjcr) tstart_trjcr = 0; // Timer stop (output is not initialized if it is a bit operation)
                trjcr = 0b00000100; // Stop counting Output pin initialization
                pmh3 = 0b10000000; // P3_7 TRJO X1KEYOUT to TRJO
                break;

            default:
                trj = TRJ250us; // 250us
                break;
        }
    }
    else // odd H period
    {// 1 3 5 7 9 11 13 15 17 19 21 23 25 27 29 31 33 35
        switch (count)
        {
            case 1: // header H
                trj = TRJ700us;
                break;

            // case 3: // start H I don't need a start bit
            // trj = TRJ750us;
            // break;

            case 35: // stop H period
                X1KEYOUT = 1; // H output
                pmh3 = 0b00000000; // P3_7 TRJO X1KEYOUT to I / O port Inverted output operation finished
                trj = TRJ1750us;
                break;

            default: // 16-bit transmission from the most significant bit
                if (0 == (SEND_DATA & 0x8000))
                {
                    trj = TRJ750us; // 0
                }
                else
                {
                    trj = TRJ1750us; // 1
                }
                SEND_DATA = (SEND_DATA << 1);
                break;
        }
    }

    count++;
    if (36 <count) count = 0;

    while (1 == trjif_trjir) trjif_trjir = 0; // Clear interrupt flag
}

// Send 1 byte for debugging in 2 hexadecimal digits (display)
void puth2(unsigned char a)
{
    X1_send (((unsigned short) 0b10111111 << 8) + tochar ((a & 0xF0) >> 4)); // Press
    X1_send (((unsigned short) 0b11111111 << 8) + 0x00); // Release

    X1_send (((unsigned short) 0b10111111 << 8) + tochar (a & 0x0F)); // Press
    X1_send (((unsigned short) 0b11111111 << 8) + 0x00); // Release

    X1_send (((unsigned short) 0b10111111 << 8) + 0x20); // Press
    X1_send (((unsigned short) 0b11111111 << 8) + 0x00); // Release
}

// Convert numbers from 0 to 15 for debugging to characters
unsigned char tochar(unsigned char a)
{
    if (a <10) a = a + 0x30;
    else a = a + 0x41-10;
    return a;
}

#ifdef __cplusplus
}
#endif
