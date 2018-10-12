#include <stdio.h>
#include <string.h>

#include "stm32f4xx_conf.h"
#include "halConfiguration.h"
#include "usart.h"

#include "Types.h"
#include "netconfig.h"
#include "W6100RelFunctions.h"
#include "loopback.h"
#include "tcp_test.h"
#include "trng.h"
#include "socket.h"

uint8 ip_ver = AF_INET;
//uint8 ip_ver = AF_INET6;
//uint8 ip_ver = AF_INET_DUAL;

uint8 con_ver = AF_INET;
//uint8 con_ver = AF_INET6;

typedef struct pingmsg
{
  uint8_t  Type; 		// 0 - Ping Reply, 8 - Ping Request
  uint8_t  Code;		// Always 0
  int16_t  CheckSum;	// Check sum
  int16_t  ID;	            // Identification
  int16_t  SeqNum; 	// Sequence Number
  int8_t	 Data[32];// Ping Data  : 1452 = IP RAW MTU - sizeof(Type+Code+CheckSum+ID+SeqNum)
	//for ipv6
	
} PINGMSGR;
typedef struct pshead
{
  uint8	 srcip[16];
	uint8	 dstip[16];
	uint32_t	 upperlen;
	uint16_t	 zero_pend;
	uint16_t	 next_head;
	
} PSHEAD;

uint16_t ping_htons(uint16_t hostshort);
uint16_t checksum(uint8_t * data_buf, uint16_t len);
uint16_t checksum6(uint8_t * data_buf, uint16_t len, uint8_t * data_buf1, uint16_t len1);

uint8 ch_status[MAX_SOCK_NUM] = { 0, }; /** 0:close, 1:ready, 2:connected */
CONFIG_MSG Config_Msg;
CHCONFIG_TYPE_DEF Chconfig_Type_Def = {5000,{192, 168, 127, 219},{ 0xFE, 0x80, 0x00, 0x00, 
                                          0x00, 0x00, 0x00, 0x00, 
                                          0x30, 0xf5, 0xe4, 0x7a,   
                                          0x1f, 0xed, 0xd8, 0x2e  }};

CHCONFIG_TYPE_DEF virtual_ip = {5000,{192,168,127,87},{ 0xFE, 0x80, 0x00, 0x00, 
                                          0x00, 0x00, 0x00, 0x00, 
                                          0x30, 0xf5, 0x4e, 0x7a,   
                                          0x1f, 0xed, 0xd8, 0x2e  }};
CHCONFIG_TYPE_DEF multi_ip = {5000,{192,168,127,67},{ 0xFE, 0x80, 0x00, 0x00, 
                                          0x00, 0x00, 0x00, 0x00, 
                                          0xad, 0x43, 0x6f, 0x2a,   
                                          0x31, 0xaa, 0x0d, 0x91  }};

uint8 txsize[MAX_SOCK_NUM] = {2,2,2,2,2,2,2,2};
uint8 rxsize[MAX_SOCK_NUM] = {2,2,2,2,2,2,2,2};

uint8 Enable_DHCP    = 0;    //OFF;
uint16 i;

//////////////////////////////////////////////////////////////////////
/*******************MAC ADDRESS**************************/
//////////////////////////////////////////////////////////////////////
uint8 WIZ_MAC[6]     = {0x00, 0x08, 0xdc, 0x57, 0x57, 0x62}; //MAC Address
uint8 PEER_MAC[6]     = {0xF4, 0x4D, 0x30, 0xAC, 0xB6, 0x1C}; //MAC Address
//////////////////////////////////////////////////////////////////////
/*******************IPv4  ADDRESS**************************/
//////////////////////////////////////////////////////////////////////
uint8 VIRTUAL_MAC[6]     = {0x00, 0x08, 0xdc, 0x57, 0x57, 0x67}; //MAC Address
uint8 VIRTUAL_IP[4]		= {192,168,127,87};
uint8 VIRTUAL_IP6[16]		= {0xFE, 0x80, 0x00, 0x00, 
                                          0x00, 0x00, 0x00, 0x00, 
                                          0x30, 0xf5, 0x4e, 0x7a,   
                                          0x1f, 0xed, 0xd8, 0x2e};
//////////////////////////////////////////////////////////////////////
/*******************IPv4  ADDRESS**************************/
//////////////////////////////////////////////////////////////////////
uint8 WIZ_IP[4]      = {192, 168, 127, 102};                   //IP Address
uint8 WIZ_realIP[4]      = {222,98,173,219};
uint8 WIZ_SubNet[4]  = {255, 255, 255, 0};                   //SubnetMask Address

#ifdef FOR_TEST_PPPoE
uint8 WIZ_GateWay[4] = {192,168,55,1};                    //Gateway Address
#else
uint8 WIZ_GateWay[4] = {222,98,173,254};                    //Gateway Address
#endif

uint16 WIZ_My_PORT = 5000;                                 //DST_IP port

uint8 WIZ_Dest_IP_virtual[4] = {192, 168, 127, 67};                  //DST_IP Address 
uint8 WIZ_Dest_IP_real[4] = {222, 98, 173, 219};                  //DST_IP Address 
uint8 WIZ_Dest_IP_Google[4] = {216, 58, 200, 174};                  //DST_IP Address 
uint8 WIZ_Dest_IP_Test[4] = {192,168,177,200};


uint8 mcastipv4_0[4] ={239,1,2,3};
uint8 mcastipv4_1[4] ={239,1,2,4};
uint8 mcastipv4_2[4] ={239,1,2,5};
uint8 mcastipv4_3[4] ={239,1,2,6};

uint16 WIZ_Dest_PORT = 5000;                                 //DST_IP port

//////////////////////////////////////////////////////////////////////
/*******************IPv6  ADDRESS**************************/
//////////////////////////////////////////////////////////////////////

uint8_t Zero_IP[16] = {0x00, };

uint8 WIZ_LLA[16] = {0xfe,0x80, 0x00,0x00, 
                     0x00,0x00, 0x00,0x00,
                     0x02,0x08, 0xdc,0xff,
                     0xfe,0x57, 0x57, 0xaa
                    };

uint8 WIZ_GUA[16] = {0x20,0x01,0x02,0xb8,
                     0x00,0x10,0x00,0x01,
                     0x02,0x08, 0xdc,0xff,
                     0xfe,0x57, 0x57, 0x67
                    };

uint8 WIZ_6SubNet[16] = {0xff,0xff,0xff,0xff,
                         0xff,0xff,0xff,0xff,
                         0x00,0x00,0x00, 0x00,
                         0x00,0x00,0x00,0x00
                        };

uint8 WIZ_GateWay6[16] = {0xfe, 0x80, 0x00,0x00, 
                          0x00,0x00,0x00,0x00,
                          0x02,0x00, 0x87,0xff, 
                          0xfe,0x08, 0x4c,0x81
                         };

uint8_t DestIP6_L[16] = { 0xFE, 0x80, 0x00, 0x00, 
                                          0x00, 0x00, 0x00, 0x00, 
                                          0x30, 0xf5, 0xe4, 0x7a,   
                                          0x1f, 0xed, 0xd8, 0x2e  };

uint8_t DestIP6_G[16] = { 0xfe, 0x80, 0x00, 0x00, 
                                         0x00, 0x00, 0x00, 0x00, 
                                          0x02, 0x00, 0x87, 0xff,   
                                          0xfe, 0x08, 0x4c, 0x81  };

uint8_t DestIP6_GOOGLE[16] = { 0x24, 0x04, 0x68, 0x00, 
                                                 0x40, 0x0a, 0x08, 0x06, 
                                                 0x00, 0x00, 0x00, 0x00, 
                                                 0x00, 0x00, 0x20, 0x04 };

uint8_t MultiIP6_SMA[16] = {0xFF, 0x02, 0x00, 0x00, 
                                            0x00, 0x00, 0x00, 0x00, 
                                            0x00, 0x00, 0x00, 0x01,
                                            0xFF, 0x62, 0xb0, 0xc6 };

uint8_t MultiIP6_ANM[16] = {0xFF, 0x02, 0x00, 0x00, 
                                            0x00, 0x00, 0x00, 0x00, 
                                            0x00, 0x00, 0x00, 0x00,   
                                            0x00, 0x00, 0x00, 0x01 };

uint8_t MultiIP6_MGA_FF02[16] = {0xFF, 0x02, 0x00, 0x00,
                                                     0x00, 0x00, 0x00, 0x00,
                                                     0x00, 0x00, 0x00, 0x00,
                                                     0x00, 0xAB, 0xCD, 0xEF };

uint8_t MultiIP6_MGA_FF0E[16] = {0xFF, 0x0E, 0x00, 0x00,
                                                     0x00, 0x00, 0x00, 0x00,
                                                     0x00, 0x00, 0x00, 0x00,
                                                     0x00, 0xAB, 0xCD, 0xEF};

uint32 Multicast_Port = 15000;
unsigned char W6100_AdrSet[2][8] = {{2,2,2,2,2,2,2,2},{2,2,2,2,2,2,2,2}};

uint32_t presentTime = 0;
uint8 data_buf [TX_RX_MAX_BUF_SIZE]; // TX Buffer for applications

void Set_network(void);

int main(void)
{
	int ret, i = 0;
	  int s = 0;//, s0 = 0;

		uint32_t trng_value;
		uint8_t uart_menu;
		int status1;
	
		GPIO_Configuration();
    USART_Configuration();
    NVIC_Configuration();
		SetSysTick_config(1000);
		
		#if _WIZCHIP_IO_MODE_ & _WIZCHIP_IO_MODE_SPI_
		reg_wizchip_spi_cbfunc(spiReadByte, spiWriteByte);
		reg_wizchip_cs_cbfunc(csEnable,csDisable);
		reg_wizchip_cris_cbfunc(EnterCris, ExitCris);
		#else
		reg_wizchip_cs_cbfunc(csEnable,csDisable);
		reg_wizchip_cris_cbfunc(EnterCris, ExitCris);
		#endif

		#if _WIZCHIP_IO_MODE_ & _WIZCHIP_IO_MODE_SPI_
		Set_SPI_Mode();
		#else
		Set_Indirect_Mode();
		#endif
		
		Interface_init();

		delay_ms(60);

		Set_network();
    sysinit( W6100_AdrSet);
		
		while(1)
		{
				printf("-b : TCP06    - ack_packet_tx_nd0\r\n");				       
				
				do{
						status1 = TCP_CLIENT_ACK_PACKET_TX_test(0, WIZ_Dest_PORT, data_buf, ip_ver, 0, con_ver  );									
						if(status1 == -1) break;
				}while(status1 != 2);
				
				printf("------------------------------------------------end of loop------------------------------------------------\r\n");
        while(1);
		}
}

uint16_t ping_htons(uint16_t hostshort)
{
#if 1
  //#ifdef LITTLE_ENDIAN
	uint16_t netshort=0;
	netshort = (hostshort & 0xFF) << 8;

	netshort |= ((hostshort >> 8)& 0xFF);
	return netshort;
#else
	return hostshort;
#endif
}
uint16_t checksum(uint8_t * data_buf, uint16_t len)
{
  uint16_t sum, tsum, i, j;
  uint32_t lsum;

  j = len >> 1;
  lsum = 0;
  tsum = 0;
  for (i = 0; i < j; i++)
    {
      tsum = data_buf[i * 2];
      tsum = tsum << 8;
      tsum += data_buf[i * 2 + 1];
      lsum += tsum;
    }
   if (len % 2)
    {
      tsum = data_buf[i * 2];
      lsum += (tsum << 8);
    }
    sum = (uint16_t)lsum;
    sum = ~(sum + (lsum >> 16));
  return sum;
}
uint16_t checksum6(uint8_t * data_buf, uint16_t len, uint8_t * data_buf1, uint16_t len1)
{
  uint16_t sum, tsum, i, j;
  uint32_t lsum;

  j = len >> 1;
  lsum = 0;
  tsum = 0;
  for (i = 0; i < j; i++)
    {
      tsum = data_buf[i * 2];
      tsum = tsum << 8;
      tsum += data_buf[i * 2 + 1];
      lsum += tsum;
    }
		printf("%x\r\n",lsum);
		j = 1;//len1 >> 1;
		for (i = 0; i < j; i++)
    {
      tsum = data_buf1[i * 2];
      tsum = tsum << 8;
      tsum += data_buf1[i * 2 + 1];
      lsum += tsum;
    }
		printf("%x\r\n",lsum);
   if ((len+len1) % 2)
    {
      tsum = data_buf[i * 2];
      lsum += (tsum << 8);
    }
    sum = (uint16_t)lsum;
    sum = ~(sum + (lsum >> 16));
  return sum;
}

void Set_network(void)
{
    uint8 tmp_array[16];
    uint8 i;

    //delay_ms(1000);

    // MAC ADDRESS
    for (i = 0 ; i < 6; i++) Config_Msg.Mac[i] = WIZ_MAC[i];
    // Local IP ADDRESS
    Config_Msg.Lip[0] = WIZ_IP[0]; Config_Msg.Lip[1] = WIZ_IP[1]; Config_Msg.Lip[2] = WIZ_IP[2]; Config_Msg.Lip[3] = WIZ_IP[3];
    // GateWay ADDRESS
    Config_Msg.Gw[0] = WIZ_GateWay[0]; Config_Msg.Gw[1] = WIZ_GateWay[1]; Config_Msg.Gw[2] = WIZ_GateWay[2]; Config_Msg.Gw[3] = WIZ_GateWay[3];
    // Subnet Mask ADDRESS
    Config_Msg.Sub[0] = WIZ_SubNet[0]; Config_Msg.Sub[1] = WIZ_SubNet[1]; Config_Msg.Sub[2] = WIZ_SubNet[2]; Config_Msg.Sub[3] = WIZ_SubNet[3];

		//WIZCHIP_READ(WZ_RTR0);
		//printf("\r\n before NETCFGLOCK : %x\r\n", WIZCHIP_READ(WZ_NETCFG_LOCKR));
		//delay_ms(1000);
    // Network Config Register Unlock
    //printf("%.2x \r\n", getVERR());
	//_CS_Low;
	//_CS_High;
	//_CS_Low;
	//_CS_High;
		printf("VERSION(%x) = %.2x \r\n", VER,getVER());
		//NETCFG_UNLOCK();
	  NETCFG_UNLOCK();
		//delay_ms(1000);
		//printf("\r\n after NETCFGLOCK : %x\r\n", WIZCHIP_READ(WZ_NETCFG_LOCKR));

    //-- Set MAC Address
    setSHAR(Config_Msg.Mac);

    //-- Set Addresses for IPv4 address
    setSUBR(Config_Msg.Sub);
    setGAR(Config_Msg.Gw);
    setSIPR(Config_Msg.Lip);

    //-- Set Addresses for IPv6 address
    //setGW6R(WIZ_GateWay6);
    setLLAR(WIZ_LLA);
		//setLLAR(DestIP6_L);
    setGUAR(WIZ_GUA);
    setSUB6R(WIZ_6SubNet);
    setGA6R(WIZ_GateWay6);

    // Set DHCP
    Config_Msg.DHCP = Enable_DHCP;

    //Set RTR and RCR register
    setRTR(2000);
    setRCR(5);

    setSLRTR(2000);
    setSLRCR(5);
    //Init. TX & RX Memory size
    //sysinit(txsize, rxsize);

    printf("\r\n----------------------------------------- \r\n");
    printf("W6100E01-M4                       \r\n");
    printf("Network Configuration Information \r\n");
    printf("----------------------------------------- ");


    getSHAR(tmp_array);
    printf("\r\nMAC : %.2X.%.2X.%.2X.%.2X.%.2X.%.2X", tmp_array[0],tmp_array[1],tmp_array[2],tmp_array[3],tmp_array[4],tmp_array[5]);

    getSIPR (tmp_array);
    printf("\r\nIP : %d.%d.%d.%d", tmp_array[0],tmp_array[1],tmp_array[2],tmp_array[3]);

    getSUBR(tmp_array);
    printf("\r\nSN : %d.%d.%d.%d", tmp_array[0],tmp_array[1],tmp_array[2],tmp_array[3]);

    getGAR(tmp_array);
    printf("\r\nGW : %d.%d.%d.%d\r\n", tmp_array[0],tmp_array[1],tmp_array[2],tmp_array[3]);

    getGA6R(tmp_array);
    printf("\r\nGW6 : %.2X%.2X:%.2X%.2X:%.2X%.2X:%.2X%.2X:%.2X%.2X:%.2X%.2X:%.2X%.2X:%.2X%.2X",
                                   tmp_array[0], tmp_array[1], tmp_array[2], tmp_array[3],
                                   tmp_array[4], tmp_array[5], tmp_array[6], tmp_array[7],
                                   tmp_array[8], tmp_array[9], tmp_array[10],tmp_array[11],
                                   tmp_array[12],tmp_array[13],tmp_array[14],tmp_array[15]);

    getLLAR(tmp_array);
    printf("\r\nLLA : %.2X%.2X:%.2X%.2X:%.2X%.2X:%.2X%.2X:%.2X%.2X:%.2X%.2X:%.2X%.2X:%.2X%.2X",
                                   tmp_array[0], tmp_array[1], tmp_array[2], tmp_array[3],
                                   tmp_array[4], tmp_array[5], tmp_array[6], tmp_array[7],
                                   tmp_array[8], tmp_array[9], tmp_array[10],tmp_array[11],
                                   tmp_array[12],tmp_array[13],tmp_array[14],tmp_array[15]);
    getGUAR(tmp_array);
    printf("\r\nGUA : %.2X%.2X:%.2X%.2X:%.2X%.2X:%.2X%.2X:%.2X%.2X:%.2X%.2X:%.2X%.2X:%.2X%.2X",
                                   tmp_array[0], tmp_array[1], tmp_array[2], tmp_array[3],
                                   tmp_array[4], tmp_array[5], tmp_array[6], tmp_array[7],
                                   tmp_array[8], tmp_array[9], tmp_array[10],tmp_array[11],
                                   tmp_array[12],tmp_array[13],tmp_array[14],tmp_array[15]);

    getSUB6R(tmp_array);
    printf("\r\n6SM : %.2X%.2X:%.2X%.2X:%.2X%.2X:%.2X%.2X:%.2X%.2X:%.2X%.2X:%.2X%.2X:%.2X%.2X",
                                   tmp_array[0], tmp_array[1], tmp_array[2], tmp_array[3],
                                   tmp_array[4], tmp_array[5], tmp_array[6], tmp_array[7],
                                   tmp_array[8], tmp_array[9], tmp_array[10],tmp_array[11],
                                   tmp_array[12],tmp_array[13],tmp_array[14],tmp_array[15]);


     printf("\r\nNETCFGLOCK : %x\r\n", WIZCHIP_READ(NETLCKR));
}