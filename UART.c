#include "UART.h"
#include "Flash.h"


void initUart(){
			int clockDIV; // variável auxiliar para definir o divisor do clock da UART
     
    // Acionamento dos clocks dos periféricos
  SIM_SOPT2 = SIM_SOPT2_UART0SRC(1); // define o clock como sendo o MCGFLLCLK
  SIM_SOPT2 &= ~SIM_SOPT2_PLLFLLSEL_MASK; // clock MCGFLLCLK
  SIM_SCGC4 |= SIM_SCGC4_UART0_MASK; // liga a UART0
  SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK; // liga a PORTA, que tem os pinos da UART
     
    // Mapeamento dos pinos utilizados
    PORTA_PCR1 |= PORT_PCR_MUX(2); // UART0 RX
    PORTA_PCR2 |= PORT_PCR_MUX(2); // UART0 TX
     
    // Configuração da UART0
    UART0_C2 &= ~ (UART0_C2_TE_MASK| UART0_C2_RE_MASK); // desabilita os pinos enquanto configura. Várias configurações só são permitidas com os pinos desabilitados
     
  // Habilita interrupções da UART0
    NVIC_EnableIRQ(UART0_IRQn);
   
    // Definição do divisor de clock.
    clockUART=DEFAULT_SYSTEM_CLOCK;//getMCGClock(); // calcula o valor do clock de entrado da UART
    //clockUART = 48000000;
    clockDIV = clockUART/( BAUD * OSR); // calcula o quanto tem que ser o divisor
    UART0_BDH = (clockDIV/256)%32; // Obtêm a parte mais significativa do divisor
  UART0_BDL = clockDIV%256; // Obtêm a parte menos significativa do divisor
  UART0_C4 = OSR-1; // define quantas vezes cada bit será lido
     
    //Configuração da transmissão
  UART0_C1 = 0; // padrão: define 8 bits de dados, 1 de parada, sem paridade
  UART0_C3 = 0x00; // tudo como dantes no quartel de abrantes
  //UART0_MA1 = 0x00;
  //UART0_MA1 = 0x00;
  UART0_S1 |= 0x1F; // Limpa os status antes de começar
  UART0_S2 |= 0xC0; //altera configurações do idle e break character !?
    //UART0_C2 |= (UART0_C2_RIE_MASK | UART0_C2_TCIE_MASK); // Habilita as interrupções de transmissão completada e recebeu dados
  UART0_C2 |= (UART0_C2_RIE_MASK ); // Habilita a interrupção de recepção
  UART0_C2 |= (UART0_C2_TE_MASK| UART0_C2_RE_MASK); // habilita os pinos de TX e RX
}


//Envia a string preparada. Basicamente habilita a interrupção transmissão finalizada e a interrupção faz o trabalho
void uartSendString(int AccX, int AccY, int AccZ){
	
		char AccXC[5];
		int resto = 0;
		for(int i=3;i>=0;i--){
		resto = AccX%10 + 48;
		AccXC[i] = resto;
		AccX = AccX/10;
		}
		AccXC[4] = 0;
	
		char AccYC[5];
		resto = 0;
		for(int i=3;i>=0;i--){
		resto = AccY%10 + 48;
		AccYC[i] = resto;
		AccY = AccY/10;
		}
		AccYC[4] = 0;
		
		char AccZC[5];
		resto = 0;
		for(int i=3;i>=0;i--){
		resto = AccZ%10 + 48;
		AccZC[i] = resto;
		AccZ = AccZ/10;
		}
		AccZC[4] = 0;

		//formatando mensagem para o padrao e armazenando valores na string msg
		strcpy(msg,"");
		strcat(msg,AccXC);
		strcat(msg," ");
		strcat(msg,AccYC);
		strcat(msg," ");
		strcat(msg,AccZC);
		strcat(msg,"\n");
		

	
		msgLength = strlen(msg); //armazena em msgLength o tamanho da string msg
    msgPointer = 0; //Inicializa o ponteiro para o inicio da string
    UART0_D = msg[msgPointer++]; // Coloca a primeira letra da string para imprimir
    UART0_C2 |= ( UART0_C2_TCIE_MASK); // Habilita as interrupção de transmissão completada
}

//interpreta e executa comandos enviados
void readCommand(char commandString[]){
	
		if((commandString[0] == (char)INICIAL) & (commandString[2] == (char)FINAL)){			
			
			switch(commandString[1]){
				
				case PING:
					returnACK();
					afterDump = 0;
					break;
				
				case STATUS:
					returnStatus();
					afterDump = 0;
					break;
				
				case ERASE:
					
					if((systemStatus == 0) | (afterDump == 1)){//caso aceito				
						//apaga o setor da FLASH:
						flash_EraseSector(FLASH_SECTOR_ADDRESS);
						bufferPointer = 0;
						returnACK();
					}
					else{//nao aceito	
						returnNAK();
					}
					afterDump = 0;				
					break;
				
				case START:
					
					if(bufferPointer >= 100){//algum problema
						returnNAK();	
					}
					
					else{
						//iniciar sistema
						systemStatus = 1;
						returnACK();
					}
					afterDump = 0;
					break;
				
				case STOP:
					if(systemStatus == 1){//sistema executando
						//para sistema
						systemStatus = 0;
						returnACK();
					}
					afterDump = 0;
					break;
				
				case LASTVAL:
					pflash_sector = FLASH_SECTOR_ADDRESS + bufferPointer;
					
					value_readedZ = *pflash_sector;
					pflash_sector--;
					value_readedY = *pflash_sector;
					pflash_sector--;
					value_readedX = *pflash_sector;
					uartSendString(value_readedX, value_readedY, value_readedZ);
					
					//teste com matriz
					//uartSendString(buffer[bufferPointer][0], buffer[bufferPointer][1], buffer[bufferPointer][2]);
					break;
				
				case DUMP:
					returnDumpFirst();
					while(msgPointer<msgLength){
					}
					//flash
					//Inicializa o endereco do ponteiro com o setor que iremos usar para salvar os dados:
					pflash_sector = FLASH_SECTOR_ADDRESS;
			
					//printar na tela os dados da flash
					for(int i = 0;i<bufferPointer;i++){
						//Le o conteúdo da FLASH e guarda na variável 'value_readed':
						value_readedX = *pflash_sector;
						pflash_sector++;
						value_readedY = *pflash_sector;
						pflash_sector++;
						value_readedZ = *pflash_sector;
						pflash_sector++;

						uartSendString(value_readedX, value_readedY, value_readedZ);
						//esperar enviar o dado
						while(msgPointer<msgLength){
						}
					}
					
					//teste com matriz
					//for(int i = 0;i<=bufferPointer;i++){
					//uartSendString(buffer[i][0], buffer[i][1], buffer[i][2]);
					//}
					returnDumpLast();
					while(msgPointer<msgLength){
					}
					afterDump = 1;
					break;
				
				default:
					//caso comando -> 1xxxxxxx (set value)
					if(((int)commandString[1] > 128) & (systemStatus == 0)){
						setValue((int)commandString[1]);						
					}
					else{
					returnNAK();
					}
				
		}
			
	}
}

//retorna pacote ack
void returnACK(){
	char ackReturn[] = {INICIAL, 0x11, FINAL};
	msgPointer = 0; //Inicializa o ponteiro para o inicio da string
	
	//retornar ack
	msgLength = strlen(ackReturn); //armazena em msgLength o tamanho da string msg
	for(int i=0;i<msgLength;i++){
		msg[i] = ackReturn[i];
	}
	UART0_D = msg[msgPointer++]; // Coloca a primeira letra da string para imprimir
	UART0_C2 |= ( UART0_C2_TCIE_MASK); // Habilita as interrupção de transmissão completada	
}

//retorna pacote nak
void returnNAK(){
	char nakReturn[] = {INICIAL, 0xEE, FINAL};
	msgPointer = 0; //Inicializa o ponteiro para o inicio da string
	
	//retornar ack
	msgLength = strlen(nakReturn); //armazena em msgLength o tamanho da string msg
	for(int i=0;i<msgLength;i++){
		msg[i] = nakReturn[i];
	}
	UART0_D = msg[msgPointer++]; // Coloca a primeira letra da string para imprimir
	UART0_C2 |= ( UART0_C2_TCIE_MASK); // Habilita as interrupção de transmissão completada	
}

//retorna inicio do dump
void returnDumpFirst(){
	char dumpReturnFirst[] = {INICIAL, 0x55, bufferPointer, FINAL};
	msgPointer = 0; //Inicializa o ponteiro para o inicio da string
	
	//retornar ack
	msgLength = strlen(dumpReturnFirst); //armazena em msgLength o tamanho da string msg
	for(int i=0;i<msgLength;i++){
		msg[i] = dumpReturnFirst[i];
	}
	UART0_D = msg[msgPointer++]; // Coloca a primeira letra da string para imprimir
	UART0_C2 |= ( UART0_C2_TCIE_MASK); // Habilita as interrupção de transmissão completada	
}

//retorna final do dump
void returnDumpLast(){
	char dumpReturnLast[] = {INICIAL, 0xAA, FINAL};
	msgPointer = 0; //Inicializa o ponteiro para o inicio da string
	
	//retornar ack
	msgLength = strlen(dumpReturnLast); //armazena em msgLength o tamanho da string msg
	for(int i=0;i<msgLength;i++){
		msg[i] = dumpReturnLast[i];
	}
	UART0_D = msg[msgPointer++]; // Coloca a primeira letra da string para imprimir
	UART0_C2 |= ( UART0_C2_TCIE_MASK); // Habilita as interrupção de transmissão completada	
}

//retorna status
void returnStatus(){
	char status[3];
	
	status[0] = systemStatus;
	
	if (bufferPointer > 90){
		status[1] = 1;
	}else{
		status[1] = 0;
	}
	
	if (bufferPointer >= 100){
		status[2] = 1;
	}else{
		status[2] = 0;
	}
	
	int mem = bufferPointer;
	
	char msgStatus[] = {INICIAL, status[0], status[1], status[2], pitIntervalMeasure, mem, FINAL};
	msgPointer = 0; //Inicializa o ponteiro para o inicio da string
	
	//retornar ack
	msgLength = 6; //armazena em msgLength o tamanho da string msg
	for(int i=0;i<msgLength;i++){
		msg[i] = msgStatus[i];
	}
	UART0_D = msg[msgPointer++]; // Coloca a primeira letra da string para imprimir
	UART0_C2 |= ( UART0_C2_TCIE_MASK); // Habilita as interrupção de transmissão completada	
}

//configura taxa de amostragem
void setValue(int samplingRate){
	pitIntervalMeasure = samplingRate - 128;
	pitIntervalLight = samplingRate - 128;
	returnACK();
}









//************************************************************************//