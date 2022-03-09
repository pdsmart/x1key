#ifdef __cplusplus
    extern "C" {
#endif

#include "sfr_r8m12a.h"
#include "uart.h"

// Buffer size must be a multiple of 2
// When changing the buffer size Adjust the stack size so that RAM does not overflow
/* #define buffsize 32

volatile unsigned char buff [buffsize];
volatile unsigned char readpos, writepos;
*/
void osc_init (void)
{
    // Switch to built-in high speed oscillator
    prc0 = 1; // Clock register permission
    ococr = 0b00000001; // High-speed on-chip oscillator oscillation Low-speed oscillation
    {
        unsigned char a;
        for (a = 0; a <255; a ++); // Wait for the oscillator oscillation to stabilize, so wait for an appropriate amount of time.
    }
    sckcr = 0b01000000; // Select high speed by selecting XIN / high speed oscillator No CPU clock division
    ckstpr = 0b10000000; // Select high speed by system clock low speed / high speed selection
    phisel = 0x00; // No system clock division
    frv1 = fr18s0; // Adjust high speed on-chip oscillator to 18.432MHz
    frv2 = fr18s1;
    prc0 = 0; // Clock register access prohibited
}

void uart_init(void)
{
    // UART0 settings
    p14sel0 = 1;
    p14sel1 = 0;
    p14sel2 = 0; // TXD P1_4

    p15sel0 = 1;
    p15sel1 = 0;
    p15sel2 = 0; // RXD P1_5

    // # define RTS p1_0
    // pd1_0 = 1;
    // RTS = 1; // 1: Please do not send 0: Please send

    mstuart = 0; // Release module standby
    u0mr = 0b00000101; // 8-bit stop bit 1 no parity
    u0c0 = 0b00010000; // LSB first push-pull output filter ON count source no frequency division
    u0brg = 119; // 9600bps
    // u0brg = 29; // 38400bps
    // u0brg = 19; // 57600bps
    // u0brg = 9; // 115200bps
    u0rrm_u0c1 = 0; // Continuous reception mode prohibited
    u0tie = 0; // Send interrupt disabled
    u0rie = 0; // Receive interrupt disabled
    te_u0c1 = 1; // Send permission
    re_u0c1 = 1; // Acceptance

    /* ilvl8 = (ilvl8 & 0x0F) | (1 << 4); // Receive interrupt level 1
    u0rie = 1; // Receive interrupt enabled

    readpos = 0;
    writepos = 0;

    asm ("LDIPL # 0"); // Processor interrupt priority level 0 (interrupts higher than this value are accepted)
    asm ("FSET I"); // Interrupt enabled
    RTS = 0; // Send request
    */
}

void putch(unsigned char a)
{
    while (ti_u0c1 == 0); // Wait until the buffer is empty

    u0tbh = 0;
    u0tbl = a;
}

void puts(unsigned char str [])
{
    unsigned int a = 0;
    while (1)
    {
        if (str [a] == 0) break;
        putch (str [a]);
        a ++;
    }
}

void puthex(unsigned char a)
{
    putch (tochar ((a & 0xF0) >> 4));
    putch (tochar (a & 0x0F));
}

unsigned char tochar(unsigned char a)
{
    if (a <10) a = a + 0x30;
    else a = a + 0x41-10;
    return a;
}

void puthexshort(unsigned short a)
{
    puthex ((a >> 8) & 0xFF);
    puthex ((a & 0xFF));
}

// Returns 1 when something is received
unsigned char keyhit(void)
{
    return (unsigned char) ri_u0c1;
}

// 1-byte read without interrupts
unsigned char getch(void)
{
    // RTS = 0; // Send request
    while (ri_u0c1 == 0); // Wait until received
    // RTS = 1; // Send prohibited
    return (unsigned char) (u0rb & 0xFF); // ri becomes 0 when reading u0rb
}

// With 1-byte read receive buffer using interrupts
/* unsigned char getch (void)
{
    unsigned char a;
    while (writepos == readpos); // Wait until received
    a = buff [readpos];
    readpos ++;
    readpos & = (buffsize-1);
    // Send request when the receive buffer becomes empty
    if (writepos == readpos) RTS = 0; // Send request

    return a;
} */


unsigned short getshort(void)
{
    unsigned short data;
    data = ((unsigned short) getch () << 8);
    data + = ((unsigned short) getch ());
    return data;
}

void putshort(unsigned short data)
{
    putch ((data >> 8) & 0xFF);
    putch ((data) & 0xFF);
}

void putdecimal(unsigned short data)
{
    putch (tochar (data / 10000));
    data = data% 10000;
    putch (tochar (data / 1000));
    data = data% 1000;
    putch (tochar (data / 100));
    data = data% 100;
    putch (tochar (data / 10));
    data = data% 10;
    putch (tochar (data));
}

/*
#pragma INTERRUPT int_uart_receive // ​​Notify the compiler that the int_uart_receive function is an interrupt routine
void int_uart_receive(void)
{
    signed char a;
    // Clear interrupt flag
    while (u0rif == 1) u0rif == 0;

    buff [writepos] = (unsigned char) u0rb;
    writepos ++;
    writepos & = (buffsize-1);

    // Prohibit transmission when it accumulates in the buffer
    // Even if RTS is set to 1, the threshold does not stop immediately, so the threshold has a margin.
    a = writepos-readpos;
    if (a> 0)
    {
        if (a <4) RTS = 1; // Send prohibited
    }
    else
    {
        if (a> -4) RTS = 1; // Send prohibited
    }
}
*/
#ifdef __cplusplus
}
#endif
