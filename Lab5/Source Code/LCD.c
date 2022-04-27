#include "LCD.h"
#include "stm32l476xx.h"
#include <time.h>
#include <stdio.h>

#define C GPIO_ODR_OD5
#define E GPIO_ODR_OD6

#define PB0 GPIO_ODR_OD0
#define PB1 GPIO_ODR_OD1
#define PB2 GPIO_ODR_OD2
#define PB3 GPIO_ODR_OD3
#define PB4 GPIO_ODR_OD4
#define PB5 GPIO_ODR_OD5
#define PB6 GPIO_ODR_OD6
#define PB7 GPIO_ODR_OD7

#define PC0 GPIO_IDR_ID0
#define PC1 GPIO_IDR_ID1
#define PC2 GPIO_IDR_ID2
#define PC3 GPIO_IDR_ID3

void delay_ms(unsigned int ms) {					//Delays for a certain amount of time
	volatile unsigned int dummyCount = 0;		//volatile int so the program keeps it
	
	while(dummyCount < ms * 4000) {					//perform dummy task for certain time
		dummyCount++;
	}
	return;
}

char read_keypad() {
	unsigned int columns;
	while(1) {
		GPIOB->ODR = 0xE0;
		columns = GPIOC->IDR;
		columns &= 0xF;
		switch(columns) {
			case 0xE: return '1';
			case 0xD: return '2';
			case 0xB: return '3';
			case 0x7: return 'A';
		}
		
		GPIOB->ODR = 0xD0;
		columns = GPIOC->IDR;
		columns &= 0xF;
		switch(columns) {
			case 0xE: return '4';
			case 0xD: return '5';
			case 0xB: return '6';
			case 0x7: return 'B';
		}
		
		GPIOB->ODR = 0xB0;
		columns = GPIOC->IDR;
		columns &= 0xF;
		switch(columns) {
			case 0xE: return '7';
			case 0xD: return '8';
			case 0xB: return '9';
			case 0x7: return 'C';
		}
		
		GPIOB->ODR = 0x70;
		columns = GPIOC->IDR;
		columns &= 0xF;
		switch(columns) {
			case 0xE: return '*';
			case 0xD: return '0';
			case 0xB: return '#';
			case 0x7: return 'D';
		}
	}
}

void LCD_WriteCom(unsigned char com) {			//Writes a command to the LCD, pass in data 4 bits at a time
	volatile unsigned char data1, data2;
	data1 = (com>>4) & 0x0f;		//Get first 4 bits of command
	data2 = com & 0x0f;					//Get next 4 bits of command
	
	GPIOA->ODR &= ~C;						//Turn on command mode
	
	resetDataPins();						//Reset bus
	GPIOB->ODR |= data1;				//Set first 4 bits
	pulse();										//Send first 4 bits
	
	resetDataPins();						//Reset bus
	GPIOB->ODR |= data2;				//Set next 4 bits
	pulse();										//Send next 4 bits
}

void resetDataPins(void) {									//Reset data pins
	GPIOB->ODR &= ~(PB0 | PB1 | PB2 | PB3);		//Set data pins to 0
}

void pulse(void) {					//Sends the bus
	GPIOA->ODR |= E;					//Turns send high
	delay_ms(10);							//Wait
	GPIOA->ODR &= ~E;					//Turns send low
	delay_ms(10);							//Wait
}

void LCD_WriteData(unsigned char dat) {			//Writes data to the LCD, pass in data 4 bits at a time
	unsigned char data1, data2;
	data1 = (dat>>4) & 0xf;				//Get first 4 bits of data
	data2 = dat & 0xf;						//Get next 4 bits of data
	
	GPIOA->ODR |= C;							//Turn on data mode
	
	resetDataPins();							//Reset bus
	GPIOB->ODR |= data1;					//Set first 4 bits
	pulse();											//Send first 4 bits
	
	resetDataPins();							//Reset bus
	GPIOB->ODR |= data2;					//Set next 4 bits
	pulse();											//Send next 4 bits
}

void LCD_setFourBit(void) {			//Initial case to set LCD
	GPIOA->ODR |= C;							//Set to data mode
	pulse();											//Send data
	
	resetDataPins();							//Reset bus
	GPIOB->ODR |= 0x2;						//Set data
	pulse();											//Send data
}

void LCD_Init(void) {																		//Initialize
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;									//Initialize GPIOA clock
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;									//Initialize GPIOB clock
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;									//Initialize GPIOC clock
	
	GPIOA->MODER &= ~(GPIO_MODER_MODE5 | GPIO_MODER_MODE6) | 0x00001400;			//Initialize output mode
	GPIOB->MODER &= ~(GPIO_MODER_MODE0 | GPIO_MODER_MODE1 | GPIO_MODER_MODE2 | GPIO_MODER_MODE3 | GPIO_MODER_MODE4 | GPIO_MODER_MODE5 | GPIO_MODER_MODE6 | GPIO_MODER_MODE7);		//Initialize output mode
	GPIOB->MODER |= 0x5555;
	GPIOB->OTYPER |= 0x00F0;
	GPIOC->MODER &= ~(GPIO_MODER_MODE0 | GPIO_MODER_MODE1 | GPIO_MODER_MODE2 | GPIO_MODER_MODE3);
	
	LCD_setFourBit();					//Initialize setup
	delay_ms(20);							//Wait
	
	LCD_WriteCom(0x28);				//Set 4 bit data
	LCD_Clear();							//Clear the board
	LCD_WriteCom(0x06);				//Initialize
	LCD_WriteCom(0x0c);				//Initialize
	delay_ms(5);							//Wait
}

void LCD_Clear(void) {			//Clear the LCD
	LCD_WriteCom(1);					//Set command to clear LCD
}

void LCD_DisplayResult(volatile char ch, volatile int charCount) {
	if (charCount == 0) {
		LCD_WriteCom(0x80);
	}
	else if (charCount == 16) {
		LCD_WriteCom(0x80 + 0x40);
	}
	
	LCD_WriteData(ch);
	delay_ms(10);
}
