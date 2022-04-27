#include "stm32l476xx.h"
#include "motor.h"
#include "keypad.h"

//global variables
volatile int running;
volatile int position[4] = {1, 1, 1, 1};
volatile int time;
volatile int buzzing = 0;
volatile int counter = 0;

//function prototypes
void SysTick_Initialize(uint32_t ticks);
void SysTick_Handler(void);
void wind_up(void);
int count_down(void);
void initialize(void);
void count_up(void);
void buzz(void);
void delay(unsigned int t);
void wind_down(void);

void initialize(void) {																	//Initialize
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;									//Initialize GPIOA clock
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;									//Initialize GPIOB clock
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;									//Initialize GPIOC clock
	
	GPIOA->MODER &= ~(GPIO_MODER_MODE5) | 0x00000400;			//Initialize output mode
	GPIOB->MODER &= ~(GPIO_MODER_MODE0 | GPIO_MODER_MODE1 | GPIO_MODER_MODE2 | GPIO_MODER_MODE3 | GPIO_MODER_MODE4 | GPIO_MODER_MODE5 | GPIO_MODER_MODE6 | GPIO_MODER_MODE7);		//Initialize output mode
	GPIOB->MODER |= 0x5555;
	GPIOB->OTYPER |= 0x00F0;
	GPIOC->MODER &= ~(GPIO_MODER_MODE0 | GPIO_MODER_MODE1 | GPIO_MODER_MODE2 | GPIO_MODER_MODE3);
	
	//SysTick_Initialize(80000);
}

void SysTick_Initialize(uint32_t ticks){
	SysTick->CTRL = 0;																						//Disable Systick
	SysTick->LOAD = ticks - 1; 																		//Set reload register
	
	//Set priority of Systick to least urgency/highest priority
	NVIC_SetPriority (SysTick_IRQn, (1 <<__NVIC_PRIO_BITS) - 1);
	SysTick->VAL = 0; 																						//reset the Systick counter value
	SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk; 									//select processor clock
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk; 										//enable systick interupt
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk; 										// enable systick
}

void delay(unsigned int t) {
	volatile unsigned int dummyCount = 0;
	
	while (dummyCount < t * 100) {
		dummyCount++;
	}
}

void SysTick_Handler(){
	volatile int tempCounter = counter;
	if (counter >= 200){ // change based on how often this is called
		buzzing = 0;
		if(running){
			if (time == 0) {
				running = 0;
				buzzing = 1;
			}
			else {
				count_down();
				time--;
			}
		}
		counter = 0;
	}
	if(buzzing == 1){
		buzz();
	}
	counter++;
}

void wind_up(void){
	for (int i = 0; i < time; i++) {
		count_up();
	}
}
void wind_down(void){
	for (int i = 0; i < time; i++) {
		count_down();
		delay(4000);
	}
}
void count_up(void) {
	uint32_t cur = 0x0;
	
	for (int i = 0; i < 34; i++) {
		cur = GPIOB->ODR;
		cur &= 0xF;
		
		if(cur == 0x7) {
			GPIOB->ODR |= 0xF;
			GPIOB->ODR &= 0xFFFFFFFE;
			delay(10);
		}
		else if (cur == 0xE) {
			GPIOB->ODR |= 0xF;
			GPIOB->ODR &= 0xFFFFFFFD;
			delay(10);
		}
		else if (cur == 0xD) {
			GPIOB->ODR |= 0xF;
			GPIOB->ODR &= 0xFFFFFFFB;
			delay(10);
		}
		else {
			GPIOB->ODR |= 0xF;
			GPIOB->ODR &= 0xFFFFFFF7;
			delay(10);
		}
	}
}

int count_down(void){
	uint32_t cur;
	
	for (int i = 0; i < 34; i++) {
		cur = GPIOB->ODR;
		cur &= 0xF;
		
		if (cur == 0x7) {
			GPIOB->ODR |= 0xF;
			GPIOB->ODR &= 0xFFFFFFFB;
			delay(10);
		}
		else if (cur == 0xB) {
			GPIOB->ODR |= 0xF;
			GPIOB->ODR &= 0xFFFFFFFD;
			delay(10);
		}
		else if (cur == 0xD) {
			GPIOB->ODR |= 0xF;
			GPIOB->ODR &= 0xFFFFFFFE;
			delay(10);
		}
		else {
			GPIOB->ODR |= 0xF;
			GPIOB->ODR &= 0xFFFFFFF7;
			delay(10);
		}
	}
	
	return 0;
}

void buzz(void) {
	//toggle pa5
	GPIOA->ODR ^= 0x00000020;
}

 int main(void){
	initialize();
	running = 0; //start with stopwatch paused
	 
	time = read_keypad();
	volatile int tempTime = time;
	wind_up();
	 wind_down();
	running = 1;
	
	while(1); //DEAD_LOOP
}
 