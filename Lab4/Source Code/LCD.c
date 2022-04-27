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

void delay_ms(unsigned int ms) {			//Delays for a certain amount of time
	volatile unsigned int dummyCount = 0;		//volatile int so the program keeps it
	
	while(dummyCount < ms * 4000) {			//perform dummy task for certain time
		dummyCount++;
	}
	return;
}

void LCD_WriteCom(unsigned char com) {			//Writes a command to the LCD, pass in data 4 bits at a time
	volatile unsigned char data1, data2;
	data1 = (com>>4) & 0x0f;			//Get first 4 bits of command
	data2 = com & 0x0f;				//Get next 4 bits of command
	
	GPIOA->ODR &= ~C;				//Turn on command mode
	
	resetDataPins();				//Reset bus
	GPIOB->ODR |= data1;				//Set first 4 bits
	pulse();					//Send first 4 bits
	
	resetDataPins();				//Reset bus
	GPIOB->ODR |= data2;				//Set next 4 bits
	pulse();					//Send next 4 bits
}

void resetDataPins(void) {				//Reset data pins
	GPIOB->ODR &= ~(PB0 | PB1 | PB2 | PB3);		//Set data pins to 0
}

void pulse(void) {					//Sends the bus
	GPIOA->ODR |= E;				//Turns send high
	delay_ms(10);					//Wait
	GPIOA->ODR &= ~E;				//Turns send low
	delay_ms(10);					//Wait
}

void LCD_WriteData(unsigned char dat) {			//Writes data to the LCD, pass in data 4 bits at a time
	unsigned char data1, data2;
	data1 = (dat>>4) & 0xf;				//Get first 4 bits of data
	data2 = dat & 0xf;				//Get next 4 bits of data
	
	GPIOA->ODR |= C;				//Turn on data mode
	
	resetDataPins();				//Reset bus
	GPIOB->ODR |= data1;				//Set first 4 bits
	pulse();					//Send first 4 bits
	
	resetDataPins();				//Reset bus
	GPIOB->ODR |= data2;				//Set next 4 bits
	pulse();					//Send next 4 bits
}

void LCD_setFourBit(void) {				//Initial case to set LCD
	GPIOA->ODR |= C;				//Set to data mode
	pulse();					//Send data
	
	resetDataPins();				//Reset bus
	GPIOB->ODR |= 0x2;				//Set data
	pulse();					//Send data
}

void LCD_Init(void) {												//Initialize
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;									//Initialize GPIOA clock
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;									//Initialize GPIOB clock
	
	GPIOA->MODER &= ~(GPIO_MODER_MODE5 | GPIO_MODER_MODE6) | 0x00001000 | 0x00000400;			//Initialize output mode
	GPIOB->MODER &= ~(GPIO_MODER_MODE0 | GPIO_MODER_MODE1 | GPIO_MODER_MODE2 | GPIO_MODER_MODE3);		//Initialize output mode
	GPIOB->MODER |= 0x55;
	
	LCD_setFourBit();				//Initialize setup
	delay_ms(20);					//Wait
	
	LCD_WriteCom(0x28);				//Set 4 bit data
	LCD_Clear();					//Clear the board
	LCD_WriteCom(0x06);				//Initialize
	LCD_WriteCom(0x0c);				//Initialize
	delay_ms(5);					//Wait
}

void LCD_Clear(void) {					//Clear the LCD
	LCD_WriteCom(1);				//Set command to clear LCD
}

void LCD_DisplayString(volatile unsigned int line, unsigned char *ptr) {	//Display the string to the board
	if (line) {								//Check if character goes on first or second line
		LCD_WriteCom(0x80 + 0x40);					//Send command for second line
	}
	else {
		LCD_WriteCom(0x80);						//Send command for first line
	}
	
	for(int i = 0; ptr[i] != '\0'; i++) {					//Go through each character
		LCD_WriteData(ptr[i]);						//Print the character
	}
}
