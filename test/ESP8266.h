#ifndef __ESP8266_H__
#define __ESP8266_H__

typedef unsigned int u16;	  //对数据类型进行声明定义
typedef unsigned char u8;

// 用户调用函数：
void Init();
void Sent_ZF(u8 dat);  //发送一个字节
void AT_Send_String(u8 *string);  //发送字符串
void ESP8266_Init();   //ESP8266初始化
void fasong();
void shuju();


#endif
