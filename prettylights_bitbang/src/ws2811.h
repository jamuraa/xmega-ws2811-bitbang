/*
 * ws2811.h
 *
 * Created: 6/1/2013 5:43:48 PM
 *  Author: Michael Janssen
 */ 


#ifndef WS2811_H_
#define WS2811_H_


#define NOP __asm__ __volatile__ ("cp r0,r0\n");

#define NOP2 NOP NOP
#define NOP1 NOP
#define NOP3 NOP NOP2
#define NOP4 NOP NOP3
#define NOP5 NOP NOP4
#define NOP6 NOP NOP5
#define NOP7 NOP NOP6
#define NOP8 NOP NOP7
#define NOP9 NOP NOP8
#define NOP10 NOP NOP9
#define NOP11 NOP NOP10
#define NOP12 NOP NOP11
#define NOP13 NOP NOP12
#define NOP14 NOP NOP13
#define NOP15 NOP NOP14

#define NOP_SHORT NOP7
#define NOP_LONG NOP_SHORT NOP_SHORT NOP_SHORT NOP_SHORT

#define NOP_100NS NOP3
#define NOP_250NS NOP7
#define NOP_1000NS NOP_250NS NOP_250NS NOP_250NS NOP_250NS

#define NOP_600NS NOP10 NOP9
#define NOP_650NS NOP10 NOP11

#define WS2811_BIT_SET(PORT, PIN, X, N) if ( X & (1<<N)) { OUTPORT.OUTSET = OUTPIN; NOP_LONG; OUTPORT.OUTCLR = OUTPIN; NOP_SHORT; } else \
	{ OUTPORT.OUTSET = OUTPIN; NOP_SHORT; OUTPORT.OUTCLR = OUTPIN; NOP_LONG; }

#define WS2811_BIT_ALL(PORT, PIN) \
WS2811_BIT_SET(PORT, PIN, cur_byte, 7); \
WS2811_BIT_SET(PORT, PIN, cur_byte, 6); \
WS2811_BIT_SET(PORT, PIN, cur_byte, 5); \
WS2811_BIT_SET(PORT, PIN, cur_byte, 4); \
WS2811_BIT_SET(PORT, PIN, cur_byte, 3); \
WS2811_BIT_SET(PORT, PIN, cur_byte, 2); \
WS2811_BIT_SET(PORT, PIN, cur_byte, 1); \
WS2811_BIT_SET(PORT, PIN, cur_byte, 0);

#define WS2811_ALL(PORT, PIN, PTR, END) \
	while(PTR != END) { register unsigned char cur_byte = *PTR++; WS2811_BIT_ALL(PORT, PIN); \
	cur_byte = *PTR++; WS2811_BIT_ALL(PORT, PIN); cur_byte = *PTR++; WS2811_BIT_ALL(PORT, PIN); }

#endif /* WS2811_H_ */