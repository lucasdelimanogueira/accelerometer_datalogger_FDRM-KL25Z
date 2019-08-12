#include "PIT.h"

/*****************************************************************************************
*
*****************************************************************************************/
int pit_Init(unsigned int value,int ch)
{
 // Verifica parametros
 if((ch < 2))
 {
 // Habilita clock para o PIT
 SIM_SCGC6 |= SIM_SCGC6_PIT_MASK;
  
 // Habilita PIT Module Control Register (PIT_MCR)
 PIT_MCR = 0;
  
 // Configura contador
 PIT_LDVAL_REG(PIT,ch) = value;
  
 // Enable interrupt and enable timer
 //PIT_TCTRL0 |= PIT_TCTRL_TIE_MASK | PIT_TCTRL_TEN_MASK;
 PIT_TCTRL_REG(PIT,ch) |= PIT_TCTRL_TIE_MASK | PIT_TCTRL_TEN_MASK;
  
 // Enable External Interrupt
 NVIC_EnableIRQ(PIT_IRQn);
  
 return 1;
 }
 else
 {
 return 0;
 }
}
/*****************************************************************************************
*
*****************************************************************************************/
void pit_Start(int ch)
{
 PIT_TCTRL_REG(PIT,ch) |= PIT_TCTRL_TEN_MASK;
}
/*****************************************************************************************
*
*****************************************************************************************/
void pit_Stop(int ch)
{
 PIT_TCTRL_REG(PIT,ch) &= ~PIT_TCTRL_TEN_MASK;
}