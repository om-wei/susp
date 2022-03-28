#include <avr/io.h>         
#include <util/delay.h>
#include "ui.h"
#include "motor.h"

uint8_t Mode = 0;
uint8_t Func = 0;

int8_t read_key(void)
{
	unsigned char i, j;
	uint8_t r = -1;
	
	//高四位输出 低电平，低四位输入 带上拉电阻
	DDRKEY = 0xf0;           
	PORTKEY = 0x0f;         
	
	if ((PINKEY & 0x0f) != 0x0f) 
	{
		_delay_ms(20);                    
		
		if ((PINKEY & 0x0f) != 0x0f) 	//判断是否误按键盘
		{
			for (i = 4; i < 8; i++)               
			{
				PORTKEY = ~(1 << i) | 0x0f;      
				for (j = 0;j < 4; j++)
				{
					if((PINKEY & (1 << j)) == 0)        
						r = ((i - 4) * 4 + j);
				}
			} 
		}
	}

	PORTKEY = 0x0f;         
	while ((PINKEY & 0x0f) != 0x0f);	//松手检测
	return r;
}

void ui_mode0_init(void)
{
	write_com(0x01);
	_delay_ms(15);
	write_com(0x0C);//0000,1100  整体显示，游标off，游标位置off
	lcd_writestr(1, 1, (unsigned char*)"选择绘制的图形↓");
	lcd_writestr(2, 2, (unsigned char*)"到点");
	lcd_writestr(6, 2, (unsigned char*)"直线");
	lcd_writestr(2, 3, (unsigned char*)"矩形");
	lcd_writestr(6, 3, (unsigned char*)"五角");
	lcd_writestr(2, 4, (unsigned char*)"圆形");
	lcd_writestr(1, 2, (unsigned char*)"◆");
}

void ui_mode1_init(void)
{
	uint8_t s[4];

	write_com(0x01);
	_delay_ms(15);
	lcd_writestr(1, 1, (unsigned char*)" a=(   ,   )cm");
	if (Func == LINE || Func == RECT)
		lcd_writestr(1, 2, (unsigned char*)" b=(   ,   )cm");
	if (Func == STAR || Func == CIRCLE)
		lcd_writestr(1, 3, (unsigned char*)" r=(   )cm");
	lcd_writestr(1, 4, (unsigned char*)" c=(   ,   )cm");
	lcd_writestr(3, 4, u8toi((uint8_t)Pcurrent.x, s));
	lcd_writestr(5, 4, u8toi((uint8_t)Pcurrent.y, s));
	write_com(0x0f);////整體顯示ON 游標ON 游標位置反白ON
}

void ui_mode0(void)
{
	uint8_t row = 2, col = 1;//static 
	int8_t t;
	
	Func = 0;
	
	while (1)
	{
		switch (t = read_key())
		{
			case UP:
				if (row > 2)
				{
					lcd_writestr(col, row--, (unsigned char*)"  ");
					lcd_writestr(col, row, (unsigned char*)"◆");
					Func -= 2;
				}
				break;
			case DOWN:
				if (row < 4)
				{
					lcd_writestr(col, row++, (unsigned char*)"  ");
					lcd_writestr(col, row, (unsigned char*)"◆");
					Func += 2;
				}
				break;	
			case LEFT:
				if (col != 1)
				{
					lcd_writestr(col, row, (unsigned char*)"  ");
					col -= 4;
					lcd_writestr(col, row, (unsigned char*)"◆");
					Func--;
				}
				break;	
			case RIGHT:
				if (col == 1)
				{
					lcd_writestr(col, row, (unsigned char*)"  ");
					col += 4;
					lcd_writestr(col, row, (unsigned char*)"◆");
					Func++;
				}
				break;
			default:	break;
		}
		if (t == CHMOD)
		{
			Mode++;
			ui_mode1_init();		
			break;
		}
	}	
}

void ui_mode1(void)
{
	while (1)
	{
		switch (Func)
		{
			case POINT:
				mkpoint();
				break;
			case LINE:
				mkline_rect(LINE);
				break;
			case RECT:
				mkline_rect(RECT);
				break;
			case STAR:
				mkstar_cir(STAR);
				break;
			case CIRCLE:
				mkstar_cir(CIRCLE);
				break;	
			case NONE:	break;
			default:	break;
		}
		
		if (read_key() == CHMOD)
		{
			Mode--;
			ui_mode0_init();
			break;
		}
	}
}
void mkpoint(void)
{
	struct point p;
	
	Func = NONE;
	p.x = getnum(3, 1);
	p.y = getnum(5, 1);
	lcd_writestr(1, 2, (unsigned char*)"ready …");
	
	while (read_key() != RUN)
		;
	lcd_writestr(1, 2, (unsigned char*)"run …");
	ptop(Pcurrent, p);
}

void mkline_rect(unsigned char type)
{
	struct point p0, p1;
	
	Func = NONE;
	p0.x = getnum(3, 1);
	p0.y = getnum(5, 1);
	p1.x = getnum(3, 2);
	p1.y = getnum(5, 2);
	lcd_writestr(1, 3, (unsigned char*)"ready …");
	
	while (read_key() != RUN)
		;
	lcd_writestr(1, 3, (unsigned char*)"run …");
	if (type == LINE)
		drawline(p0, p1);
	if (type == RECT)
		drawrect(p0, p1);
}

void mkstar_cir(unsigned char type)
{
	struct point center;
	uint8_t r;
	
	Func = NONE;
	center.x = getnum(3, 1);
	center.y = getnum(5, 1);
	r = getnum(3, 3);
	lcd_writestr(1, 2, (unsigned char*)"ready …");
	
	while (read_key() != RUN)
		;
	lcd_writestr(1, 2, (unsigned char*)"run …");
	if (type == STAR)
		drawstar(center, r);
	if (type == CIRCLE)
		drawcircle(center, r);
}

uint8_t getnum(uint8_t x, uint8_t y)
{
	uint8_t i, j = 0;
	
	lcd_setxy(x, y);
	while ((i = read_key()) != NEXT)
	{
		if (i >= 0 && i <= 9)
		{
			write_dat(i + '0');
			j = j * 10 + i;
		}
		else if (i == MOD)	//修改输入值
		{
			lcd_writestr(x, y, (unsigned char*)"   ");
			lcd_setxy(x, y);
			i = j = 0;
		}
	}
	return (j);
}

unsigned char* u8toi(uint8_t i, unsigned char* s)
{
	s[0] = i / 100 + '0';
	s[1] = i % 100 / 10 + '0';
	s[2] = i % 10 + '0';
	s[3] = '\0';
	
	return s;
}
