#include <avr/io.h>
#define SIGNAL_PIN (1<<PB0)
RTMP  = 16
RA1   = 17
RA2   = 18

#org 0x30
#cseg
.section .text
.global main
main:
	jmp init

go:
	ldi RTMP,(1<<PB5)|SIGNAL_PIN
	out _SFR_IO_ADDR(PORTB),RTMP
	mainloop:
		call poll
		call wait_1000cycles
	jmp mainloop

foo:
	jmp foo

init:
    ldi RTMP,lo8(RAMEND)
    out _SFR_IO_ADDR(SPL),RTMP
    ldi RTMP, hi8(RAMEND)
    out _SFR_IO_ADDR(SPH),RTMP
	ldi RTMP, ((1<<PB5)|(1<<PB0))
	out _SFR_IO_ADDR(DDRB), RTMP
	jmp go



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



poll:				; Send 0b00000011 on signal pin
    ldi RA1, 0x06	; number of zeros
    ldi RA2, 0x02	; number of ones
    zero:
		ldi RTMP, 0
		out _SFR_IO_ADDR(PORTB), RTMP		 ; set pin to zero
		ldi RTMP, 0x03		 ; wait for 15 cycles / 3µs
		zeroloop:
			dec RTMP
			brne zeroloop    ; 1 / 2 
		nop
		nop
		ldi RTMP, SIGNAL_PIN
		out _SFR_IO_ADDR(PORTB), RTMP		; set pin to one after 15 cycles
		dec RA1				; don't nop because setting the next bit
		brne zero			; and branching takes 4-5 cycles 
    one:
		ldi RTMP, 0
		out _SFR_IO_ADDR(PORTB), RTMP
		nop
		nop
		nop
		ldi RTMP, SIGNAL_PIN
		out _SFR_IO_ADDR(PORTB), RTMP	; 5 cycles aka 1µs low
		ldi RTMP, 0x03
		oneloop:
			dec RTMP
			brne oneloop
		dec RA2				; don't nope because we need time to
		brne one			; to jump etc.
	ret
