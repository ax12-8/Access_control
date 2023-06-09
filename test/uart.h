#ifndef _uart_H
#define _uart_H

#include "stdio.h"

typedef unsigned int u16;	//对系统默认数据类型进行重定义
typedef unsigned char u8;

#define UART_REC_LEN  			10  	//定义最大接收字节数 50

extern u8  UART_RX_BUF[UART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 UART_RX_STA;         		//接收状态标记	


void UART_Init(void);
void UART_SendData(u8 dat);
void UART_SendString(u8 *pbuf);


#endif
