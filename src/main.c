/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Name:            main.c
// Created:         Jan 2022
// Version:         v1.0
// Author(s):       Philip Smart
// Description:     X1 Keyboard logic.
//                  This source file was originally written by Sato Kyoichi and has been translated and
//                  changes made to accommodate the updated hardware key. Please see this repository
//                  for the original source if needed.
//                  It contains the startup code to configure the I/O and timers, initialise the sub-
//                  modules and then start key conversion, incoming keys are mapped realtime and sent
//                  immediately to the X1.
// Credits:         Kyoichi Sato http://kyoutan.jpn.org/ - Firmware based on and uses components of
//                  X1Keybord by Kyoichi Sato.
// Copyright:       Changes from original source (c) 2022 Philip Smart <philip.smart@net2net.org>
//
// History:         Jan 2022 - Initial write.
//
// Notes:           See Makefile to enable/disable conditional components
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
// This source file is free software: you can redistribute it and#or modify
// it under the terms of the GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This source file is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
/////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
    extern "C" {
#endif

#include "sfr_r8m12a.h"
#include "iodefine.h"
#include "x1key.h"
#include "ps2.h"
#include "timer.h"
#include "keyconv.h"
#include "main.h"

void main(void)
{
    // Locals.
    int           idx;
    unsigned long idx2;

    // Disable interrupts during config to prevent spurious triggers.
    DI();

    // Initialise the oscillator, use internal 18.432MHz clock.
    osc_init();

    // Initialise all the I/O ports, default interrupts/level and timer settings.
    io_init();

    // Start the PS/2 timer.
    timer_start();

/*
    // PS / 2 lead test
    while (TRUE)
    {
        puth2 (ps2get ());
        // puth2 (PS2RPOS);
        // puth2 (PS2WPOS);
    }
*/

/*
    // X1 delivery test
    {
        unsigned short a = 0;
        unsigned char b = 0x20;

        while (TRUE)
        {
            if (10 <(TIMER-a)) // Every second
            {
                a = TIMER;

                X1_send (((unsigned short) 0b10111111 << 8) + b); // Press
                X1_send (((unsigned short) 0b11111111 << 8) + 0x00); // Release

                if (0x7F == b)
                {
                    b = 0x20;
                }
                else
                {
                    b ++;
                }
            }
        }
    }
*/

    // Enable interrupts as we are ready to process PS/2 data -> MZ-2500/2800.
    EI();

    // Feel good factor, show the unit is working by blinking LED.
    for(idx=0; idx < 10; idx++)
    {
        LED = OFF;
        for(idx2=0; idx2 < 100000; idx2++);
        LED = ON;
        for(idx2=0; idx2 < 100000; idx2++);
    }
    LED = OFF;

    // Key conversion. PS/2 data arrives via interrupt, X1 data is sent via interrupt.
    while(TRUE)
    {
        keyconv();
    }
}

void osc_init(void)
{
    // Switch to built-in high speed oscillator
    prc0 = 1;                                  // Clock register permission
    ococr = 0b00000001;                        // High-speed on-chip oscillator oscillation Low-speed oscillation
    {
        unsigned char a;
        for (a = 0; a <255; a ++) asm ("nop"); // Wait for the oscillator oscillation to stabilize, so wait for an appropriate amount of time.
    }
    sckcr = 0b01000000;                        // Select high speed by selecting XIN / high speed oscillator No CPU clock division
    ckstpr = 0b10000000;                       // Select high speed by system clock low speed / high speed selection
    phisel = 0x00;                             // No system clock division
    frv1 = fr18s0;                             // Adjust high speed on-chip oscillator to 18.432MHz
    frv2 = fr18s1;
    prc0 = 0;                                  // Clock register access prohibited
}

void io_init(void)
{
    // I/O ports available on processor and applicable use.
    //
    // P1_0 LED   - 1 = Light LED, 0 = LED off.
    // P1_1 PS2_CLK_OUT  - Clock output to PS/2 keyboard (used to pull clock low as keyboard generates actual clock).
    // P1_2 
    // P1_3 
    // P1_4 TXD   - for writing / communication
    // P1_5 RXD   - communication (TRJIO)
    // P1_6 RXD   - for writing (TRJO)
    // P1_7 

    // P3_3 PS2_DATA_IN  - (/INT3) Input data from PS/2 keyboard.
    // P3_4 
    // P3_5 
    // P3_7 X1DATA - used when hardware converts PS/2 to X1 data stream. (TRJ0)

    // P4_2 PS2_DATA_OUT - Output data to PS/2 keyboard.
    // P4_5 PS2_CLK_IN(/INT0) - Clock intput from PS/2 keyboard.
    // P4_6 
    // P4_7 

    // PA_0 /RESET

    // Setup default pin value.
    X1KEYOUT = 1;
    p1_4 = 1; // TXD

    // Port orientation 0: Input, 1: Output
    pd1 = 0b00011111;           // P1_5 P1_6 RXD are inputs. LED, PS2_CLK_OUT, TXD are outputs.
    pd3 = 0b11110111;           // P3_7 TRJO X1KEYOUT output | P3_3 PS/2 DATA input
    pd4 = 0b11011111;           // P4_5 INT0 PS/2 CLK input

    // Pull up 0: No. 1: Yes
    pur1 = 0b01100000;          // RXD
    pur3 = 0b00001000;          // PS2_DATA_IN
    pur4 = 0b00100000;          // PS2_CLK_IN

    // Open drain output 1: Yes
    pod1 = 0b00000000;
    pod3 = 0b00001000;
    pod4 = 0b00100000;

    // Initialise the sub modules for data in/out.
    x1key_init ();
    ps2key_init ();
    timer_init ();

    // Interrupt priority level
    // CAUTION: It takes more than 20 cycles to respond to interrupts. ie. 1uS at 20MHz
    ilvlb = 0x01;               // TIMER RJ 1
    ilvlc = 0x01;               // TIMER RB 1
    ilvle = 0x20;               // INT0 2 Higher priority than TIMER RJ

    // Processor interrupt priority level 0 (interrupts higher than this value are accepted)
    asm ("LDIPL # 0");

    // Port mapping
    pml1 = 0b00000000;          // P1_0 I/O Port for LED control.
    pmh1 = 0b00000101;          // P1_4: TXD P1_5: RXD
    pmh1e = 0b00000000;
    pml3 = 0b00000000;
    pmh3 = 0b10000000;          // P3_7 TRJO X1KEYOUT
    pml4 = 0b00000000;
    pmh4 = 0b00000100;          // P4_5 INT0 PS / 2 CLK
    pmh4e = 0b00010000;         // P4_6 Enhanced bit to 0
    pamcr = 0b00010001;         // PA reset
}

#ifdef __cplusplus
}
#endif
