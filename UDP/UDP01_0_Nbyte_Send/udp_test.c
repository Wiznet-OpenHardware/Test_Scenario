#include "udp_test.h"
//#include "tcp_test.c"
#define DATA_BUF_SIZE 2048




uint8_t udp_send_flag;


int32_t zero_byte_send_test_udp(uint8_t sn, uint8_t* buf, uint16_t port, uint8_t* udp_destip, uint16_t udp_destport, uint16_t interval)
{
    int32_t ret;
    uint8_t tmp = 0;
    switch(getSn_SR(sn))
    {
        case SOCK_UDP :

        	for(int i=0; i<10; i++) {
        		printf("--%dth\r\n",i);
				setSn_DIPR(sn, udp_destip);
				setSn_DPORT(sn, udp_destport);
				setSn_TX_WR(sn, getSn_TX_WR(sn));
				setSn_CR(sn,Sn_CR_SEND);
				// wait to process the command...
				 while(1)
				   {
				      tmp = getSn_IR(sn);
				      if(tmp & Sn_IR_SENDOK)
				      {
				         setSn_IR(sn, Sn_IR_SENDOK);
				         break;
				      }
				      //M:20131104
				      //else if(tmp & Sn_IR_TIMEOUT) return SOCKERR_TIMEOUT;
				      else if(tmp & Sn_IR_TIMEOUT)
				      {
				         setSn_IR(sn, Sn_IR_TIMEOUT);

				      }
				      ////////////
				   }
				if(getSn_IR(sn)){
					printf("Sn_IR : %.2x\r\n\r\n", getSn_IR(sn));
					setSn_IR(sn, getSn_IR(sn));
				}
				delay_ms(1000);
				printf("zero\r\n");
        	}
        	close(sn);
        	return 2;
            break;
        case SOCK_CLOSED:
        	close(sn);
            setIMR(getIMR() | 1 <<sn);
            if((ret = socket(sn, Sn_MR_UDP, port, 0x00)) != sn) return ret;
            printf("%d:Opened, 0 byte send UDP, port [%d], Interval [%d]\r\n", sn, port, interval);

            break;
        default:
        break;
    }
    return 1;
}


int32_t one_to_xxxxbyte_send_test_udp(uint8_t sn, uint8_t* buf, uint16_t port, uint8_t* udp_destip, uint16_t udp_destport, uint16_t test_size)
{
    int32_t ret,i;
    uint16_t size = 0, sentsize=0;
    uint8_t  destip[4];
    uint16_t destport;
    switch(getSn_SR(sn))
    {
        case SOCK_UDP :

        	send_test_size++;
			ret = sendto(sn, send_test_buf, send_test_size, udp_destip, udp_destport);
			if(ret < 0)
			{
				printf("%d: sendto error : %ld\r\n",sn, ret);
				return ret;
			}
			if(send_test_size>=test_size)
			{
				close(sn);
				return 2;
			}

            break;

        case SOCK_CLOSED:
            send_test_size=0;
            for(i=0; i<test_size; i++)
            {
                send_test_buf[i]=i%256;
            }
          //  printf("%d:Set up to %d Buf\r\n", sn, test_size);

            if((ret = socket(sn, Sn_MR_UDP, port, 0x00)) != sn) return ret;
            printf("%d:Opened, 1~%d byte send UDP, port [%d]\r\n", sn, test_size, port);

            break;
        default:
            break;
    }
    return 1;
}


int32_t maximum_send_test_udp(uint8_t sn, uint8_t* buf, uint16_t port, uint8_t* udp_destip, uint16_t udp_destport, uint8_t buffer_size)
{
    int32_t ret,i;
    uint8_t  destip[4];
    //uint8_t	buffer_size;
    uint16_t destport;
    switch(getSn_SR(sn))
    {
        case SOCK_UDP :

			ret = sendto(sn, send_test_buf, ((buffer_size<<10)-10), udp_destip, udp_destport);
			if(ret < 0)
			{
				printf("%d: sendto error : %ld\r\n",sn, ret);
				return ret;
			}
			printf("send size : %d, return size : %d\r\n",(buffer_size<<10)-10 ,ret);

			ret = sendto(sn, send_test_buf, (buffer_size<<10), udp_destip, udp_destport);
			if(ret < 0)
			{
				printf("%d: sendto error : %ld\r\n",sn, ret);
				return ret;
			}
			printf("send size : %d, return size : %d\r\n",(buffer_size<<10) ,ret);

			ret = sendto(sn, send_test_buf, (buffer_size<<10)+10, udp_destip, udp_destport);
			if(ret < 0)
			{
				printf("%d: sendto error : %ld\r\n",sn, ret);
				return ret;
			}
			printf("send size : %d, return size : %d\r\n",(buffer_size<<10)+10 ,ret);

			close(sn);
			return 2;


            break;

        case SOCK_CLOSED:
        	setSn_TXBUF_SIZE(sn,  buffer_size);

//        	printf("buffer_size : %d\r\n", buffer_size);
        	//while(1);
            for(i=0; i<((buffer_size*1024)+10); i++)
            {
                send_test_buf[i]=i%256;
            }
            if((ret = socket(sn, Sn_MR_UDP, port, 0x00)) != sn) return ret;
            printf("%d:Opened,  UDP port [%d], buffer size [%dKB]\r\n", sn,  port, getSn_TXBUF_SIZE(sn) );

            break;
        default:
            break;
    }
    return 1;
}


int32_t mss_loopback_test_udp(uint8_t sn, uint8_t* buf, uint16_t port, uint16_t mss_size)
{
    int32_t  ret, i;
    uint16_t size;
    uint8_t  destip[4];
    uint16_t destport;

    switch(getSn_SR(sn))
    {
        case SOCK_UDP :

            if((size = getSn_RX_RSR(sn)) > 0)
            {
                memset(buf, 0, DATA_BUF_SIZE);
                if(size > DATA_BUF_SIZE) size = DATA_BUF_SIZE;
                ret = recvfrom(sn, buf, size, destip, (uint16_t*)&destport);
                if(ret <= 0)
                {
                    printf("%d: recvfrom error. %ld\r\n",sn, ret);
                    return ret;
                }
                printf("RX_RD = %d, RXRD = %d\r\n",getSn_RX_RD(0),getSn_RX_WR(0));
              //  printf("%d: recv : %s", sn, buf);

                printf("%d:peer - %d.%d.%d.%d : %d\r\n",sn, destip[0], destip[1], destip[2], destip[3], destport);

                ret = sendto(sn, buf, size-8, destip, destport);
                if(ret < 0)
                {
                    printf("%d: sendto error. %ld\r\n",sn, ret);
                    return ret;
                }
            }

            break;
        case SOCK_CLOSED:



            printf("%d:MSS Test UDP start\r\n",sn);
            setSn_MSSR(sn, mss_size);;
            if((ret = socket(sn, Sn_MR_UDP, port, 0x00)) != sn)
            return ret;
            printf("%d:Current MSS Size : %d\r\n", sn, getSn_MSSR(sn));
            printf("%d:setMSS\r\n",mss_size);
            printf("%d:Opened, MSS Test UDP, port [%d]\r\n", sn, port);


            break;
        default :
            break;
    }
    return 1;
}



int32_t mss_send_test_udp(uint8_t sn, uint8_t* buf, uint16_t port, uint8_t* udp_destip, uint16_t udp_destport, uint16_t mss_size)
{
    int32_t  ret, i;
    uint16_t size;
    uint8_t  destip[4];
    uint16_t destport;

    switch(getSn_SR(sn))
    {
        case SOCK_UDP :
        	ret = sendto(sn, buf, 2048, udp_destip, udp_destport);
        	if(ret < 0)
        	{
        		printf("%d: sendto error. %ld\r\n",sn, ret);
        		return ret;
        	}
                else {
                	printf("send size : %d\r\n", ret);
                	close(sn);
                	return 2;
                }

            break;
        case SOCK_CLOSED:
            printf("%d:MSS Test UDP start\r\n",sn);
            setSn_MSSR(sn, mss_size);
            if((ret = socket(sn, Sn_MR_UDP, port, 0x00)) != sn)
            return ret;
            printf("%d:Current MSS Size : %d\r\n", sn, getSn_MSSR(sn));
            printf("%d:Opened, MSS Test UDP, port [%d]\r\n", sn, port);

            break;
        default :
            break;
    }
    return 1;
}


int32_t arp_timeout_udp(uint8_t sn, uint8_t* buf, uint16_t port, uint8_t* udp_arp_dest_ip, uint16_t rtr, uint8_t rcr)
{
    int32_t  ret;
    uint16_t size;
    uint8_t  destip[4];
    uint16_t destport;

    switch(getSn_SR(sn))
    {
        case SOCK_UDP :
        	ret = sendto(sn, send_test_buf, 13, udp_arp_dest_ip, 60000);
        	if(ret < 0)
        	{
        		printf("%d: sendto error. %ld\r\n",sn, ret);
        		close(sn);
        		return 2;
        	}


        	break;
        case SOCK_CLOSED:
            setRTR(rtr);
            setRCR(rcr);
            setIMR(1<<sn | getIMR());
            sprintf(send_test_buf, "%s", "hi i'm w5100s");
            printf("%d:Set up Test Buf\r\n", sn);

            printf("%d:ARP Timeout Test UDP start\r\n",sn);

            if((ret = socket(sn, Sn_MR_UDP, port, 0x00)) != sn)
            return ret;

            printf("Current Retry Time : %d\r\n", getRTR());
            printf("Current Retry Count : %d\r\n", getRCR());

            printf("%d:Opened, ARP Timeout Test UDP, port [%d]\r\n", sn, port);
            delay_ms(5000);

            break;
        default :
            break;
    }
    return 1;
}


int32_t force_arp_mac_send_test(uint8_t sn, uint8_t* buf, uint16_t port, uint8_t* destip, uint16_t destport, uint8_t* destmac)
{
	int32_t ret;

	 switch(getSn_SR(sn))
	    {
	        case SOCK_UDP :
	        	printf("1. mac send command ----------------------\r\n");
	        	setSn_DHAR(sn, destmac);
	        	ret=sendmacto(sn, buf, 2048, destip, destport);
	        	if(ret < 0)
	        	{
	        		printf("%d: send fail : %ld\r\n", sn, ret);
	        	}
	        	else printf("[SENDTO] : %d\r\n", ret);


	        	//--------------set force arp -------------------------//
	        	setMR2(MR2_UDPFARP|getMR2());
	        	printf("2. send command, force arp-------------------\r\n");
	        	ret=sendto(sn, buf, 2048, destip, destport);
	        	if(ret < 0)
	        	{
	        		printf("%d: send fail : %ld\r\n", sn, ret);
	        	}
	        	else printf("[SENDTO] : %d\r\n", ret);

	        	printf("3. mac send command, force arp -----------------\r\n");
	        	ret=sendmacto(sn, buf, 2048, destip, destport);
	        	if(ret < 0)
	        	{
	        		printf("%d: send fail : %ld\r\n", sn, ret);
	        	}
	        	else printf("[SENDTO] : %d\r\n", ret);

	        	close(sn);
	    		return 2;
	            break;

	        case SOCK_CLOSED:

	            if((ret = socket(sn, Sn_MR_UDP, port, 0x00)) != sn) return ret;
	            printf("-------------------------------------------------------------\r\n");
	            printf("%d:Opened, force arp & mac send test UDP, port [%d]\r\n", sn, port);

	            break;
	        default:
	        break;
	    }
	    return 1;

}



int32_t port_unreachable_recv_test_udp(uint8_t sn, uint8_t* buf, uint16_t port, uint8_t* udp_destip, uint16_t udp_destport)
{
    int32_t ret;
    uint8_t  destip[4];
    uint8_t unreachable_ip[4];
    uint16_t destport;
    uint16_t size, sentsize;
    uint8_t tmp;
    uint8_t test_buf[]={"port unreachable test"};

    switch(getSn_SR(sn))
    {
        case SOCK_UDP :

        	printf("port unreachable interrupt test!\r\n");
            ret = sendto(sn, test_buf, sizeof(test_buf), udp_destip, udp_destport);
            if(ret < 0)
            {
            	printf("%d: send fail : %ld\r\n", sn, ret);
            	close(sn);
            	return ret;
            }

            while(!getIR());
            getUIPR(unreachable_ip);
            printf("Unreachable IP - %d.%d.%d.%d\r\n",unreachable_ip[0], unreachable_ip[1], unreachable_ip[2], unreachable_ip[3]);
            printf("Unreachable port - %d\r\n",getUPORTR());
            printf("------------------------------------------------------------------------------------");
            setIR(0xff);
            close(sn);
            return 2;
            break;

        case SOCK_CLOSED:
            if((ret = socket(sn, Sn_MR_UDP, port, 0x00)) != sn) return ret;
            printf("-------------------------------------------------------------\r\n");
            printf("%d:Opened, port unreachable UDP, port [%d]\r\n", sn, port);
            setIMR(IR_UNREACH);
            break;

        default:
        break;
    }
    return 1;
}




int32_t rx_buf_full_test_udp(uint8_t sn, uint8_t* buf, uint16_t port, uint8_t rxsize)
{
    int32_t  ret;
    static uint16_t size=0;
    uint8_t  destip[4];
    uint16_t destport;
    uint8_t uart_menu;

    switch(getSn_SR(sn))
    {
        case SOCK_UDP :

			if(size!=getSn_RX_RSR(sn)){
				size=getSn_RX_RSR(sn);
				printf("%d:rx_rsr size : %d(free: %d)\r\n", sn, size, (rxsize<<10)-size);
			}


			uart_menu=_getchar_nb();
			if(uart_menu=='r') {
				ret = recvfrom(sn, buf, getSn_RX_RSR(sn), destip, (uint16_t*)&destport);
				if(ret <= 0)
				{
					printf("%d: recvfrom error. %ld\r\n",sn, ret);
				    return ret;
				}
			}

			if(uart_menu=='e') {
				close(sn);
				return 2;
			}

            break;
        case SOCK_CLOSED:
        	setSn_RXBUF_SIZE(sn, rxsize);
            if((ret = socket(sn, Sn_MR_UDP, port, 0x00)) != sn)
            return ret;

            printf("%d:Opened, RX Buffer Full Test UDP, port [%d], buffer size [%dKB]\r\n", sn, port, getSn_RXBUF_SIZE(sn));
            break;
        default :
            break;
    }
    return 1;
}




int32_t unicast_test_udp(uint8_t sn, uint8_t* buf, uint16_t port)
{
    int32_t  ret;
    uint16_t size, sentsize;
    uint8_t  destip[4];
    uint16_t destport;

    //uint8_t test_uipr[4];
    //uint16_t test_uport;
    switch(getSn_SR(sn))
    {
        case SOCK_UDP :
            if((size = getSn_RX_RSR(sn)) > 0)
            {
                memset(buf, 0, DATA_BUF_SIZE);
                if(size > DATA_BUF_SIZE) size = DATA_BUF_SIZE;
                ret = recvfrom(sn, buf, size, destip, (uint16_t*)&destport);
                if(ret <= 0)
                {
                    printf("%d: recvfrom error. %ld\r\n",sn, ret);
                    return ret;
                }

                if(strstr( buf, "end" ))
                {
                    close(sn);
                    printf("%d: MR2: 0x%02x\r\n", sn, getMR2());
                    return 2;
                }

            }
            break;
        case SOCK_CLOSED:
            printf("%d:Unicast Broad Block Test UDP start\r\n",sn);
            setSn_MR2(sn, getSn_MR2(sn) |  0x02);
            if((ret = socket(sn, Sn_MR_UDP, port, 0x00)) != sn) return ret;
            printf("%d:Opened, Unicast Test UDP, port [%d]\r\n", sn, port);
            break;
        default :
            break;
    }
    return 1;
}
int32_t broadcast_test_udp(uint8_t sn, uint8_t* buf, uint16_t port, uint8_t* udp_bro_destip, uint16_t udp_destport )
{
    int32_t  ret;
    uint16_t size;
    uint8_t  destip[4];
    uint16_t destport;
    uint16_t i;
    uint8_t send_test_buf[2048];

    switch(getSn_SR(sn))
    {
        case SOCK_UDP :
        	ret = sendto(sn, send_test_buf, 2048, udp_bro_destip, udp_destport);
        	if(ret < 0)
        	{
        		printf("%d: sendto error. %ld\r\n",sn, ret);
        		return ret;
        	}
        	close(sn);
        	return 2;
            break;

        case SOCK_CLOSED:
            for(i=0; i<2048; i++)
            {
                send_test_buf[i]=i%256;
            }
            if((ret = socket(sn, Sn_MR_UDP, port, 0x00)) != sn) return ret;
            printf("%d:Opened, Broadcast Test UDP, port [%d]\r\n", sn, port);
            break;
        default :
            break;
    }
    return 1;
}





int32_t multicast_test_udp(uint8_t sn, uint8_t* buf, uint16_t port, uint8_t* udp_multi_dest_ip, uint8_t mac_flag, uint8_t blocking_kind, uint8_t igmp_kind)
{
    int32_t  ret;
    uint16_t size, sentsize;
    uint8_t  destip[4];
    uint16_t destport;
    uint8_t mcastmacv4[6] =  {0x01,0x00,0x5E,0x01,0x01,0x0b};

    uint8_t mcastmacv4_check[6] = {0,};
    uint8_t udp_multi_dest_ip_check[4] = {0,};
    uint16_t port_check=0;

    //uint8_t test_dest_ip[4] = {192, 168, 0, 2};
    //uint16_t test_dest_port = 5555;
    //uint16_t i;

    switch(getSn_SR(sn))
    {
        case SOCK_UDP :
            /*ret = sendto(sn, buf+sentsize, size-sentsize, destip, destport);
            if(ret < 0)
            {
            printf("%d: sendto error. %ld\r\n",sn, ret);
            return ret;
            }*/

#if 1
            if((size = getSn_RX_RSR(sn)) > 0)
            {
                if(size > DATA_BUF_SIZE) size = DATA_BUF_SIZE;
                memset(buf, 0, DATA_BUF_SIZE);
                ret = recvfrom(sn, buf, size, destip, (uint16_t*)&destport);
                if(ret <= 0)
                {
                    printf("%d: recvfrom error. %ld\r\n",sn, ret);
                    return ret;
                }
                printf("%d:recv : %s\t",sn, buf);
                printf("%d:peer - %d.%d.%d.%d : %d\r\n",sn, destip[0], destip[1], destip[2], destip[3], destport);
                if(strstr( buf, "end" ))
                {
                    close(sn);
                    printf("%d: SN_MR: 0x%02x\r\n", sn, getSn_MR(sn));
                    printf("%d: SN_MR2: 0x%02x\r\n", sn, getSn_MR2(sn));
                    return 2;
                }
                size = (uint16_t) ret;
                sentsize = 0;
                while(sentsize != size)
                {

                    ret = sendto(sn, buf+sentsize, size-sentsize, destip, destport);
                    //printf("multicast sendto 2\r\n");
                    //ret |= sendto(sn, buf+sentsize, size-sentsize, udp_multi_dest_ip, port);
                    //printf("multicast sendto 3\r\n");
                    //ret |= sendto(sn, buf+sentsize, size-sentsize, test_dest_ip, test_dest_port);
                    printf("multicast sendto %d\r\n", ret);
                    if(ret < 0)
                    {
                        printf("%d: sendto error. %ld\r\n",sn, ret);
                        return ret;
                    }
                    sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
                }
                //printf("%d: SN_MR: 0x%02x\r\n", sn, getSn_MR(sn));
                //printf("%d: SN_MR2: 0x%02x\r\n", sn, getSn_MR2(sn));
            }
            /*ret = sendto(sn, send_test_buf, 2048, udp_multi_dest_ip, destport);
            if(ret < 0)
            {
            printf("%d: sendto error. %ld\r\n",sn, ret);
            return ret;
            }*/
            #endif
            break;
        case SOCK_CLOSED:


            /*for(i=0; i<2048; i++)
            {
            send_test_buf[i]=i%256;
            }
            printf("%d:Set up to %d Buf\r\n", sn, 2048);*/

            setSn_DPORT(sn, port);
            if(mac_flag==1)
            {
                setSn_DHAR( sn, mcastmacv4);
                printf("%d:Multicast Test UDP, Set MAC(DHAR), ",sn);
            }
            else if(mac_flag==2)
            {
                printf("%d:Multicast Test UDP, Set No MAC(DHAR), ",sn);
            }
            else
            {
                printf("\r\n%d:mac_flag flag is wrong!!!\r\n",sn);
                close(sn);
                return 2;
            }
            setSn_DIPR(sn, udp_multi_dest_ip);


            if(blocking_kind==1)
            {
                setSn_MR2(sn, Sn_MR2_UUBLK);
                printf("Set UnicastBlocked, ");
            }
            else if(blocking_kind==2)
            {
                setSn_MR2(sn, Sn_MR2_UBBLK);
                printf("Set BroadcastBlocked, ");
            }
            else if(blocking_kind==3)
            {
                setSn_MR2(sn, Sn_MR2_UBBLK |Sn_MR2_UUBLK);
                printf("Set Broadcast and UnicastBlocked, ");
            }
            else if(blocking_kind==4)
            {
                printf("Set None Blocked, ");
            }
            else
            {
                printf("\r\n%d:blocking_kind is wrong!!!\r\n",sn);
                close(sn);
                return 2;
            }

            if(igmp_kind==1)
            {
                printf("IGMPv1 Multicast Test UDP start\r\n");
                if((ret = socket(sn, Sn_MR_UDP, port, SF_MULTI_ENABLE|SF_TCP_NODELAY)) != sn)
                return ret;
            }
            else if(igmp_kind==2)
            {
                printf("IGMPv2 Multicast Test UDP start\r\n");
                if((ret = socket(sn, Sn_MR_UDP, port, SF_MULTI_ENABLE)) != sn)
                return ret;

            }
            else
            {
                printf("\r\n%d:igmp_kind is wrong!!!\r\n",sn);
                close(sn);
                return 2;
            }

            printf("%d: SN_MR: 0x%02x\r\n", sn, getSn_MR(sn));
            printf("%d: SN_MR2: 0x%02x\r\n", sn, getSn_MR2(sn));
            port_check = getSn_DPORT(sn);
            getSn_DHAR( sn, mcastmacv4_check);
            getSn_DIPR(sn, udp_multi_dest_ip_check);

            printf("%d:Dest Group IP - %d.%d.%d.%d : %d\r\n",sn, udp_multi_dest_ip_check[0], udp_multi_dest_ip_check[1], udp_multi_dest_ip_check[2], udp_multi_dest_ip_check[3], port_check);
            printf("%d:Dest Group MAC - %d:%d:%d:%d:%d:%d\r\n",sn, mcastmacv4_check[0], mcastmacv4_check[1], mcastmacv4_check[2], mcastmacv4_check[3], mcastmacv4_check[4], mcastmacv4_check[5]);

            printf("%d:Opened, Test UDP, port [%d]\r\n", sn, port);

            //ret = sendto(sn, buf, 2048, udp_multi_dest_ip_check, port_check);

            break;
        default :
            break;
    }
    return 1;
}





int32_t max_send_test_udp(uint8_t sn, uint8_t* buf, uint16_t port, uint16_t max_size)
{
    int32_t  ret, i;
    uint16_t size;
    uint8_t  destip[4];
    uint16_t destport;
    uint8_t  test_destip1[4]={192,168,254,232};
    uint8_t  test_destip2[4]={255,255,255,255};
    uint16_t test_destport=6000;




    switch(getSn_SR(sn))
    {
        case SOCK_UDP :

            if((size = getSn_RX_RSR(sn)) > 0)
            {
                memset(buf, 0, DATA_BUF_SIZE);
                if(size > DATA_BUF_SIZE) size = DATA_BUF_SIZE;
                ret = recvfrom(sn, buf, size, destip, (uint16_t*)&destport);
                if(ret <= 0)
                {
                    printf("%d: recvfrom error. %ld\r\n",sn, ret);
                    return ret;
                }
                printf("%d:peer - %d.%d.%d.%d : %d\r\n",sn, destip[0], destip[1], destip[2], destip[3], destport);
                printf("%d: recv : %s", sn, buf);
                if(strstr( buf, "end" ))
                {
                    close(sn);
                    return 2;
                }
            }



            if(udp_send_flag)
            {
                udp_send_flag=0;
                ret = sendto(sn, send_test_buf, max_size, test_destip1, test_destport);
                printf("%d: sendto. %ld\r\n",sn, ret);
                if(ret < 0)
                {
                    printf("%d: sendto error. %ld\r\n",sn, ret);
                    return ret;
                }
                ret = sendto(sn, send_test_buf, max_size, test_destip2, test_destport);
                printf("%d: sendto. %ld\r\n",sn, ret);
                if(ret < 0)
                {
                    printf("%d: sendto error. %ld\r\n",sn, ret);
                    return ret;
                }
            }
            break;
        case SOCK_CLOSED:

            udp_send_flag=1;
            for(i=0; i<max_size; i++)
            {
                send_test_buf[i]=i%256;
            }

            printf("%d:Set up to %d Buf\r\n", sn, max_size);
            printf("%d:MAX Send Test UDP start\r\n",sn);
            if((ret = socket(sn, Sn_MR_UDP, port, 0x00)) != sn) return ret;

            printf("Current Retry Time : %d\r\n", getRTR());
            printf("Current Retry Count : %d\r\n", getRCR());
            printf("%d:Current MSS Size : %d\r\n", sn, getSn_MSSR(sn));

            printf("%d:Opened, MAX Send Test UDP, port [%d], size[%d]\r\n", sn, port, max_size);
            break;
        default :
            break;
    }
    return 1;
}
int32_t mss_loopback_test_udp_e(uint8_t sn, uint8_t* buf, uint16_t port,uint16_t mss)
{
   int32_t  ret;
   uint16_t size, sentsize;
   uint8_t  destip[4];
   uint16_t destport;

   switch(getSn_SR(sn))
   {
      case SOCK_UDP :
         if((size = getSn_RX_RSR(sn)) > 0)
         ;
            if(size >= DATA_BUF_SIZE) size = DATA_BUF_SIZE;
                ret = recvfrom(sn, buf, size, destip, (uint16_t*)&destport);
            if(ret <= 0)
            {

               return ret;
            }
            size = (uint16_t) ret;
            sentsize = 0;
           printf("%d : RX Size\r\n",getSn_RX_RSR(sn));
             while(sentsize != size)
            {
               ret = sendto(sn, buf+sentsize, size-sentsize, destip, destport);
               if(ret < 0)
               {

                  return ret;
               }
               sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
 

         }
         break;
      case SOCK_CLOSED:
    	//  setSn_MSSR(sn,mss);
    	           if((ret = socket(sn, Sn_MR_UDP, port, 0x00)) != sn)
            return ret;

         printf("%d:Opened, UDP loopback, port [%d]\r\n", sn, port);
         printf("%d:Current MSS Size : %d\r\n", sn, getSn_MSSR(sn));
         printf("%d:setMSS\r\n",mss);
         printf("%d:Opened, MSS Test UDP, port [%d]\r\n", sn, port); 


         break;
      default :
         break;
   }
   return 1;
}
