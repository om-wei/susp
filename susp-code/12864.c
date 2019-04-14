
#include <avr/io.h>
#include <util/delay.h>

#define uchar unsigned char
#define uint unsigned int

#define LCD_PORT PORTB	//定义LED控制端口
#define LCD_DDR DDRB
//#define CS   3    	
#define SID  0		
#define SCLK 1		

void lcd_init(void);
void send_byte(uchar dat);
void write_com(uchar dat);
void write_dat(uchar dat);
void lcd_setxy(uchar x,uchar y);
void lcd_writestr(uchar dis_addr_x,uchar dis_addr_y,uchar* str);
 

/****************************************************** 
	函 数 名: lcd_init()
	功 能：液晶初始化 
	入口参数：无 
	出口参数：无 
	返 回 值：无 
******************************************************/ 
void lcd_init(void)
{
	LCD_DDR |= _BV(SCLK) | _BV(SID);    
	LCD_PORT |= _BV(SCLK) | _BV(SID);

//	LCD_PORT |= _BV(CS);  	//片选有效
	_delay_ms(50);    		
//	LCD_PORT &=~ _BV(CS);  	//片选无效

	write_com(0x30);//功能设置，一次送8位数据，基本指令集
	_delay_us(150);
	write_com(0x30);
	_delay_us(50);
	
	write_com(0x0C);//0000,1100  整体显示，游标off，游标位置off
//	write_com(0x0f);//整體顯示ON 游標ON 游標位置反白ON
	_delay_us(150);
	
	write_com(0x01);//0000,0001 清DDRAM
	_delay_ms(15);
	
	write_com(0x02);//0000,0010 DDRAM地址归位
	write_com(0x80);//1000,0000 设定DDRAM 7位地址000，0000到地址计数器AC
   	//write_com(0x04);//点设定，显示字符/光标从左到右移位，DDRAM地址加 一
    //write_com(0x0F);//显示设定，开显示，显示光标，当前显示位反白闪动

}

/****************************************************** 
	函 数 名: write_com()
	功 能：写命令 
	入口参数：uchar dat 
	出口参数：无 
	返 回 值：无 
******************************************************/ 
void write_com(uchar dat)
{
//	LCD_PORT |= _BV(CS);
	send_byte(0xf8);  		//命令字1111 1000
	send_byte(dat & 0xf0);	//高四位为:high = dat & 0xf0;作为第一个字节发送 
	send_byte(dat << 4);    //低四位为:low = dat << 4; 作为第二个字节发送 
//	LCD_PORT &= ~_BV(CS);
	_delay_us(100);
}

/****************************************************** 
	函 数 名: write_dat()
	功 能：写数据 
	入口参数：uchar dat 
	出口参数：无 
	返 回 值：无 
******************************************************/ 
void write_dat(uchar dat)
{
//	LCD_PORT |= _BV(CS);
	send_byte(0xfa);        //命令字1111 1010                                                       
	send_byte(dat & 0xf0);  //高四位为:high = dat & 0xf0;作为第一个字节发送
	send_byte(dat << 4);    //低四位为:low = dat << 4; 作为第二个字节发送
//	LCD_PORT &= ~_BV(CS);
}

/****************************************************** 
	函 数 名: send_byte()
	功 能：发送字节数据 
	入口参数：uchar dat 
	出口参数：无 
	返 回 值：无 
******************************************************/ 
void send_byte(uchar dat)
{
	uchar i;
	for(i=8;i>0;i--)
	{
		if(dat & _BV(i-1)) 
			LCD_PORT |= _BV(SID);   
		else 
			LCD_PORT &= ~_BV(SID);    
		LCD_PORT |= _BV(SCLK);     
		_delay_us(1);
		LCD_PORT &= ~_BV(SCLK);
	}
}

/****************************************************** 
	函 数 名: lcd_setxy()
	功 能：设置显示位置X(1~16),Y(1~4)
	入口参数：uchar x,uchar y
	出口参数：无 
	返 回 值：无 
******************************************************/ 
void lcd_setxy(uchar x,uchar y)
{ 
    switch(y)
	{
		case 1:
			write_com(0X7F+x);	break;
		case 2:
			write_com(0X8F+x);	break;
		case 3:
			write_com(0X87+x);	break;
		case 4:
			write_com(0X97+x);	break;
		default:				break;
	}
}

/****************************************************** 
	函 数 名: lcd_writestr()
	功 能：在指定位置显示字符串
	入口参数：uchar dis_addr_x,uchar dis_addr_y,uchar* str
	出口参数：无 
	返 回 值：无 
******************************************************/ 
void lcd_writestr(uchar dis_addr_x, uchar dis_addr_y, uchar* str)
{ 
    lcd_setxy(dis_addr_x, dis_addr_y);

    while(*str) 
        write_dat(*str++);
}
