/*
	PS/2 �L�[�{�[�h�� SHARP X1 �ɂȂ�
	100[ms] �̃C���^�[�o���^�C�}�[

	2014�N7��22�� �쐬
	
	��������  http://kyoutan.jpn.org/

	���ۏ؂ł��B
	�������ꂪ�쐬���������͗p�r�ɐ�����݂��܂���B���p�E�񏤗p�ɂ�����炸���R�Ɏg�p���Ē����č\���܂���B
	����ɕ���������A����������A�z�z������A�������肵�Ă��ǂ��Ƃ������Ƃł��B
	�A���s�v�ł��B
*/

extern volatile unsigned short TIMER;		// 100[ms]�^�C�}�[

void timer_init(void);
//void timer_start(void);
#define timer_start()	tstart_trbcr=1	// TIMER RB2 �J�E���g�J�n 
