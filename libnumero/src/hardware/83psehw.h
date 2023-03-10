#ifndef TI83PSEHW_H
#define TI83PSEHW_H
#include "corecalc.h"
#include "ti_stdint.h"

#ifndef LINK_READ
#define LINK_READ
#define LinkRead (((cpu->pio.link->host & 0x03) | (cpu->pio.link->client[0] & 0x03))^3)
#endif

typedef struct TIMER {
	/* determines which clock if any is used for time */
	unsigned long long lastTstates;
	double lastTicks;
	double divsor;
	BOOL loop;
	BOOL interrupt;
	BOOL underflow;
	BOOL generate;
	BOOL active;
	unsigned char clock;
	unsigned char count;
	unsigned char max;
} TIMER_t;

typedef struct XTAL {
	double lastTime;			/* actual real time of last tick */
	unsigned long long ticks;	/* ticks of the xtal timer */
	TIMER_t timers[3];
} XTAL_t;

typedef struct LINKASSIST {
	unsigned char link_enable;
	unsigned char in;
	unsigned char out;
	unsigned char working;
	BOOL receiving;
	BOOL read;
	BOOL ready;
	BOOL error;
	BOOL sending;
	double last_access;
	int bit;
} LINKASSIST_t;

typedef struct MD5 {
	/* 32 bit registers */
	union {
		struct {
			uint32_t a;
			uint32_t b;
			uint32_t c;
			uint32_t d;
			uint32_t x;
			uint32_t ac;
		};
		uint32_t reg[6];
	};
	uint8_t s;
	uint8_t mode ;
} MD5_t;

typedef struct DELAY {
	union {
		struct {
			unsigned char lcd1;
			unsigned char lcd2;
			unsigned char lcd3;
			unsigned char lcd4;
			unsigned char unkown;
			unsigned char mad; /*memory access delay*/
			unsigned char lcdwait;
		};
		unsigned char reg[7];
	};
} DELAY_t;

typedef struct CLOCK {
	unsigned char enable;
	unsigned long set;
	unsigned long base;
	double lasttime;
} CLOCK_t;

typedef struct USB {
	unsigned int USBLineState;		//Whether each line is low or high
	unsigned int USBEvents;			//Whether interrupts have occurred
	unsigned int USBEventMask;		//Whether interrupts should be generated when USB lines change
	BOOL LineInterrupt;
	BOOL ProtocolInterrupt;
	BOOL ProtocolInterruptEnabled;
	unsigned int DevAddress;		//Current USB device address
	int version;
	BOOL USBPowered;


	unsigned char Port4A;
	unsigned char Port4C;
	unsigned char Port54;
} USB_t;

enum USB_MASK {
	DPLUS_LOW_MASK = 0x01,
	DPLUS_HIGH_MASK = 0x02,
	DMINUS_LOW_MASK = 0x04,
	DMINUS_HIGH_MASK = 0x08,
	ID_LOW_MASK = 0x10,
	ID_HIGH_MASK = 0x20,
	VBUS_HIGH_MASK = 0x40,
	VBUS_LOW_MASK = 0x80,
};

typedef struct SE_AUX {
	CLOCK_t clock;
	DELAY_t delay;
	MD5_t md5;
	LINKASSIST_t linka;
	XTAL_t xtal;
	unsigned char gpio;
	unsigned long long gpio_write_tstates;
	double gpio_write_elapsed;
	USB_t usb;
} SE_AUX_t;

int device_init_83pse(CPU_t*, int model);
int memory_init_83pse(memc *);
int memory_init_84p(memc *);
int memory_init_84pcse(memc *mc);
void UpdateDelays(CPU_t *cpu, DELAY_t *delay);
void handlextal(CPU_t *cpu,XTAL_t* xtal);
void mod_timer(CPU_t *cpu, XTAL_t* xtal);
void GenerateUSBEvent(CPU_t *cpu, USB_t *usb, int bit, BOOL lowToHigh);
void md5ports(CPU_t *cpu, device_t *dev);
void delay_ports(CPU_t *cpu, device_t *dev);
//CLOCK
void clock_enable(CPU_t *cpu, device_t *dev);
void clock_set(CPU_t *cpu, device_t *dev);
void clock_read(CPU_t *cpu, device_t *dev);

void port0_83pse(CPU_t *, device_t *);
void port2_83pse(CPU_t *, device_t *);
void port3_83pse(CPU_t *, device_t *);
void port4_83pse(CPU_t *, device_t *);
void port6_83pse(CPU_t *, device_t *);
void port7_83pse(CPU_t *, device_t *);
void port14_83pse(CPU_t *, device_t *);
uint8_t GetCPUSpeed(CPU_t *);

#endif 
