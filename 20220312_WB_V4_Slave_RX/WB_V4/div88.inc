;************************************************************************
;*                                                                      *
;*                      unsigned division 24 / 16 bit                   *
;*              Author: Peter Dannegger                                 *
;*                                                                      *
;************************************************************************
;
//.def		DDRT0		=			// DIVIDENDO E RESULTADO
//.def		DDRT1		=			// DIVIDENDO E RESULTADO
//.def		DDRT2		=			// DIVIDENDO E RESULTADO	
//.def		DVRS0		=			// DIVISOR E RESTO
//.def		DVRS1		=			// DIVISOR E RESTO
//.def		TMP			=			// TEMPORARIO
//.def		TMP2		=			// TEMPORARIO
//.def		TMP3		=			// TEMPORARIO
/*
udiv24_16:
	clr     TMP
	clr     TMP2
	ldi     TMP3, 24
_udi1:
	lsl     DDRT0
	rol     DDRT1
	rol     DDRT2
	rol     TMP
	rol     TMP2
	brcs    _udi2           ; handle carry (e.g. 0x10000 / 0xFFFF) !
	cp      TMP, DVRS0
	cpc     TMP2, DVRS1
	brcs    _udi3
_udi2:
	sub     TMP, DVRS0
	sbc     TMP2, DVRS1
	inc     DDRT0
_udi3:
	dec     TMP3
	brne    _udi1
	mov     DVRS0, TMP
	mov     DVRS1, TMP2
	ret
*/

;***************************************************************************
;*
;* "mpy16u" - 16x16 Bit Unsigned Multiplication
;*
;* This subroutine multiplies the two 16-bit register variables 
;* mp16uH:mp16uL and mc16uH:mc16uL.
;* The result is placed in m16u3:m16u2:m16u1:m16u0.
;*  
;* Number of words	:14 + return
;* Number of cycles	:153 + return
;* Low registers used	:None
;* High registers used  :7 (mp16uL,mp16uH,mc16uL/m16u0,mc16uH/m16u1,m16u2,
;*                          m16u3,mcnt16u)	
;*
;***************************************************************************

;***** Subroutine Register Variables

//.def	mc16uL	=r16		;multiplicand low byte
//.def	mc16uH	=r17		;multiplicand high byte
//.def	mp16uL	=r18		;multiplier low byte
//.def	mp16uH	=r19		;multiplier high byte
//.def	m16u0	=r18		;result byte 0 (LSB)
//.def	m16u1	=r19		;result byte 1
//.def	m16u2	=r20		;result byte 2
//.def	m16u3	=r21		;result byte 3 (MSB)
//.def	mcnt16u	=r22		;loop counter

;***** Code
/*
mpy16u:	
	clr		m16u3			;clear 2 highest bytes of result
	clr		m16u2
	ldi		mcnt16u,16		;init loop counter
	lsr		mp16uH
	ror		mp16uL

m16u_1:	
	brcc	noad8			;if bit 0 of multiplier set
	add		m16u2,mc16uL	;add multiplicand Low to byte 2 of res
	adc		m16u3,mc16uH	;add multiplicand high to byte 3 of res
noad8:	
	ror		m16u3			;shift right result byte 3
	ror		m16u2			;rotate right result byte 2
	ror		m16u1			;rotate result byte 1 and multiplier High
	ror		m16u0			;rotate result byte 0 and multiplier Low
	dec		mcnt16u			;decrement loop counter
	brne	m16u_1			;if not done, loop more
	ret

	*/

;************************************************************************
;*                                                                      *
;*              unsigned division 32 / 16 bit                           *
;*              based on Peter Dannegger's code                         *
;*                                                                      *
;************************************************************************
;
//.def		DDRT0		=			// DIVIDENDO E RESULTADO
//.def		DDRT1		=			// DIVIDENDO E RESULTADO
//.def		DDRT2		=			// DIVIDENDO E RESULTADO
//.def		DDRT3		=			// DIVIDENDO E RESULTADO	
//.def		DVRS0		=			// DIVISOR E RESTO
//.def		DVRS1		=			// DIVISOR E RESTO
//.def		TMP			=			// TEMPORARIO
//.def		TMP2		=			// TEMPORARIO
//.def		TMP3		=			// TEMPORARIO
//.def		TMP4		=			// TEMPORARIO

udiv32_16:
	clr     TMP
	clr     TMP2
	CLR		TMP3
	ldi     TMP4, 32
_udi1:
	lsl     DDRT0
	rol     DDRT1
	rol     DDRT2
	ROL		DDRT3
	rol     TMP
	rol     TMP2
	brcs    _udi2           ; handle carry (e.g. 0x10000 / 0xFFFF) !      PULA PARA _udi2 QUANDO ACONTECER OVERFLOW EM TMP2
	cp      TMP, DVRS0
	cpc     TMP2, DVRS1	
	brcs    _udi3			
_udi2:
	sub     TMP, DVRS0		// DECREMENTA DIVIDENDO EM MULTIPLOS DE DIVISOR, INCREMENTA RESULTADO
	sbc     TMP2, DVRS1
	inc     DDRT0
_udi3:	
	dec     TMP4			// DECREMENTA INDICADOR DE BIT
	brne    _udi1
	mov     DVRS0, TMP		// CARREGA RESTO EM DVSR0
	mov     DVRS1, TMP2
	ret