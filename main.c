#include <avr/io.h>
#include <avr/iom32.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "sd_cmd.h"

// Useful macros.
#define SETB(port, bit) port|=(1<<bit)
#define CLRB(port, bit) port&=~(1<<bit)

// Motor pinouts.
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

// Timer interrupt, in which we update stepper motors,
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

// Servo pinout.
#define SERVO     PORTD
#define SERVO_DDR DDRD
#define SERVO_PIN 7

// Timer1 interrupts, in which we switch the servo pin to control
// its position,
ISR(TIMER1_COMPA_vect){
	CLRB(SERVO, SERVO_PIN);
}

ISR(TIMER1_OVF_vect){
	SETB(SERVO, SERVO_PIN);
}

// Function for setting servo position - argument is pulse length in ms.
void set_servo(int pos){
	OCR1A=pos;
	_delay_ms(500); // Wait for a while for servo to adjust position.
}

// Pulse lengths corresponding to servo's top and writing positions.
#define SERVO_TOP   700
#define SERVO_WRITE 1200

// Initialization.
void init(){
	// Motor and servo pins as outputs.
	LEFT_DDR |=(1<<LPIN0)|(1<<LPIN1)|(1<<LPIN2)|(1<<LPIN3);
	RIGHT_DDR|=(1<<RPIN0)|(1<<RPIN1)|(1<<RPIN2)|(1<<RPIN3);
	SERVO_DDR|=(1<<SERVO_PIN);

	// Timer 0 (motors)
	// Count to OCR0, prescaler=64, so each tick takes 8us.
	TCCR0|=(1<<WGM01)|(1<<CS01)|(1<<CS00);
	// Interrupt every OCR0 clocks - value obtained experimentally.
	// Too big, and stepper motors are slow, too small, and stepper 
	// motors start missing steps, causing inaccuracies.
	OCR0=250;
	// Enable timer interrupt.
	TIMSK|=1<<OCIE0;

	// Timer 1 (servo)
	// Fast PWM mode.
	TCCR1A|=(1<<WGM11);
	// Prescaler = 8 - each tick takes 1us.
	TCCR1B|=(1<<WGM13)|(1<<WGM12)|(1<<CS11);
	ICR1=20000; // Period=20ms (50Hz)
	// Enable interrupt on compare match and on overflow.
	TIMSK|=(1<<OCIE1A)|(1<<TOIE1);

	// SD card initialization.
	init_fs();

	// Initial servo position - top.
	set_servo(SERVO_TOP);
	// Enable interrupts.
	sei();
}

// On any error, or at the end of programmed sequence, set robot in safe
// state - with pen in top position and motors unable to move (interrupts 
// cleared).
void die(){
	set_servo(SERVO_TOP);
	cli();
	while(1);
}

// Main function.
int main(void){
	init();
	// After initialization, read commands from SD card one by one
	// and execute them,
	command cmd;
	while(1){
		get_cmd(&cmd);
		switch(cmd.type){
		case CMD_END:
			die();
			break;
		case CMD_RAISE_PEN:
			set_servo(SERVO_TOP);
			break;
		case CMD_DROP_PEN:
			set_servo(SERVO_WRITE);
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
		// Wait for a moment after each command. This compensates in part
		// for robots inertia.
		_delay_ms(100);
	}
}
