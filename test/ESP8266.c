#include <REGX52.H>
#include "Delay.h"
#include "LCD1602.h"
#include "AT24C02.h"

typedef unsigned int u16;	  //���������ͽ�����������
typedef unsigned char u8;
u8 dat;
u8 Weather;
u8* ptr;


void Init()		//���ڳ�ʼ������
{
 
	TMOD=0x20;	//��ʱ��������ʽ2,8λ�Զ�����(0010 0000)
	TL1=0xfd;		//װ���ֵ
	TH1=0xfd;		
	TR1=1;	//������ʱ��1
	REN=1;    //�����пڽ�������
	SM0=0;    //������ʽ1,10λ�첽�շ�
	SM1=1;   
	EA=1;   //��ȫ���жϿ���
	ES=1;     //�򿪴��п��ж�
}

 void Sent_ZF(u8 dat)  //����һ���ֽ�
{
	ES=0;	 //�رմ��п��ж�
	TI=0;	 //�رշ����жϱ�־λ
	SBUF=dat;
	while(!TI);
    TI=0;
    ES=1;	//�򿪴��п��ж�
}

void AT_Send_String(u8 *string)  //�����ַ���
{
  while(*string!='\0')
  {
    Sent_ZF(*string);
	string++;
	Delay(10);
  }
}


void ESP8266_Init()   //ESP8266��ʼ��
{	
    AT_Send_String("AT+CWMODE=1\r\n");  
	Delay(1000);
	Delay(1000);
	AT_Send_String("AT+CWJAP=\"123456\",\"12345678\"\r\n");  
	Delay(1000);
	Delay(1000);
	AT_Send_String("AT+CIPMUX=1\r\n"); 
	Delay(500); 
	AT_Send_String("AT+CIPSERVER=1,8266\r\n");
	Delay(500);  

}

void fasong()
{
AT_Send_String("AT+CIPMUX=0\r\n");  //����Ϊ������ģʽ������ģ��
	Delay(500);
	Delay(500);
  	AT_Send_String("AT+CIPSTART=\"TCP\",\"192.168.37.1\",8080\r\n"); //�����������ö˿�
  	Delay(500);
	Delay(500);
	 AT_Send_String("AT+CIPMODE=1\r\n"); //����͸��
  	Delay(500);
	Delay(500);
   	AT_Send_String("AT+CIPSEND\r\n"); //����͸��ģʽ
  	Delay(500);
	Delay(500);
	AT_Send_String("OK"); //����OK
  	Delay(500);
	Delay(500);
}


// void shuju()
//{
//AT_Send_String("\r\n"); //�����������ö˿�
//  	Delay(500);
//	Delay(500);
//} 


void InterruptUART() interrupt 4  
{ 
		u8 dat;
		u8 Weather;
        RI=0;        //����
        dat= SBUF;    //�յ�������ռ�浽SBUF��
		Delay(5);
		SBUF=dat;	//�����յ������ݷ��뵽���ͼĴ���
		AT24C02_WriteByte(0, dat % 256);     
		Delay(5);
		AT24C02_WriteByte(1, dat / 256);
		Delay(5);
		Weather = AT24C02_ReadByte(0);
		Weather |= AT24C02_ReadByte(1) << 8;
	    ptr = &Weather;
		LCD_ShowString(2, 1, ptr);
		while(!TI);
		TI=0;

}