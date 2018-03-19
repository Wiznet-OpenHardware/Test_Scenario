#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "common.h"
uint8_t send_test_buf[(8192+10)]={0,};
uint8_t one_to_xxxx_send_flag=0;
uint16_t send_test_size=0;
uint8_t udp_debug=0;
uint16_t rx_buf_full_test_udp_cnt=0;
uint8_t hal_connection_sned_cnt=0;

uint8_t tcp_connect_cnt=0;
uint8_t zero_byte_send_test_first_data_flag=0;

uint8_t internet_request_flag=0;
uint8_t internet_disconnect_flag=0;
uint8_t receive_test_flag=0;

uint8_t udp_send_flag=0;

uint8_t close_flag=0;
