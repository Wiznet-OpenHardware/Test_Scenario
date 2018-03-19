#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdint.h>

/* DATA_BUF_SIZE define for test */
#ifndef DATA_BUF_SIZE
	#define DATA_BUF_SIZE			2048
#endif

extern uint8_t send_test_buf[8192+10];
extern uint16_t send_test_size;
extern uint8_t one_to_xxxx_send_flag;
extern uint8_t udp_debug;
extern uint16_t rx_buf_full_test_udp_cnt;
extern uint8_t tcp_connect_cnt;
extern uint8_t hal_connection_sned_cnt;
extern uint8_t zero_byte_send_test_first_data_flag;
extern uint8_t internet_request_flag;
extern uint8_t internet_disconnect_flag;
extern uint8_t udp_send_flag;

extern uint8_t close_flag;

#endif
