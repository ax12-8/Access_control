#include <REGX52.H>
#include "Delay.h"

/**
  * @brief  ������̶�ȡ��������
  * @param  ��
  * @retval KeyNumber ���°����ļ���ֵ
			����������²��ţ������ͣ���ڴ˺��������ֵ�һ˲�䣬���ذ������룬û�а�������ʱ������0
  */
unsigned char MatrixKey()
{
	unsigned char KeyNumber = 0;

	P1=0x0f;//�������и�ֵ0����ȫΪ1
	if(P1!=0x0f)//�жϰ����Ƿ���
	{
		Delay(20);//����
		if(P1!=0x0f)
		{
			//������
			P1=0x0f;
			switch(P1)//������Ϊ0���������º����ֵ	
			{
				case 0x07: KeyNumber=1;break;
				case 0x0b: KeyNumber=2;break;
				case 0x0d: KeyNumber=3;break;
				case 0x0e: KeyNumber=4;break;
			}
			//������
			P1=0xf0;
			switch(P1)//������Ϊ0���������º�ļ�ֵ	
			{
				case 0x70: KeyNumber=KeyNumber;break;
				case 0xb0: KeyNumber=KeyNumber+4;break;
				case 0xd0: KeyNumber=KeyNumber+8;break;
				case 0xe0: KeyNumber=KeyNumber+12;break;
			}
			while(P1!=0xf0);//�ȴ������ɿ�	
		}
	}
	else
		KeyNumber=0;		
	
	return KeyNumber;		
}

