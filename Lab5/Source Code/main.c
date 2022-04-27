#include "stm32l476xx.h"
#include "LCD.h"

int main(void){
	volatile char result = 0;
	
	LCD_Init();
	delay_ms(20);
	LCD_Clear();
	
	for (volatile int i = 0; i < 32; i++) {
		result = read_keypad();
		delay_ms(20);
		LCD_DisplayResult(result, i);
	}
	
	while(1);
}