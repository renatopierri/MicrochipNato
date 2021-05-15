/*
 * SC2.cpp
 *
 * Created: 08/02/2020 09:56:14
 * Author : renat
 */ 
# define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define MUDA_LED PORTB = PORTB ^ 0x20;// togle PINB5
#define REFRESH_DISPLAY PORTB = PORTB ^ 0x10; // togle PINB4
#define INICIO PORTB = PORTB ^ 0x08; // togle PINB3
#define LIGADO 1;
#define DESLIGADO 0;

void initIntervaloLeitura (uint8_t freq);
void initTemporizacaoDisplay();
void initConversorADC();
void liga();

uint16_t adc_read(uint8_t ch);
uint16_t valorADC;
uint16_t tensao;
uint16_t display;
uint8_t displayDaVez, interruptor, ativo, controle;

ISR(TIMER1_COMPA_vect){	
	cli();
	MUDA_LED;
	valorADC = adc_read(0);
	tensao = 0.5 * valorADC;//(511.0*valorADC/1022.0);
	display = tensao%10;
	display |= (tensao/10)%10 << 4;
	display |= (tensao/100)%10 << 8;
}

ISR(TIMER0_COMPA_vect){
	cli();
	REFRESH_DISPLAY;
	PORTD |= 1 << PORTD4; //Apaga digito menos significativo
	PORTD |= 1 << PORTD5; //Apaga digito do meio
	PORTD |= 1 << PORTD6; //Apaga digito mais significativo
	PORTD &= 0xF0; // Apaga o digito antigo
	uint8_t controle;
	controle = 0;
		uint8_t pbfd, ct10, ctsh3;
	if((displayDaVez == 0) && (controle == 0)) {	
		PORTD |= (display & 0x0F00) >> 8;
		PORTD &= ~(1 << PORTD6); //Acende digito mais significativo
		displayDaVez =1;
		controle = 1;
	}
	if((displayDaVez == 1) && (controle == 0)){			
		PORTD |= (display & 0x00F0) >> 4;
		PORTD &= ~(1 << PORTD5); //Acende digito do meio
		displayDaVez = 2;	
		controle = 1;	
	}
	if((displayDaVez == 2) && (controle == 0)){					
		PORTD |= (display & 0x000F);
		PORTD &= ~(1 << PORTD4); //Acende digito menos significativo
		displayDaVez = 0;	
		controle = 1;
	}

}

int main(void){
			
	DDRD |= (1<<DDD0)|(1<<DDD1)|(1<<DDD2)|(1<<DDD3)|(1<<DDD4)|(1<<DDD5)|(1<<DDD6)|(1<<DDD7);
	DDRB |= (1<<DDB0)|(1<<DDB1)|(1<<DDB3)|(1<<DDB4)|(1<<DDB5);
	DDRB &= ~(1<<DDB7);
	DDRC |= (1<<DDC1)|(1<<DDC2)|(1<<DDC4)|(1<<DDC5);
	DDRC &= ~(1<<DDC3);
	PORTC = 0x08; //LIGANDO PULL UP ENTRADA A3


	//Inicializando o temporizador e a frequencia da interrupcao
	displayDaVez = 0;
	initIntervaloLeitura(4);
	initTemporizacaoDisplay();

	//Inicializando o conversor ADC
	initConversorADC();
	ativo = 0;
	controle = 0;

	sei();
	
    /* Replace with your application code */
    while (1) 
    {
		sei();
		liga();
		
    }
}

void initIntervaloLeitura (uint8_t freq){
	TCCR1B |= (1<<CS12)|(1<<CS10)|(1<<WGM12);
	TIMSK1 |= (1<<OCIE1A);
	OCR1A = 7812.5/freq - 1; //(F_CPU/(freq*2*1024)-1); = 7812,5/4Hertz - 1
}

void initTemporizacaoDisplay(){
	TCCR0A |= (1 << WGM01);
	TCCR0B |= (1 << CS02) | (1 << CS00); //Prescaler - divide o clock por 1024
	OCR0A = 64; //Periodo em ms * 7,8125 - 1 //129 para 60Hz, 64 p/ 120Hz
	TIMSK0 |= (1 << OCIE0A);
}

void initConversorADC()
{
	// AREF = AVcc
	ADMUX = (1<<REFS0);
	// Habilita conversao ADC e o prescaler com valor de 128
	ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
}

// Lendo valor ADC
uint16_t adc_read(uint8_t ch)
{
	// Selecionando o canal de 0 - 7
	// A operacao AND com o valor 7 garante que o canal correto seja selecionado
	ch &= 0b00000111;  
	ADMUX = (ADMUX & 0xF8)|ch; // Informando para o registro ADMUX o número do canal selecionado
	
	// Escreve 1 no ADSC para garantir conversao unica
	ADCSRA |= (1<<ADSC);
	
	// Aguarda a conversao AD terminar
	while(ADCSRA & (1<<ADSC));
	
	return (ADC);
}

void liga(){
	uint8_t temp = 0;
	temp = PINC & 0x08;
	if((temp == 0)&(ativo == 0)){
		ativo = 1;
		PORTC = ((PORTC & 0xF9) | ((controle & 0x03) << 1));
		PORTC = ((PORTC & 0xCF) | ((controle & 0x0C) << 2));
		PORTB = ((PORTB & 0xFD) | ((controle & 0x10) >> 3));
		controle ++;
	}else{
		if(temp == 0x08){
			ativo = 0;
			if (controle == 32){
				controle = 0;
			}
		}
	}
		
}

		//if((PINC & 0x08)!=8){ //lendo sinal encender / control. PINC3
			////PORTC |= (1<<PINC1); // liga D51 positivo / desliga D52 negativo
			//PORTC &= ~(1<<PINC1); // liga D52 negativo / desliga D51 positivo
			//PORTC |= (1<<PINC2); // desliga campo
			//PORTC |= (1<<PINC4); // control baixo
			//PORTC |= (1<<PINC5); // Aciona Q4. Equivalente desligar
			//PORTB |= (1<<PINB1); //// Gate de Q30 = 1.
			//}else{
			//PORTC |= (1<<PINC1); // liga D51 positivo / desliga D52 negativo
			////PORTC &= ~(1<<PINC1); // liga D52 negativo / desliga D51 positivo
			//PORTC &= ~(1<<PINC2); // liga campo
			//PORTC &= ~(1<<PINC4); // control alto
			//PORTC &= ~(1<<PINC5); // Não aciona Q4.
			//PORTB &= ~(1<<PINB1); // Gate de Q30 = 0.
		//}
		
				//interruptor = LIGADO;
				//PORTB |= (1<<PINB1);  //Liga D9
				////PORTC &= ~(1<<PINC5); //
				//PORTC |= (1<<PINC5);  //Aciona Q4