#include <avr/io.h>
;#define SIGNAL_PIN (1<<PB5)|(1<<PB0) ;the led will dim a bit but who cares we loose less cycles
#define SIGNAL_PIN 0
RTMP  = 16
RA1   = 17
RA2   = 18

#org 0x30
#cseg
.section .text
.global main
main:
    ldi RTMP,lo8(RAMEND)
    out _SFR_IO_ADDR(SPL),RTMP
    ldi RTMP, hi8(RAMEND)
    out _SFR_IO_ADDR(SPH),RTMP
	sbi _SFR_IO_ADDR(DDRB),5
	sbi _SFR_IO_ADDR(PORTB),5
	jmp go

go:
	mainloop:
		call poll
		call wait_1000cycles
		call wait_1000cycles
		call wait_1000cycles
	jmp mainloop

wait_1000cycles:
	ldi RA1, 0xff ; 166 loops
	wait_loop1:
		dec RA1
		brne wait_loop1
	ldi RA1, 0xff ; 166 loops
	wait_loop2:
		dec RA1
		brne wait_loop2
	ret

poll:				; Send 0b000000011 (8 bits) on signal pin
	;config output
	sbi _SFR_IO_ADDR(PORTB),SIGNAL_PIN
	sbi _SFR_IO_ADDR(DDRB),SIGNAL_PIN
	ldi RA1, 0x07	; number of zeros
	ldi RA2, 0x02	; number of ones
	zero:
		;3µs 0
		cbi _SFR_IO_ADDR(PORTB), SIGNAL_PIN			; set pin to zero
		ldi RTMP, 15								; 48 cycles for 3µs 15*3cycles = 45 cycles (1 instruction ~ 1cycle)
		zerozeroloop:								; each loop takes 3 cycles unless we dont branch
			dec RTMP
			brne zerozeroloop						; 1 cycles for non branch / 2 cycles for branching
		nop											; get the last cycles done and end with 46 cycles

		;1µs 1
		sbi _SFR_IO_ADDR(PORTB), SIGNAL_PIN			; set pin to one
		ldi RTMP, 3									; 16 cycles for 1µs so we start wasting 9 of em (we want to be faster due to inaccurate clocks or something)
		zerooneloop:
			dec RTMP
			brne zerooneloop
		nop
		nop
		dec RA1
		brne zero
	one:
		;1µs 0
		cbi _SFR_IO_ADDR(PORTB), SIGNAL_PIN			; set pin to zero
		ldi RTMP, 4									; waste 12 cycles as 16 are 1µs
		onezeroloop:
			dec RTMP
			brne onezeroloop
		nop
		nop											; end zero with 14 cycles

		;3µs 1
		sbi _SFR_IO_ADDR(PORTB), SIGNAL_PIN			; set pin to one
		ldi RTMP, 14								; again waste 42 cycles
		oneoneloop:
			dec RTMP
			brne oneoneloop							; 1 cycles for non branch / 2 cycles for branching
		nop
		dec RA2
		brne one
	nop												; same amount of cycles for branching and nonbranching
	cbi _SFR_IO_ADDR(PORTB), SIGNAL_PIN				; set input
	cbi _SFR_IO_ADDR(DDRB),SIGNAL_PIN
	ret
