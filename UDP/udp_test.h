#include <stdio.h>
#include "socket.h"
#include "common.h"


int32_t zero_byte_send_test_udp(uint8_t sn, uint8_t* buf, uint16_t port, uint8_t* udp_destip, uint16_t udp_destport, uint16_t interval);
int32_t one_to_xxxxbyte_send_test_udp(uint8_t sn, uint8_t* buf, uint16_t port, uint8_t* udp_destip, uint16_t udp_destport, uint16_t test_size);
int32_t maximum_send_test_udp(uint8_t sn, uint8_t* buf, uint16_t port, uint8_t* udp_destip, uint16_t udp_destport, uint8_t buffer_size);
int32_t mss_loopback_test_udp(uint8_t sn, uint8_t* buf, uint16_t port, uint16_t mss_size);
int32_t mss_send_test_udp(uint8_t sn, uint8_t* buf, uint16_t port, uint8_t* udp_destip, uint16_t udp_destport, uint16_t mss_size);
int32_t arp_timeout_udp(uint8_t sn, uint8_t* buf, uint16_t port, uint8_t* udp_arp_dest_ip, uint16_t rtr, uint8_t rcr);
int32_t force_arp_mac_send_test(uint8_t sn, uint8_t* buf, uint16_t port, uint8_t* destip, uint16_t destport, uint8_t* destmac);
int32_t port_unreachable_recv_test_udp(uint8_t sn, uint8_t* buf, uint16_t port, uint8_t* udp_destip, uint16_t udp_destport);
int32_t rx_buf_full_test_udp(uint8_t sn, uint8_t* buf, uint16_t port, uint8_t rxsize);
int32_t broadcast_test_udp(uint8_t sn, uint8_t* buf, uint16_t port, uint8_t* udp_bro_destip, uint16_t udp_destport );

