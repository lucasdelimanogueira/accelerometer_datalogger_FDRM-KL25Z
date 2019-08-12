/*************************************************************
// Exemplo de uso da memória Flash na plataforma FRDM-KL25Z
// Autores: João Paulo Cerquinho Cajueiro
//  				José Rodrigues de Oliveira Neto
// Data: 17/abr/2019
// Última versão: 17/abr/2019
//
// Codigo baseado no disponivel em:
// https://community.arm.com/developer/ip-products/system/b/embedded-blog/posts/writing-data-to-flash-during-program-execution-with-the-freescale-kl25z-series
*************************************************************/
#include <MKL25Z4.h>

//COMANDOS PARA ESCREVER NA FLASH
#define FLASH_WRITE_LONGWORD  0x06 //Escreve uma palavra de 32bits na flash
#define FLASH_ERASE_SECTOR    0x09 //Apaga um setor da flash indicado
//ENDERECO INICIAL DO SETOR DA FLASH QUE SERA MANIPULADO
#define FLASH_SECTOR_ADDRESS  0x00010000 //Setor da flash que estamos utilizando


//VARIAVEIS GLOBAIS


 /*##############################################################################
												           FLASH
 *##############################################################################*/
/*------------------------------------------------------------------------------
 Apaga um setor da fash cujo o endereco e o parametro da funcao
 OBS: Antes de escrever na flash e necessario apagar o setor
 *------------------------------------------------------------------------------*/
int flash_EraseSector(long int sector)
{
	int temp1,temp2,temp3;
  // Dividindo o endereco do setor em 3 bytes antes de comecar a carregar os valores
	// nos registradores FCCOBn
  
	// ENDERECO:
  temp1 = (sector << 8); // primeiro manda o byte para o final da variável (bits 31-24)
  temp1 = temp1 >> 24;   // depois leva o byte para o primeiros oito bits (bits 7-0)
	temp2 = (sector << 16);// o motivo é zerar todo o int (que tem 32 bits) e deixar apenas
	temp2 = temp2 >> 24;	 // os 8 bits a serem gravados no registrador 
	temp3 = (sector << 24);// repete isso para os demais bytes
	temp3 = temp3 >> 24;  
    
	while(((FTFA->FSTAT)&(1UL << 7))==0x00)
	{
		//Espera os registradores estarem disponíveis para serem utilizados
		//Isso ocorre quando CCIF = '1'
	}
    
  if(!((FTFA->FSTAT)==0x80)) //Verifica se tem alguma flag levantada além do CCIF = '1'
  {
		FTFA->FSTAT = 0x30; //Limpa possiveis erros antigos (bits ACCERR e FPVIOL do registrador FTFA->FSTAT)
	}
	//Comando: Apagar setor da flash
  FTFA->FCCOB0 = FLASH_ERASE_SECTOR; 
  //Endereco:
	FTFA->FCCOB1 = temp1;    // bits 23-16
  FTFA->FCCOB2 = temp2;    // bits 15-8
  FTFA->FCCOB3 = temp3;    // bits  7-0

	// Limpa o bit CCIF para iniciar o comando
	// escrevendo 0x80 no registrador FSTAT
  FTFA->FSTAT = 0x80;  

  while(((FTFA->FSTAT)&(1UL << 7))==0x00)
	{
		// Espera terminar de apagar o setor da memória (CCIF = '1')
	}

   return 1;
}

/*------------------------------------------------------------------------------
  Escreve uma palavra de 32bits na flash no endereco indicado
  OBS: A sequencia de escrita segue o fluxograma indicado na figura 27-25 do manual KL25P80M48SF0RM.pdf
 *------------------------------------------------------------------------------*/
int flash_writeLongWord(long int locat,long int value)
{
	
	//Variáveis temporárias para guardar os dados e endereço
	int temp1,temp2,temp3,temp4,temp5,temp6,temp7;
	
	//AGRUPANDO O ENDERECO
  temp1 = (locat << 8);  // primeiro manda o byte para o final da variável (bits 31-24)
  temp1 = temp1 >> 24;   // depois leva o byte para o primeiros oito bits (bits 7-0)
  temp2 = (locat << 16); // o motivo é zerar todo o int (que tem 32 bits) e deixar apenas
  temp2 = temp2 >> 24;   // os 8 bits a serem gravados no registrador 
  temp3 = (locat << 24); 
  temp3 = temp3 >> 24;
	
	//AGRUPANDO O DADO
  temp4 = value >> 24;
  temp5 = (value << 8);
  temp5 = temp5 >> 24;
  temp6 = (value << 16);
  temp6 = temp6 >> 24;
  temp7 = (value << 24);
  temp7 = temp7 >> 24;


	while(((FTFA->FSTAT)&(1UL << 7))==0x00)
	{
		//Espera os registradores estarem disponíveis para serem utilizados
	}    
	
	if(!((FTFA->FSTAT)==0x80)) // Verifica se o comando anterior foi completado (bit CCIF do registrador FTFA->FSTAT)
	{
		FTFA->FSTAT = 0x30; //Limpa possíveis erros antigos (bits ACCERR e FPVIOL do registrador FTFA->FSTAT)
	}
	
	// Comando: Escreve palavra de 32bits na flash
	FTFA->FCCOB0 = FLASH_WRITE_LONGWORD;    
	// Endereco :
	FTFA->FCCOB1 = temp1;   // bits 23-16
	FTFA->FCCOB2 = temp2;   // bits 15-8
	FTFA->FCCOB3 = temp3;   // bits  7-0
	// Dados:
	FTFA->FCCOB4 = temp4;   // Byte 0
	FTFA->FCCOB5 = temp5;   // Byte 1
	FTFA->FCCOB6 = temp6;   // Byte 2
	FTFA->FCCOB7 = temp7;   // Byte 3
	
	// Limpa o bit CCIF para iniciar o comando
	// escrevendo 0x80 no registrador FSTAT
	FTFA->FSTAT = 0x80;     
                          
	
	while(((FTFA->FSTAT)&(1UL << 7))==0x00)
	{
		// Espera os registradores serem escritos
		// Quando terminar o bit CCIF vai para '1'
	}

  return 1;
}