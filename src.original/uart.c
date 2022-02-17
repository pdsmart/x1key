 
#include "sfr_r8m12a.h"
#include "uart.h"

// �o�b�t�@�T�C�Y��2�̔{������Ȃ���_��
// �o�b�t�@�T�C�Y��ύX����ꍇ�X�^�b�N�T�C�Y�𒲐�����RAM�����Ȃ��悤��
/*#define	buffsize 32

volatile unsigned char buff[buffsize];
volatile unsigned char readpos,writepos;
*/
void osc_init(void)
{
	// ���������I�V���[�^�[�ɐ؂�ւ�
	prc0=1;				// �N���b�N���W�X�^�A�N�Z�X����
	ococr=0b00000001;	// �����I���`�b�v�I�V���[�^�[���U �ᑬ�����U
	{
		unsigned char a;
		for(a=0; a<255; a++);	// �����ŃI�V���[�^�[�̔��U�����肷��̂�҂ĂƂ���̂œK���Ɏ��ԑ҂�
	}
	sckcr=0b01000000;	// XIN/�����I�V���[�^�[�I���ō�����I�� CPU�N���b�N��������
	ckstpr=0b10000000;	// �V�X�e���N���b�N�ᑬ/�����I���ō�����I��
	phisel=0x00;		// �V�X�e���N���b�N��������
	frv1=fr18s0;		// �����I���`�b�v�I�V���[�^�[��18.432MHz�ɒ���
	frv2=fr18s1;
	prc0=0;				// �N���b�N���W�X�^�A�N�Z�X�֎~
}	

void uart_init(void)
{
	// UART0�̐ݒ�
	p14sel0=1;
	p14sel1=0;
	p14sel2=0;	// P1_4��TXD
	
	p15sel0=1;
	p15sel1=0;
	p15sel2=0;	// P1_5��RXD

//#define RTS p1_0
//	pd1_0=1;
//	RTS=1;	// 1:���M���Ȃ��ł������� 0:���M���Ă�������
	
	mstuart=0;			// ���W���[���X�^���o�C����
	u0mr=0b00000101;	// 8�r�b�g �X�g�b�v�r�b�g1 �p���e�B����
	u0c0=0b00010000;	// LSB�t�@�[�X�g �v�b�V���v���o�� �t�B���^ON �J�E���g�\�[�X��������
	u0brg=119;			// 9600bps
	//u0brg=29;			// 38400bps
	//u0brg=19;			// 57600bps
	//u0brg=9;			// 115200bps
	u0rrm_u0c1=0;		// �A����M���[�h�֎~
	u0tie=0;			// ���M���荞�݋֎~
	u0rie=0;			// ��M���荞�݋֎~
	te_u0c1=1;			// ���M����
	re_u0c1=1;			// ��M����

	/*ilvl8=(ilvl8 & 0x0F) | (1 << 4);	// ��M���荞�݃��x��1
	u0rie=1;			// ��M���荞�݋���
	
	readpos=0;
	writepos=0;
	
	asm("LDIPL #0");	// �v���Z�b�T���荞�ݗD�惌�x��0�i���̒l�����������x���̊��荞�݂��󂯕t������j
	asm("FSET I");		// ���荞�݋���
	RTS=0;				// ���M�v��
	*/
}	

void putch(unsigned char a)
{
	while(ti_u0c1==0);	// �o�b�t�@����ɂȂ�܂ő҂�
	
	u0tbh=0;
	u0tbl=a;
}	

void puts(unsigned char str[])
{
	unsigned int a=0;
	while(1)
	{
		if(str[a]==0) break;
		putch(str[a]);
		a++;
	}
}

void puthex(unsigned char a)
{
	putch(tochar((a&0xF0)>>4));
	putch(tochar(a&0x0F));
}

unsigned char tochar(unsigned char a)
{
	if(a<10) a=a+0x30;
	else a=a+0x41-10;
	return a;
}

void puthexshort(unsigned short a)
{
	puthex((a>>8)&0xFF);
	puthex((a&0xFF));
}

// �Ȃɂ���M������1��Ԃ�
unsigned char keyhit(void)
{
	return (unsigned char)ri_u0c1;
}

// ���荞�݂��g��Ȃ�1�o�C�g���[�h
unsigned char getch(void)
{
//	RTS=0;	// ���M�v��
	while(ri_u0c1==0);	// ��M����܂ő҂�
//	RTS=1;	// ���M�֎~
	return (unsigned char)(u0rb&0xFF);	// u0rb��ǂނ�ri��0�ɂȂ�
}

// ���荞�݂��g��1�o�C�g���[�h ��M�o�b�t�@�t��
/*unsigned char getch(void)
{
	unsigned char a;
	while(writepos==readpos); // ��M����܂ő҂�
	a=buff[readpos];
	readpos++;
	readpos&=(buffsize-1);
	// ��M�o�b�t�@����ɂȂ����瑗�M�v��
	if(writepos==readpos) RTS=0;	// ���M�v��
	
	return a;
}*/


unsigned short getshort(void)
{
	unsigned short data;
	data =((unsigned short)getch()<<8);
	data+=((unsigned short)getch()   );
	return data;
}

void putshort(unsigned short data)
{
	putch((data>>8)&0xFF);
	putch((data   )&0xFF);
}

void putdecimal(unsigned short data)
{
	putch(tochar(data/10000));
	data=data%10000;
	putch(tochar(data/1000));
	data=data%1000;
	putch(tochar(data/100));
	data=data%100;
	putch(tochar(data/10));
	data=data%10;
	putch(tochar(data));
}

/*
#pragma INTERRUPT int_uart_receive	// int_uart_receive �֐��͊��荞�݃��[�`���ł���ƃR���p�C���ɂ��m�点
void int_uart_receive(void)
{
	signed char a;
	// ���荞�݃t���O�N���A
	while(u0rif==1) u0rif==0;

	buff[writepos]=(unsigned char)u0rb;
	writepos++;
	writepos&=(buffsize-1);
	
	// �o�b�t�@�ɗ��܂��Ă����瑗�M�֎~�ɂ���
	// RTS��1�ɂ��Ă��A�����ɂ͎~�܂�Ȃ��̂�臒l�͗]�T������
	a=writepos-readpos;
	if(a>0)
	{
		if(a<4) RTS=1;	// ���M�֎~
	}
	else
	{
		if(a>-4) RTS=1;	// ���M�֎~
	}
}
*/