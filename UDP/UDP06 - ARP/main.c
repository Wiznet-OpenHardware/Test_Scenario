#include <stdio.h>
#include <math.h>


#include "wizchip_conf.h"

#include "test_list.h"

//#include "loopback.h"

//#include "W5100S_TEST_OS.h"

//#include "inttypes.h"

#include "stm32f4xx_gpio.h"

#include "stm32f4xx_exti.h"

#include "W5100SRelFunctions.h"

#include "socket.h"

#include "HALInit.h"

#include "config.h"



#define CLKSEL



#define SERVER



#ifdef SERVER

wiz_NetInfo gWIZNETINFO = { .mac = {0x00,0x08,0xdc,0xff,0xff,0x04},

							.ip = {192,168,0,4},

							.sn = {255, 255, 255, 0},

							.gw = {192, 168,0 , 254},

							.dns = {168, 126, 63, 1},

							.dhcp = NETINFO_STATIC};



#else

wiz_NetInfo gWIZNETINFO = { .mac = {0x00,0x08,0xdc,0xff,0xff,0x03},

							.ip = {192,168,0, 3},

							.sn = {255, 255, 255, 0},

							.gw = {192, 168, 138, 254},

							.dns = {168, 126, 63, 1},

							.dhcp = NETINFO_STATIC};

#endif









#define ETH_MAX_BUF_SIZE	2048





unsigned char ethBuf0[ETH_MAX_BUF_SIZE];

unsigned char data_buf[ETH_MAX_BUF_SIZE];

unsigned char ethBuf1[ETH_MAX_BUF_SIZE];

unsigned char ethBuf2[ETH_MAX_BUF_SIZE];

unsigned char ethBuf3[ETH_MAX_BUF_SIZE];







uint8_t bLoopback = 1;

uint8_t bRandomPacket = 0;

uint8_t bAnyPacket = 0;

uint16_t pack_size = 0;



void print_network_information(void);

//void Read_reg_map(void);

//void Read_sn_reg_map(uint8_t sn);

void EnterCris();

void ExitCris();



unsigned char data_buf[2048];


void set_phy_opmode(uint8_t auto_n, uint8_t spd_n, uint8_t dpx_n)
{
	volatile uint16_t i , j;
	volatile unsigned long linktime;
    volatile uint8_t linkstat;

	uint8_t opmode, tphycr1;
	opmode = (auto_n & 0x01);
	opmode <<= 1;
	opmode |= (spd_n & 0x01);
	opmode <<= 1;
	opmode |= (dpx_n & 0x01);

	printf("\r\nopmode = %02X : %s %d %c\r\n",opmode, (auto_n) ? "Fix" : "Auto", spd_n ? 10:100,(dpx_n) ? 'H' : 'F');

	//FSMCLowSpeed();
	for(i = 0 ; i < 2000 ; i++);

	while(getVER() != 0x51){printf("*"); for(i=0;i<1000;i++);};

	setPHYCFGR(0x53);
	setPHYCR0(opmode);

#ifdef PHY_RETRY
PHY_RESET_RETRY:
#endif
	tphycr1 = getPHYCR1();
	setPHYCFGR(0x53);
	setPHYCR1(tphycr1 | 0x01 );
	while(getPHYCR1() & 0x01);
	for(i = 0 ; i < 3000 ; i++); //200us
//	while((getPHYCR1() & 0x01) == 0x00)
//	{
//		setPHYCR1(tphycr1 | 0x01);
//	}
	while(getPHYSR0() & 0x80);
    //TIM2_settimer();
	for(i = 0 ; i < 3000 ; i++); //200us
	while( (getPHYSR0() & 0x01) != 0x01 )
		{	//printf("test = %x\r\n",getPHYSR0());
#ifdef PHY_RETRY
		 if(TIM2_gettimer() > 100)
		 {
			 printf("Retry Phy config\r\n");
			 goto PHY_RESET_RETRY;
		 }
#endif
		}
	linktime = TIM2_gettimer();
	linkstat = getPHYSR0();
	printf("\r\nPHY LINK OK : Stat %d%c, Time = %d.%d\r\n",
			((linkstat >> 1) & 0x01) ? 10 : 100, ((linkstat >> 2) & 01) ? 'H' : 'F',
			linktime/10, linktime % 10);
	while(getVER() != 0x51)
	{
		printf("-");
		for(i=0;i<1000;i++);
	}
	//FSMCHighSpeed();
	for(i=0; i<2000; i++);
	while(getVER() != 0x51)
	{
		printf("-");
		for(i=0;i<1000;i++);
	}

}

// auto_n : 0 - auto, 1 - fixed0
// spd_n : 0 - 100M , 1 - 10M
// dpx_n : 0 - FDX,   1 - HDX
void set_phy_mdc(uint8_t auto_n, uint8_t spd_n, uint8_t dpx_n)
{
	volatile uint16_t i, j;
	volatile unsigned long linktime;
    volatile uint8_t linkstat;
	uint16_t bmcr;
//	wiz_mdio_write(0x00, 0x8000); // Reset;
//	while(wiz_mdio_read(0x00) & 0x8000)
//	{
//		for(i=0; i<10000;i++);
//	}
	bmcr = wiz_mdio_read(0x00);
	printf("bmcr=%04X\r\n", bmcr);
	if(!auto_n)
	{
		bmcr = bmcr | (1 << 12);
		bmcr = bmcr |  (1 << 13);
		bmcr = bmcr |  (1<<8);
	}
	else
	{

		bmcr = bmcr & (~(1<<12));
		if(spd_n) bmcr = bmcr & (~(1<<13));
		else      bmcr = bmcr |  (1 << 13);
		if(dpx_n) bmcr = bmcr & (~(1<<8));
		else      bmcr = bmcr |  (1<<8);
	}
	printf("\r\BMCR = %04X : %s %d %c\r\n",bmcr, (auto_n) ? "Fix" : "Auto", spd_n ? 10:100,(dpx_n) ? 'H' : 'F');
//	wiz_mdio_write(0x00, bmcr);
	wiz_mdio_write(0x00, bmcr | 0x8000);
	//wiz_mdio_read(0x00);
	//while(getPHYSR0() & 0x01);
	TIM2_settimer();
	while(TIM2_gettimer() < 1);
	//wiz_mdio_write(0x00, bmcr & ~0x8000);
	//while(getPHYSR0() & 0x80);
    TIM2_settimer();
	while( (getPHYSR0() & 0x01) != 0x01 )
		{
#ifdef PHY_RETRY
		 if(TIM2_gettimer() > 100)
		 {
			 printf("Retry Phy config\r\n");
			 goto PHY_RESET_RETRY;
		 }
#endif
		}
	linktime = TIM2_gettimer();

    linkstat = getPHYSR0();
	printf("\r\nPHY LINK OK : Stat %d%c, Time = %d.%d\r\n",
			((linkstat >> 1) & 0x01) ? 10 : 100, ((linkstat >> 2) & 01) ? 'H' : 'F',
			linktime/10, linktime % 10);
}

//0 - Ethernet PHY Power up
//1 - Ethernet PHY Power Down
void set_phy_power_mode(uint8_t mode)
{
	volatile uint32_t i;

	FSMCLowSpeed();
	for(i = 0 ; i < 3000 ; i++);

	while(getVER() != 0x51){printf("L"); for(i=0;i<1000;i++);};

	// Power UP
	if(!mode)
	{
		setPHYCFGR(0x53);
		setPHYCR1(getPHYCR1() & ~(0x20));
		for(i = 0 ; i < 3000 ; i++);
		FSMCHighSpeed();
		for(i = 0 ; i < 3000 ; i++);
		while(getVER() != 0x51){printf("H"); for(i=0;i<1000;i++);};
		printf("\r\nEthernet Power UP!!!!!\r\n");
	}
	else
	{
		setPHYCFGR(0x53);
		setPHYCR1(getPHYCR1() | (0x20));
		for(i = 0 ; i < 3000 ; i++);
		while(getVER() != 0x51){printf("L"); for(i=0;i<1000;i++);};
		printf("\r\nEthernet Power DOWN!!!!!\r\n");
	}
}

//0 - System Clock 100MHz
//1 - System Clock 25MHz
void set_sysclock_mode(uint8_t mode)
{
	volatile uint32_t i;

	FSMCLowSpeed();
	for(i = 0 ; i < 3000 ; i++);

	while(getVER() != 0x51){printf("L"); for(i=0;i<1000;i++);};

	// 100MHz
	if(!mode)
	{
		setCHIPCFGR(0xCE);
		setMR2(getMR2() & ~(0x80));
		for(i = 0 ; i < 3000 ; i++);
		FSMCHighSpeed();
		for(i = 0 ; i < 3000 ; i++);
		while(getVER() != 0x51){printf("H"); for(i=0;i<1000;i++);};
		printf("\r\nSystem Clock changed to 100MHz!!!!!\r\n");
	}
	else
	{
		setCHIPCFGR(0xCE);
		setMR2(getMR2() | (0x80));
		for(i = 0 ; i < 3000 ; i++);
		while(getVER() != 0x51){printf("H"); for(i=0;i<1000;i++);};
		printf("\r\nSystem Clock changed to 25MHz!!!!!\r\n");
	}
}

int main(void)

{

	volatile int i,cnt=0xff, sn=0;

	volatile int j,k,size, connect_cnt=0;

	uint8_t dest_ip[4]={192,168,0,128};

	uint8_t bLink = 0;

	uint8_t uart_menu;
	int ret;

	for(i=0; i<ETH_MAX_BUF_SIZE; i=i+1) {

		if(cnt==0xff) cnt=0x00;

		else cnt++;

		data_buf[i]=cnt;



	}

	for(i=0; i<ETH_MAX_BUF_SIZE; i=i+2) { ethBuf0[i]=0xaa;}

	for(i=1; i<ETH_MAX_BUF_SIZE; i=i+2) { ethBuf0[i]=0x55;}





	gpioInitialize();

	//W5100SModeSelect();

	GPIO_ResetBits(SPI_ENABLE_Port, SPI_ENABLE_Pin);

			GPIO_ResetBits(SPI_SEL_Port, SPI_SEL1_Pin);

			GPIO_ResetBits(SPI_SEL_Port, SPI_SEL0_Pin);

	usartInitialize();





#if _WIZCHIP_IO_MODE_ & _WIZCHIP_IO_MODE_SPI_

	/* SPI method callback registration */

	reg_wizchip_spi_cbfunc(spiReadByte, spiWriteByte);



	/* CS function register */

	reg_wizchip_cs_cbfunc(csEnable,csDisable);

#else

	/* Indirect bus method callback registration */

	//reg_wizchip_bus_cbfunc(busReadByte, busWriteByte);

#endif





	reg_wizchip_cris_cbfunc(EnterCris,ExitCris);



#if _WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_BUS_INDIR_

	FSMCInitialize();

#else

	spiInitailize();

#endif



	resetAssert();

	for(i = 0 ; i < 2000 ; i++);

	resetDeassert();

	for(i = 0 ; i < 200 ; i++)
	{
		for( j = 0 ; j < 10000 ; j++){}
	}





#ifdef CLKSEL

    CHIPUNLOCK();

    setMR2(MR2_CLKSEL | MR2_G_IEN);

    while(getVER()!=0x51);

	//printf("\r\nCHIP Version: %02x\r\n", getVER());

    setIMR(0x01);



#else

    while(getVER()!=0x51);

    printf("\r\nCHIP Version: %02x\r\n", getVER());

    FSMCHighSpeed();

#endif

#define	PHY_AUTO
#ifndef PHY_AUTO
	setPHYCFGR(0x53);
	setPHYCR0(0x07); // 10H

	//setPHYCR0(0x06); // 10F
	//setPHYCR0(0x04); // 100F
	//setPHYCR0(0x05); //100H
//	setPHYCR1(getPHYCR1()&0xfe);
	uint8_t tphycr1 = getPHYCR1();
	setPHYCR1(tphycr1 & 0xFE );
	while(getPHYCR1() & 0x01);

//	FSMCLowSpeed();
	for(i = 0 ; i < 10000 ; i++)
	{
		for( j = 0 ; j < 10000 ; j++){}
	}
	while((getPHYCR1() & 0x01) == 0x00)
	{
		setPHYCFGR(0x53);
		setPHYCR1(tphycr1 | 0x01);
	}
#endif
	FSMCHighSpeed();
	for(i = 0 ; i < 10000 ; i++)
	{
		for( j = 0 ; j < 5000 ; j++){}
	}


	printf(" PHYMODE:%02x\r\n",getPHYACR());

	printf(" CHIP Version: %02x\r\n", getVER());

	wizchip_setnetinfo(&gWIZNETINFO);




	printf("Register value after W5100S initialize!\r\n");

	print_network_information();

///////////////////////////////////////////////////////
/////////// Enter TEST Scenario //////////////////////
//////////////////////////////////////////////////////
uint8_t test_buf[]={"ARP Test"};
		uint8_t udp_destip[4] = {192,168,0,111};
		uint16_t udp_destmac[6] = {0xFF,0xAA,0x14,0xE7,0x77,0xBF};
		uint16_t udp_destport = 5000;
		uint16_t port = 3000;
		uint8_t dst_ip[4];
		uint8_t dst_mac[6];



		while(1){
			   switch(getSn_SR(0))
			    {
			        case SOCK_CLOSED:
			        	setSn_IMR(0,0xff);
			        	ret = socket(0, Sn_MR_UDP , port, 0x00);
			        	printf("ret = %d\r\n",ret);
			        	if(ret == -1) break;

			            break;
			        case SOCK_UDP:
			            ret = sendto(0, test_buf, sizeof(test_buf), udp_destip, udp_destport);
			        	if(ret < 0)
			            {
			                printf("%d: sendto error. %ld\r\n",0, ret);
			                close(0);
			                return 2;
			            }
			            break;

			    }
		}

/////////////////////////////////////////////////////////////////



}





void delay_ms(uint8_t ms){

	uint16_t time;

	WIZCHIP_WRITE(0x88, 0xff);

	while(1){

		time=((WIZCHIP_READ(0x82)<<8)|(WIZCHIP_READ(0x83)));

		if( (ms)*10 <= time) break;

	}

}



void print_network_information(void)

{

	wizchip_getnetinfo(&gWIZNETINFO);

	printf("Mac address: %02x:%02x:%02x:%02x:%02x:%02x\n\r",gWIZNETINFO.mac[0],gWIZNETINFO.mac[1],gWIZNETINFO.mac[2],gWIZNETINFO.mac[3],gWIZNETINFO.mac[4],gWIZNETINFO.mac[5]);

	printf("IP address : %d.%d.%d.%d\n\r",gWIZNETINFO.ip[0],gWIZNETINFO.ip[1],gWIZNETINFO.ip[2],gWIZNETINFO.ip[3]);

	printf("SM Mask	   : %d.%d.%d.%d\n\r",gWIZNETINFO.sn[0],gWIZNETINFO.sn[1],gWIZNETINFO.sn[2],gWIZNETINFO.sn[3]);

	printf("Gate way   : %d.%d.%d.%d\n\r",gWIZNETINFO.gw[0],gWIZNETINFO.gw[1],gWIZNETINFO.gw[2],gWIZNETINFO.gw[3]);

	printf("DNS Server : %d.%d.%d.%d\n\r",gWIZNETINFO.dns[0],gWIZNETINFO.dns[1],gWIZNETINFO.dns[2],gWIZNETINFO.dns[3]);

}





/*W5100S interrupt handler*/

void EXTI15_10_IRQHandler(void)

{



	printf("///////////////////////////////\r\n");

	printf("EXTI handler\r\n");



	if(EXTI_GetITStatus(EXTI_Line14) != RESET)

	{

		/* Do something */

		printf("IR:%.2x, IR2:%.2x, SLIR:%.2x\r\n",getIR(), getIR2(), getSLIR() );

		printf("S0_IR : %.2x, S1_IR : %.2x, S2_IR : %.2x, S3_IR : %.2x\r\n",getSn_IR(0),getSn_IR(1),getSn_IR(2),getSn_IR(3));

		EXTI_ClearFlag(EXTI_Line14);

	}

	printf("///////////////////////////////\r\n");



}







void EnterCris(){

	NVIC_DisableIRQ(EXTI15_10_IRQn);



}



void ExitCris(){



	NVIC_EnableIRQ(EXTI15_10_IRQn);

}
