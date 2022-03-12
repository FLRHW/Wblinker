
/*	**********************************************************************************************
	FELIPE LOPASSO RODRIGUES
	********************************************************************************************** */






// -----------------------------------------------------------------------------------------------
//		LACO PRINCIPAL

MAIN_LOOP:

MAIN_LOOP2:
	RCALL	UPDATE_DOUT

	SBRC	FL,MASTER					// CHECKS IF BOARD OPERATES IN SLAVE MOVE, IF SO, SKIPS FREQUENCY UPDATE
	RCALL	FREQ_AD

	RCALL	SHOWS_DATA					// PRINTS DATA SENT AND RECEIVED
//	RCALL	BLINK

	RJMP	MAIN_LOOP2

// -----------------------------------------------------------------------------------------------
//		

FREQ_AD:
RET
	BST		SW_DATA,1
	BRTS	INC_DEL
	BST		SW_DATA,2
	BRTS	DEC_DEL
	RJMP	END_F_ADJ
INC_DEL:
	LDS		TMP2,ICR1L					// LOADS CURRENT TOP VALUE IN ICR1
	LDS		TMP,ICR1H
	CLR		TMP3
	LDI		TMP4,1
	SUB		TMP2,TMP4
	SBC		TMP,TMP3

	RJMP	F_UPD

DEC_DEL:
	LDS		TMP2,ICR1L					// LOADS CURRENT TOP VALUE IN ICR1
	LDS		TMP,ICR1H
	CLR		TMP3
	LDI		TMP4,1
	ADD		TMP2,TMP4
	ADC		TMP,TMP3
//RJMP	F_UPD

F_UPD:
// UPDATES TIMER 1
	STS		ICR1H,TMP					// (HIGH BYTE MUST BE WRITEN FIRST; READING MUST BE FIRST ON LOW BYTE)
	STS		ICR1L,TMP2
	LSR		TMP							// DIVIDES VALUE BY 2 FOR DUTY = 50 %
	ROR		TMP2
	STS		OCR1AH,TMP					// LOADS TOP/2 (DUTY) IN OCR1A AND OCR1B
	STS		OCR1AL,TMP2
	STS		OCR1BH,TMP			
	STS		OCR1BL,TMP2	
// UPDATES DISPLAY
	RCALL	FREQ_CALC					// CALCULATES CARRIER FREQUENCY BASED ON CURRENT ICR1(TOP) VALUE AND STORES IN SRAM (FREQ_H:FREQ_M:FREQ_L)
	MOV		TMP3,FREQ_H					// FREQ_PWM = FREQHZ/(1+TOP)
	MOV		TMP4,FREQ_M
	MOV		TMP5,FREQ_L
	RCALL	SHOWS_FREQ					// PRINTS FREQUENCY
END_F_ADJ:
	RET	


// -----------------------------------------------------------------------------------------------
//		UPDATES VALUE TO BE TRASMITTED

UPDATE_DOUT:
	MOV		DATA_OUT,SW_DATA
//	SBR		FL,(1<<TX_RDY)
	RET
	
// -----------------------------------------------------------------------------------------------
//		LED BLINKER FOR DEBUG PURPOSE ~250 mS

BLINK:
	SBI		PORTB,LED3
	LDI		TMP,3
	RCALL	DELAY_MS
	CBI		PORTB,LED3
	LDI		TMP,27
	RCALL	DELAY_MS
	RET

// --------------------------------
MODE_DISP:
	POS_DEF	2, 0
	P_CHAR	'F'
	P_CHAR	'0'
	P_CHAR	':'
	P_CHAR	' '
	MOV		TMP5,FL
	CLR		TMP							// DOES NOT PRINT DECIMAL SEPARATOR
	CLR		TMP4						// CLEARS REGISTER AS VALUE IS NOT HIGHER THAN 255
	RCALL	IMP_SEQ3					// PRINTS DATA
	RET



// -----------------------------------------------------------------------------------------------
//		CALCULATES CARRIER FREQUENCY TO DISPLAY

FREQ_CALC:
	LDI		TMP,LOW(FREQHZ)				// LOADS DIVIDEND = FREQHZ
	MOV		DDRT0,TMP
	LDI		TMP,BYTE2(FREQHZ)
	MOV		DDRT1,TMP
	LDI		TMP,BYTE3(FREQHZ)
	MOV		DDRT2,TMP
	LDI		TMP,BYTE4(FREQHZ)
	MOV		DDRT3,TMP
	CLR		DVRS0						// LOADS DIVISOR (DIVISOR = 1 + TOP; FIRST STEP: LOADS 1 @ LOW BYTE, 0 @ HIGH BYTE)
	CLR		DVRS1	
	INC		DVRS0
	LDS		TMP2,ICR1L					// LOADS CURRENT TOP VALUE IN TMP2:TMP
	LDS		TMP,ICR1H
	ADD		DVRS0,TMP2					// SUMS TOP TO VALUE STORED IN DIVISOR (DIVISOR = 1 + TOP)
	ADC		DVRS1,TMP
	RCALL	udiv32_16					// DIVIDES FREQHZ / (1 + TOP), STORES RESULT IN DDRT0, DDRT1, DDRT2
	MOV		FREQ_L,DDRT0				// STORES RESULTS IN SRAM
	MOV		FREQ_M,DDRT1
	MOV		FREQ_H,DDRT2
	RET

