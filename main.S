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
		call poll
	mainloop:
		;call wait_1000cycles
	jmp mainloop

wait_1000cycles:
	ldi RA1, 0xa6 ; 166 loops
	wait_loop1:
		dec RA1
		brne wait_loop1
	ldi RA1, 0xa6 ; 166 loops
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
		cbi _SFR_IO_ADDR(PORTB), SIGNAL_PIN		 ; set pin to zero
		ldi RTMP, 14
		zerozeroloop:
			dec RTMP
			brne zerozeroloop    ; 1 / 2
		nop
		nop
		nop

		;1µs 1
		sbi _SFR_IO_ADDR(PORTB), SIGNAL_PIN
		ldi RTMP, 3
		zerooneloop:
			dec RTMP
			brne zerooneloop
		nop
		dec RA1
		brne zero
	one:
		;1µs 0
		cbi _SFR_IO_ADDR(PORTB), SIGNAL_PIN
		ldi RTMP, 4
		onezeroloop:
			dec RTMP
			brne onezeroloop
		nop
		nop

		;3µs 1
		;ldi RTMP, SIGNAL_PIN
		sbi _SFR_IO_ADDR(PORTB), SIGNAL_PIN
		ldi RTMP, 14
		oneoneloop:
			dec RTMP
			brne oneoneloop    ; 1 / 2 
		dec RA2
		brne one
	nop										;same amount of cycles
	cbi _SFR_IO_ADDR(PORTB), SIGNAL_PIN
	cbi _SFR_IO_ADDR(DDRB),SIGNAL_PIN
	ret
