#ifndef SOURCES_ACC_H_
#define SOURCES_ACC_H_

#include "MKL25Z4.h"
#include "i2c.h"
#include "PIT.h"

 
// Endere�o i2c do aceler�metro
#define MMA8451_ADDRESS 0x1D
// Endere�o do registrador CTRL_REG1 no aceler�metro
#define MMA8451_CTRL_REG1 0x2A
// Endere�o do registrador STATUS no aceler�metro
#define MMA8451_STATUS_REG 0x00
// M�scara do campo de medidas prontas no reg. STATUS
#define MMA8451_STATUS_MASK 0x08
// Endere�o do primeiro registrador de dados no aceler�metro (X_MSB)
#define MMA8451_DATA_REG 0x01
 
// Vari�veis espelhos dos registradores do aceler�metro
extern uint8_t acc_ctrl,acc_status;
extern uint8_t accs_regs[6];
// Acelera��es nos 3 eixos
extern int16_t accX, accY, accZ;
 
 
// Fun��o para passar o aceler�metro para modo ativo (on) ou standby (off)
void accOnOff(int On);


//ler valores e salva em accX, accY, accZ
void readAcc();

void accInit();

#endif /* SOURCES_ACC_H_ */