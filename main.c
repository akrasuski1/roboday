#include <avr/io.h>
#include <avr/iom32.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "sd_cmd.h"

#define SETB(port, bit) port|=(1<<bit)
#define CLRB(port, bit) port&=~(1<<bit)

#define LEFT      PORTA
#define LEFT_DDR  DDRA
#define RIGHT     PORTA
#define RIGHT_DDR DDRA
#define LPIN0 0
#define LPIN1 1
#define LPIN2 2
#define LPIN3 3
#define RPIN0 4
#define RPIN1 5
#define RPIN2 6
#define RPIN3 7

void led_on(){
	SETB(PORTD,2);
}
void led_off(){
	CLRB(PORTD,2);
}

volatile int stepsl=0,stepsr=0;
ISR(TIMER0_COMP_vect){
	static int stepl=0, stepr=0;
	if(stepsl>0){
		stepl++;
		stepl&=7;
		stepsl--;
		switch(stepl){
			case 0:		SETB(LEFT,LPIN0);		break;
			case 1:		CLRB(LEFT,LPIN3);		break;
			case 2:		SETB(LEFT,LPIN1);		break;
			case 3:		CLRB(LEFT,LPIN0);		break;
			case 4:		SETB(LEFT,LPIN2);		break;
			case 5:		CLRB(LEFT,LPIN1);		break;
			case 6:		SETB(LEFT,LPIN3);		break;
			case 7:		CLRB(LEFT,LPIN2);		break;
		}
	} else if(stepsl<0){
		stepl--;
		if(stepl<0){stepl=7;}
		stepsl++;
		switch(stepl){
			case 0:		SETB(LEFT,LPIN3);		break;
			case 1:		CLRB(LEFT,LPIN1);		break;
			case 2:		SETB(LEFT,LPIN0);		break;
			case 3:		CLRB(LEFT,LPIN2);		break;
			case 4:		SETB(LEFT,LPIN1);		break;
			case 5:		CLRB(LEFT,LPIN3);		break;
			case 6:		SETB(LEFT,LPIN2);		break;
			case 7:		CLRB(LEFT,LPIN0);		break;
		}
	}
	if(stepsr>0){
		stepr++;stepsr--;
		stepr&=7;
		switch(stepr){
			case 0:		SETB(RIGHT,RPIN0);		break;
			case 1:		CLRB(RIGHT,RPIN3);		break;
			case 2:		SETB(RIGHT,RPIN1);		break;
			case 3:		CLRB(RIGHT,RPIN0);		break;
			case 4:		SETB(RIGHT,RPIN2);		break;
			case 5:		CLRB(RIGHT,RPIN1);		break;
			case 6:		SETB(RIGHT,RPIN3);		break;
			case 7:		CLRB(RIGHT,RPIN2);		break;
		}
	} else if(stepsr<0){
		stepr--;stepsr++;
		if(stepr<0)stepr=7;
		switch(stepr){
			case 0:		SETB(RIGHT,RPIN3);		break;
			case 1:		CLRB(RIGHT,RPIN1);		break;
			case 2:		SETB(RIGHT,RPIN0);		break;
			case 3:		CLRB(RIGHT,RPIN2);		break;
			case 4:		SETB(RIGHT,RPIN1);		break;
			case 5:		CLRB(RIGHT,RPIN3);		break;
			case 6:		SETB(RIGHT,RPIN2);		break;
			case 7:		CLRB(RIGHT,RPIN0);		break;
		}
	}
}

#define SERVO     PORTB
#define SERVO_DDR DDRB
#define SERVO_PIN 0

ISR(TIMER1_COMPA_vect){
	CLRB(SERVO, SERVO_PIN);
}

ISR(TIMER1_OVF_vect){
	SETB(SERVO, SERVO_PIN);
}

void set_servo(int pos){
	OCR1A=pos;
}

void init(){
	SETB(DDRD, 2); // TODO erase it

	LEFT_DDR |=(1<<LPIN0)|(1<<LPIN1)|(1<<LPIN2)|(1<<LPIN3);
	RIGHT_DDR|=(1<<RPIN0)|(1<<RPIN1)|(1<<RPIN2)|(1<<RPIN3);

	SERVO_DDR|=(1<<SERVO_PIN);

	// Timer 0 (motors)
	// Count to OCR0, prescaler=64
	TCCR0|=(1<<WGM01)|(1<<CS01)|(1<<CS00);
	// Interrupt every OCR0 clocks.
	OCR0=200;
	// Enable interrupt.
	TIMSK|=1<<OCIE0;

	// Timer 1 (servo)
	// Fast PWM
	TCCR1A|=(1<<WGM11);
	// Prescaler = 8
	TCCR1B|=(1<<WGM13)|(1<<WGM12)|(1<<CS11);
	ICR1=20000; // period=20ms (50Hz)
	set_servo(1400); // neutral position
	// Enable interrupt on compare match and on overflow.
	TIMSK|=(1<<OCIE1A)|(1<<TOIE1);

	init_fs();

	sei();
}

void die(){
	cli();
	while(1);
}

int main(void){
	init();
	command cmd;
	while(1){
		get_cmd(&cmd);
		switch(cmd.type){
		case CMD_END:
			led_on();
			die();
			break;
		case CMD_RAISE_PEN:
			set_servo(600);
			_delay_ms(500);
			break;
		case CMD_DROP_PEN:
			set_servo(2100);
			_delay_ms(500);
			break;
		case CMD_TURN_LEFT:
			stepsl-=cmd.num;
			stepsr+=cmd.num;
			while(stepsl || stepsr);
			break;
		case CMD_TURN_RIGHT:
			stepsl+=cmd.num;
			stepsr-=cmd.num;
			while(stepsl || stepsr);
			break;
		case CMD_FORWARD:
			stepsl+=cmd.num;
			stepsr+=cmd.num;
			while(stepsl || stepsr);
			break;
		case CMD_BACKWARD:
			stepsl-=cmd.num;
			stepsr-=cmd.num;
			while(stepsl || stepsr);
			break;
		default:
			die();
		}
	}
}
