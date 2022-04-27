#include "stm32l476xx.h"
#include <stdlib.h>
#include <stdio.h>

int main(void);
void initialize(void);
void usart_read(USART_TypeDef *USARTx, uint8_t *buffer, uint32_t nBytes);
void usart_write(USART_TypeDef *USARTx, uint8_t *buffer, uint32_t nBytes);
float readTemp(void);
void ADC_Wakeup(void);

void initialize(void) {
	//enable gpioa pins to be used as usart
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	
	GPIOA->MODER &= ~(0xF << (2 * 2));
	GPIOA->MODER |= 0xA << (2 * 2);
	
	GPIOA->AFR[0] |= 0x77 << (4*2);
	
	GPIOA->OSPEEDR |= 0xF << (2*2);
	
	GPIOA->PUPDR &= ~(0xF << (2 * 2));
	GPIOA->PUPDR |= 0x5 << (2 * 2);
	
	GPIOA->OTYPER &= ~(0x3 << 2);
	
	//usart clock
	RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;
	
	RCC->CCIPR &= ~(RCC_CCIPR_USART1SEL | RCC_CCIPR_UART4SEL);
	RCC->CCIPR |= (RCC_CCIPR_USART1SEL_0 | RCC_CCIPR_UART4SEL_0);
	
	//enable usart
	USART2->CR1 &= ~USART_CR1_UE;
	
	USART2->CR1 &= ~USART_CR1_M;
	
	USART2->CR2 &= ~USART_CR2_STOP;
	
	USART2->CR1 &= ~USART_CR1_PCE;
	
	USART2->CR1 &= ~USART_CR1_OVER8;

	USART2->BRR = 0x1A0;		//Baud Rate
	
	USART2->CR1 |= (USART_CR1_TE | USART_CR1_RE);
	
	USART2->CR1 |= USART_CR1_UE;
	while((USART2->ISR & USART_ISR_TEACK) == 0);
	while((USART2->ISR & USART_ISR_REACK) == 0);
	
	//adc initialize
	RCC->CR |= RCC_CR_HSION;
	while(!(RCC->CR & RCC_CR_HSIRDY));
	
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	
	GPIOB->MODER &= 0xFFFFFFF3;
	GPIOB->MODER |= 0x4;
	
	GPIOA->MODER |= 0x3;
	
	GPIOA->ASCR |= 0x1;
	
	RCC->AHB2ENR |= RCC_AHB2ENR_ADCEN;
	
	ADC1->CR &= ~(ADC_CR_ADEN);
	
	SYSCFG->CFGR1 |= SYSCFG_CFGR1_BOOSTEN;
	
	ADC123_COMMON->CCR |= ADC_CCR_VREFEN;
	
	ADC123_COMMON->CCR &= ~(ADC_CCR_PRESC);
	
	ADC123_COMMON->CCR |= (ADC_CCR_CKMODE_0);
	
	ADC123_COMMON->CCR &= ~(ADC_CCR_DUAL);
	
	ADC_Wakeup();
	
	ADC1->CFGR &= ~(ADC_CFGR_RES);
	
	ADC1->CFGR &= ~(ADC_CFGR_ALIGN);
	
	ADC1->SQR1 &= ~(ADC_SQR1_L);
	
	ADC1->SQR1 |= (17 << 6);	//specify channel number 6
	
	ADC1->SMPR2 |= ADC_SMPR2_SMP17;
	
	ADC1->CFGR &= ~(ADC_CFGR_CONT);
	
	ADC1->CFGR &= ~(ADC_CFGR_EXTEN);
	
	ADC1->CR |= ADC_CR_ADEN;
	while(!(ADC1->ISR & ADC_ISR_ADRDY));
	
	ADC123_COMMON->CCR |= 0x800000;
}

void usart_read(USART_TypeDef *USARTx, uint8_t *buffer, uint32_t nBytes) {
	for (uint32_t i = 0; i < nBytes; i++) {
		while (!(USARTx->ISR & USART_ISR_RXNE));
		buffer[i] = USARTx->RDR;
	}
}

void usart_write(USART_TypeDef *USARTx, uint8_t *buffer, uint32_t nBytes) {
	for (int i = 0; i < nBytes; i++) {
		while (!(USARTx->ISR & USART_ISR_TXE));
		USARTx->TDR = buffer[i] & 0xFF;
	}
	
	while (!(USARTx->ISR & USART_ISR_TC));
	
	USARTx->ICR |= USART_ICR_TCCF;
}

uint16_t ts_cal1;
uint16_t ts_cal2;
float readTemp(void) {
	  ts_cal1 = *((uint16_t*)0x1FFF75A8);
	  ts_cal2 = *((uint16_t*)0x1FFF75CA);
	
	ADC1->CR |= ADC_CR_ADSTART;
	while(!(ADC123_COMMON->CSR & ADC_CSR_EOC_MST));
	
	uint16_t tempNum = ADC1->DR;
	
	return (((80.0) / (float)(ts_cal2 - ts_cal1)) * (float)(tempNum - ts_cal1)) + 80.0;
}

void ADC_Wakeup() {
	volatile int wait_time;
	
	if ((ADC1->CR & ADC_CR_DEEPPWD) == ADC_CR_DEEPPWD) {
		ADC1->CR &= ~ADC_CR_DEEPPWD;
	}
	
	ADC1->CR |= ADC_CR_ADVREGEN;
	
	wait_time = 20 * (4000000 / 1000000);
	while (wait_time != 0) {
		wait_time--;
	}
}

int main(void) {
	initialize();
	uint8_t c;
	float tempNum;
	int count;
	char arr[20];
	
	while(1) {
		usart_read(USART2, &c, 1);
 		usart_write(USART2, &c, 1);
		
		if (c == 't' || c == 'T') {
			tempNum = readTemp();
			count = sprintf(arr, ": %f\n\r", (double)tempNum);
			usart_write(USART2, arr, (uint32_t)count);
		}
	}
		
	while(1);
}
