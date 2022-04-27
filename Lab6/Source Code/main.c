#include "stm32l476xx.h"
#include "LCD.h"

//global variables
volatile int running;
volatile int time[5];

//function prototypes
void SysTick_Initialize(uint32_t ticks);
void SysTick_Handler(void);
void initialize_other_interrupts(void);
void EXTI0_IRQHandler(void); //yellow button
void EXTI1_IRQHandler(void); //red button
void EXTI2_IRQHandler(void); //green button

void SysTick_Initialize(uint32_t ticks){
	SysTick->CTRL = 0;					//Disable Systick
	SysTick->LOAD = ticks - 1; //Set reload register
	//Set priority of Systick to least urgency/highest priority
	NVIC_SetPriority (SysTick_IRQn, (1<<__NVIC_PRIO_BITS) - 1);
	SysTick->VAL = 0; //reset the Systick counter value
	SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk; //select processor clock
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk; //enable systick interupt
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk; // enable systick
	LCD_DisplayStopwatch(time);
}

void SysTick_Handler(){
	if(running){
		time[0]++;
		if(time[0] > 9){ // tenths of seconds limit
			time[1]++;
			time[0] = 0;
		}
		if(time[1] > 9){ // seconds limit
			time[2]++;
			time[1] = 0;
			
			LCD_DisplayString(1, "You're Dumb");
		}
		if(time[2] > 5){  // tens of seconds limit
			time[3]++;      
			time[2] = 0;
		}
		if(time[3] > 9){ // minutes limit
			time[4]++;
			time[3] = 0;
		}
		if(time[4] > 5){  // tens of minutes limit      
			time[4] = 0;
		}
		LCD_DisplayStopwatch(time);
	}
}

void initialize_other_interrupts(){
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;									//Initialize GPIOC clock
	GPIOC->MODER &= ~(GPIO_MODER_MODE0 | GPIO_MODER_MODE1 | GPIO_MODER_MODE2 | GPIO_MODER_MODE3);// enable input mode
	
	GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPD0 | GPIO_PUPDR_PUPD1 | GPIO_PUPDR_PUPD2);
	GPIOC->PUPDR |= 0xFFFFFFEA;
	
	//enable exti0 1 and 2 as interrupts
	NVIC_EnableIRQ(EXTI0_IRQn);
	NVIC_EnableIRQ(EXTI1_IRQn);
	NVIC_EnableIRQ(EXTI2_IRQn);
	
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	
	//set up gpioc 0,1 and 2 as interrupt pins
	SYSCFG->EXTICR[0] &= ~(SYSCFG_EXTICR1_EXTI0 | SYSCFG_EXTICR1_EXTI1 | SYSCFG_EXTICR1_EXTI2); 
	SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PC | SYSCFG_EXTICR1_EXTI1_PC | SYSCFG_EXTICR1_EXTI2_PC;
	
	EXTI->IMR1 |= EXTI_IMR1_IM0;
	EXTI->IMR1 |= EXTI_IMR1_IM1;
	EXTI->IMR1 |= EXTI_IMR1_IM2;
	
	EXTI->RTSR1 |= EXTI_RTSR1_RT0;
	EXTI->RTSR1 |= EXTI_RTSR1_RT1;
	EXTI->RTSR1 |= EXTI_RTSR1_RT2;
}

void EXTI0_IRQHandler(){ //yellow button
	running = 0;
	EXTI->PR1 |= EXTI_PR1_PIF0;
}

void EXTI1_IRQHandler(){ //red button
	running = 0;
	for (int i = 0; i< 5; i++){ //reset time to 0
		time[i] = 0;
	}
	LCD_Clear();
 
	LCD_DisplayStopwatch(time);
	EXTI->PR1 |= EXTI_PR1_PIF1;
}

void EXTI2_IRQHandler(){ //green button
	running = 1;
	EXTI->PR1 |= EXTI_PR1_PIF2;
}


 int main(void){
	running = 0; //start with stopwatch paused
	for (int i = 0; i< 5; i++){ //start with stopwatch at 0
		time[i] = 0;
	}
	
	LCD_Init();
	delay_ms(20);
	LCD_Clear();
	LCD_WriteData('D');
	
	initialize_other_interrupts();
	SysTick_Initialize(400000);
	
	
	
	
	
	while(1); //DEAD_LOOP
}