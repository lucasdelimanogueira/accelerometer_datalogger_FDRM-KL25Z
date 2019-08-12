#include <String.h>
#include "MKL25Z4.h"
#include "PIT.h"
//**************Comunicação UART**********************//

#define BAUD 57600 // Velocidade de transmissão da comunicação serial
#define OSR 8            // Número de leituras por bit (OverSampling Rate)
#define INICIAL 0x55
#define FINAL 0xAA
#define PING  0b00000001 //1
#define STATUS 0b00000010 //2
#define ERASE 0b00000011 //3
#define START 0b00000100 //4
#define STOP 0b00000101 //5
#define LASTVAL 0b00000110 //6
#define DUMP 0b00000111 //7
#define SETVAL 

 
extern int clockUART;     // variável para guardar o valor do clock que chega na UART
extern char msg[100]; // string a ser enviada
extern int msgLength;// comprimento da string
extern int msgPointer; // ponteiro da string
extern int pitIntervalMeasure;
extern long int value_readedX;
extern long int value_readedY;
extern long int value_readedZ;
extern long int *pflash_sector;

//status do sistema (1 = ligado; 0 = desligado)
extern int systemStatus;

extern int afterDump;//flag para indicar se ocorreu um dump

//buffer memoria
extern int buffer[100][3];
extern int bufferPointer;

//configura uart
void initUart();

//envia os dados lidos no acerelometro
void uartSendString(int AccX, int AccY, int AccZ);

//interpreta e executa comandos enviados
void readCommand(char commandString[]);

//retorna pacote ack
void returnACK();	

//retorna pacote nak
void returnNAK();	

//retorna inicio do dump
void returnDumpFirst();

//retorna final do dump
void returnDumpLast();

//retorna status
void returnStatus();

//configura taxa de amostragem
void setValue();