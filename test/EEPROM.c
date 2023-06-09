#include <REGX52.H>
#include "EEPROM.h"


/*----------------------------
Disable ISP/IAP/EEPROM function
Make MCU in a safe state
----------------------------*/
//��ֹISP/IAP/EEPROM������ȷ��MCU���ڰ�ȫ״̬
void IspIdle()
{
    ISP_CONTR = 0;                  //Close ISP function
    ISP_CMD = 0;                    //Clear command to standby
    ISP_TRIG = 0;                   //Clear trigger register
    ISP_ADDRH = 0x80;               //Data ptr point to non-EEPROM area
    ISP_ADDRL = 0;                  //Clear ISP address to prevent misuse
}

/*----------------------------
Read one byte from ISP/IAP/EEPROM area
Input: addr (ISP/IAP/EEPROM address)
Output:Flash data
----------------------------*/
//��EEPROM��ȡ1���ֽ�
//���룺��ַ uint addr
//���������
uchar IspReadByte(uint addr)
{
    uchar dat;                       //Data buffer

    ISP_CONTR = ENABLE_ISP;         //Open ISP function, and set wait time
    ISP_CMD = CMD_READ;             //Set ISP/IAP/EEPROM READ command
    ISP_ADDRL = addr;               //Set ISP/IAP/EEPROM address low
    ISP_ADDRH = addr >> 8;          //Set ISP/IAP/EEPROM address high
    ISP_TRIG = 0x46;                //Send trigger command1 (0x46)
    ISP_TRIG = 0xb9;                //Send trigger command2 (0xb9)
    _nop_();                        //MCU will hold here until ISP/IAP/EEPROM operation complete
    dat = ISP_DATA;                 //Read ISP/IAP/EEPROM data
    IspIdle();                      //Close ISP/IAP/EEPROM function

    return dat;                     //Return Flash data
}

/*----------------------------
Program one byte to ISP/IAP/EEPROM area
Input: addr (ISP/IAP/EEPROM address)
       dat (ISP/IAP/EEPROM data)
Output:-
----------------------------*/
//д1���ֽڵ�ISP/IAP/EEPROM
//���룺	��ַ uint addr
//			���� uchar dat
void IspWriteByte(uint addr, uchar dat)
{
    ISP_CONTR = ENABLE_ISP;         //Open ISP function, and set wait time
    ISP_CMD = CMD_PROGRAM;          //Set ISP/IAP/EEPROM PROGRAM command
    ISP_ADDRL = addr;               //Set ISP/IAP/EEPROM address low
    ISP_ADDRH = addr >> 8;          //Set ISP/IAP/EEPROM address high
    ISP_DATA = dat;                 //Write ISP/IAP/EEPROM data
    ISP_TRIG = 0x46;                //Send trigger command1 (0x46)
    ISP_TRIG = 0xb9;                //Send trigger command2 (0xb9)
    _nop_();                        //MCU will hold here until ISP/IAP/EEPROM operation complete
    IspIdle();
}

/*----------------------------
Erase one sector area
Input: addr (ISP/IAP/EEPROM address)
Output:-
----------------------------*/
//����1������
//���룺��ַ uint addr
void IspEraseSector(uint addr)
{
    ISP_CONTR = ENABLE_ISP;         //Open IAP function, and set wait time
    ISP_CMD = CMD_ERASE;            //Set ISP/IAP/EEPROM ERASE command
    ISP_ADDRL = addr;               //Set ISP/IAP/EEPROM address low
    ISP_ADDRH = addr >> 8;          //Set ISP/IAP/EEPROM address high
    ISP_TRIG = 0x46;                //Send trigger command1 (0x46)
    ISP_TRIG = 0xb9;                //Send trigger command2 (0xb9)
    _nop_();                        //MCU will hold here until ISP/IAP/EEPROM operation complete
    IspIdle();
}

