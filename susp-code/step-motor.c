#include <avr/io.h>
#include <avr/interrupt.h>
#include <math.h>
#include "motor.h"
#include "ui.h"


struct point Pcurrent = {20, 25};	//在此初始化起始点坐标
static volatile uint16_t Count0, Count1;
static volatile uint8_t Flagm0 = READY, Flagm1 = READY;

void motor_init(void)
{
	DDRPWM0 |= _BV(PWM0);
	DDRDIR0 |= _BV(DIR0);
	DDRPWM1 |= _BV(PWM1);
	DDRDIR1 |= _BV(DIR1);
	
	TCCR0 |= _BV(WGM01) | _BV(COM00);	//比较输出模式,比较匹配发生时OC0 取反
	OCR0 = TOP;
	TIMSK |= _BV(OCIE0);
	
	TCCR2 |= _BV(WGM21) | _BV(COM20);	//比较输出模式,比较匹配发生时OC2 取反
	OCR2 = TOP;
	TIMSK |= _BV(OCIE2);
}

void drawcircle(struct point centre, uint8_t r)
{
	uint16_t i;
	struct point pt;
	
	pt.x = centre.x + r;
	pt.y = centre.y;
	ptop(Pcurrent, pt);

	for (i = 1; i <= 360; i++)
	{
		pt.x = cos(2 * M_PI * i / 360) * r + centre.x;
		pt.y = sin(2 * M_PI * i / 360) * r + centre.y;
		
		ptop(Pcurrent, pt);		
	}
}

void drawline(struct point p0, struct point p1)
{
	double ratx, raty, rad;
	double l, r;
	struct point pt;
	
	ptop(Pcurrent, p0);

	rad = atan((p1.y - p0.y) / (p1.x - p0.x));	
	if ((p1.x - p0.x) < 0)
		rad += M_PI;
	
	ratx = cos(rad);
	raty = sin(rad);
	l = sqrt(square(p1.y - p0.y) + square(p1.x - p0.x));
	
	for (r = 0.3; r < l; r += 0.3) //0.3为每次量化的线段长度
	{
		pt.x = p0.x + r * ratx;
		pt.y = p0.y + r * raty;
			
		ptop(Pcurrent, pt);
	}
	ptop(Pcurrent, p1);
}

void drawrect(struct point p0, struct point p1)
{
	struct point pt;
	
	pt.x = fmax(p0.x, p1.x);
	pt.y = fmax(p0.y, p1.y);
	p0.x = fmin(p0.x, p1.x);
	p0.y = fmin(p0.y, p1.y);
	
	p1.x = pt.x;
	p1.y = p0.y;
	drawline(p0, p1);
	drawline(p1, pt);
	p1.x = p0.x;
	p1.y = pt.y;
	drawline(pt, p1);
	drawline(p1, p0);
}

void drawstar(struct point centre, uint8_t r)
{
	struct point p[5] = 
	{
		{centre.x, centre.y + r},
		{centre.x - r * cos(2*M_PI*54/360), centre.y - r * sin(2*M_PI*54/360)},
		{centre.x + r * cos(2*M_PI*18/360), r * sin(2*M_PI*18/360) + centre.y},
		{centre.x - r * cos(2*M_PI*18/360), r * sin(2*M_PI*18/360) + centre.y},
		{centre.x + r * cos(2*M_PI*54/360), centre.y - r * sin(2*M_PI*54/360)}
	};
	
	drawline(p[0], p[1]);
	drawline(p[1], p[2]);
	drawline(p[2], p[3]);
	drawline(p[3], p[4]);
	drawline(p[4], p[0]);
}

/*	快速从点p0移动到p1	*/
void ptop(struct point p0, struct point p1)
{
	double a, b;
	static uint8_t x;
	
	while ((Flagm0 == BUSY) || (Flagm1 == BUSY))	//为两个电机同步，等待全部就绪
		;

	a = sqrt(square(p1.x) + square(HIGH - p1.y));
	b = sqrt(square(WIDE - p1.x) + square(HIGH - p1.y));
	a -= sqrt(square(p0.x) + square(HIGH - p0.y));
	b -= sqrt(square(WIDE - p0.x) + square(HIGH - p0.y));

	step_m0((uint16_t)(fabs(a) / STEP_LONG), (a > 0) ? CW : CCW);
	step_m1((uint16_t)(fabs(b) / STEP_LONG), (b > 0) ? CCW : CW);
	
	Pcurrent = p1;


/*	ui_mode = 1时 显示当前坐标*/	
	if ((Mode == 1) && (x != Pcurrent.x))
	{
		unsigned char s[4];
		lcd_writestr(3, 4, u8toi((uint8_t)Pcurrent.x, s));
		lcd_writestr(5, 4, u8toi((uint8_t)Pcurrent.y, s));
		x = Pcurrent.x;
	}
}

void step_m0(uint16_t nstep, uint8_t dir)
{
	if (nstep != 0)
	{
		Count0 = nstep * 2;	//计数值与CTC产生方波的方式有关，两次匹配中断说明产生一个脉冲
		
		if (dir == CW)
			PORTDIR0 |= _BV(DIR0);
		else
			PORTDIR0 &= ~_BV(DIR0);
		TCCR0 |= _BV(CS02);	//启动计数 clk/256
		Flagm0 = BUSY;
	}
}

void step_m1(uint16_t nstep, uint8_t dir)
{
	if (nstep != 0)
	{
		Count1 = nstep * 2;

		if (dir == CW)
			PORTDIR1 |= _BV(DIR1);
		else
			PORTDIR1 &= ~_BV(DIR1);
		TCCR2 |= _BV(CS22) | _BV(CS21);	//启动计数 clk/256
		Flagm1 = BUSY;
	}
}

ISR(TIMER0_COMP_vect)
{

	if(--Count0 == 0)
	{
		TCCR0 &= ~_BV(CS02);
		TCNT0 = 0;
		Flagm0 = READY;
	}
}

ISR(TIMER2_COMP_vect)
{
	
	if(--Count1 == 0)
	{
		TCCR2 &= ~(_BV(CS22) | _BV(CS21));
		TCNT2 = 0;
		Flagm1 = READY;
	}	
}