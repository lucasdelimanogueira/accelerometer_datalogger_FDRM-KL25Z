#include "accelerometer.h"
#include "UART.h"
#include "MKL25Z4.h"
#include "Board_LED.h"
#include "Flash.h"

// Variáveis espelhos dos registradores do acelerômetro
uint8_t acc_ctrl,acc_status;
uint8_t accs_regs[6];
// Acelerações nos 3 eixos
int16_t accX, accY, accZ;

// Variáveis que serão mexidas nas interrupções
// flags para a recepção e envio
volatile char c; // o char recebido
volatile int flagCommReceived = 0;
volatile int flagCommSend = 0; 

//intervalo interrupções de medição e pisca luz
int pitIntervalMeasure = 5000;
int pitIntervalLight = 2120;

int clockUART;     // variável para guardar o valor do clock que chega na UART
char msg[100]; // string a ser enviada
int msgLength = 10; // comprimento da string
int msgPointer = 0; // ponteiro da string
int commandPointer = 0;
char command[100];
int buffer[100][3]; //matriz para teste sem a flash
int bufferPointer = 10; //contador posicao da memoria

int systemStatus = 0;//sistema ligado = 1, desligado = 0
int afterDump = 0;//flag para indicar se ocorreu um dump

long int *pflash_sector; //ponteiro para o endereço da flash onde guardaremos os dados
long int value_readedX; //variável para ler os dados armazenados na flash
long int value_readedY; //variável para ler os dados armazenados na flash
long int  value_readedZ; //variável para ler os dados armazenados na flash

//flags para os LEDs
int flag_light = 0;

//flag leitura accelerometro
int flagRead = 0;


//função da interrupção da UART0
	void UART0_IRQHandler(){
    if (UART0_S1&UART_S1_RDRF_MASK){ // o RDRF indica que chegou algum dado
      c = UART0_D;                                      // então lê o que chegou
      flagCommReceived = 1;                    // e marca que recebeu			
    }
    if ((UART0_S1&UART_S1_TDRE_MASK)){ // o TDRE indica que já foi enviado o dado
        if (msgPointer<msgLength){ // se ainda tem coisa para mandar,
            UART0_D = msg[msgPointer++]; // manda a próxima letra
        }else{ // se já mandou tudo,
    UART0_C2 &= ~(UART0_C2_TCIE_MASK); // desabilita a interrupções de transmissão completada
   
        }
    }
}
	

 /*****************************************************************************************
* Handles PIT interrupt if enabled
*****************************************************************************************/
void PIT_IRQHandler(void)
{
     
    if(PIT_TFLG0){ // Canal 0
        PIT_TFLG0 = PIT_TFLG_TIF_MASK;
				flagRead = 1;
    }
    if(PIT_TFLG1){ // Canal 1
        PIT_TFLG1 = PIT_TFLG_TIF_MASK;
        flag_light = 1;
    }
  
}



/*------------------------------------------------------------------------------
Coloca o core em modo STOP
 *------------------------------------------------------------------------------*/
void powermodeSleep()
{
	__asm
	{
		WFI //Wait for an interruption;
	}
}


int main(void){	
	
	
char charAux[] = {0,'\0'};
	
	//Inicializa acelerometro
	accInit();	
	//Inicializa UART
	initUart();
	//inicializa leds
	LED_Initialize();

//apagar memoria sempre ao começar o programa
//flash_EraseSector(FLASH_SECTOR_ADDRESS);

	
	while(1){ // sempre
			powermodeSleep();
		
			//*******leitura de dados do acelerometro*********
			if(flagRead == 1 && systemStatus == 1){
				readAcc();   
				
				//escreve na FLASH:
				bufferPointer++;
				flash_writeLongWord(FLASH_SECTOR_ADDRESS + bufferPointer, accX);
				bufferPointer++;				
				flash_writeLongWord(FLASH_SECTOR_ADDRESS + bufferPointer, accY);
				bufferPointer++;
				flash_writeLongWord(FLASH_SECTOR_ADDRESS + bufferPointer, accZ);			
				
				//escreve na matriz buffer apenas para teste
				//buffer[bufferPointer][0] = accX;
				//buffer[bufferPointer][1] = accY;
				//buffer[bufferPointer][2] = accZ;
							

				flagRead = 0;
			}
			
			if(flag_light == 1){
				//normal ligado
				if((systemStatus == 1) & (bufferPointer < 90)){
					LED_On(1);//liga led verde
					for(volatile int i=10000;i>0;i--);
					LED_Off(1);//desliga led verde
				}
				
				//memoria ficando cheia
				else if((systemStatus == 1) & (bufferPointer >= 90)){
					//liga led amarelo
					LED_On(0);
					LED_On(1);
					for(volatile int i=10000;i>0;i--);
					//desliga led amarelo
					LED_On(0);
					LED_On(1);
				}
				
				//normal desligado
				else if(systemStatus == 0){
					LED_On(2);//liga led azul
					for(volatile int i=10000;i>0;i--);
					LED_Off(2);//desliga led azul
				}
					
				else {
					LED_On(0);//liga led vermelho
					for(volatile int i=10000;i>0;i--);
					LED_Off(0);//desliga led vermelho
				}
				
				flag_light = 0;
			}
			//*************************************************
			
			//***********enviar comandos***********************
			if(flagCommReceived){ // se recebeu um bit
            flagCommReceived = 0;				
						if(commandPointer < 3){
							command[commandPointer] = c;
							commandPointer++;
						}
						
						if(commandPointer == 3){
							readCommand(command);
							commandPointer = 0;
					}
			}
	
     
  }       		
}




