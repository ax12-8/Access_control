#include <REGX52.H>
#include <stdlib.h>
#include "wifi_control.h"
#include "uart.h"
#include "LCD1602.h"
#define uchar unsigned char 
#define uint unsigned int 	

//L298	
sbit MA=P2^3; 
sbit MB=P2^4;

/*
   ���ת������
*/

void fanzhuan()
{
	MA=0;MB=1;  //��
	Delay(6);
	MA=0;MB=0;
}

void zhengzhuan()
{
	MA=1;MB=0;  //��
	Delay(6);
	MA=0;MB=0;
}


//ESP8266 WIFI����ATָ��
//pbuf��ATָ��ַ�����ʽ���磺"AT"
void ESP8266_SendCmd(u8 *pbuf)
{
	while(*pbuf!='\0') //�����ո�����ѭ��	
	{
		UART_SendData(*pbuf);
		delay_10us(5);
		pbuf++;	
	}
	delay_10us(5);
	UART_SendData('\r');//�س�
	delay_10us(5);
	UART_SendData('\n');//����
	Delay(1000);
}

//ESP8266 WIFI�������ݵ�APP
//pbuf������
void ESP8266_SendData(u8 *pbuf)
{
	ESP8266_SendCmd("AT+CIPSEND=0,7");
	while(*pbuf!='\0') //�����ո�����ѭ��	
	{
		UART_SendData(*pbuf);
		delay_10us(5);
		pbuf++;	
	}
	UART_SendData('\n');//����	
}

//ESP8266-WIFIģ�鹤��ģʽ��ʼ��
void ESP8266_ModeInit(void)
{
	ESP8266_SendCmd("ATE0");
	ESP8266_SendCmd("AT+CWMODE=1");
	ESP8266_SendCmd("AT+CIPMUX=1");//������
	ESP8266_SendCmd("AT+CIPSERVER=1,8266");	//����TCP/IP �˿�Ϊ8266 ʵ�ֻ����������	
} 

//WIFI���Ƴ�ʼ��
void wifi_control_init(void)
{
	
	ES=1;//�������ж�
}


//�����жϷ�����
//�����ֻ�APP���͵��źź���ư�����Դ
void UART_IRQn() interrupt 4
{
	static u8 i=0;

	if(RI)
	{
		RI=0;
		UART_RX_BUF[i]=SBUF;//��ȡ���յ�������
		if(UART_RX_BUF[0]=='+')i++;
		else i=0;
		if(i==10)
		{
			if(UART_RX_BUF[9]-'A'==20)
			{
				fanzhuan();
				i=0;
			}
			if(UART_RX_BUF[9]-'A'==3)
			{
				zhengzhuan();
				i=0;
			}
		}
		//��ȡ��������
		if(i==23)
		{
			LCD_ShowChar(1,1,UART_RX_BUF[10]);
			LCD_ShowChar(1,2,UART_RX_BUF[11]);
			LCD_ShowChar(1,3,UART_RX_BUF[12]);
			LCD_ShowChar(1,4,UART_RX_BUF[13]);
			LCD_ShowChar(1,5,UART_RX_BUF[14]);
			LCD_ShowChar(1,6,UART_RX_BUF[15]);
			LCD_ShowChar(1,7,UART_RX_BUF[16]);
			LCD_ShowChar(1,8,UART_RX_BUF[17]);
			LCD_ShowChar(1,9,UART_RX_BUF[18]);
			LCD_ShowString(1,11,"Week");
			LCD_ShowChar(1,16,UART_RX_BUF[22]);
			if(UART_RX_BUF[19]-'A'==50)
				LCD_ShowString(2,1,"Sunny");
			if(UART_RX_BUF[19]-'A'==34)
				LCD_ShowString(2,1,"Cloudy");
			if(UART_RX_BUF[19]-'A'==14)
				LCD_ShowString(2,1,"Overcast");
			if(UART_RX_BUF[19]-'A'==17)
				LCD_ShowString(2,1,"Rainy");
			if(UART_RX_BUF[19]-'A'==3)
				LCD_ShowString(2,1,"Dust");
			if(UART_RX_BUF[19]-'A'==5)
				LCD_ShowString(2,1,"Foggy");
			if(UART_RX_BUF[19]-'A'==39)
				LCD_ShowString(2,1,"Haze");
			if(UART_RX_BUF[19]-'A'==22)
				LCD_ShowString(2,1,"Windy");
			if(UART_RX_BUF[19]-'A'==2)
				LCD_ShowString(2,1,"Cold");
			if(UART_RX_BUF[19]-'A'==7)
				LCD_ShowString(2,1,"Hot");
			LCD_ShowString(2,11,"T:");
			LCD_ShowChar(2,14,UART_RX_BUF[20]);
			LCD_ShowChar(2,15,UART_RX_BUF[21]);
			LCD_ShowString(2,16,"C");
			i=0;
		}	
	}		
}



