
void Delay(unsigned int xms)
{
	unsigned char i, j;
	while(xms--)
	{
		i = 2;
		j = 239;
		do
		{
			while (--j);
		} while (--i);
	}
}

/*******************************************************************************
* �� �� ��       : delay_10us
* ��������		 : ��ʱ������ten_us=1ʱ����Լ��ʱ10us
* ��    ��       : ten_us
* ��    ��    	 : ��
*******************************************************************************/
void delay_10us(unsigned int ten_us)
{
	while(ten_us--);	
}																   