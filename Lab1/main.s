;******************** (C) Yifeng ZHU *******************************************
; @file    main.s
; @author  Yifeng Zhu
; @date    May-17-2015
; @note
;           This code is for the book "Embedded Systems with ARM Cortex-M 
;           Microcontrollers in Assembly Language and C, Yifeng Zhu, 
;           ISBN-13: 978-0982692639, ISBN-10: 0982692633
; @attension
;           This code is provided for education purpose. The author shall not be 
;           held liable for any direct, indirect or consequential damages, for any 
;           reason whatever. More information can be found from book website: 
;           http:;www.eece.maine.edu/~zhu/book
;*******************************************************************************


	INCLUDE core_cm4_constants.s
	INCLUDE stm32l476xx_constants.s
	
	AREA    main, CODE, READONLY
	EXPORT	__main
	ENTRY			
				
__main	PROC
		
    ; Enable the clock to GPIO Port A	
	LDR r0, =RCC_BASE
	LDR r1, [r0, #RCC_AHB2ENR]
	ORR r1, r1, #RCC_AHB2ENR_GPIOAEN
	STR r1, [r0, #RCC_AHB2ENR]
	
	; Enable the clock to GPIO Port C	
	LDR r0, =RCC_BASE
	LDR r1, [r0, #RCC_AHB2ENR]
	ORR r1, r1, #RCC_AHB2ENR_GPIOCEN
	STR r1, [r0, #RCC_AHB2ENR]
	
	; MODE: 00: Input mode, 01: General purpose output mode
    ;       10: Alternate function mode, 11: Analog mode (reset state)
	
	
	
	; Turn on the LED
	LDR r0, =GPIOA_BASE
	LDR r1, [r0, #GPIO_MODER]
	AND r1, r1, #0xFFFFF3FF
	ORR r1, r1, #0x00000400
	STR r1, [r0, #GPIO_MODER]
	
										;initialize GIPO ports;
	LDR r2, =GPIOC_BASE									;load values
	LDR r3, [r2, #GPIO_MODER]
	AND r3, r3, #0xF3FFFFFF				;change values
	STR r3, [r2, #GPIO_MODER]			;store values
	
state1								;state1	
	LDR r1, [r0, #GPIO_ODR]				;load gpio of light
	AND r1, r1, #0xFFFFFFDF				;change pin 21 to 0
	STR r1, [r0, #GPIO_ODR]
	
	LDR r3, [r2, #GPIO_IDR]
	MOV r4, #0x0
	CMP r4, r3
	BNE state1
	
	MOV r4, #0x0
	MOV r5, #0xFFFF
	ADD r5, r5, #0x2F
	
wait1
	ADD r4, r4, #0x1
	CMP r4, r5
	BNE wait1
	
state2
	ORR r1, r1, #0x00000020
	STR r1, [r0, #GPIO_ODR]
	LDR r3, [r2, #GPIO_IDR]
	MOV r4, #0x0
	CMP r4, r3
	BNE state2
	
	MOV r4, #0x0
	MOV r5, #0xFFFF
	ADD r5, r5, #0x2F
	
wait2
	ADD r4, r4, #0x1
	CMP r4, r5
	BNE wait2
	
state3
	ORR r1, r1, #0x00000020
	STR r1, [r0, #GPIO_ODR]
	AND r1, r1, #0xFFFFFFDF
	STR r1, [r0, #GPIO_ODR]
	
	LDR r3, [r2, #GPIO_IDR]
	MOV r4, #0x0
	CMP r4, r3
	BNE state3
	
	MOV r4, #0x0
	MOV r5, #0xFFFF
	ADD r5, r5, #0x2F
	
wait3
	ADD r4, r4, #0x1
	CMP r4, r5
	BNE wait3
	
	B state1
	  
stop 	B 		stop

	ENDP
					
	ALIGN			

	END