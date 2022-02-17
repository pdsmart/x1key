/*
    Connect a PS / 2 keyboard to SHARP X1
    X1 keyboard transmission process

    Created July 22, 2014

    Kyoichi Sato http://kyoutan.jpn.org/

    There is no guarantee.
    The part created by Kyoichi Sato has no restrictions on its use. You can use it freely regardless of whether it is commercial or non-commercial.
    It means that you can copy, modify, distribute, or sell it without permission.
    No need to contact.
*/

#ifndef X1KEY_H
#define X1KEY_H

#ifdef __cplusplus
    extern "C" {
#endif

/*  18.432MHz
    1     cycle = 1 / 18.432 = 0.0543 [us]
    256   cycles = 256 / 18.432 = 13.89 [us]
    65536 cycle = 65536 / 18.432 = 3555 [us]
    
    1750 [us] 1750 / (1 / 18.432) = 1750 * (18.432/1) = 1750 * 18.432
                            = 32256 = 0x7E00
    1000 [us] = 1000 * 18.432 = 18432 Cycle = 0x4800
     750 [us] = 13824 = 0x3600
     700 [us] = 12902 = 0x3266
     250 [us] = 250 * 18.432 = 4608 = 0x1200
      
    I thought that the program waveform generation mode of timer RB2 would be good, but for the primary period
    I couldn't request an interrupt due to underflow, so
    I decided to use the pulse output mode of timer RJ2.
*/

#define TRJ1750us (0x7E00 - 2)
#define TRJ1000us (0x4800 - 0) // Since it is used with the timer stopped, do not pull here
#define TRJ750us  (0x3600 - 2)
#define TRJ700us  (0x3266 - 1)
#define TRJ250us  (0x1200 - 2)
#define TRJSTOP   (0xFFFF) // Good night period after stop bit
// It takes 2 to 3 cycles from writing to the TRJ register to transferring to the reload register.
// Pull that much

void          X1_send(unsigned short data);
void          x1key_init(void);

unsigned char tochar(unsigned char a);
void          puth2(unsigned char a);

// extern volatile unsigned short SEND_DATA; // X1 transmission data
// b15: 0 = Input from the numeric keypad
// b14: 0 = With key input
// b13: 0 = With repeat
// b12: 0 = GRAPH ON
// b11: 0 = CAPS ON
// b10: 0 = Kana ON
// b09: 0 = SHIFT ON
// b08: 0 = CTRL ON
// b07-00: ASCII CODE (0x00 = KEY OFF)

#ifdef __cplusplus
}
#endif
#endif // X1KEY_H
