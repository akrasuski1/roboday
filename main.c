#include<avr/io.h>
#include<util/delay.h>
#include<avr/interrupt.h>

#define SETB(port, bit) port|=(1<<bit)
#define CLRB(port, bit) port&=~(1<<bit)

#define LEFT      PORTB
#define LEFT_DDR  DDRB
#define RIGHT     PORTC
#define RIGHT_DDR DDRC
#define LPIN0 0
#define LPIN1 1
#define LPIN2 2
#define LPIN3 3
#define RPIN0 0
#define RPIN1 1
#define RPIN2 2
#define RPIN3 3

volatile int stepsl=0,stepsr=0;
ISR(TIMER0_OVF_vect){
	static int stepl=0, stepr=0;
	if(stepsl>0){
		stepl++;stepsl--;
		stepl&=7;
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
		stepl--;stepsl++;
		if(stepl<0)stepl=7;
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

#define SERVO     PORTC
#define SERVO_DDR DDRC
#define SERVO_PIN 5

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
	LEFT_DDR|=(1<<LPIN0)|(1<<LPIN1)|(1<<LPIN2)|(1<<LPIN3);
	//RIGHT_DDR|=(1<<RPIN0)|(1<<RPIN1)|(1<<RPIN2)|(1<<RPIN3);

	SERVO_DDR|=(1<<SERVO_PIN);
	// Count to OCR0A
	TCCR0A|=(1<<WGM01)|(1<<WGM00);
	// Prescaler = 64
	TCCR0B|=(1<<WGM02)|(1<<CS01)|(1<<CS00);
	// Fast PWM
	TCCR1A|=(1<<WGM11);
	// Prescaler = 8
	TCCR1B|=(1<<WGM13)|(1<<WGM12)|(1<<CS11);

	ICR1=20000; // period=20ms (50Hz)
	set_servo(1500); // neutral position

	TIMSK1|=(1<<OCIE1A)|(1<<TOIE1);

	OCR0A=200;
	TIMSK0|=1<<TOIE0;
	sei();
}

int main(void){
	_delay_ms(200);
	init();
	_delay_ms(1000);
	while(1){
		stepsl-=1500;
		set_servo(1000);
		_delay_ms(2500);
		set_servo(1500);
		_delay_ms(2500);

		stepsl+=(1500);
		set_servo(2000);
		_delay_ms(2500);
		set_servo(1500);
		_delay_ms(2500);

	}
}
