#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>

#define SIGNAL_PIN (1<<PB0)
#define TIMERSTART 253
#define CONTROLLERBITS 32

struct state{
	char current :1;
	char last : 1;
};

uint32_t MicroSecClock = 0;
static uint8_t count = 0;			// interrupt counter
uint32_t currentTime;
struct state bitState;
uint32_t controllerValue;
uint8_t controllerBitsToRead;

ISR(PCINT0_vect){
	bitState.last=bitState.current;
	bitState.current=PORTB&SIGNAL_PIN;

	uint32_t interval=MicroSecClock-currentTime;
	if(~bitState.last){//"rising edge"
		if(interval>1){ //3µs
			controllerValue&=~(1<<controllerBitsToRead--);
		}else{
			controllerValue|=(1<<controllerBitsToRead--);
		}
	}

	currentTime=MicroSecClock;
}

ISR(TIMER0_OVF_vect){

	if( (++count & 0x01) == 0 ){		// bump the interrupt counter
		++MicroSecClock;				// & count uSec every other time.
		PORTB=(PORTB&SIGNAL_PIN)? PORTB&~SIGNAL_PIN : PORTB|SIGNAL_PIN; //test output. works btw
	}

	TCNT0 = TIMERSTART;			// reset counter
}

//{{{configfoo
void init(){
	//enable input interrupt
	PCICR|=(1<<PCIE0); //enable input-interrupt for PCINT[7:0] (Port B)

	//general interrupt enable
	sei();
}

void initTimer(){
	TCCR0B=0x00;			//disable Timer (no prescaler etc)
	TCCR0A=0x00;			//normal operation no OCA or OCB
	TIMSK0|= (1<<TOIE0);	//enable timer overflow interrupt
	TCNT0 = TIMERSTART;			//set counter to inital value 253 out of 255 (2 clock cycles)
	TCCR0B|=(1<<CS01);		//enable timer with prescale 8 (should give us .5µs)
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
//}}}endconfigfoo

void sendPollSignal(){
	configOutput();

}

uint32_t poll(){
	sendPollSignal();
	configInput();
	currentTime=MicroSecClock;
	controllerBitsToRead=CONTROLLERBITS;
	while(controllerBitsToRead>0){};//wait until all bits are read
	return controllerValue;
}

int main(){
	init();
	configOutput();
	initTimer();
	bitState.current=0;
	while(1){
	}
	return 0;
}
