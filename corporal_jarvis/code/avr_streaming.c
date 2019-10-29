/*
 * ardScope.c
 *
 * Created: 8/1/2016 9:20:16 AM
 */ 

#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 16000000UL
#define USART_BAUDRATE 230400
#define UBRR_VALUE ((F_CPU/4/USART_BAUDRATE)-1)/2


void USART0Init(void)
{
	// Set the baud rate divider to 8 to double the
	// transfer rate
	UCSR0A |= (1<<U2X0);
	// Set baud rate based on UBBR_VALUE
	UBRR0H = (uint8_t)(UBRR_VALUE>>8);
	UBRR0L = (uint8_t)UBRR_VALUE;
	// Set frame format to 8 data bits, no parity, 1 stop bit
	UCSR0C |= (1<<UCSZ01)|(1<<UCSZ00);
	// Enable transmission and reception
	UCSR0B |= (1<<RXEN0)|(1<<TXEN0);
}

// Initialize debug ports
void InitPort(void)
{
	// Set PD6 and PD2 as output
	DDRD |= (1<<PD2)|(1<<PD6);
}

// Initialize timer0
void InitTimer0(void)
{
	// Set Initial Timer value
	TCNT0=0;
	// Place TOP timer value to Output compare register
	OCR0A = 99;
	// Set CTC mode
	// and make toggle PD6/OC0A pin on compare match
	TCCR0A |=(1<<COM0A0)|(1<<WGM01);
}

// Start timer0 with prescaler 8
void StartTimer0(void)
{
	// Set prescaler 8 and start timer
	TCCR0B |=(1<<CS01);
}

void InitADC()
{
	// Select Vref=AVcc
	// and set left adjust result
	ADMUX |= (1<<REFS0)|(1<<ADLAR);
	// Set prescaler to 32
	// Enable auto-triggering
	// Enable ADC interrupt
	// and enable ADC
	ADCSRA |= (1<<ADPS2)|(1<<ADPS0)|(1<<ADATE)|(1<<ADIE)|(1<<ADEN);
	// Set ADC trigger source - Timer0 compare match A
	ADCSRB |= (1<<ADTS1)|(1<<ADTS0);
}

void SetADCChannel(uint8_t ADCchannel)
{
	// Select ADC channel with safety mask
	ADMUX = (ADMUX & 0xF0) | (ADCchannel & 0x0F);
}

void StartADC(void)
{
	ADCSRA |= (1<<ADSC);
}

// ADC conversion complete ISR
ISR(ADC_vect)
{
	// Clear timer compare match flag
	TIFR0=(1<<OCF0A);
	// Toggle pin PD2 to track the end of ADC conversion
	PIND = (1<<PD2);
	// Wait while previous byte is completed
	while(!(UCSR0A&(1<<UDRE0))){};
	// Transmit data
	UDR0 = ADCH;
}

int main(void)
{
	// Initialize USART0
	USART0Init();
	// Initialize ports
	InitPort();
	// Initialize ADC
	InitADC();
	// Select ADC channel
	SetADCChannel(0);
	// Initialize timer0
	InitTimer0();
	// Start timer0
	StartTimer0();
	// Start conversion
	StartADC();
	// Enable global interrupts
	sei();

	while(1)
	{		
	}
}

