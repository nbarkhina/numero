#ifndef TI83HW_H
#define TI83HW_H
#include "corecalc.h"
#include "ti_stdint.h"

STDINT_t* INT83_init(timer_context_t *);
int device_init_83(CPU_t *, BOOL);
int memory_init_83(memc *);

void port0_83(CPU_t *, device_t *);
void port2_83(CPU_t *, device_t *);
void port3_83(CPU_t *, device_t *);
void port4_83(CPU_t *, device_t *);
void port6_83(CPU_t *, device_t *);
void port7_83(CPU_t *, device_t *);
void port14_83(CPU_t *, device_t *);


#endif 
