/*
	PS/2 �L�[�{�[�h�� SHARP X1 �ɂȂ�
	100[ms] �̃C���^�[�o���^�C�}�[
	
	PS/2 �̎�M�^�C���A�E�g�Ŏg���Ă��܂��B
	

	2014�N7��22�� �쐬
	
	��������  http://kyoutan.jpn.org/

	���ۏ؂ł��B
	�������ꂪ�쐬���������͗p�r�ɐ�����݂��܂���B���p�E�񏤗p�ɂ�����炸���R�Ɏg�p���Ē����č\���܂���B
	����ɕ���������A����������A�z�z������A�������肵�Ă��ǂ��Ƃ������Ƃł��B
	�A���s�v�ł��B
*/

#include "sfr_r8m12a.h"
#include "timer.h"
#include "ps2.h"
#include "iodefine.h"

volatile unsigned short TIMER=0;		// 100[ms]�^�C�}�[

void timer_init(void)
{
	// TIMER RB2 ������ 100ms �C���^�[�o���^�C�}�[
	msttrb=0;	// �X�^���o�C����
	trbcr=0b00000100;	// �J�E���g��~
	trbmr=0b01100100;	// f64���� 16�r�b�g �^�C�}���[�h
/*
 100[ms] = 100000[us]	=  1843200
 						f2  921600
						f4  460800
						f8  230400
						f16 115200
						f32  57600
						f64  28800 = 0x7080
*/
	// 100[ms] = f64 0x7080
	trbpre=0x80;	// ����8�r�b�g
	trbpr=0x70;		// ���8�r�b�g
	trbie_trbir=1;	// ���荞�݋���
}

/*
void timer_start(void)
{
	tstart_trbcr=1;	// TIMER RB2 �J�E���g�J�n 
}
*/

#pragma INTERRUPT INT_trb(vect=24)
void INT_trb(void)
{
	TIMER++;
	if(0xFF != PS2TIMER) PS2TIMER++;	// �^�C���A�E�g�����p�I�[�o�[�t���[�����Ȃ�
	
	while(trbif_trbir==1) trbif_trbir=0;	// ���荞�݃t���O�N���A
}
