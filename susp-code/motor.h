#ifndef	MOTOR_H
#define MOTOR_H

#define TOP 		117	// OCRX = (FOSC / (2*N*FREOC0) - 1),输出频率200Hz

#define SUBDIVS		0.45
#define STEP_LONG	((SUBDIVS/360)*(2*M_PI*1.85))	//单位cm
#define HIGH 		70
#define WIDE		46

#define CCW		1
#define CW		0
#define BUSY	1
#define READY	0

//	电路接口
#define DDRPWM0		DDRB
#define DDRDIR0		DDRB
#define PORTDIR0	PORTB
#define PWM0		PB3	//(OC0/AIN1) PB3
#define DIR0		PB4

#define DDRPWM1		DDRD
#define DDRDIR1		DDRD
#define PORTDIR1	PORTD
#define PWM1		PD7	//PD7 (OC2)
#define DIR1		PD6

struct point 
{
	double x;
	double y;
};

extern struct point Pcurrent;

void motor_init(void);
void step_m0(uint16_t nstep, uint8_t dir);
void step_m1(uint16_t nstep, uint8_t dir);
void ptop(struct point p0, struct point p1);
void drawcircle(struct point centre, uint8_t r);
void drawline(struct point p0, struct point p1);
void drawrect(struct point p0, struct point p1);
void drawstar(struct point centre, uint8_t r);

#endif