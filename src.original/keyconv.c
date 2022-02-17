/*
	PS/2 �L�[�{�[�h�� SHARP X1 �ɂȂ�
	�L�[�R�[�h�ϊ�����

	2014�N7��23���쐬
	
	��������  http://kyoutan.jpn.org/

	���ۏ؂ł��B
	�������ꂪ�쐬���������͗p�r�ɐ�����݂��܂���B���p�E�񏤗p�ɂ�����炸���R�Ɏg�p���Ē����č\���܂���B
	����ɕ���������A����������A�z�z������A�������肵�Ă��ǂ��Ƃ������Ƃł��B
	�A���s�v�ł��B
*/

#include "keyconv.h"
#include "keytable.h"
#include "ps2.h"
#include "x1key.h"

volatile unsigned short x1shift=0xFF;	// X1 �V�t�g��ԕۑ� 0�ŗL��
#define TENKEY	((unsigned char)(1<<7))
#define PRESS	((unsigned char)(1<<6))
#define REPEAT	((unsigned char)(1<<5))
#define GRAPH	((unsigned char)(1<<4))
#define CAPS	((unsigned char)(1<<3))
#define KANA	((unsigned char)(1<<2))
#define SHIFT	((unsigned char)(1<<1))
#define CTRL	((unsigned char)(1<<0))


volatile unsigned char ps2ex=0;			// PS2�L�[�{�[�h �g���L�[�t���O
#define EXKEY		((unsigned char)(1<<0))
#define RELEASE		((unsigned char)(1<<1))
#define PAUSE_BREAK	((unsigned char)(1<<2))


unsigned char codeconv(unsigned char data);
unsigned char checkbreak(void);
unsigned char x1code(unsigned char data);
void x1trans(unsigned char data);


void keyconv(void)
{
	unsigned char data;
	
	data=ps2get();	// PS/2�L�[�{�[�h�����M����܂ő҂��āA1�o�C�g�ǂݍ���
	switch(data)
	{
		case 0xE0:	// �g���L�[
			ps2ex|=EXKEY;	// �g���L�[�t���O�Z�b�g
			break;
		
		case 0xF0:	// ������
			ps2ex|=RELEASE;	// �������t���O�Z�b�g
			break;
			
		default:
			// PS/2 �̃R�[�h��������R�[�h�ɕϊ�
			if(0xE1==data) data=checkbreak();	// PAUSE/BREAK �L�[�̔���
			
			if(0==(ps2ex & EXKEY))
			{	// �ʏ�L�[
				if(0x8F<data) data=0x00;
				data=KEY106[data];
			}
			else
			{	// �g���L�[
				if(0x5F<data) data=KEY106[data + 0x30];
				else data=KEY106[data];
			}
			ps2ex&=~EXKEY;	// �g���L�[�t���O���Z�b�g
			
			x1trans(x1code(data));	// �����R�[�h����X1�̃R�[�h�ɕϊ����đ��M
			break;
	}
}

// �����R�[�h����X1�̃R�[�h�ɕϊ�
unsigned char x1code(unsigned char data)
{
	x1shift|=TENKEY;	// ����L�[�t���O���Z�b�g
	// �e���L�[��t�@���N�V�����L�[ SHIFT CTRL �Ȃ񂩂̓���L�[���`�F�b�N
	if(((0x3A<data)&&(0x59>data))||(0x5A<data)) x1shift&=~TENKEY;	// ����L�[�t���O�Z�b�g
			
	switch(data)	// �����R�[�h�ŕ���
	{
		case GRAPH_CODE1:
		case GRAPH_CODE2:
			if(0==(ps2ex&RELEASE)) x1shift&=~GRAPH;	// ������
			else x1shift|=GRAPH;	// ������
			data=0;
			break;

		case CAPS_CODE:
			if(0!=(ps2ex&RELEASE)) x1shift=(x1shift|CAPS)&(~(x1shift&CAPS));	// �������уg�O��
			data=0;
			break;

		case KANA_CODE:
			if(0!=(ps2ex&RELEASE)) x1shift=(x1shift|KANA)&(~(x1shift&KANA));	// �������уg�O��
			data=0;
			break;
					
		case SHIFTL_CODE:
		case SHIFTR_CODE:
			if(0==(ps2ex&RELEASE)) x1shift&=~SHIFT;	// ������
			else x1shift|=SHIFT;	// ������
			data=0;
			break;
				
		case CTRL_CODE:
			if(0==(ps2ex&RELEASE)) x1shift&=~CTRL;	// ������
			else x1shift|=CTRL;	// ������
			data=0;
			break;

		case UP_CODE:
			data=0x1E;
			break;
				
		case DOWN_CODE:
			data=0x1F;
			break;
				
		case LEFT_CODE:
			data=0x1D;
			break;
				
		case RIGHT_CODE:
			data=0x1C;
			break;

		case INS_CODE:
			data=0x12;
			break;

		case DEL_CODE:	// BS�͒ʏ�L�[�̃e�[�u���ɓ����Ă���
			data=0x08;
			break;

		case HOME_CODE:
			x1shift&=~SHIFT;	// +SHIFT
			data=0x0B;
			break;
					
		default:	// �ʏ�L�[ �����R�[�h����X1�̃R�[�h�ɕϊ�
			data=codeconv(data);
			break;
	}
	
	return data;
}

// PAUSE/BREAK �L�[�͒ʏ�ƈႤ�R�[�h�Ŏn�܂�̂ŁA�ʏ����Ŕ���
unsigned char checkbreak(void)
{
	unsigned char a;
	for(a=1; a<8; a++)
	{
		if(ps2get()!=BREAK_CODE[a]) break;
	}
	
	if(8==a) // BREAK�L�[ ������
	{
		ps2ex|=PAUSE_BREAK;
		return 0x80;
	}
	
	return 0x00; // BREAK�L�[ ����Ȃ�����
}

// X1�ɑ��M
void x1trans(unsigned char data)
{
	static unsigned char lastshift=0xFF;

	if(0==(ps2ex&RELEASE))
	{	// ������
		x1shift&=~PRESS;
			
		// �[�����ᖳ�����ƃV�t�g��ԕω����������M
		if((0x00!=data)||(x1shift!=lastshift)) X1_send(((unsigned short)x1shift << 8) | data);
		
		if(0!=(ps2ex&PAUSE_BREAK))	// PAUSE/BREAK�̎������[�X�R�[�h������
		{
			x1shift|=TENKEY;	// ����L�[�t���O�N���A
			X1_send(((unsigned short)x1shift << 8) | 0x00);
			ps2ex&=~PAUSE_BREAK;
		}
	}
	else
	{	// ������
		x1shift|=PRESS;
		X1_send(((unsigned short)x1shift << 8) | 0x00);
		ps2ex&=~RELEASE;	// �������t���O�N���A
		if(0x0B==data) x1shift|=SHIFT;		// HOME�̎� -SHIFT
	}

	lastshift=x1shift;
}

unsigned char codeconv(unsigned char data)
{
	unsigned char status;

	status=(x1shift & (CAPS | GRAPH | KANA | SHIFT | CTRL));
	
	if((KANA | GRAPH | SHIFT | CTRL )==status) 						// CAPS
			{
				unsigned char ret;
				ret=CHR_TBL0[data];
				if((0x60 < ret)&&(0x7b > ret)) ret-=0x20;	// �A���t�@�x�b�g�啶����
				return ret;
			}
	if((KANA | GRAPH | CTRL         )==status)							// CAPS+SHIFT
			{
				unsigned char ret;
				ret=CHR_TBL1[data];
				if((0x40 < ret)&&(0x5b > ret)) ret+=0x20;	// �A���t�@�x�b�g��������
				return ret;
			}
			
	status&=(GRAPH | KANA | SHIFT | CTRL);
	
	if((GRAPH | KANA  | CTRL )==status) return CHR_TBL1[data];	// SHIFT
	if((GRAPH | KANA  | SHIFT)==status) return CHR_TBL3[data];	// CTRL
	if((KANA  | SHIFT | CTRL )==status) return CHR_TBL2[data];	// GRAPH
	if((        SHIFT | CTRL )==status) return CHR_TBL2[data];	// GRAPH+KANA
	if((GRAPH | SHIFT | CTRL )==status) return CHR_TBL4[data];	// KANA
	if((GRAPH | CTRL         )==status) return CHR_TBL5[data];	// KANA+SHIFT

	return CHR_TBL0[data];	// �V�t�g�Ȃ�
}