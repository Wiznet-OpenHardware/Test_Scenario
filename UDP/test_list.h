

#include "udp_test.h"
#include "common_test.h"


int32_t tcp_test(uint8_t select_test);
int32_t slc_test(uint8_t select_test);
int32_t udp_test(uint8_t select_test);
int32_t common_test(uint8_t select_test);
int32_t ipraw_test(uint8_t select_test);
int32_t macraw_test(uint8_t select_test);
#ifndef BANDWIDTH77
#define BANDWIDTH0
#endif

wiz_NetInfo gWIZNETINFO_Client = { .mac = {0x00,0x08,0xdc,0x11,0x90,0x71},
							.ip = {192,168,254, 236},
							.sn = {255, 255, 255, 0},
							.gw = {192, 168, 254, 1},
							.dns = {168, 126, 63, 1},
							.dhcp = NETINFO_STATIC};

int status1, status2;
uint8_t uart_menu;

#ifdef BANDWIDTH77
uint8_t tcp_dest_ip2[4] = {192,168,77,45};

uint8_t tcp_dest_ip[4] = {192,168,77,232};
uint8_t udp_dest_ip[4] = {192,168,77,232};
uint8_t slc_dest_ip[4] = {192,168,77,232};

uint8_t ipraw_dest_ip[4] = {192,168,77,232};
uint8_t macraw_dest_ip[4] = {192,168,77,232};


uint8_t ipraw_dest_mac[6]={0x08,0x62,0x66,0x2a,0xce,0xe3};

uint8_t arp_timeout_ip[4]={192,168,77,5};

uint8_t udp_bro_dest_ip[4] = {255, 255, 255, 255};


uint8_t subnet_case1[4] = {255, 255, 255, 0};
uint8_t udp_sub_bro_dest_ip_case_1[4] = {192, 168, 77, 255};

uint8_t subnet_case2[4] = {255, 255, 0, 0};
uint8_t udp_sub_bro_dest_ip_case_2[4] = {192, 168, 255, 255};

uint8_t subnet_case3[4] = {255, 0, 0, 0};
uint8_t udp_sub_bro_dest_ip_case_3[4] = {192, 255, 255, 255};

#endif

#ifdef BANDWIDTH0
uint8_t tcp_dest_ip2[4] = {192,168,0,45};

uint8_t tcp_dest_ip[4] = {192,168,0,200};
uint8_t udp_dest_ip[4] = {192,168,0,200};
uint8_t slc_dest_ip[4] = {192,168,0,200};

uint8_t ipraw_dest_ip[4] = {192,168,0,4};
uint8_t macraw_dest_ip[4] = {192,168,0,200};


uint8_t ipraw_dest_mac[6]={0x08,0x62,0x66,0x2a,0xce,0xe3};

uint8_t arp_timeout_ip[4]={192,168,0,5};

uint8_t udp_bro_dest_ip[4] = {255, 255, 255, 255};


uint8_t subnet_case1[4] = {255, 255, 255, 0};
uint8_t udp_sub_bro_dest_ip_case_1[4] = {192, 168, 0, 255};

uint8_t subnet_case2[4] = {255, 255, 0, 0};
uint8_t udp_sub_bro_dest_ip_case_2[4] = {192, 168, 255, 255};

uint8_t subnet_case3[4] = {255, 0, 0, 0};
uint8_t udp_sub_bro_dest_ip_case_3[4] = {192, 255, 255, 255};

#endif

uint8_t udp_multi_dest_ip[4] = {224, 1, 1, 11};
uint16_t tcp_dest_port = 5000;
uint16_t udp_dest_port = 5000;
uint16_t ipraw_dest_port = 3000;
uint16_t macraw_dest_port = 5000;
uint8_t interval_ms;
