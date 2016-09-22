#include <avr/io.h>
#include <avr/interrupt.h>

#define SIGNAL_PIN (1<<PB0)

ISR(PCINT0_vect){

}

void init(){
	//interrupt enable
	//enable input interrupt
	PCICR|=(1<<PCIE0); //enable inputinterrupt for PCINT[7:0] (Port B)
}

void initTimer(){
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
	return 0;
}
