/*
    Connect a PS / 2 keyboard to SHARP X1
    Key code conversion process

    Created on July 23, 2014

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
#include "keyconv.h"
#include "iodefine.h"
#include "keytable.h"
#include "ps2.h"
#include "x1key.h"

volatile unsigned short x1shift = 0xFF; // Valid at X1 shift state save 0
#define TENKEY      ((unsigned char) (1 << 7))
#define PRESS       ((unsigned char) (1 << 6))
#define REPEAT      ((unsigned char) (1 << 5))
#define GRAPH       ((unsigned char) (1 << 4))
#define CAPS        ((unsigned char) (1 << 3))
#define KANA        ((unsigned char) (1 << 2))
#define SHIFT       ((unsigned char) (1 << 1))
#define CTRL        ((unsigned char) (1 << 0))


volatile unsigned char ps2ex = 0; // PS2 keyboard extended key flag
#define EXKEY       ((unsigned char) (1 << 0))
#define RELEASE     ((unsigned char) (1 << 1))
#define PAUSE_BREAK ((unsigned char) (1 << 2))


unsigned char codeconv(unsigned char data);
unsigned char checkbreak(void);
unsigned char x1code(unsigned char data);
void x1trans(unsigned char data);


void keyconv(void)
{
    unsigned char data;

    data = ps2get (); // Wait until it is received from the PS / 2 keyboard and read 1 byte
    switch (data)
    {
        case 0xE0: // Extended key
            ps2ex |= EXKEY; // Extended key flag set
            break;

        case 0xF0: // separated
            ps2ex |= RELEASE; // Separated flag set
            break;

        default:
            // Convert PS / 2 code to internal code
            if (0xE1 == data) data = checkbreak (); // PAUSE / BREAK key determination

            if (0 == (ps2ex & EXKEY))
            {// normal key
                if (0x8F <data) data = 0x00;
                data = KEY106 [data];
            }
            else
            {// Extended key
                if (0x5F <data) data = KEY106 [data + 0x30];
                else data = KEY106 [data];
            }
            ps2ex &= ~EXKEY; // Extended key flag reset

            x1trans (x1code (data)); // Convert internal code to X1 code and send
            break;
    }
}

// Convert from internal code to X1 code
unsigned char x1code(unsigned char data)
{
    x1shift |= TENKEY; // Special key flag reset
    // Check if it is a special key such as numeric keypad or function key SHIFT CTRL
    if (((0x3A <data) && (0x59> data)) || (0x5A <data)) x1shift &= ~TENKEY; // Special key flag set

    switch (data) // branch by internal code
    {
        case GRAPH_CODE1:
        case GRAPH_CODE2:
            if (0 == (ps2ex & RELEASE)) x1shift &= ~GRAPH; // Pressed
            else x1shift |= GRAPH; // separated
            data = 0;
            break;

        case CAPS_CODE:
            if (0 != (ps2ex & RELEASE)) x1shift = (x1shift | CAPS) & (~(x1shift & CAPS)); // Toggle every time you release
            data = 0;
            break;

        case KANA_CODE:
            if (0 != (ps2ex & RELEASE)) x1shift = (x1shift | KANA) & (~ (x1shift & KANA)); // Toggle every time you release
            data = 0;
            break;

        case SHIFTL_CODE:
        case SHIFTR_CODE:
            if (0 == (ps2ex & RELEASE)) x1shift &= ~SHIFT; // Pressed
            else x1shift |= SHIFT; // separated
            data = 0;
            break;

        case CTRL_CODE:
            if (0 == (ps2ex & RELEASE)) x1shift &= ~CTRL; // Pressed
            else x1shift |= CTRL; // released
            data = 0;
            break;

        case UP_CODE:
            data = 0x1E;
            break;

        case DOWN_CODE:
            data = 0x1F;
            break;

        case LEFT_CODE:
            data = 0x1D;
            break;

        case RIGHT_CODE:
            data = 0x1C;
            break;

        case INS_CODE:
            data = 0x12;
            break;

        case DEL_CODE: // BS is usually in the key table
            data = 0x08;
            break;

        case HOME_CODE:
            x1shift &= ~SHIFT; // + SHIFT
            data = 0x0B;
            break;

        default: // Convert from normal key internal code to X1 code
        data = codeconv (data);
        break;
    }

    return data;
}

// The PAUSE / BREAK key starts with an unusual code, so it can be determined by another process.
unsigned char checkbreak(void)
{
    unsigned char a;
    for (a = 1; a <8; a++)
    {
    if (ps2get () != BREAK_CODE [a]) break;
    }

    if (8 == a) // BREAK key
    {
    ps2ex |= PAUSE_BREAK;
    return 0x80;
    }

    return 0x00; // wasn't the BREAK key
}

// Send to X1
void x1trans(unsigned char data)
{
    static unsigned char lastshift = 0xFF;

    if (0 == (ps2ex & RELEASE))
	{	// pushed
        x1shift &= ~PRESS;

        // Send when it is not zero and when the shift state changes
        if ((0x00 != data) || (x1shift != lastshift)) X1_send (((unsigned short) x1shift << 8) | data);

        if (0 != (ps2ex & PAUSE_BREAK)) // Send the release code when PAUSE / BREAK
        {
            x1shift |= TENKEY; // Clear special key flag
            X1_send (((unsigned short) x1shift << 8) | 0x00);
            ps2ex &= ~PAUSE_BREAK;
        }
    }
    else
    {// separated
        x1shift |= PRESS;
        X1_send (((unsigned short) x1shift << 8) | 0x00);
        ps2ex &= ~RELEASE; // Clear the released flag
        if (0x0B == data) x1shift |= SHIFT; // At HOME -SHIFT
    }

    lastshift = x1shift;
}

unsigned char codeconv(unsigned char data)
{
    unsigned char status;

    status = (x1shift & (CAPS | GRAPH | KANA | SHIFT | CTRL));

    if ((KANA | GRAPH | SHIFT | CTRL) == status) // CAPS
            {
                unsigned char ret;
                ret = CHR_TBL0 [data];
                if ((0x60 <ret) && (0x7b> ret)) ret -= 0x20; // Uppercase alphabet
                return ret;
            }
    if ((KANA | GRAPH | CTRL) == status) // CAPS + SHIFT
            {
                unsigned char ret;
                ret = CHR_TBL1 [data];
                if ((0x40 <ret) && (0x5b> ret)) ret += 0x20; // In lowercase letters
                return ret;
            }

    status &= (GRAPH | KANA | SHIFT | CTRL);

    if ((GRAPH | KANA | CTRL) == status) return CHR_TBL1 [data]; // SHIFT
    if ((GRAPH | KANA | SHIFT) == status) return CHR_TBL3 [data]; // CTRL
    if ((KANA | SHIFT | CTRL) == status) return CHR_TBL2 [data]; // GRAPH
    if ((SHIFT | CTRL) == status) return CHR_TBL2 [data]; // GRAPH + KANA
    if ((GRAPH | SHIFT | CTRL) == status) return CHR_TBL4 [data]; // KANA
    if ((GRAPH | CTRL) == status) return CHR_TBL5 [data]; // KANA + SHIFT

    return CHR_TBL0 [data]; // No shift
}
#ifdef __cplusplus
}
#endif
