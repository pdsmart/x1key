/*
	PS/2 �L�[�{�[�h�� SHARP X1 �ɂȂ�
	PS/2 �L�[�{�[�h�̎�M����

	2014�N7��22�� �쐬
	
	��������  http://kyoutan.jpn.org/

	���ۏ؂ł��B
	�������ꂪ�쐬���������͗p�r�ɐ�����݂��܂���B���p�E�񏤗p�ɂ�����炸���R�Ɏg�p���Ē����č\���܂���B
	����ɕ���������A����������A�z�z������A�������肵�Ă��ǂ��Ƃ������Ƃł��B
	�A���s�v�ł��B
*/

#define PS2TIMEOUT	30		// PS2 �^�C���A�E�g 30*100[ms] = 3[s]
#define PS2BUFFSIZE 0x10

extern volatile unsigned short PS2TIMER;		// PS2��M�^�C���A�E�g�^�C�}�[
//extern volatile unsigned char PS2BUFF[PS2BUFFSIZE];	// PS2��M�o�b�t�@
//extern volatile unsigned char PS2RPOS;		// PS2�ǂݏo���ʒu
//extern volatile unsigned char PS2WPOS;		// PS2�������݈ʒu

void ps2key_init(void);
unsigned char ps2size(void);
void ps2clear(void);
unsigned char ps2read(void);
unsigned char ps2get(void);
