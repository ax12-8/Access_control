#ifndef __EEPROM_H__
#define __EEPROM_H__

#include <intrins.h>
#define uchar unsigned char
#define uint unsigned int 


/*Declare SFR associated with the ISP */
//ISP�������Ĵ�����ַ����
sfr ISP_DATA    =   0xE2;           //Flash data register			//���ݼĴ���				ISP_DATA
sfr ISP_ADDRH   =   0xE3;           //Flash address HIGH			//��ַ��8λ�Ĵ���	ISP_ADDRH
sfr ISP_ADDRL   =   0xE4;           //Flash address LOW				//���Ƶ�8λ�Ĵ���	ISP_ADDRL
sfr ISP_CMD     =   0xE5;           //Flash command register	//����Ĵ���				ISP_CMD
sfr ISP_TRIG    =   0xE6;           //Flash command trigger		//������Ĵ���		ISP_TRIG
sfr ISP_CONTR   =   0xE7;           //Flash control register	//����Ĵ���				ISP_CONTR

/*Define ISP/IAP/EEPROM command*/
//���� ISP/IAP/EEPROM ����
#define CMD_IDLE    0               //Stand-By			//����
#define CMD_READ    1               //Byte-Read			//�ֽڶ�
#define CMD_PROGRAM 2               //Byte-Program	//�ֽ�д
#define CMD_ERASE   3               //Sector-Erase	//��������

/*Define ISP/IAP/EEPROM operation const for ISP_CONTR*/
//���õȴ�ʱ�䣺ISP_CONTR�Ĵ����� WT2 WT1 WT0 3λ�������
//	WT2	WT1 WT0  
//   0   1   1   0x83			<5MHz
//   0   1   0   0x82     <10MHz
//	 0   0   1   0x81     <20MHz
//   0   0   0   0x80     <40MHz
//#define ENABLE_ISP 0x80           //if SYSCLK<40MHz
#define ENABLE_ISP   0x81           //if SYSCLK<20MHz
//#define ENABLE_ISP x82            //if SYSCLK<10MHz
//#define ENABLE_ISP 0x83           //if SYSCLK<5MHz

//����STC89C52RC EEPROM ��ʼ��ַ0x2000
#define ISP_ADDRESS 0x02000

//����8��������ʼ��ַ��STC89C51RC/C52RC��ʼ��ַΪ0x2000����ÿ������512�ֽ�
#define SECTOR1 0x2000							//��1������ʼ��ַ(0x2000 - 0x21FF)
#define SECTOR2 0x2200							//��2������ʼ��ַ(0x2200 - 0x23FF)
#define SECTOR3 0x2400							//��3������ʼ��ַ(0x2400 - 0x25FF)
#define SECTOR4 0x2600							//��4������ʼ��ַ(0x2600 - 0x27FF)
#define SECTOR5 0x2800							//��5������ʼ��ַ(0x2800 - 0x29FF)
#define SECTOR6 0x2A00							//��6������ʼ��ַ(0x2A00 - 0x2BFF)
#define SECTOR7 0x2C00							//��7������ʼ��ַ(0x2C00 - 0x2DFF)
#define SECTOR8 0x2E00							//��8������ʼ��ַ(0x2E00 - 0x2FFF)

void IspIdle();
uchar IspReadByte(uint addr);
void IspWriteByte(uint addr, uchar dat);
void IspEraseSector(uint addr);

#endif