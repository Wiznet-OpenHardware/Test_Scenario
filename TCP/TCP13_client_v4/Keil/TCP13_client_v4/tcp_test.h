#ifndef _TCP_TEST_H
#define _TCP_TEST_H


#include "stm32f4xx.h"
#include "Types.h"
#include "W6100.h"

//void sysinit( uint8_t * tx_size, uint8_t * rx_size  );

int TCP_Recv_Test(SOCKET s, uint16_t port, uint8_t *data_buf, uint8_t ip_ver);

int TCP_SERVER_CON_ACTIVE_DISCON_test(SOCKET s, uint16 port, uint8 * data_buf, uint8 ip_ver );
int TCP_SERVER_CON_PASSIVE_DISCON_test(SOCKET s, uint16 port, uint8 * data_buf, uint8 ip_ver );
int TCP_CLIENT_CON_ACTIVE_DISCON_test(SOCKET s, uint16 port, uint8 * data_buf, uint8 ip_ver, uint8 con_ver );
int TCP_CLIENT_CON_PASSIVE_DISCON_test(SOCKET s, uint16 port, uint8 * data_buf, uint8 ip_ver, uint8 con_ver  );
int TCP_CLIENT_PORT_REUSED_test(SOCKET s, uint16 port, uint8 * data_buf, uint8 ip_ver, uint8 con_ver );

int TCP_CLIENT_KA_Half_Close_test(SOCKET s, uint16 port, uint8 * data_buf, uint8 ip_ver, uint8 con_ver );
int TCP_CLIENT_KA_HC_KPALVTR_test(SOCKET s, uint16 port, uint8 * data_buf, uint8 ip_ver, uint8 con_ver );

int TCP_CLIENT_0BYTE_SEND_test(SOCKET s, uint16 port, uint8 * data_buf, uint8 ip_ver, uint8 con_ver );
int TCP_CLIENT_0BYTE_SEND_inKEEPALIVE_test(SOCKET s, uint16 port, uint8 * data_buf, uint8 ip_ver, uint8 con_ver );
int TCP_CLIENT_0BYTE_SEND_asACK_test(SOCKET s, uint16 port, uint8 * data_buf, uint8 ip_ver, uint8 con_ver );

int TCP_CLIENT_ACK_PACKET_TX_test(SOCKET s, uint16 port, uint8 * data_buf, uint8 ip_ver, uint8 ndack, uint8 con_ver );

int TCP_CLIENT_SYN_TX_withoutARP_test(SOCKET s, uint16 port, uint8 * data_buf, uint8 ip_ver, uint8 * peer_mac, uint8 * virtual_mac, uint8 con_ver );

int TCP_CLIENT_WINDOW_FULL_test(SOCKET s, uint16 port, uint8 * data_buf, uint8 ip_ver, uint8 con_ver );

int TCP_CLIENT_ARP_RETRANSMISSION_test(SOCKET s, uint16 port, uint8 * data_buf, uint8 ip_ver, uint8 con_ver );
int TCP_CLIENT_DATA_RETRANSMISSION_test(SOCKET s, uint16 port, uint8 * data_buf, uint8 ip_ver, uint8 con_ver );

int TCP_SERVER_3ACK_test(SOCKET s, uint16 port, uint8 * data_buf, uint8 ip_ver );

int TCP_CLIENT_3ACK_SEND_test(SOCKET s, uint16 port, uint8 * data_buf, uint8 ip_ver );

int TCP_SERVER_RSTBLK_test(SOCKET s, uint16 port, uint8 * data_buf, uint8 ip_ver, uint8 rstblk );


//int TCP_Keep_Alive_Timer_Test(SOCKET s, uint16_t port, uint8_t *data_buf, uint8_t ip_ver, uint8_t timer_delay);
//int TCP_Keep_Alive_Send_Test(SOCKET s, uint16_t port, uint8_t *data_buf, uint8_t ip_ver);
//int TCP_Transmit_Test(SOCKET s, uint16_t port, uint8_t *data_buf, uint32_t size, uint32_t send_cnt, uint8_t ip_ver);
//int TCP_Status_Check(SOCKET sn, uint16_t port, uint8_t *data_buf, uint8_t ip_ver);
//int TCP_TXRX_RDWR_Pointer_Check(SOCKET sn, uint16_t port, uint8_t *data_buf, uint8_t ip_ver);

#endif
