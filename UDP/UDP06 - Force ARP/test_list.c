

/*#include "test_list.h"
#include "loopback.h"
unsigned char data_buf[2048];
unsigned char data_buf_8KB[2048*4];

//#define BANDWIDTH77




uint8_t line_cmd1[10];
uint8_t line_cmd2[10];
uint8_t line_cmd3[10];
uint8_t line_cmd4[10];
uint8_t line_cmd5[10];*/

/*
int32_t tcp_test(uint8_t select_test) {
	//force psh flag
	//WIZCHIP_WRITE(Sn_MR2(0), WIZCHIP_READ(Sn_MR2(0))|0x02);
	switch(select_test){
	case 'a' :
		do
		{
		status1=zero_byte_send_test_tcps(0, data_buf, 5000);
		}while(status1!=2);
		break;

	case 'c' :

		do
		{
		status1=zero_byte_send_test_tcps(0, data_buf, 5000);
		}while(status1!=2);
		break;

	case 'i' :
		printf("[socket number]\r\n");
		while(1){
			uart_menu = _getchar_nb();
			if( uart_menu != '\0') {
				uart_menu= uart_menu-'0';
				break;
			}
		}
		do
		{
			status1=socket_interrupt_test(uart_menu, data_buf, 5000);
		}while(status1!=2);
		break;
	case 'j' :
		printf("RX RD pointer TEST\r\n");

		do
		{
			status1=rx_rd_poiner_test(uart_menu, data_buf, 5000);
		}while(status1!=2);
		break;


	case 'k' :
		printf("Timer Status TEST\r\n");

		do
		{
			status1=timer_status_test(uart_menu, data_buf, 5000);
		}while(status1!=2);
		break;

	case 'l' :
		printf("Send Timeout TEST\r\n");

		do
		{
			//status1=send_timeout_test_tcp(uart_menu, data_buf, 5000);
		}while(status1!=2);
		break;


	case 'u' :

		printf("[Input Retry Time]\r\n");
		gets_echo(line_cmd1,4);
		printf("[Input Retry Count]\r\n");
		gets_echo(line_cmd2,4);
		printf("[Input No TCP RST flag(ok:1, no:0)]\r\n");
		gets_echo(line_cmd3, 1);
		printf("[Input No Delayed Ack flag(ok:1, no:0)]\r\n");
		gets_echo(line_cmd4, 1);
		do{
			status1 = delay_ack_test_tcps(0, 5000, data_buf, atoi((const char*)line_cmd4), atoi((const char*)line_cmd1), atoi((const char*)line_cmd2), atoi((const char*)line_cmd3));
			if(status1 == -1) break;
		}while(status1 != 2);

		break;

	case 'v' :
		printf("TMSR 8KB, RMSR 2KB Loopback Client Test\r\n");
		do
		{
			wizchip_setnetinfo(&gWIZNETINFO_Client);
			setTMSR(0x03);
			loopback_tcpc(0, data_buf_8KB, tcp_dest_ip2, tcp_dest_port);
		}while(status1!=2);
		break;

	case 'w' :
		printf("TMSR 8KB, RMSR 2KB Loopback Server Test\r\n");
		do
		{
			setTMSR(0x03);
			loopback_tcps(0, data_buf_8KB, 5000);
		}while(status1!=2);
		break;

	case 'x' :
		printf("Window Full TEST Client\r\n");
		do
		{
			wizchip_setnetinfo(&gWIZNETINFO_Client);
			status1=window_full_test_tcpc(0, data_buf, 5000, tcp_dest_ip2, tcp_dest_port);
		}while(status1!=2);
		break;


	case 'y' :
		printf("Window Full TEST Server\r\n");
		do
		{
			status1=window_full_test_tcps(0, data_buf, 5000);
		}while(status1!=2);
		break;
	}

}

int32_t udp_test(uint8_t select_test){

	switch(select_test){
	case 'a' :
		printf("================================\r\n");
		printf(" 0byte send test\r\n");
		printf("================================\r\n");

        do{
            status1 =zero_byte_send_test_udp(0, data_buf, 5000, udp_dest_ip, udp_dest_port, 5000);
            if(status1 == -1) break;
        }while(status1 != 2);
		break;


	case 'b':
		printf("================================\r\n");
		printf("1 ~ xxxbyte send test\r\n");
		printf("================================\r\n");
        printf("[Input send size]\r\n");
        gets_echo(line_cmd1,5);
        printf("\r\n");
        do{
            status1 = one_to_xxxxbyte_send_test_udp(0, data_buf, 5000, udp_dest_ip, udp_dest_port, atoi((const char*)line_cmd1));
            if(status1 == -1) break;
        }while(status1 != 2);
        break;

	case 'c':
		printf("================================\r\n");
		printf("max send test\r\n");
		printf("================================\r\n");
        printf("[Input socket buffer size (1,2,4,8)]\r\n");
        gets_echo(line_cmd1,5);
        printf("\r\n");
        do{
            status1 = maximum_send_test_udp(0, data_buf, 5000, udp_dest_ip, udp_dest_port, atoi((const char*)line_cmd1));
            if(status1 == -1) break;
        }while(status1 != 2);
        break;


	case 'd':
		printf("================================\r\n");
		printf("mss loopback test\r\n");
		printf("================================\r\n");
        printf("[Input socket buffer size (1,2,4,8)]\r\n");
        printf("[Input MSS size]\r\n");
        gets_echo(line_cmd1,4);
        do{
        	status1 =mss_loopback_test_udp(0, data_buf, 5000,  atoi((const char*)line_cmd1));
            if(status1 == -1) break;
        }while(status1 != 2);
		break;

	case 'e' :
		printf("================================\r\n");
		printf("mss send test\r\n");
		printf("================================\r\n");
        printf("[Input socket buffer size (1,2,4,8)]\r\n");
        printf("[Input MSS size]\r\n");
        gets_echo(line_cmd1,4);
        do{
        	status1 =mss_send_test_udp(0, data_buf, 5000, udp_dest_ip, udp_dest_port, atoi((const char*)line_cmd1));
        	if(status1 == -1) break;
        }while(status1 != 2);
		break;
	case 'f':
		printf("================================\r\n");
		printf("arp timeout test\r\n");
		printf("================================\r\n");
		printf("[Input Retry Time]\r\n");
		gets_echo(line_cmd1, 5);
		printf("[Input Retry Count]\r\n");
		gets_echo(line_cmd2, 3);
		do{
			status1 = arp_timeout_udp(0, data_buf, 5000, arp_timeout_ip, atoi((const char*)line_cmd1), atoi((const char*)line_cmd2));
			if(status1 == -1) break;
		}while(status1 != 2);
		break;

	case 'g' :
		printf("================================\r\n");
		printf("force arp & mac send test\r\n");
		printf("================================\r\n");
        do{

            status1 =force_arp_mac_send_test(0, data_buf, 5000, udp_dest_ip, udp_dest_port, ipraw_dest_mac);
            if(status1 == -1) break;
        }while(status1 != 2);

		break;

	case 'h' :
		printf("================================\r\n");
		printf("port unreachable recv test!\r\n");
		printf("================================\r\n");
        do{
            status1 =port_unreachable_recv_test_udp(0, data_buf, 5000, udp_dest_ip, udp_dest_port);
            if(status1 == -1) break;
        }while(status1 != 2);
		break;

	case 'i' :
		printf("================================\r\n");
		printf("port unreachable send test!\r\n");
		printf("================================\r\n");

		setMR2(getMR2() | MR2_UDPURB);
		do{
			printf("[Input 'ok' when you want to finish test\r\n");
			gets_echo(line_cmd1, 2);
			printf("\r\n");
			if(line_cmd1[0]=='o' && line_cmd1[1]=='k'){
				setMR2(getMR2() & ~MR2_UDPURB);
				status1=2;
			}

		}while(status1 != 2);
		break;

	case 'j' :
		printf("================================\r\n");
		printf("rx buffer full test\r\n");
		printf("================================\r\n");
		printf("[Input RX Buffer size(1,2,4,8)\r\n");
		gets_echo(line_cmd1, 4);
		do{
			status1 =rx_buf_full_test_udp(0, data_buf, 5000, atoi((const char*)line_cmd1));
			if(status1 == -1) break;
		}while(status1 != 2);
		break;

	case 'k' :
		printf("================================\r\n");
		printf("Broadcast Test\r\n");
		printf("================================\r\n");
		do{
			status1 = broadcast_test_udp(0, data_buf, 5000, udp_bro_dest_ip, 5000);
            if(status1 == -1) break;
		}while(status1 != 2);
		break;

	case 'l' :
		printf("================================\r\n");
		printf("Subnet Broadcast Case 1 Test\r\n");
		printf("================================\r\n");
		setSUBR(subnet_case1);
        do{
        status1 = broadcast_test_udp(0, data_buf, 5000, udp_sub_bro_dest_ip_case_1, 5000);
        if(status1 == -1) break;
        }while(status1 != 2);
		break;

	case 'm':
		printf("================================\r\n");
		printf("Subnet Broadcast Case 2 Test\r\n");
		printf("================================\r\n");
		setSUBR(subnet_case2);
		do{
			status1 = broadcast_test_udp(0, data_buf, 5000, udp_sub_bro_dest_ip_case_2, 5000);
			if(status1 == -1) break;
		}while(status1 != 2);
		break;

	case 'n':
		printf("================================\r\n");
		printf("Subnet Broadcast Case 3 Test\r\n");
		printf("================================\r\n");
		setSUBR(subnet_case3);
		do{
			status1 = broadcast_test_udp(0, data_buf, 5000, udp_sub_bro_dest_ip_case_3, 5000);
			if(status1 == -1) break;
		}while(status1 != 2);
		break;

	case 'o':
        printf("[Input set mac(ok:1, no:2)]\r\n");
        gets_echo(line_cmd1,1);
        printf("[Input block kind(uni:1, bro:2, unibro:3, no:4)]\r\n");
        gets_echo(line_cmd2,1);
        printf("[Input igmp version(v1:1, v2:2)]\r\n");
        gets_echo(line_cmd3,1);
        do{
            status1 = multicast_test_udp(0, data_buf, 5000, udp_multi_dest_ip, atoi((const char*)line_cmd1), atoi((const char*)line_cmd2), atoi((const char*)line_cmd3));
            if(status1 == -1) break;
        }while(status1 != 2);
		break;
//

	case 'y' :
        do{
            loopback_udps(0, data_buf, 5000);
        }while(status1 != 2);
		break;

	default :
		break;

	}
}
*/

/*
int32_t common_test(uint8_t select_test) {
	switch(select_test){
	case 'a' :
		do{
			status1=recv_wol_test();
		}while(status1!=2);
		break;

	case 'b' :
		do{
			status1=mdc_mdio_test();
		}while(status1!=2);
		break;

	case 'c' :
		do{
			status1=ping_block_test();
		}while(status1!=2);
		break;



	case 'd' :
		do{
			status1=ip_conflict_test();
		}while(status1!=2);
		break;

	default :
		break;
	}
}
*/
