#include <stdio.h>
#include <string.h>

#include "W6100.h"
#include "trng.h"



void trng_start()
{
	WIZCHIP_WRITE(RNGSTR,1);	
	printf("RNG_START\r\n");
}
void trng_stop()
{
	WIZCHIP_WRITE(RNGSTR,0);
	printf("RNG_STOP\r\n");
}
void trng_init()
{
	trng_start();		
	
//	WIZCHIP_WRITE(WZ_RNG_CTRL,(1<<7) | (1<<4) | (1<<3) | (1<<2) | (0<<1) | (0<<0)); //000F version
	WIZCHIP_WRITE(RNGCR,(1<<2) | (1<<1) | (0<<0)); //000A version
}

void wait_trng()
{
	uint8_t pre_data, post_data;
	
	while(!WIZCHIP_READ(RNGCHKR));
//	printf("TRNG_OK\r\n");
	pre_data = WIZCHIP_READ(RNGCDR);	
	do{
			post_data = WIZCHIP_READ(RNGCDR);
			if(pre_data != post_data) break;
		}while(pre_data == post_data);
//	printf("PERMIT_TRNG_READ\r\n");
}

uint32_t trng_latch()
{		
	uint32_t result_rng=0;

//000A version		
	result_rng = WIZCHIP_READ(RNGVR)<<24 | WIZCHIP_READ(RNGVR)<<16 | WIZCHIP_READ(RNGVR)<<8 | WIZCHIP_READ(RNGVR);
	printf("%10u\r\n",result_rng);	
	
//000F version	
//	while(!WIZCHIP_READ(WZ_TRNG_FLAG));
//	result_rng = WIZCHIP_READ(WZ_TRNG_VALUE3)<<24 | WIZCHIP_READ(WZ_TRNG_VALUE2)<<16 | WIZCHIP_READ(WZ_TRNG_VALUE1)<<8 | WIZCHIP_READ(WZ_TRNG_VALUE0);
//	printf("%10u\r\n",result_rng);	
	
	return result_rng;
}
