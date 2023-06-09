#include <REGX52.H>
#include "Delay.h"
#include "beep.h"

void BeepOnce()
{	
	Beep = 0;
	Delay(100);;
	Beep = 1;		   	
}	
void BeepTwice()
{
	Beep = 0;
	Delay(30);
	Beep = 1;	
	Delay(30);	
	Beep = 0;
	Delay(30);
	Beep = 1;	
}
void Beep3times()
{
	Beep = 0;
	Delay(30);
	Beep = 1;	
	Delay(30);	
	Beep = 0;
	Delay(30);
	Beep = 1;	
	Delay(30);
	Beep = 0;
	Delay(30);
	Beep = 1;	
}