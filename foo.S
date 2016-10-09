#include "avr/io.h"
#define SIGNAL_PIN ((1<<PB5)|(1<<PB0))
RTMP  = 16
RA1   = 17
RA2   = 18

#org 0x30
#cseg
.section .text
.global main
main:
	rcall init

init:
    ldi RTMP,lo8(RAMEND)
    out _SFR_IO_ADDR(SPL),RTMP
    ldi RTMP, hi8(RAMEND)
    out _SFR_IO_ADDR(SPH),RTMP
	ldi RTMP, SIGNAL_PIN
	out _SFR_IO_ADDR(DDRB), RTMP
	out _SFR_IO_ADDR(PORTB), RTMP
	ret
