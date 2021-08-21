#include <xc.h>

#include<avr/io.h>
#include<avr/interrupt.h>

//Declara vari�vel para fazer a contagem de tempo
int timer;

//Declara flag utilizada para habilitar o incremento do n�mero mostrado no display
int contar;

//Declara flag que define o modo de contagem.
unsigned char modo = 0;

//Declara flag para zerar a contagem
unsigned int zerar = 0;

//Declara fun��o utilizada para atualizar o display
void display(int unidade, int dezena);


//Fun��o que trata a interrup��o de INT1
ISR (INT1_vect)  
{

    //Verifica se o timer 0 est� ligado 
    if ((TCCR0B && 0x05) == 0x00)
    {   
	    // Aciona o timer com uma pr� escala de 64
	    TCCR0B |= 0x02;
		
		//Habilita interrup��o de compara��o
		TIMSK0 |= (1 << OCIE0A);
    }
  
    else
    {
	    //Verifica o estado do comparado do timer 0 se est� acionado ou n�o
	    if ((TIMSK0 && (1 << OCIE0A)) )
	    {
		    // Desabilita interrup��es de compara��o do timer 0
		    TIMSK0 &= (0 << OCIE0A);
	    }
	    else
	    {
		    // Habilita interrup��es de compara��o do timer 0
		    TIMSK0 |= (1 << OCIE0A);
	    }
    }

}

//Fun��o que trata a interrup��o de INT0
ISR (INT0_vect)         
{
	//Verifica se o modo de contagem � crescente
	if (modo == 0)
		
		//Define o modo de contagem como decrescente
		modo = 1;
	else
		
		//Define o modo de contagem como crescente
		modo = 0;
}

//Fun��o que trata a interrup��o de INT20
ISR (PCINT2_vect) 
{
	//Seta a flag para zerar a contagem em 1
	zerar = 1;
}

//Fun��o que trata a interrup��o do timer
ISR(TIMER0_COMPA_vect)
{
	//conta 1 ms
	timer++;
	
	//Se a contagem chegar em 1000, quer dizer que passaram 1 segundo
	if (timer == 1000)
	{
		//Zera a contagem de tempo
		timer = 0;
		
		//Seta a flag para mudar o n�mero mostrado no display 
		contar = 1;
		
	}

}

int main()
{
	//Configura todos os pinos do PORTB como sa�da
	DDRB |= 0xFF;
	//Configura todos os pinos do PORTC como sa�da
	DDRC |= 0x3F;
	//Configura PD2, PD3 e PD4 com pull-up
	PORTD |= 0x1C;

	//Seta o timer para zerar quando atingir o valor de compara��o
	TCCR0A |= (1 << WGM01) ;
	//Seta valor valor de configura��o como 125
	OCR0A = 125;

	//Habilita a interrup��o 0
	EIMSK |= 1<<INT0; 
	//Seta a interrup��o em borda de descida   
	EICRA &= 0<<ISC00;
	//Seta a interrup��o em borda de descida   
	EICRA |= 1<<ISC01;
	
	//Habilita a interrup��o 1
	EIMSK |= 1<<INT1; 
	
	//Seta a interrup��o em borda de descida      
	EICRA &= 0<<ISC10;
	
	//Seta a interrup��o em borda de descida   
	EICRA |= 1<<ISC11;
	
	PCICR |= 1<<PCIE2;    
	PCMSK2 |= 1<<PCINT20; 
 
	//Habilita as interrup��es globais
	sei();                     

	//Declara as vari�veis unidade e dezena, para mostrar os n�meros no display
	int unidade = 0, dezena = 0;
	
	//Inicia o loop infinito
    while(1)
    {
		
		//Verifica se a flag para mudar o n�mero a ser mostrado no display foi setado
	    if((contar & 1)){
			
			//Zera a flag contar
		    contar = 0;
			
			//Verifica qual modo de contagem est� sendo utilizado
		    if(!(modo & 1)){
				//Incrementa a unidade
			    unidade++;
				
				//Verifica se a unidade � maior que 9 para incrementar a dezena
			    if(unidade > 9){
					
					//Zera o valor da unidade
				    unidade = 0;
					
					//Incrementa a dezena
				    dezena++;
					
					//Verfifica se a dezena � maior que 9, para resetar a contagem
				    if(dezena > 9){
						
						//zera a contagem 
					    dezena = 0;
				    }
			    }
		    }
			
			//Caso o modo seja decremental
		    else{
				
				//Subtrai 1 da unidade 
			    unidade--;
				
				//Verifica se a unidade � menor que 0 para decrementar a dezena
			    if(unidade < 0){
					
					//Atribui 9 ao valor da unidade
				    unidade = 9;
					
					//subtrai a dezena
				    dezena--;
					
					//VErifica se a dezena � menor que zero
				    if(dezena < 0){
						
						//Reseta a contagem da dezena
					    dezena = 9;
				    }
			    }
		    }
			//Chama a fun��o para atualizar os displays.
			display(unidade,  dezena);
	    }
		
		 //Verifica se a flag de zerar a contagem n�o esta ativada
		 if(zerar & 1){
			 
			//Zera a dezena
			dezena = 0;
			
			//Zera a unidade
			unidade = 0;
			
			//Zera a flag zerar
			zerar = 0;
			
			//Atualiza o display novamente
			display(unidade, dezena);
		 }
    }
}



//Fun��o que atualiza os display
void display(int unidade, int dezena){
			//Switch que troca o valor da unidade
	switch(unidade){
		case 0:
			PORTB = 0b10000000;
			PORTC = 0b00011111;
		break;	
		case 1:
			PORTC = 0b00000011;
			PORTB &= 0b01111111;
		break;
				
		case 2:
			PORTB = 0b10000000;
			PORTC = 0b00101101;
		break;
			
		case 3:
			PORTB = 0b10000000;
			PORTC = 0b00100111;
		break;
				
		case 4:
			PORTC = 0b00110011;
			PORTB &= 0b01111111;
		break;
		case 5:
			PORTB = 0b10000000;
			PORTC = 0b00110110;
		break;
				
		case 6:
			PORTB = 0b10000000;
			PORTC = 0b00111110;
		break;
				
		case 7:
			PORTB = 0b10000000;
			PORTC = 0b00000011;
		break;
			
		case 8:
			PORTB = 0b10000000;
			PORTC = 0b00111111;
		break;
				
		case 9:
			PORTB = 0b10000000;
			PORTC = 0b00110111;
		break;
	}
	//switch que troca a dezena
	switch(dezena){
		case 0:
			PORTB |= 0b00111111;
		break;
		case 1:
			PORTB |= 0b00000110;
		break;
		case 2:
			PORTB |= 0b01011011;
		break;
		case 3:
			PORTB |= 0b01001111;
		break;
		case 4:
			PORTB |= 0b01100110;
		break;
		case 5:
			PORTB |= 0b01101101;
		break;
		case 6:
			PORTB |= 0b01111101;
		break;
		case 7:
			PORTB |= 0b00000111;
		break;
		case 8:
			PORTB |= 0b01111111;
		break;
		case 9:
			PORTB |= 0b01101111;
		break;
	}
}