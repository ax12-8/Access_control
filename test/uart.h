#ifndef _uart_H
#define _uart_H

#include "stdio.h"

typedef unsigned int u16;	//��ϵͳĬ���������ͽ����ض���
typedef unsigned char u8;

#define UART_REC_LEN  			10  	//�����������ֽ��� 50

extern u8  UART_RX_BUF[UART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 UART_RX_STA;         		//����״̬���	


void UART_Init(void);
void UART_SendData(u8 dat);
void UART_SendString(u8 *pbuf);


#endif
