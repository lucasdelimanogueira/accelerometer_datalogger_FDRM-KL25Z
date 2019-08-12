//COMANDOS PARA ESCREVER NA FLASH
#define FLASH_WRITE_LONGWORD  0x06 //Escreve uma palavra de 32bits na flash
#define FLASH_ERASE_SECTOR    0x09 //Apaga um setor da flash indicado
//ENDERECO INICIAL DO SETOR DA FLASH QUE SERA MANIPULADO
#define FLASH_SECTOR_ADDRESS  0x00010000 //Setor da flash que estamos utilizando

//VARIAVEIS GLOBAIS

//FLASH:
extern long int *pflash_sector; //ponteiro para o endereço da flash onde guardaremos os dados


int flash_EraseSector(long int sector);
int flash_writeLongWord(long int locat,long int value);
