#ifndef __EEPROM_H__
#define __EEPROM_H__

#include <intrins.h>
#define uchar unsigned char
#define uint unsigned int 


/*Declare SFR associated with the ISP */
//ISP相关特殊寄存器地址声明
sfr ISP_DATA    =   0xE2;           //Flash data register			//数据寄存器				ISP_DATA
sfr ISP_ADDRH   =   0xE3;           //Flash address HIGH			//地址高8位寄存器	ISP_ADDRH
sfr ISP_ADDRL   =   0xE4;           //Flash address LOW				//机制低8位寄存器	ISP_ADDRL
sfr ISP_CMD     =   0xE5;           //Flash command register	//命令寄存器				ISP_CMD
sfr ISP_TRIG    =   0xE6;           //Flash command trigger		//命令触发寄存器		ISP_TRIG
sfr ISP_CONTR   =   0xE7;           //Flash control register	//命令寄存器				ISP_CONTR

/*Define ISP/IAP/EEPROM command*/
//定义 ISP/IAP/EEPROM 命令
#define CMD_IDLE    0               //Stand-By			//就绪
#define CMD_READ    1               //Byte-Read			//字节读
#define CMD_PROGRAM 2               //Byte-Program	//字节写
#define CMD_ERASE   3               //Sector-Erase	//扇区擦除

/*Define ISP/IAP/EEPROM operation const for ISP_CONTR*/
//设置等待时间：ISP_CONTR寄存器的 WT2 WT1 WT0 3位组合设置
//	WT2	WT1 WT0  
//   0   1   1   0x83			<5MHz
//   0   1   0   0x82     <10MHz
//	 0   0   1   0x81     <20MHz
//   0   0   0   0x80     <40MHz
//#define ENABLE_ISP 0x80           //if SYSCLK<40MHz
#define ENABLE_ISP   0x81           //if SYSCLK<20MHz
//#define ENABLE_ISP x82            //if SYSCLK<10MHz
//#define ENABLE_ISP 0x83           //if SYSCLK<5MHz

//定义STC89C52RC EEPROM 起始地址0x2000
#define ISP_ADDRESS 0x02000

//定义8个扇区起始地址（STC89C51RC/C52RC起始地址为0x2000），每个扇区512字节
#define SECTOR1 0x2000							//第1扇区起始地址(0x2000 - 0x21FF)
#define SECTOR2 0x2200							//第2扇区起始地址(0x2200 - 0x23FF)
#define SECTOR3 0x2400							//第3扇区起始地址(0x2400 - 0x25FF)
#define SECTOR4 0x2600							//第4扇区起始地址(0x2600 - 0x27FF)
#define SECTOR5 0x2800							//第5扇区起始地址(0x2800 - 0x29FF)
#define SECTOR6 0x2A00							//第6扇区起始地址(0x2A00 - 0x2BFF)
#define SECTOR7 0x2C00							//第7扇区起始地址(0x2C00 - 0x2DFF)
#define SECTOR8 0x2E00							//第8扇区起始地址(0x2E00 - 0x2FFF)

void IspIdle();
uchar IspReadByte(uint addr);
void IspWriteByte(uint addr, uchar dat);
void IspEraseSector(uint addr);

#endif