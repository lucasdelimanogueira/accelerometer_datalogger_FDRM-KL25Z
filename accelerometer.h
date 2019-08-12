#ifndef SOURCES_ACC_H_
#define SOURCES_ACC_H_

#include "MKL25Z4.h"
#include "i2c.h"
#include "PIT.h"

 
// Endereço i2c do acelerômetro
#define MMA8451_ADDRESS 0x1D
// Endereço do registrador CTRL_REG1 no acelerômetro
#define MMA8451_CTRL_REG1 0x2A
// Endereço do registrador STATUS no acelerômetro
#define MMA8451_STATUS_REG 0x00
// Máscara do campo de medidas prontas no reg. STATUS
#define MMA8451_STATUS_MASK 0x08
// Endereço do primeiro registrador de dados no acelerômetro (X_MSB)
#define MMA8451_DATA_REG 0x01
 
// Variáveis espelhos dos registradores do acelerômetro
extern uint8_t acc_ctrl,acc_status;
extern uint8_t accs_regs[6];
// Acelerações nos 3 eixos
extern int16_t accX, accY, accZ;
 
 
// Função para passar o acelerômetro para modo ativo (on) ou standby (off)
void accOnOff(int On);


//ler valores e salva em accX, accY, accZ
void readAcc();

void accInit();

#endif /* SOURCES_ACC_H_ */