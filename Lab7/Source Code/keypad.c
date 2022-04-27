#include "keypad.h"

char read_character() {
	unsigned int columns;
	while(1) {
		GPIOB->ODR = 0xE0;
		columns = GPIOC->IDR;
		columns &= 0xF;
		switch(columns) {
			case 0xE: return '1';
			case 0xD: return '2';
			case 0xB: return '3';
			default: ;
			//case 0x7: return 'A';
		}
		
		GPIOB->ODR = 0xD0;
		columns = GPIOC->IDR;
		columns &= 0xF;
		switch(columns) {
			case 0xE: return '4';
			case 0xD: return '5';
			case 0xB: return '6';
			default: ;
			//case 0x7: return 'B';
		}
		
		GPIOB->ODR = 0xB0;
		columns = GPIOC->IDR;
		columns &= 0xF;
		switch(columns) {
			case 0xE: return '7';
			case 0xD: return '8';
			case 0xB: return '9';
			default: ;
			//case 0x7: return 'C';
		}
		
		GPIOB->ODR = 0x70;
		columns = GPIOC->IDR;
 		columns &= 0xF;
		switch(columns) {
			//case 0xE: return '*';
			case 0xD: return '0';
			case 0xB: return '#';
			default: ;
			//case 0x7: return 'D';
		}
	}
}

int read_keypad() {
	char command[3] = {'0', '0', '0'};
	char temp;
	int time = 0;
	
	for (int i = 0; i < 3; i++) {
		delay(200);
		temp = read_character();
		delay(200);
		
		if (temp == '#') {
			if (i == 2) {
				time += (command[0] - 48) * 10;
				time += command[1] - 48;
				return time;
			}
			else {
				return command[0] - 48;
			} 
		}
		else {
			command[i] = temp;
		}
	}
	
	return 0;
}
