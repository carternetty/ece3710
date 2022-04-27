;******************************************************************************
; @file    main.s
; @author  Carter Nettesheim and Daniel Mortenson
; @date    September-30-2021
; @note	   This code is for designing an LED counter to constantly count up
;		   When certain buttons are pressed it will turn on and off the counter
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
	
	; Enable the clock to GPIO Port B	
	LDR r0, =RCC_BASE
	LDR r1, [r0, #RCC_AHB2ENR]
	ORR r1, r1, #RCC_AHB2ENR_GPIOBEN
	STR r1, [r0, #RCC_AHB2ENR]
	
	; MODE: 00: Input mode, 01: General purpose output mode
    ;       10: Alternate function mode, 11: Analog mode (reset state)
	
	; Turn on the LED
	LDR r0, =GPIOA_BASE
	LDR r1, [r0, #GPIO_MODER]
	AND r1, r1, #0xFFFF03FF
	STR r1, [r0, #GPIO_MODER]
	
	LDR r1, [r0, #GPIO_PUPDR]
	AND r1, r1, #0xFFFF00FF
	ORR r1, r1, #0x00005400
	STR r1, [r0, #GPIO_PUPDR]
										
	LDR r2, =GPIOB_BASE
	LDR r3, [r2, #GPIO_MODER]
	AND r3, r3, #0xFFF0FFFF
	AND r3, r3, #0xFFFF00FF
	AND r3, r3, #0xFFFFFF00
	ORR r3, r3, #0x00000005
	ORR r3, r3, #0x00000550
	ORR r3, r3, #0x00055000
	STR r3, [r2, #GPIO_MODER]
	
	LDR r3, [r2, #GPIO_OTYPER]
	AND r3, r3, #0xFC00
	ORR r3, r3, #0x00FF
	ORR r3, r3, #0x0300
	STR r3, [r2, #GPIO_OTYPER]
	
	LDR r3, [r2, #GPIO_ODR]
	
reset
	MOV r3, #0xFFFFFFFF
	STR r3, [r2, #GPIO_ODR]
	
paused
	LDR r1, [r0, #GPIO_IDR]
	MOV r4, r1
	AND r4, r4, #0xFFFFFF0F
	ORR r4, r4, #0x00000060
	CMP r1, r4
	BEQ counting
	MOV r4, r1
	AND r4, r4, #0xFFFFFF0F
	ORR r4, r4, #0x000000C0
	CMP r1, r4
	BEQ reset
	B paused
	
counting
	SUB r3, r3, #0x1
	STR r3, [r2, #GPIO_ODR]
	
	MOV r4, #0x0
	MOV r5, #0x3C000
	;MOV r5, #0x1
	
wait1
	ADD r4, r4, #0x1
	CMP r4, r5
	BNE wait1
	
	LDR r1, [r0, #GPIO_IDR]
	MOV r4, r1
	AND r4, r4, #0xFFFFFF0F
	ORR r4, r4, #0x000000A0
	CMP r1, r4
	BEQ paused
	MOV r4, r1
	AND r4, r4, #0xFFFFFF0F
	ORR r4, r4, #0x000000C0
	CMP r1, r4
	BEQ reset
	B counting
	
stop B stop

	ENDP
					
	ALIGN			

	END