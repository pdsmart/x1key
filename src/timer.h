/*
    Connect a PS / 2 keyboard to SHARP X1
    100 [ms] interval timer

    Created July 22, 2014

    Kyoichi Sato http://kyoutan.jpn.org/

    There is no guarantee.
    The part created by Kyoichi Sato has no restrictions on its use. You can use it freely regardless of whether it is commercial or non-commercial.
    It means that you can copy, modify, distribute, or sell it without permission.
    No need to contact.
*/

#ifndef TIMER_H
#define TIMER_H

#ifdef __cplusplus
    extern "C" {
#endif

extern volatile unsigned short TIMER; // 100 [ms] timer

void timer_init (void);
// void timer_start(void);
#define timer_start() tstart_trbcr = 1    // TIMER RB2 Count start

#ifdef __cplusplus
}
#endif
#endif // TIMER_H
