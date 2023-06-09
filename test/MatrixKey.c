#include <REGX52.H>
#include "Delay.h"

/**
  * @brief  矩阵键盘读取按键键码
  * @param  无
  * @retval KeyNumber 按下按键的键码值
			如果按键按下不放，程序会停留在此函数，松手的一瞬间，返回按键键码，没有按键按下时，返回0
  */
unsigned char MatrixKey()
{
	unsigned char KeyNumber = 0;

	P1=0x0f;//给所有行赋值0，列全为1
	if(P1!=0x0f)//判断按键是否按下
	{
		Delay(20);//消抖
		if(P1!=0x0f)
		{
			//测试列
			P1=0x0f;
			switch(P1)//保存行为0，按键按下后的列值	
			{
				case 0x07: KeyNumber=1;break;
				case 0x0b: KeyNumber=2;break;
				case 0x0d: KeyNumber=3;break;
				case 0x0e: KeyNumber=4;break;
			}
			//测试行
			P1=0xf0;
			switch(P1)//保存列为0，按键按下后的键值	
			{
				case 0x70: KeyNumber=KeyNumber;break;
				case 0xb0: KeyNumber=KeyNumber+4;break;
				case 0xd0: KeyNumber=KeyNumber+8;break;
				case 0xe0: KeyNumber=KeyNumber+12;break;
			}
			while(P1!=0xf0);//等待按键松开	
		}
	}
	else
		KeyNumber=0;		
	
	return KeyNumber;		
}

