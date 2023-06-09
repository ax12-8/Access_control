#ifndef _wifi_control_H
#define _wifi_control_H
#include"Delay.h"


void wifi_control_init(void);
void wifi_control(void);
void ESP8266_SendData(unsigned char *pbuf);
void zhengzhuan();
void fanzhuan();
#endif
