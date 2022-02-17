/*
	PS/2 �L�[�{�[�h�� SHARP X1 �ɂȂ�
	X1 �L�[�{�[�h�̑��M����
	
	X1�Z���^�[���� (http://www.x1center.org/) �́A�L�[�{�[�h�̓`���t�H�[�}�b�g�d�l���Q�l�ɂ��܂����B
	X1�L�[�{�[�h�̎����������Ă��Ȃ��̂ŁA��Ϗ�����܂����B
	X1�Z���^�[����̎����́u�g���镨������Ύ��R�Ɏg���ĉ������v�Ƃ̂��Ƃł��B
	

	2014�N7��22�� �쐬
	
	��������  http://kyoutan.jpn.org/

	���ۏ؂ł��B
	�������ꂪ�쐬���������͗p�r�ɐ�����݂��܂���B���p�E�񏤗p�ɂ�����炸���R�Ɏg�p���Ē����č\���܂���B
	����ɕ���������A����������A�z�z������A�������肵�Ă��ǂ��Ƃ������Ƃł��B
	�A���s�v�ł��B
*/


#include "sfr_r8m12a.h"
#include "x1key.h"
#include "iodefine.h"

unsigned short SEND_DATA;	// X1���M�f�[�^

void X1_send(unsigned short data)
{
	while(0 != tcstf_trjcr);	// ���M���Ȃ�I���܂ő҂�
	
	SEND_DATA=data;

	/* TIMER RJ */
	trjioc=	0b00000011;	/* TRJO H����o�͊J�n�ATRJIO �g�O���o�͖��� */
	trjmr=	0b00000001;	/* �p���X�o�̓��[�h�A�������� */
	trjcr=	0b00000100;	/* �J�E���g��~ �o�̓s�������� */
	pmh3 =	0b10000000;	// P3_7 TRJO X1KEYOUT �� TRJO �� 
	trjie_trjir=1;	/* TIMER RJ ���荞�݋��� */
	trj=TRJ250us;	// 250us �Z�b�g �i250us ��� L �w�b�_�[���j250us�ɈӖ��͖����B�����ƒZ���Ă��ǂ�
	tstart_trjcr=1;	// �^�C�}�[�X�^�[�g
}

void x1key_init(void)
{
	X1KEYOUT=1;
	
	/* TIMER RJ ������*/
	msttrj=0;	// �X�^���o�C����
	trjcr=	0b00000100;	/* �J�E���g��~ */
	trjioc=	0b00000011;	/* TRJO H����o�͊J�n�ATRJIO �g�O���o�͖��� */
	trjmr=	0b00000001;	/* �p���X�o�̓��[�h�A�������� */
	trjcr=	0b00000100;	/* �J�E���g��~ �o�̓s�������� */
}

// TIMER RJ �A���_�[�t���[���荞��
// �^�C�}�[�̋@�\�ŁA�_�E���J�E���^���A���_�[�t���[���邽�тɁA
// �o�̓s�� (TRJO) �̏o�͂����]���܂�
#pragma INTERRUPT INT_trj (vect=22)
void INT_trj(void)
{
	static unsigned char count=0;
	
	if(0==(count&1))	// ���� L ����
	{	// 0 2 4 6 8 10 12 14 16 18 20 22 24 26 28 30 32 34 36
		switch(count)
		{
			case 0:		// header L
				trj=TRJ1000us;	// 1000us
				break;
				
			case 36:	// stop �I��
				while(1==tcstf_trjcr) tstart_trjcr=0; // �^�C�}�[��~�i�r�b�g���삾�Əo�͏���������Ȃ��悤�j
				trjcr=	0b00000100;	// �J�E���g��~ �o�̓s��������
				pmh3 =0b10000000;	// P3_7 TRJO X1KEYOUT �� TRJO �� 
				break;
			
			default:
				trj=TRJ250us;	// 250us
				break;
		}
	}
	else				// � H ����
	{	// 1 3 5 7 9 11 13 15 17 19 21 23 25 27 29 31 33 35
		switch(count)
		{
			case 1:		// header H
				trj=TRJ700us;
				break;
					
			//case 3:		// start H	�X�^�[�g�r�b�g����Ȃ�
			//trj=TRJ750us;
			//	break;
					
			case 35:	// stop H ����
				X1KEYOUT=1;	// H �o��
				pmh3 =0b00000000;	// P3_7 TRJO X1KEYOUT ��I/O�|�[�g�� ���]�o�͓���I��
				trj=TRJ1750us;
				break;
					
			default:	// ��ʃr�b�g����16�r�b�g���M
				if(0==(SEND_DATA & 0x8000))
				{
					trj=TRJ750us;	// 0
				}
				else
				{
					trj=TRJ1750us;	// 1
				}
				SEND_DATA=(SEND_DATA<<1);
				break;
		}
	}
	
	count++;
	if(36<count) count=0;
	
	while(1==trjif_trjir) trjif_trjir=0;	// ���荞�݃t���O�N���A
}

// �f�o�b�O�p 1�o�C�g��16�i2���ő��o�i�\���j
void puth2(unsigned char a)
{
	X1_send(((unsigned short)0b10111111 << 8) + tochar((a&0xF0)>>4));//����
	X1_send(((unsigned short)0b11111111 << 8) + 0x00);//����
	
	X1_send(((unsigned short)0b10111111 << 8) + tochar(a&0x0F));//����
	X1_send(((unsigned short)0b11111111 << 8) + 0x00);//����

	X1_send(((unsigned short)0b10111111 << 8) + 0x20);//����
	X1_send(((unsigned short)0b11111111 << 8) + 0x00);//����
}

// �f�o�b�O�p 0�`15�̐��𕶎��ɕϊ�
unsigned char tochar(unsigned char a)
{
	if(a<10) a=a+0x30;
	else a=a+0x41-10;
	return a;
}
