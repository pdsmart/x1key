/*
	PS/2 �L�[�{�[�h�� SHARP X1 �ɂȂ�
	I/O �s���̒�`�ق�

	2014�N7��22�� �쐬
	
	��������  http://kyoutan.jpn.org/

	���ۏ؂ł��B
	�������ꂪ�쐬���������͗p�r�ɐ�����݂��܂���B���p�E�񏤗p�ɂ�����炸���R�Ɏg�p���Ē����č\���܂���B
	����ɕ���������A����������A�z�z������A�������肵�Ă��ǂ��Ƃ������Ƃł��B
	�A���s�v�ł��B
*/

#define PS2DATA		p3_3
#define X1KEYOUT	p3_7

#define	TRUE	1
#define	FALSE	0
#define	NULL	0

#define DI()	asm("FCLR I")	// ���荞�݋֎~
#define EI()	asm("FSET I")	// ���荞�݋���
