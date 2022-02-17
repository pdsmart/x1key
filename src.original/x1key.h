/*
	PS/2 �L�[�{�[�h�� SHARP X1 �ɂȂ�
	X1 �L�[�{�[�h�̑��M����

	2014�N7��22�� �쐬
	
	��������  http://kyoutan.jpn.org/

	���ۏ؂ł��B
	�������ꂪ�쐬���������͗p�r�ɐ�����݂��܂���B���p�E�񏤗p�ɂ�����炸���R�Ɏg�p���Ē����č\���܂���B
	����ɕ���������A����������A�z�z������A�������肵�Ă��ǂ��Ƃ������Ƃł��B
	�A���s�v�ł��B
*/

/*
18.432MHz
1���� = 1/18.432 = 0.0543[us]
256���� = 256/18.432	= 13.89[us]
65536���� = 65536/18.432 = 3555[us]

1750[us] 1750/(1/18.432) = 1750*(18.432/1) = 1750*18.432
                        = 32256      = 0x7E00
1000[us] = 1000 *18.432 = 18432 ���� = 0x4800
 750[us]                = 13824      = 0x3600
 700[us]                = 12902      = 0x3266
 250[us] = 250*18.432   =  4608      = 0x1200
  
�^�C�}�[RB2�̃v���O�����g�`�������[�h���������Ǝv�������ǁA�v���C�}�����Ԃ�
�A���_�[�t���[�Ŋ��荞�ݗv���ł��Ȃ��悤�������̂ŁA
�^�C�}�[RJ2�̃p���X�o�̓��[�h���g�����Ƃɂ���B
*/

#define TRJ1750us	(0x7E00 - 2)
#define TRJ1000us	(0x4800 - 0)	// �^�C�}�[��~��ԂŎg���̂ŁA�����͈����Ȃ�
#define TRJ750us	(0x3600 - 2)
#define TRJ700us	(0x3266 - 1)
#define TRJ250us	(0x1200 - 2)
#define TRJSTOP		(0xFFFF)	// �X�g�b�v�r�b�g��̂��₷�݊���
// TRJ���W�X�^�ɏ����Ă���A�����[�h���W�X�^�ɓ]�������܂� 2�`3�T�C�N��������̂�
// ���̂Ԃ�����Ă���

void X1_send(unsigned short data);
void x1key_init(void);

unsigned char tochar(unsigned char a);
void puth2(unsigned char a);


//extern volatile unsigned short SEND_DATA;	// X1���M�f�[�^
// b15 : 0=�e���L�[����̓���
// b14 : 0=�L�[���͗L��
// b13 : 0=���s�[�g�L��
// b12 : 0=GRAPH ON
// b11 : 0=CAPS ON
// b10 : 0=�J�i ON
// b09 : 0=SHIFT ON
// b08 : 0=CTRL ON
// b07-00 : ASCII CODE (0x00=KEY OFF)
