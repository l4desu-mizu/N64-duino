#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>

#define SIGNAL_PIN (1<<PB0)
#define TIMERSTART 255
#define CONTROLLERREADBITS 32
#define CONTROLLERSENDBITS 8

struct state{
	char current :1;
	char last : 1;
};

uint32_t MicroSecClock = 0;
static uint8_t count = 0;			// interrupt counter
uint32_t currentTime = 0;
struct state bitState;
uint32_t controllerValue;
int8_t controllerBitsToRead;
int8_t controllerBitsToSend;
const uint8_t pollSignal = 0b00000011;

ISR(PCINT0_vect){
	//TODO: uint8_t should suffice
	uint32_t interval=MicroSecClock-currentTime;

	bitState.last=bitState.current;
	bitState.current=PORTB&SIGNAL_PIN;

	//configInput|configOutput already handle the recieving/sending state by en-/disabling Pinchange-Interrupt, so no further check is needed
	if(~bitState.last){//"rising edge"
		if(interval>1){ //3µs (not quite sure about the timing and unit of MicroSecClock)
			controllerValue&=~(1<<controllerBitsToRead--);
		}else{
			controllerValue|=(1<<controllerBitsToRead--);
		}
	}

	currentTime=MicroSecClock;
}

ISR(TIMER0_OVF_vect){

	//TODO: guess the if isn't needed since the clock is timed at .5µs and counts 2 times to overflow
	//if( (++count & 0x01) == 0 ){		// bump the interrupt counter
	//	++MicroSecClock;				// & count uSec every other time.
	//}
	PORTB^=SIGNAL_PIN;
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
	controllerBitsToSend=CONTROLLERSENDBITS-1; //need this offset for shift op
	PORTB&=~SIGNAL_PIN;
	TCNT0 = TIMERSTART; //reset counter
	//TODO: check for racecondition on MicroSecClock, possibly use of interval is better
	MicroSecClock=0;
	while(controllerBitsToSend>=0){ //wait until all bits are send
		if(pollSignal>>controllerBitsToSend & 0x01){//send 0
			if(MicroSecClock>3){
				PORTB|=SIGNAL_PIN;
			}
		}else{//send 1
			if(MicroSecClock>1){
				PORTB|=SIGNAL_PIN;
			}
		}
		if(MicroSecClock>=4){
			PORTB&=~SIGNAL_PIN;
			controllerBitsToSend--;
			MicroSecClock=0;
		}
	}

}

uint32_t poll(){
	sendPollSignal();
	configInput();
	controllerBitsToRead=CONTROLLERREADBITS;//offset not relevant since we just want to recieve 32 bits
	while(controllerBitsToRead>0){//wait until all bits are read
		//timeout
		if(currentTime>130){ //poll should be 128µs
			return 0;
		}
	}
	return controllerValue;
}

int main(){
	init();
	configOutput();
	initTimer();
	bitState.current=0;
	while(1){
		/*sendPollSignal();
		if(poll()>0){
			PORTB|=(1<<PB5);
		}else{
			PORTB&=~(1<<PB5);
		}*/
	}
	return 0;
}
