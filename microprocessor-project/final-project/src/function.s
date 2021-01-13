	.syntax unified
	.cpu cortex-m4
	.thumb

.text
	.global max7219_init
	.global MAX7219Send

	.equ RCC_AHB2ENR,   0x4002104C
    .equ GPIOC_MODER,   0x48000800
    .equ GPIOC_OTYPER,  0x48000804
    .equ GPIOC_OSPEEDR, 0x48000808
    .equ GPIOC_PUPDR,   0x4800080C
    .equ GPIOC_IDR,     0x48000810

    .equ GPIOC_BASE,    0x48000800
    .equ GPIO_BSRR_OFFSET,   0x18
    .equ GPIO_BRR_OFFSET,    0x28

    .equ DECODE_MODE,   0x9
    .equ INTENSITY,     0xA
    .equ SCAN_LIMIT,    0xB
    .equ SHUTDOWN,      0xC
    .equ DISPLAY_TEST,  0xF
    .equ DIGIT_0,       0x1

    .equ DATA,          0x1     // PC0
    .equ LOAD,          0x2     // PC1
    .equ CLOCK,         0x4     // PC2

MAX7219Send:
	push {R4-R11, LR}
	//input parameter: r0 is ADDRESS , r1 is DATA
	//TODO: Use this function to send a message to max7219
	LSL R0, R0, #8
	ADD R0, R0, R1
	LDR R1, =GPIOC_BASE
	LDR R2, =LOAD
	LDR R3, =DATA
	LDR R4, =CLOCK
	LDR R5, =GPIO_BSRR_OFFSET
	LDR R6, =GPIO_BRR_OFFSET
	MOV R7, #16					// r7 = i
MAX7219Send_loop:
    MOV R8, #1
    SUB R9, R7, #1
    LSL R8, R8, R9          // r8 = mask
    STR R4, [R1, R6]        // HAL_GPIO_WritePin ( GPIOA, CLOCK, 0 );
    TST R0, R8
    BEQ bit_not_set         // bit not set
    STR R3, [R1, R5]
    B if_done
bit_not_set:
    STR R3, [R1, R6]
if_done:
    STR R4, [R1, R5]
    SUBS R7, R7, #1
    BGT MAX7219Send_loop
    STR R2, [R1, R6]
    STR R2, [R1, R5]
    //BX LR
    pop {R4-R11, PC}

max7219_init:
	push {R4-R11, LR}
	//TODO: Initialize max7219 registers
	//PUSH {LR}
	MOV R1, R0
	LDR R0, =DECODE_MODE
	//LDR R0, =DECODE_MODE
	//LDR R1, =#0xFF			// Decode B for digits 7-0
	BL MAX7219Send

	LDR R0, =DISPLAY_TEST
	LDR R1, =#0x00			// Normal operation
	BL MAX7219Send

	LDR R0, =SCAN_LIMIT
	LDR R1, =#0x07			// Display digit 7-0
	BL MAX7219Send

	LDR R0, =INTENSITY
	LDR R1, =#0x0A			// Set brightness
	BL MAX7219Send

	LDR R0, =SHUTDOWN
	LDR R1, =#0x01			// Normal operation
	BL MAX7219Send
	//POP {PC}
	//BX LR
	pop {R4-R11, PC}

