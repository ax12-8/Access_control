#ifndef __ESP8266_H__
#define __ESP8266_H__

typedef unsigned int u16;	  //���������ͽ�����������
typedef unsigned char u8;

// �û����ú�����
void Init();
void Sent_ZF(u8 dat);  //����һ���ֽ�
void AT_Send_String(u8 *string);  //�����ַ���
void ESP8266_Init();   //ESP8266��ʼ��
void fasong();
void shuju();


#endif
