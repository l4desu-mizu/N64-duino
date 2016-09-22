#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>

#define SIGNAL_PIN (1<<PB0)
//#define current_my_time (TCNT0)

uint32_t MicroSecClock = 0;
static uint8_t count = 0;			// interrupt counter

ISR(PCINT0_vect){

}

ISR(TIMER0_OVF_vect){

	if( (++count & 0x01) == 0 ){		// bump the interrupt counter
		++MicroSecClock;				// & count uSec every other time.
		PORTB=(PORTB&SIGNAL_PIN)? PORTB&~SIGNAL_PIN : PORTB|SIGNAL_PIN ;
	}

	TCNT0 = 253;                    // reset counter
}

void init(){
	//enable input interrupt
	PCICR|=(1<<PCIE0); //enable inputinterrupt for PCINT[7:0] (Port B)

	//interrupt enable
	sei();
}

void initTimer(){
	TCCR0A=0x00;//TCCR0A&=~(1<<COM0A1)&~(1<<COM0A0)&~(1<<COM0B1)&~(1<<COM0B0)&~(1<<WGM01)&~(1<<WGM00); //normal operation no OCA or OCB
	TIMSK0|= (1<<TOIE0); //enable timer overflow interrupt
	TCCR0B&=~(1<<FOC0A)&~(1<<FOC0B)&~(1<<WGM02)&~(1<<CS02)&~(1<<CS01);
	TCNT0 = 253;                    // set counter to inital value 253 out of 255 (2 clock cycles)
	TCCR0B|=(1<<CS01); //prescale by 8 (should give us .5µs)
}

void configOutput(){
	DDRB|=SIGNAL_PIN;   //output
	PORTB&=~SIGNAL_PIN; //zero
	PCMSK0&=~SIGNAL_PIN; //disable pinchange interrupt ?needed
}

void configInput(){
	DDRB&=~SIGNAL_PIN; //input
	PORTB|=SIGNAL_PIN; //pullup
	PCMSK0|=SIGNAL_PIN; //enable pinchange interrupt
}

void pollsignal(){
}

int main(){
	init();
	configOutput();
	initTimer();
	while(1){
	}
	return 0;
}
