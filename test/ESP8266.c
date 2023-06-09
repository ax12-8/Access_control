#include <REGX52.H>
#include "Delay.h"
#include "LCD1602.h"
#include "AT24C02.h"

typedef unsigned int u16;	  //对数据类型进行声明定义
typedef unsigned char u8;
u8 dat;
u8 Weather;
u8* ptr;


void Init()		//串口初始化函数
{
 
	TMOD=0x20;	//定时器工作方式2,8位自动重载(0010 0000)
	TL1=0xfd;		//装入初值
	TH1=0xfd;		
	TR1=1;	//启动定时器1
	REN=1;    //允许串行口接收数据
	SM0=0;    //工作方式1,10位异步收发
	SM1=1;   
	EA=1;   //打开全局中断控制
	ES=1;     //打开串行口中断
}

 void Sent_ZF(u8 dat)  //发送一个字节
{
	ES=0;	 //关闭串行口中断
	TI=0;	 //关闭发送中断标志位
	SBUF=dat;
	while(!TI);
    TI=0;
    ES=1;	//打开串行口中断
}

void AT_Send_String(u8 *string)  //发送字符串
{
  while(*string!='\0')
  {
    Sent_ZF(*string);
	string++;
	Delay(10);
  }
}


void ESP8266_Init()   //ESP8266初始化
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
AT_Send_String("AT+CIPMUX=0\r\n");  //设置为单连接模式，启动模块
	Delay(500);
	Delay(500);
  	AT_Send_String("AT+CIPSTART=\"TCP\",\"192.168.37.1\",8080\r\n"); //服务器的设置端口
  	Delay(500);
	Delay(500);
	 AT_Send_String("AT+CIPMODE=1\r\n"); //开启透传
  	Delay(500);
	Delay(500);
   	AT_Send_String("AT+CIPSEND\r\n"); //进入透传模式
  	Delay(500);
	Delay(500);
	AT_Send_String("OK"); //发送OK
  	Delay(500);
	Delay(500);
}


// void shuju()
//{
//AT_Send_String("\r\n"); //服务器的设置端口
//  	Delay(500);
//	Delay(500);
//} 


void InterruptUART() interrupt 4  
{ 
		u8 dat;
		u8 Weather;
        RI=0;        //清零
        dat= SBUF;    //收到的数据占存到SBUF中
		Delay(5);
		SBUF=dat;	//将接收到的数据放入到发送寄存器
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