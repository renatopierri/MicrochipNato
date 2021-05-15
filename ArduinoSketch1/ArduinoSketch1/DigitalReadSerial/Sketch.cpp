#include <Arduino.h>

#define SAIDA OUTPUT
#define DESLIGADO LOW
#define LIGADO HIGH
#define PORTA_RUA 12
#define PORTA_INTERNA 11
#define SEGURO 10
#define ATENCAO 9
#define ALARME 8
int a = 0;
int x = 0;

int aciona2();
void aciona1();

void setup(){
	
	pinMode (ALARME, SAIDA);
	pinMode (PORTA_INTERNA, SAIDA);
	pinMode (10, SAIDA);
	pinMode (11, SAIDA);
	pinMode (12, SAIDA);
	
	

	
}

void loop(){
	aciona1();
//	delay (500);
	x = aciona2();
//	delay (500);
}

void aciona1(){
	digitalWrite(PORTA_RUA,DESLIGADO);
	digitalWrite(PORTA_INTERNA,LIGADO);
	digitalWrite(SEGURO,DESLIGADO);
	digitalWrite(ATENCAO,LIGADO);
	digitalWrite(ALARME,DESLIGADO);
}

int aciona2(){
	digitalWrite(PORTA_RUA,LIGADO);
	digitalWrite(PORTA_INTERNA,DESLIGADO);
	digitalWrite(SEGURO,LIGADO);
	digitalWrite(ATENCAO,DESLIGADO);
	digitalWrite(ALARME,LIGADO);
	a++;
	return a;
}




