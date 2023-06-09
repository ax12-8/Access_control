#include <REGX52.H>
#include "LCD1602.h"
#include "AT24C02.h"
#include "Delay.h"
#include "MatrixKey.h"
#include "uart.h"
#include "wifi_control.h"
#include "rc522.h"




#define OPEN 0					//锁-开						
#define CLOSE 1					//锁-关

#define WELCOME 0									//欢迎页
#define READ 1										//读卡正常页
#define READ_ERR 2								//读卡错误页
#define SET_MAIN_ADD 10						//设置页-主界面-添加卡片
#define SET_MAIN_DELETE 20				//设置页-主界面-删除卡片
#define SET_ADD_PUT 11						//设置页-添加卡片页-提示放置卡片
#define SET_ADD_INFO 12						//设置页-添加卡片页-显示卡片信息
#define SET_DELETE_INFO1 21				//设置页-删除卡片页-显示卡片0信息
#define SET_DELETE_INFO2 22				//设置页-删除卡片页-显示卡片1信息
#define SET_DELETE_INFO3 23				//设置页-删除卡片页-显示卡片2信息
#define SET_DELETE_INFO4 24				//设置页-删除卡片页-显示卡片3信息
#define SET_SUCCESS	3							//设置成功页


sbit KEY_BACK = P3^7;						  //返回键用P2.4


extern uchar UID[4],Temp[4];
extern uchar UI0[4];					     	//卡片0ID数组
extern uchar UI1[4];							//卡片1ID数组
extern uchar UI2[4];							//卡片2ID数组
extern uchar UI3[4];							//卡片3ID数组

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

uchar count0;		//定时器0.5ms计时
uchar KeyNum;        //键盘扫描值
uchar LOCK;          //锁-状态
uint Num=12345;	     //存储的密码
uint Password;
uint Count,times=0;    //计数
uint locktime;										//开锁后延时时间
uint page;	
char cardnum;										  //卡自定义编号

void Lock_Num();
void Change_Password();
void ctrl_process();
void RFID_Check();								//RFID读卡函数声明
void ShowID(uchar *p);						//显示卡ID函数声明
void Tmr0init();									//定时器0初始化函数声明
void LCD_page(uchar pagenum);				//LCD显示函数声明
void keyscan();										//按键扫描函数声明
void zhengzhuan();							 //闸机-关
void fanzhuan();							//闸机-开
//void ReadEEPROM(uint sectr);			//从EEPROM读取卡ID号


void main(void)
{	
	LOCK = CLOSE;     //默认锁状态-关
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
	AT24C02_WriteByte(0, Num % 256);     //存储初始密码
	Delay(5);
	AT24C02_WriteByte(1, Num / 256);
	Delay(5);
	KeyNum = MatrixKey();
	if(KeyNum)
	{
		if(KeyNum<=10)	//如果S1~S10按键按下，输入密码
		{
			if(Count<5)	//如果输入次数小于5
			{
				Password*=10;				//密码左移一位
				Password+=KeyNum%10;		//获取一位密码
				Count++;	//计次加一
			}
			LCD_ShowNum(2,1,Password,5);	//更新显示
		}
	
		if(KeyNum==11)	//如果S11按键按下，确认
		{
			if(times<=3)
			{
				Num = AT24C02_ReadByte(0);
				Num |= AT24C02_ReadByte(1) << 8;  //读取数据
				if(Password==Num)	//如果密码等于正确密码
				{	
					fanzhuan();
					Delay(1000);
					zhengzhuan();
					LCD_Clear();

					LCD_ShowString(1,7,"PASS");	//显示PASS
					Password=0;		//密码清零																			   
					Count=0;		//计次清零
					times=0;
					LOCK = OPEN;
				}
				else				//否则
				{
					times++;
					LCD_ShowString(2,7,"ERR");	//显示ERR
					Delay(1000);
					LCD_Clear();
					Delay(1000);
					Password=0;		//密码清零
					Count=0;		//计次清零
					LCD_ShowString(1,1,Disp0);
					LCD_ShowString(2, 1, Disp1);	//更新显示
				}
			}
			else
			{
				LCD_Clear();
				Delay(1000);
				LCD_ShowString(1,6,"Locked");
				Password=0;		//密码清零
				Count=0;	  	//计次清零		   
				times=0;
			}
		}
		if(KeyNum==12)	//如果S12按键按下，取消
		{
			LCD_Clear();
			Delay(500);
			Password=0;		//密码清零
			Count=0;		//计次清零
			LCD_ShowString(1,1,Disp0);
			LCD_ShowNum(2,1,Password,5);	//更新显示
		}
		if(KeyNum==13)	//修改密码
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
				LCD_ShowNum(2,1,Password,5);	//更新显示
			}
			else
			{	
				Password=0;
				LCD_ShowString(2, 1,"No permission");
				Delay(1000);
				LCD_Clear();
				Delay(1000);
				LCD_ShowString(1,1,Disp0);
				LCD_ShowString(2, 1, Disp1);	//更新显示	
			}	
		}
		if(KeyNum==14)	//读取密码
		{
			Num = AT24C02_ReadByte(0);
			Num |= AT24C02_ReadByte(1) << 8;
			LCD_ShowNum(2, 1, Num, 5);
			Delay(1000);
			LCD_ShowString(2, 1, "Read OK ");
			Delay(1000);
			Password=0;		//密码清零
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
				if(KeyNum<=10)	//如果S1~S10按键按下，输入密码
				{
					if(Count<5)	//如果输入次数小于5
					{
						Password*=10;				//密码左移一位
						Password+=KeyNum%10;		//获取一位密码
						Count++;	//计次加一
					}
					LCD_ShowNum(2,1,Password,5);	//更新显示
				}
				if(KeyNum==11)
				{
					Num = Password;
					AT24C02_WriteByte(0, Num % 256);     //存储初始密码
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
					Password=0;		//密码清零
					Count=0;		//计次清零
					LCD_ShowString(1,1,"New password:");
					LCD_ShowNum(2,1,Password,5);	//更新显示	
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
	page = WELCOME;									//默认在欢迎界面
	Tmr0init();											//定时器0初始化
	RFID_Init();										//RFID模块初始化
	LCD_Init();											//LCD1602初始化函数
	LCD_page(WELCOME);							//显示欢迎页"Welcome!"
//	ReadEEPROM(SECTOR1);						//从EEPROM扇区1读取卡1ID
//	ReadEEPROM(SECTOR2);						//从EEPROM扇区2读取卡2ID
//	ReadEEPROM(SECTOR3);						//从EEPROM扇区3读取卡3ID
//	ReadEEPROM(SECTOR4);						//从EEPROM扇区4读取卡4ID
	while(1)
	{
		Delay(100);
		if(LOCK == CLOSE && page < 10)							//仅在锁状态为关闭时读卡，锁开时不读卡
		{
			RFID_Check();								//读卡
		}
		keyscan();
		if(page == SET_ADD_PUT)				//页面提示放置卡片时,寻找空卡，并录入新卡信息
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
	TMOD |= 0x01;										//设置定时器模式:定时器0，工作方式1,16位定时器
	TL0 = 0x33;											//设置定时初始值低8位：500us
	TH0 = 0xFE;											//设置定时初始值高8位
	TR0 = 1;												//启动定时器0
	ET0 = 1;
	EA = 1;
}

void RFID_Check()									//读卡器函数，读卡并获取卡编号
{
	cardnum = Rc522Test();					//获取卡编号
	if(cardnum == 0)								//如果为0，表示“卡片错误”，系统中没有这张卡
	{
		LCD_page(READ_ERR);						//显示卡片错误页
//		Beep3times();									//蜂鸣器响3声
	}
	else if(cardnum==1||cardnum==2||cardnum==3||cardnum == 4)			//如果卡编号为1-4，说明是系统中的4张卡
	{	
		LCD_page(READ);
		LOCK = OPEN;									//锁状态-开
//		BeepOnce();										//蜂鸣器响1声
	}
	Delay(100);	
}

void LCD_page(uchar pagenum)			//按页面序号显示对应内容(菜单功能)
{
	switch(pagenum)
	{
		case WELCOME:
		{
			LCD_Clear();								//清屏
			LCD_ShowString(1,6,"Welcome");
		}break;		
		case READ:										//正常读卡界面，第一行显示卡ID号，第二行显示卡自定义编号，锁状态
		{
			LCD_Clear();							//清屏
			ShowID(UID);								//第一行显示卡ID号
			LCD_WriteCommand(0xC0);			//第二行第一格开始
			LCD_String(Disp3);					//显示"CardNO.:"
			LCD_WriteData(cardnum+48);				//显示卡编号					
			LCD_ShowString(2,12,Disp4);
			fanzhuan();
			Delay(1000);
			zhengzhuan();
		}break;
		case READ_ERR:								//读卡错误界面，第一行显示卡ID号，第二行显示卡片错误
		{
			LCD_Clear();								//清屏
			ShowID(UID);								//第一行显示卡ID号
			LCD_ShowString(2,1,Disp2);					//显示"Err Card"
			Delay(1000);
		}break;
		case SET_MAIN_ADD:						//设置页-主界面-添加卡片
		{
			LCD_Clear();								//清屏
			LCD_ShowString(1,3,	Disp5);				//显示"Add New Card"
		}break;
		case SET_MAIN_DELETE:					//设置页-主界面-删除卡片
		{
			LCD_Clear();								//清屏
			LCD_ShowString(1,3,Disp6);					//显示"Delete Card"
		}break;
		case SET_ADD_PUT:							//设置也-添加卡片页-放置卡片提示
		{
			LCD_Clear();								//清屏
			LCD_ShowString(1,3,Disp7);					//显示"PUT CARD ON"
		}break;
		case SET_DELETE_INFO1:
		{
			LCD_Clear();								//清屏
			LCD_ShowString(1,5,Disp9);					//显示"DELETE:"
			LCD_WriteCommand(0xC2);		//第2行第5格开始
			LCD_String(Disp3);					//显示"CardNO.:"
			LCD_WriteData(1+48);				//显示卡编号
		}break;
		case SET_DELETE_INFO2:
		{
			LCD_Clear();								//清屏
			LCD_ShowString(1,5,Disp9);					//显示"DELETE:"
			LCD_WriteCommand(0xC2);		//第2行第5格开始
			LCD_String(Disp3);					//显示"CardNO.:"
			LCD_WriteData(2+48);				//显示卡编号
		}break;
		case SET_DELETE_INFO3:
		{
			LCD_Clear();								//清屏
			LCD_ShowString(1,5,Disp9);					//显示"DELETE:"
			LCD_WriteCommand(0xC2);		//第2行第5格开始
			LCD_String(Disp3);					//显示"CardNO.:"
			LCD_WriteData(3+48);				//显示卡编号
		}break;
		case SET_DELETE_INFO4:
		{
			LCD_Clear();								//清屏
			LCD_ShowString(1,5,Disp9);					//显示"DELETE:"
			LCD_WriteCommand(0xC2);		//第2行第5格开始
			LCD_String(Disp3);					//显示"CardNO.:"
			LCD_WriteData(4+48);				//显示卡编号
		}break;
		case SET_SUCCESS:
		{
			LCD_Clear();								//清屏
			LCD_ShowString(1,5,Disp8);					//显示"SUCCESS"
		}
		default:break;
	}
}


void ShowID(uchar *p)	 						//显示卡的卡号，以十六进制显示
{	
	uchar i;
	LCD_WriteCommand(0x80);				//LCD第一行第一格位置
	LCD_String("CardID:");							//显示"CardID:"
	for(i=0;i<4;i++)
	{
		(p[i]/16)>9?(LCD_WriteData(p[i]/16+55)):(LCD_WriteData(p[i]/16+48));
		(p[i]%16)>9?(LCD_WriteData(p[i]%16+55)):(LCD_WriteData(p[i]%16+48));	//将十六进制转化为字符，显示
		Delay(1);
	}
}


void keyscan()
{
	uchar i;
	KeyNum = MatrixKey();
	if(KeyNum)
	{
		if(KeyNum==1)								//设置键
		{
			if(page < 10)								//非设置界面才跳转到设置页，已在设置页时按键无效
			{
				page = SET_MAIN_ADD;
			}
		}	
		if(KeyNum==2)						//翻页键
		{
			if(page == SET_MAIN_ADD) 		page = SET_MAIN_DELETE;
			else if(page == SET_MAIN_DELETE)	page = SET_MAIN_ADD;
			if(page == SET_DELETE_INFO1) page = SET_DELETE_INFO2;
			else if(page == SET_DELETE_INFO2) page = SET_DELETE_INFO3;
			else if(page == SET_DELETE_INFO3) page = SET_DELETE_INFO4;
			else if(page == SET_DELETE_INFO4) page = SET_DELETE_INFO1;
		}
		if(KeyNum==3)						//确定键
		{			
			if(page == SET_DELETE_INFO1) //删除卡0
			{
				for(i = 0;i < 4;i++)
				{
					UI0[i] = 0x00;
				}
				page = SET_SUCCESS;
			}
			if(page == SET_DELETE_INFO2) //删除卡1
			{
				for(i = 0;i < 4;i++)
				{
					UI1[i] = 0x00;
				}
				page = SET_SUCCESS;
			}
			if(page == SET_DELETE_INFO3) //删除卡2
			{
				for(i = 0;i < 4;i++)
				{
					UI2[i] = 0x00;
				}
				page = SET_SUCCESS;
			}
			if(page == SET_DELETE_INFO4) //删除卡3
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
		if(KeyNum==4)								//返回键，返回欢迎界面
		 	{
				page = WELCOME;
			}
		
	}

		LCD_page(page);
									//LCD更新显示对应页面
}


void T0isr() interrupt 1					//定时器0中断程序，中断号1，每0.5ms中断1次
{	
	TL0 = 0x33;											//设置定时初始值 @500us
	TH0 = 0xFE;											//设置定时初始值
	if(LOCK == OPEN)								//锁状态-开时
	{
		locktime++;									//锁开延时开始计时
		if(locktime >= 50)					//到达1秒时
		{
			LOCK = CLOSE;								//锁状态-关闭
			locktime = 0;								//清零计时
		}
	}
}
