/*
	PS/2 �L�[�{�[�h�� SHARP X1 �ɂȂ�
	
	Renesas R8C/M12A �p
	�W���� �v���O����ROM:2KB �f�[�^�t���b�V��:2KB RAM:256B �Ɏ��܂�܂��B

	2014�N7��16�� �쐬

		
	��������  http://kyoutan.jpn.org/
	
	���ۏ؂ł��B
	X1�̃L�[�{�[�h���������L���Ă��Ȃ��̂ŁA���삪�������̂��悭�킩��܂���B
	����e�X�g�� X1 turbo III �ōs���܂������A�T�˂��������ɓ����Ă���悤�ł��B
	
	�������ꂪ�쐬���������͗p�r�ɐ�����݂��܂���B���p�E�񏤗p�ɂ�����炸���R�Ɏg�p���Ē����č\���܂���B
	����ɕ���������A����������A�z�z������A�������肵�Ă��ǂ��Ƃ������Ƃł��B
	�A���s�v�ł��B
*/

#include "sfr_r8m12a.h"
#include "iodefine.h"
#include "x1key.h"
#include "ps2.h"
#include "timer.h"
#include "keyconv.h"

void main(void);
void osc_init(void);
void io_init(void);

void main(void)
{
	DI();	// ���荞�݋֎~
	osc_init();
	io_init();
	timer_start();
	EI();	// ���荞�݋���
	
/*	
	// PS/2 ���[�h�e�X�g
	while(TRUE)
	{
		puth2(ps2get());
		//puth2(PS2RPOS);
		//puth2(PS2WPOS);
	}
*/

/*
	// X1 ���o�e�X�g
	{
		unsigned short a=0;
		unsigned char b=0x20;

		while(TRUE)
		{
			if(10 < (TIMER-a))	// 1�b��
			{
				a=TIMER;
			
				X1_send(((unsigned short)0b10111111 << 8) + b);//����
				X1_send(((unsigned short)0b11111111 << 8) + 0x00);//����
			
				if(0x7F==b)
				{
					b=0x20;
				}
				else
				{
					b++;
				}
			}
		}
	}
*/

	while(TRUE)
	{
		keyconv();
	}
}

void osc_init(void)
{
	// ���������I�V���[�^�[�ɐ؂�ւ�
	prc0=1;				// �N���b�N���W�X�^�A�N�Z�X����
	ococr=0b00000001;	// �����I���`�b�v�I�V���[�^�[���U �ᑬ�����U
	{
		unsigned char a;
		for(a=0; a<255; a++)  asm("nop");	// �����ŃI�V���[�^�[�̔��U�����肷��̂�҂ĂƂ���̂œK���Ɏ��ԑ҂�
	}
	sckcr=0b01000000;	// XIN/�����I�V���[�^�[�I���ō�����I�� CPU�N���b�N��������
	ckstpr=0b10000000;	// �V�X�e���N���b�N�ᑬ/�����I���ō�����I��
	phisel=0x00;		// �V�X�e���N���b�N��������
	frv1=fr18s0;		// �����I���`�b�v�I�V���[�^�[��18.432MHz�ɒ���
	frv2=fr18s1;
	prc0=0;				// �N���b�N���W�X�^�A�N�Z�X�֎~
}

void io_init(void)
{
	// I/O �|�[�g
	// P1_0 
	// P1_1 
	// P1_2 
	// P1_3
	// P1_4 TXD �������݁E�ʐM�p
	// P1_5 RXD �ʐM�p		(TRJIO)
	// P1_6 RXD �������ݗp	(TRJO)
	// P1_7

	// P3_3 PS/2 DATA
	// P3_4
	// P3_5
	// P3_7 TRJO X1KEYOUT

	// P4_2
	// P4_5 INT0 PS/2 CLK
	// P4_6 (TRJIO X1KEYOUT TRJO�̔��]�o�� �g��Ȃ�)
	// P4_7

	// PA_0
	
	X1KEYOUT=1;
	p1_4=1;	// TXD

	// �|�[�g�̌��� 1:�o��
	pd1=0b10011111;	// P1_5 P1_6 ���� RXD
	pd3=0b11110111;	// P3_7 TRJO X1KEYOUT �o�� | P3_3 PS/2 DATA ����
	pd4=0b11011111;	// P4_5 INT0 PS/2 CLK ����
	
	// �v���A�b�v 1:����
	pur1=0b01100000;
	pur3=0b00001000;
	pur4=0b00100000;
	
	// �I�[�v���h���C���o�� 1:����
	pod1=0b00000000;
	pod3=0b00001000;
	pod4=0b00100000;

	x1key_init();
	ps2key_init();	
	timer_init();
		
	// ���荞�ݗD�惌�x��
	ilvlb=0x01;		// TIMER RJ	1
	ilvlc=0x01;		// TIMER RB 1
	ilvle=0x20;		// INT0		2	TIMER RJ ���D��x����
	// ���荞�݉����� 20�T�C�N�����ƌ��\������B
	
	asm("LDIPL #0");	// �v���Z�b�T���荞�ݗD�惌�x��0�i���̒l�����������x���̊��荞�݂��󂯕t������j

	// �|�[�g�}�b�s���O
	pml1 =0b00000000;
	pmh1 =0b00000101;	// P1_4:TXD P1_5:RXD
	pmh1e=0b00000000;
	pml3 =0b00000000;
	pmh3 =0b10000000;	// P3_7 TRJO X1KEYOUT
	pml4 =0b00000000;
	pmh4 =0b00000100;	// P4_5 INT0 PS/2 CLK
	pmh4e=0b00010000;
	pamcr=0b00010001;	// PA�̓��Z�b�g
}
