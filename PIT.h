#include "MKL25Z4.h"

extern int pitIntervalMeasure;
extern int pitIntervalLight;

int pit_Init(unsigned int value,int ch);
void pit_Start(int ch);
void pit_Stop(int ch);
