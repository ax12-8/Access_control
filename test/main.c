#include <REGX52.H>
#include "LCD1602.h"
#include "AT24C02.h"
#include "Delay.h"
#include "MatrixKey.h"
#include "uart.h"
#include "wifi_control.h"
#include "rc522.h"




#define OPEN 0					//��-��						
#define CLOSE 1					//��-��

#define WELCOME 0									//��ӭҳ
#define READ 1										//��������ҳ
#define READ_ERR 2								//��������ҳ
#define SET_MAIN_ADD 10						//����ҳ-������-��ӿ�Ƭ
#define SET_MAIN_DELETE 20				//����ҳ-������-ɾ����Ƭ
#define SET_ADD_PUT 11						//����ҳ-��ӿ�Ƭҳ-��ʾ���ÿ�Ƭ
#define SET_ADD_INFO 12						//����ҳ-��ӿ�Ƭҳ-��ʾ��Ƭ��Ϣ
#define SET_DELETE_INFO1 21				//����ҳ-ɾ����Ƭҳ-��ʾ��Ƭ0��Ϣ
#define SET_DELETE_INFO2 22				//����ҳ-ɾ����Ƭҳ-��ʾ��Ƭ1��Ϣ
#define SET_DELETE_INFO3 23				//����ҳ-ɾ����Ƭҳ-��ʾ��Ƭ2��Ϣ
#define SET_DELETE_INFO4 24				//����ҳ-ɾ����Ƭҳ-��ʾ��Ƭ3��Ϣ
#define SET_SUCCESS	3							//���óɹ�ҳ


sbit KEY_BACK = P3^7;						  //���ؼ���P2.4


extern uchar UID[4],Temp[4];
extern uchar UI0[4];					     	//��Ƭ0ID����
extern uchar UI1[4];							//��Ƭ1ID����
extern uchar UI2[4];							//��Ƭ2ID����
extern uchar UI3[4];							//��Ƭ3ID����

uchar code *Disp0="Password:" ;
uchar code *Disp1="        " ;						
uchar code *Disp2="Err Card";
uchar code *Disp3="CardNO.:";
uchar code *Disp4="OPEN";
uchar code *Disp5="Add New Card";
uchar code *Disp6="Delete Card";
uchar code *Disp7="Put Card On";
uchar code *Disp8="Success";
uchar code *Disp9="Delete:";

uchar count0;		//��ʱ��0.5ms��ʱ
uchar KeyNum;        //����ɨ��ֵ
uchar LOCK;          //��-״̬
uint Num=12345;	     //�洢������
uint Password;
uint Count,times=0;    //����
uint locktime;										//��������ʱʱ��
uint page;	
char cardnum;										  //���Զ�����

void Lock_Num();
void Change_Password();
void ctrl_process();
void RFID_Check();								//RFID������������
void ShowID(uchar *p);						//��ʾ��ID��������
void Tmr0init();									//��ʱ��0��ʼ����������
void LCD_page(uchar pagenum);				//LCD��ʾ��������
void keyscan();										//����ɨ�躯������
void zhengzhuan();							 //բ��-��
void fanzhuan();							//բ��-��
//void ReadEEPROM(uint sectr);			//��EEPROM��ȡ��ID��


void main(void)
{	
	LOCK = CLOSE;     //Ĭ����״̬-��
	LCD_Init();
	LCD_ShowString(1,1,Disp0);
	UART_Init();
	ESP8266_ModeInit();
	while (1)
	{
		Lock_Num();
	}

}



void Lock_Num()
{
	AT24C02_WriteByte(0, Num % 256);     //�洢��ʼ����
	Delay(5);
	AT24C02_WriteByte(1, Num / 256);
	Delay(5);
	KeyNum = MatrixKey();
	if(KeyNum)
	{
		if(KeyNum<=10)	//���S1~S10�������£���������
		{
			if(Count<5)	//����������С��5
			{
				Password*=10;				//��������һλ
				Password+=KeyNum%10;		//��ȡһλ����
				Count++;	//�ƴμ�һ
			}
			LCD_ShowNum(2,1,Password,5);	//������ʾ
		}
	
		if(KeyNum==11)	//���S11�������£�ȷ��
		{
			if(times<=3)
			{
				Num = AT24C02_ReadByte(0);
				Num |= AT24C02_ReadByte(1) << 8;  //��ȡ����
				if(Password==Num)	//������������ȷ����
				{	
					fanzhuan();
					Delay(1000);
					zhengzhuan();
					LCD_Clear();

					LCD_ShowString(1,7,"PASS");	//��ʾPASS
					Password=0;		//��������																			   
					Count=0;		//�ƴ�����
					times=0;
					LOCK = OPEN;
				}
				else				//����
				{
					times++;
					LCD_ShowString(2,7,"ERR");	//��ʾERR
					Delay(1000);
					LCD_Clear();
					Delay(1000);
					Password=0;		//��������
					Count=0;		//�ƴ�����
					LCD_ShowString(1,1,Disp0);
					LCD_ShowString(2, 1, Disp1);	//������ʾ
				}
			}
			else
			{
				LCD_Clear();
				Delay(1000);
				LCD_ShowString(1,6,"Locked");
				Password=0;		//��������
				Count=0;	  	//�ƴ�����		   
				times=0;
			}
		}
		if(KeyNum==12)	//���S12�������£�ȡ��
		{
			LCD_Clear();
			Delay(500);
			Password=0;		//��������
			Count=0;		//�ƴ�����
			LCD_ShowString(1,1,Disp0);
			LCD_ShowNum(2,1,Password,5);	//������ʾ
		}
		if(KeyNum==13)	//�޸�����
		{
			if(LOCK == OPEN)
			{
				Password=0;
				LCD_Clear();
				Delay(1000);
				LCD_ShowString(1, 1,"New password:");
				Change_Password();
				LCD_Clear();
				Delay(1000);
				LCD_ShowString(1,1,Disp0);
				LCD_ShowNum(2,1,Password,5);	//������ʾ
			}
			else
			{	
				Password=0;
				LCD_ShowString(2, 1,"No permission");
				Delay(1000);
				LCD_Clear();
				Delay(1000);
				LCD_ShowString(1,1,Disp0);
				LCD_ShowString(2, 1, Disp1);	//������ʾ	
			}	
		}
		if(KeyNum==14)	//��ȡ����
		{
			Num = AT24C02_ReadByte(0);
			Num |= AT24C02_ReadByte(1) << 8;
			LCD_ShowNum(2, 1, Num, 5);
			Delay(1000);
			LCD_ShowString(2, 1, "Read OK ");
			Delay(1000);
			Password=0;		//��������
			Count=0;
			LCD_ShowString(2, 1, Disp1);
		}									
		if(KeyNum==15)
			{
				
				wifi_control_init();
				LCD_Init();
				ESP8266_SendData("weather");
				
			}
		if(KeyNum==16)
		{
			ctrl_process();
		}	
	}
 }

void Change_Password()
{
	while(1)
	{
		if(LOCK==OPEN)
		{
			KeyNum = MatrixKey();
			if(KeyNum)
			{
				if(KeyNum<=10)	//���S1~S10�������£���������
				{
					if(Count<5)	//����������С��5
					{
						Password*=10;				//��������һλ
						Password+=KeyNum%10;		//��ȡһλ����
						Count++;	//�ƴμ�һ
					}
					LCD_ShowNum(2,1,Password,5);	//������ʾ
				}
				if(KeyNum==11)
				{
					Num = Password;
					AT24C02_WriteByte(0, Num % 256);     //�洢��ʼ����
				  	Delay(5);
				  	AT24C02_WriteByte(1, Num / 256);
				  	Delay(5);
					LCD_ShowString(2, 1, "Write OK ");
					Delay(500);
					Password=0;
					LOCK=CLOSE;
				}
				if(KeyNum==12)
				{	
					Password=0;		//��������
					Count=0;		//�ƴ�����
					LCD_ShowString(1,1,"New password:");
					LCD_ShowNum(2,1,Password,5);	//������ʾ	
				}
			}
		}
		else
		break;
	}
}


void ctrl_process(void)
{
	uchar tempcard;
	page = WELCOME;									//Ĭ���ڻ�ӭ����
	Tmr0init();											//��ʱ��0��ʼ��
	RFID_Init();										//RFIDģ���ʼ��
	LCD_Init();											//LCD1602��ʼ������
	LCD_page(WELCOME);							//��ʾ��ӭҳ"Welcome!"
//	ReadEEPROM(SECTOR1);						//��EEPROM����1��ȡ��1ID
//	ReadEEPROM(SECTOR2);						//��EEPROM����2��ȡ��2ID
//	ReadEEPROM(SECTOR3);						//��EEPROM����3��ȡ��3ID
//	ReadEEPROM(SECTOR4);						//��EEPROM����4��ȡ��4ID
	while(1)
	{
		Delay(100);
		if(LOCK == CLOSE && page < 10)							//������״̬Ϊ�ر�ʱ����������ʱ������
		{
			RFID_Check();								//����
		}
		keyscan();
		if(page == SET_ADD_PUT)				//ҳ����ʾ���ÿ�Ƭʱ,Ѱ�ҿտ�����¼���¿���Ϣ
		{
			if(UI0[0] + UI0[1] + UI0[2] + UI0[3] == 0) tempcard = 0;
			else if(UI1[0] + UI1[1] + UI1[2] + UI1[3] == 0) tempcard = 1;
			else if(UI2[0] + UI2[1] + UI2[2] + UI2[3] == 0) tempcard = 2;
			else if(UI3[0] + UI3[1] + UI3[2] + UI3[3] == 0) tempcard = 3;
			
			while(page == SET_ADD_PUT)
			{
				if(KEY_BACK == 0) break;
				Delay(100);
			if (PcdRequest(0x52, Temp) == MI_OK)
			{
				if (PcdAnticoll(UID) == MI_OK)
				{
					switch(tempcard)
					{
						case 0:UI0[0] = UID[0];UI0[1] = UID[1];UI0[2] = UID[2];UI0[3] = UID[3];page = SET_SUCCESS;break;
						case 1:UI1[0] = UID[0];UI1[1] = UID[1];UI1[2] = UID[2];UI1[3] = UID[3];page = SET_SUCCESS;break;
						case 2:UI2[0] = UID[0];UI2[1] = UID[1];UI2[2] = UID[2];UI2[3] = UID[3];page = SET_SUCCESS;break;
						case 3:UI3[0] = UID[0];UI3[1] = UID[1];UI3[2] = UID[2];UI3[3] = UID[3];page = SET_SUCCESS;break;
						default:break;
					}
				}
			}
		}
			LCD_page(page);
		}
		if(page == SET_SUCCESS)
		{
//			BeepTwice();
			Delay(1000);
			page = WELCOME;
		}
	}
}


void Tmr0init()
{
	TMOD |= 0x01;										//���ö�ʱ��ģʽ:��ʱ��0��������ʽ1,16λ��ʱ��
	TL0 = 0x33;											//���ö�ʱ��ʼֵ��8λ��500us
	TH0 = 0xFE;											//���ö�ʱ��ʼֵ��8λ
	TR0 = 1;												//������ʱ��0
	ET0 = 1;
	EA = 1;
}

void RFID_Check()									//��������������������ȡ�����
{
	cardnum = Rc522Test();					//��ȡ�����
	if(cardnum == 0)								//���Ϊ0����ʾ����Ƭ���󡱣�ϵͳ��û�����ſ�
	{
		LCD_page(READ_ERR);						//��ʾ��Ƭ����ҳ
//		Beep3times();									//��������3��
	}
	else if(cardnum==1||cardnum==2||cardnum==3||cardnum == 4)			//��������Ϊ1-4��˵����ϵͳ�е�4�ſ�
	{	
		LCD_page(READ);
		LOCK = OPEN;									//��״̬-��
//		BeepOnce();										//��������1��
	}
	Delay(100);	
}

void LCD_page(uchar pagenum)			//��ҳ�������ʾ��Ӧ����(�˵�����)
{
	switch(pagenum)
	{
		case WELCOME:
		{
			LCD_Clear();								//����
			LCD_ShowString(1,6,"Welcome");
		}break;		
		case READ:										//�����������棬��һ����ʾ��ID�ţ��ڶ�����ʾ���Զ����ţ���״̬
		{
			LCD_Clear();							//����
			ShowID(UID);								//��һ����ʾ��ID��
			LCD_WriteCommand(0xC0);			//�ڶ��е�һ��ʼ
			LCD_String(Disp3);					//��ʾ"CardNO.:"
			LCD_WriteData(cardnum+48);				//��ʾ�����					
			LCD_ShowString(2,12,Disp4);
			fanzhuan();
			Delay(1000);
			zhengzhuan();
		}break;
		case READ_ERR:								//����������棬��һ����ʾ��ID�ţ��ڶ�����ʾ��Ƭ����
		{
			LCD_Clear();								//����
			ShowID(UID);								//��һ����ʾ��ID��
			LCD_ShowString(2,1,Disp2);					//��ʾ"Err Card"
			Delay(1000);
		}break;
		case SET_MAIN_ADD:						//����ҳ-������-��ӿ�Ƭ
		{
			LCD_Clear();								//����
			LCD_ShowString(1,3,	Disp5);				//��ʾ"Add New Card"
		}break;
		case SET_MAIN_DELETE:					//����ҳ-������-ɾ����Ƭ
		{
			LCD_Clear();								//����
			LCD_ShowString(1,3,Disp6);					//��ʾ"Delete Card"
		}break;
		case SET_ADD_PUT:							//����Ҳ-��ӿ�Ƭҳ-���ÿ�Ƭ��ʾ
		{
			LCD_Clear();								//����
			LCD_ShowString(1,3,Disp7);					//��ʾ"PUT CARD ON"
		}break;
		case SET_DELETE_INFO1:
		{
			LCD_Clear();								//����
			LCD_ShowString(1,5,Disp9);					//��ʾ"DELETE:"
			LCD_WriteCommand(0xC2);		//��2�е�5��ʼ
			LCD_String(Disp3);					//��ʾ"CardNO.:"
			LCD_WriteData(1+48);				//��ʾ�����
		}break;
		case SET_DELETE_INFO2:
		{
			LCD_Clear();								//����
			LCD_ShowString(1,5,Disp9);					//��ʾ"DELETE:"
			LCD_WriteCommand(0xC2);		//��2�е�5��ʼ
			LCD_String(Disp3);					//��ʾ"CardNO.:"
			LCD_WriteData(2+48);				//��ʾ�����
		}break;
		case SET_DELETE_INFO3:
		{
			LCD_Clear();								//����
			LCD_ShowString(1,5,Disp9);					//��ʾ"DELETE:"
			LCD_WriteCommand(0xC2);		//��2�е�5��ʼ
			LCD_String(Disp3);					//��ʾ"CardNO.:"
			LCD_WriteData(3+48);				//��ʾ�����
		}break;
		case SET_DELETE_INFO4:
		{
			LCD_Clear();								//����
			LCD_ShowString(1,5,Disp9);					//��ʾ"DELETE:"
			LCD_WriteCommand(0xC2);		//��2�е�5��ʼ
			LCD_String(Disp3);					//��ʾ"CardNO.:"
			LCD_WriteData(4+48);				//��ʾ�����
		}break;
		case SET_SUCCESS:
		{
			LCD_Clear();								//����
			LCD_ShowString(1,5,Disp8);					//��ʾ"SUCCESS"
		}
		default:break;
	}
}


void ShowID(uchar *p)	 						//��ʾ���Ŀ��ţ���ʮ��������ʾ
{	
	uchar i;
	LCD_WriteCommand(0x80);				//LCD��һ�е�һ��λ��
	LCD_String("CardID:");							//��ʾ"CardID:"
	for(i=0;i<4;i++)
	{
		(p[i]/16)>9?(LCD_WriteData(p[i]/16+55)):(LCD_WriteData(p[i]/16+48));
		(p[i]%16)>9?(LCD_WriteData(p[i]%16+55)):(LCD_WriteData(p[i]%16+48));	//��ʮ������ת��Ϊ�ַ�����ʾ
		Delay(1);
	}
}


void keyscan()
{
	uchar i;
	KeyNum = MatrixKey();
	if(KeyNum)
	{
		if(KeyNum==1)								//���ü�
		{
			if(page < 10)								//�����ý������ת������ҳ����������ҳʱ������Ч
			{
				page = SET_MAIN_ADD;
			}
		}	
		if(KeyNum==2)						//��ҳ��
		{
			if(page == SET_MAIN_ADD) 		page = SET_MAIN_DELETE;
			else if(page == SET_MAIN_DELETE)	page = SET_MAIN_ADD;
			if(page == SET_DELETE_INFO1) page = SET_DELETE_INFO2;
			else if(page == SET_DELETE_INFO2) page = SET_DELETE_INFO3;
			else if(page == SET_DELETE_INFO3) page = SET_DELETE_INFO4;
			else if(page == SET_DELETE_INFO4) page = SET_DELETE_INFO1;
		}
		if(KeyNum==3)						//ȷ����
		{			
			if(page == SET_DELETE_INFO1) //ɾ����0
			{
				for(i = 0;i < 4;i++)
				{
					UI0[i] = 0x00;
				}
				page = SET_SUCCESS;
			}
			if(page == SET_DELETE_INFO2) //ɾ����1
			{
				for(i = 0;i < 4;i++)
				{
					UI1[i] = 0x00;
				}
				page = SET_SUCCESS;
			}
			if(page == SET_DELETE_INFO3) //ɾ����2
			{
				for(i = 0;i < 4;i++)
				{
					UI2[i] = 0x00;
				}
				page = SET_SUCCESS;
			}
			if(page == SET_DELETE_INFO4) //ɾ����3
			{
				for(i = 0;i < 4;i++)
				{
					UI3[i] = 0x00;
				}
				page = SET_SUCCESS;
			}
			if(page == SET_MAIN_ADD) 		page = SET_ADD_PUT;
			if(page == SET_MAIN_DELETE)	page = SET_DELETE_INFO1;

		}
		if(KeyNum==4)								//���ؼ������ػ�ӭ����
		 	{
				page = WELCOME;
			}
		
	}

		LCD_page(page);
									//LCD������ʾ��Ӧҳ��
}


void T0isr() interrupt 1					//��ʱ��0�жϳ����жϺ�1��ÿ0.5ms�ж�1��
{	
	TL0 = 0x33;											//���ö�ʱ��ʼֵ @500us
	TH0 = 0xFE;											//���ö�ʱ��ʼֵ
	if(LOCK == OPEN)								//��״̬-��ʱ
	{
		locktime++;									//������ʱ��ʼ��ʱ
		if(locktime >= 50)					//����1��ʱ
		{
			LOCK = CLOSE;								//��״̬-�ر�
			locktime = 0;								//�����ʱ
		}
	}
}
