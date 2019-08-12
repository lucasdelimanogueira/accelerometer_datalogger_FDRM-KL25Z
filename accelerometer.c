#include "accelerometer.h"

void accOnOff(int On){
    // Lê o registrador
    acc_ctrl = i2c_ReadRegister(I2C0,MMA8451_ADDRESS,MMA8451_CTRL_REG1);
    // Modifica a cópia
    if(On){
        acc_ctrl |= 1; // seta o campo ACTIVE
    } else {
        acc_ctrl &= ~1; // reseta o campo ACTIVE
    }
    // Escreve de volta
    i2c_WriteRegister(I2C0,MMA8451_ADDRESS,MMA8451_CTRL_REG1,acc_ctrl);
}

void readAcc(){
	i2c_Start(I2C0); // Liga o periférico I2C0
        accOnOff(1);     // Liga o acelerômetro
        acc_status = 0;
        // Espera que a medida esteja feita
        while(!(acc_status & MMA8451_STATUS_MASK)){ 
            acc_status = i2c_ReadRegister(I2C0,MMA8451_ADDRESS,MMA8451_STATUS_REG);
        }
        // Lê os 6 registradores de dados (x, y e z, MSP e LSB)
        i2c_ReadMultRegister(I2C0,MMA8451_ADDRESS,MMA8451_DATA_REG,6,&accs_regs[0]);
        accOnOff(0);    // Desliga o acelerômetro
        i2c_Stop(I2C0); // Desliga o periférico I2C
				
				// Converte para 16 bits
				accX = (((int16_t)accs_regs[0]<<8) + ((int16_t)accs_regs[1]))/4;
        accY = (((int16_t)accs_regs[2]<<8) + ((int16_t)accs_regs[3]))/4;
        accZ = (((int16_t)accs_regs[4]<<8) + ((int16_t)accs_regs[5]))/4;

}

void accInit(){
	//Inicializa o I2C0, usando a opção ALT0 (pinos PE24 e PE25)
  i2c_Init(I2C0,ALT0,MULT0,0x14);

	SystemCoreClockUpdate();
	//Inicializa PIT canal 0
  pit_Init(pitIntervalMeasure*(SystemCoreClock/1000),0);
  //Inicia PIT canal 0
  pit_Start(0);
  // Incializa PIT canal 1
  pit_Init(pitIntervalLight*(SystemCoreClock/1000),1);
  // Inicia PIT canal 1
  pit_Start(1);
	

}
