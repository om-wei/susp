#ifndef UI_H
#define UI_H

#define PORTKEY	PORTA
#define PINKEY	PINA
#define DDRKEY	DDRA

#define UP		10
#define CHMOD	11
#define RIGHT	12
#define LEFT	13
#define DOWN	14
#define RUN		15
#define NEXT	14
#define MOD		10

#define POINT	0
#define LINE	1
#define RECT	2
#define STAR	3
#define CIRCLE 	4
#define NONE	5


int8_t read_key(void); 
void ui_mode0(void);
void ui_mode1(void);
void ui_mode0_init(void);
void ui_mode1_init(void);
uint8_t getnum(uint8_t x, uint8_t y); //参数为液晶显示坐标值
void mkpoint(void);
void mkline_rect(unsigned char type);
void mkstar_cir(unsigned char type);

void lcd_init(void);
void send_byte(unsigned char dat);
void write_com(unsigned char dat);
void write_dat(unsigned char dat);
void lcd_setxy(unsigned char x,unsigned char y);
void lcd_writestr(unsigned char dis_addr_x,unsigned char dis_addr_y,unsigned char* str);

unsigned char* u8toi(uint8_t i, unsigned char* s);

uint8_t Mode;
uint8_t Func;

#endif