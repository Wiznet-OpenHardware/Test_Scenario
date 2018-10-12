#include "halConfiguration.h"
#include "W6100RelFunctions.h"
#include "netconfig.h"
#include "w6100.h"
#include "socket.h"
#include <string.h>
#include <stdio.h>
#include "tcp_test.h"
#include "usart.h"
#include "wizchip_conf.h"

#define tick_second 1
#define DATA_BUF_SIZE			2048

extern uint8 ch_status[MAX_SOCK_NUM];
uint8_t end_signal[MAX_SOCK_NUM] = {0,};

extern CHCONFIG_TYPE_DEF Chconfig_Type_Def;
extern CHCONFIG_TYPE_DEF virtual_ip;
extern CHCONFIG_TYPE_DEF multi_ip;
extern uint32_t presentTime;
//extern uint16 any_port;
uint16 any_port = 1000;

int TCP_Recv_Test(SOCKET s, uint16_t port, uint8_t *data_buf, uint8_t ip_ver)
{
    uint16 RSR_len;
    uint16 RSR_Max_LEN;

    if(ip_ver == AF_INET6)
    {
        RSR_Max_LEN = 1440;
    }
    else
    {
        RSR_Max_LEN = 1460;
    }

		switch (getSn_SR(s))
    {
        case SOCK_ESTABLISHED:              /* if connection is established */
            if(ch_status[s]==1)
            {
                printf("[SOCK_ESTABLISHED]\r\n");
                printf("[REQUEST_SEND]\r\n");
                ch_status[s] = 2;
            }
            if((RSR_len = getSn_RX_RSR(s)) > 0)
            {
                if (RSR_len > TX_RX_MAX_BUF_SIZE) RSR_len = TX_RX_MAX_BUF_SIZE;  /* if Rx data size is lager than TX_RX_MAX_BUF_SIZE *//* the data size to read is MAX_BUF_SIZE. */
                recv(s, data_buf, RSR_len);     /* read the received data */
                printf("[RECV_LEN]:%d\r\n",RSR_len);
                
                if(RSR_len >= RSR_Max_LEN)
                {
                    printf("[REQUEST_CLOSE]\r\n");
                    end_signal[s] = 1;
                }
            }						
            break;
            
        case SOCK_CLOSE_WAIT:                              /* If the client request to close */
            printf("[SOCK_CLOSE_WAIT]\r\n");
            if ((RSR_len = getSn_RX_RSR(s)) > 0)     /* check Rx data */
            {
                if (RSR_len > TX_RX_MAX_BUF_SIZE) RSR_len = TX_RX_MAX_BUF_SIZE;  /* if Rx data size is lager than TX_RX_MAX_BUF_SIZE *//* the data size to read is MAX_BUF_SIZE. */
                recv(s, data_buf, RSR_len);     /* read the received data */
            }
            disconnect(s);
            ch_status[s] = 0;
            break;
        
        case SOCK_CLOSED:                                       /* if a socket is closed */
             if(end_signal[s] == 1)
             {
                 end_signal[s] = 0;
                 printf("[TEST_FINISHED]\r\n");
                 return 2;
             }
        
            if(!ch_status[s])
            {
                printf("[SOCK_CLOSED]\r\n");
                ch_status[s] = 1;
            }

            //-- socket open						
            if(ip_ver == AF_INET6)
            {
                s = socket(s,(Sn_MR_TCP6),port,Sn_MR_ND);
            }
            else if(ip_ver == AF_INET_DUAL)
            {
                s = socket(s,(Sn_MR_DUALT),port,Sn_MR_ND);
            }
            else
            {
                s = socket(s,(Sn_MR_TCP),port,Sn_MR_ND);        
            }
        
        case SOCK_INIT:   /* if a socket is initiated */
            printf("[SOCK_INIT]\r\n");
            listen(s);
            delay_ms(500);
            printf("[REQUEST_CONNECT]\r\n");
            break;

        default:
            break;
    }
    
    return 0;        
}

int TCP_SERVER_BASE_FORMAT_test(SOCKET s, uint16 port, uint8 * data_buf, uint8 ip_ver )
{
    uint16 RSR_len;
    uint16 received_len, test_buf_len=16;
		uint8 end_sign;
		uint8_t test_buf[]={"hello i'm W6100"};
//		uint32_t loop_cnt;

    switch (getSn_SR(s))
    {
     case SOCK_ESTABLISHED:                 /* if connection is established */
        if(ch_status[s]==1)
        {
            
            printf("%d : Connected\r\n",s);
					
						send(s, test_buf, test_buf_len);         /* sent the received data */				
					
            ch_status[s] = 2;
        }
				//send(s, test_buf, test_buf_len);         /* sent the received data */								
				delay_ms(100);
        break;

    case SOCK_CLOSE_WAIT:                                 /* If the client request to close */
        printf("%d : CLOSE_WAIT\r\n", s);
        if ((RSR_len = getSn_RX_RSR(s)) > 0)         /* check Rx data */
        {
            if (RSR_len > TX_RX_MAX_BUF_SIZE) RSR_len = TX_RX_MAX_BUF_SIZE;   /* if Rx data size is lager than TX_RX_MAX_BUF_SIZE */
            /* the data size to read is MAX_BUF_SIZE. */
            received_len = recv(s, data_buf, RSR_len);         /* read the received data */
        }
				delay_ms(10);
        disconnect(s);
        ch_status[s] = 0;
				return 2;
    
    case SOCK_CLOSED:                                               /* if a socket is closed */
				if(end_sign == 1)
				{
					end_sign = 0;
					return 2;
				}
        if(!ch_status[s])
        {
            printf("%d : Loop-Back TCP Client Started. port: %d\r\n", s, port);
            ch_status[s] = 1;
        }
        //-- socket open
        if(ip_ver == AF_INET6)
        {
            s = socket(s,(Sn_MR_TCP6),port,Sn_MR_ND);
        }
        else if(ip_ver == AF_INET_DUAL)
        {
            s = socket(s,(Sn_MR_DUALT),port,Sn_MR_ND);
        }
        else
        {
            s = socket(s,(Sn_MR_TCP),port,Sn_MR_ND);        
        }
        
    case SOCK_INIT:     /* if a socket is initiated */        
				printf("[SOCK_INIT]\r\n");
        listen(s);
        printf("[REQUEST_CONNECT]\r\n");
        break;

    default:
        break;
    }
		return 0;
}
int TCP_CLIENT_BASE_FORMAT_test(SOCKET s, uint16 port, uint8 * data_buf, uint8 ip_ver )
{
    uint16 RSR_len;
    uint16 received_len, test_buf_len=16;
		uint8 end_sign;
		uint8_t test_buf[]={"hello i'm W6100"};
//		uint32_t loop_cnt;

    switch (getSn_SR(s))
    {
     case SOCK_ESTABLISHED:                 /* if connection is established */
        if(ch_status[s]==1)
        {
            
            printf("%d : Connected\r\n",s);
					
						send(s, test_buf, test_buf_len);         /* sent the received data */				
					
            ch_status[s] = 2;
        }
				//send(s, test_buf, test_buf_len);         /* sent the received data */								
				delay_ms(100);
        break;

    case SOCK_CLOSE_WAIT:                                 /* If the client request to close */
        printf("%d : CLOSE_WAIT\r\n", s);
        if ((RSR_len = getSn_RX_RSR(s)) > 0)         /* check Rx data */
        {
            if (RSR_len > TX_RX_MAX_BUF_SIZE) RSR_len = TX_RX_MAX_BUF_SIZE;   /* if Rx data size is lager than TX_RX_MAX_BUF_SIZE */
            /* the data size to read is MAX_BUF_SIZE. */
            received_len = recv(s, data_buf, RSR_len);         /* read the received data */
        }				
				delay_ms(10);
        disconnect(s);
        ch_status[s] = 0;
				return 2;
    
    case SOCK_CLOSED:                                               /* if a socket is closed */
				if(end_sign == 1)
				{
					end_sign = 0;
					return 2;
				}
        if(!ch_status[s])
        {
            printf("%d : Loop-Back TCP Client Started. port: %d\r\n", s, port);
            ch_status[s] = 1;
        }
        //-- socket open
        if(ip_ver == AF_INET6)
        {
            s = socket(s,Sn_MR_TCP6,any_port++,Sn_MR_ND);
        }
        else if(ip_ver == AF_INET_DUAL)
        {
            s = socket(s,Sn_MR_DUALT,any_port++,Sn_MR_ND);
        }
        else
        {
            s = socket(s,Sn_MR_TCP,any_port++,Sn_MR_ND);        
        }
        
    case SOCK_INIT:     /* if a socket is initiated */
        if((time_return() - presentTime) >= (tick_second * 3)) /* For TCP client's connection request delay : 3 sec */
        {
            if(ip_ver == AF_INET6)
                connect(s, Chconfig_Type_Def.destipv6, port, 16); /* Try to connect to TCP server(Socket, DestIP, DestPort) */
            else
                connect(s, Chconfig_Type_Def.destip, port,4);

            presentTime = time_return();						
        }
        break;

    default:
        break;
    }
		return 0;
}

//--test sce start------------------------------------------------------------------------------------------------------------------------------------//
int TCP_SERVER_CON_ACTIVE_DISCON_test(SOCKET s, uint16 port, uint8 * data_buf, uint8 ip_ver )
{
    uint16 RSR_len;
    uint16 received_len;
		uint8 end_sign;
//		uint8_t test_buf[]={"hello i'm W6100"};
//		uint32_t loop_cnt;

    switch (getSn_SR(s))
    {
     case SOCK_ESTABLISHED:                 /* if connection is established */
        if(ch_status[s]==1)
        {
            
            printf("%d : Connected\r\n",s);					
						//send(s, test_buf, test_buf_len);         /* sent the received data */				
						if ((RSR_len = getSn_RX_RSR(s)) > 0)         /* check Rx data */
						{
							if (RSR_len > TX_RX_MAX_BUF_SIZE) RSR_len = TX_RX_MAX_BUF_SIZE;   /* if Rx data size is lager than TX_RX_MAX_BUF_SIZE */
							/* the data size to read is MAX_BUF_SIZE. */
							received_len = recv(s, data_buf, RSR_len);         /* read the received data */
						}					
						//send(s, test_buf, test_buf_len);         /* sent the received data */
            ch_status[s] = 2;
        }
				//send(s, test_buf, test_buf_len);         /* sent the received data */					
				if ((RSR_len = getSn_RX_RSR(s)) > 0)         /* check Rx data */
				{
					if (RSR_len > TX_RX_MAX_BUF_SIZE) RSR_len = TX_RX_MAX_BUF_SIZE;   /* if Rx data size is lager than TX_RX_MAX_BUF_SIZE */
					/* the data size to read is MAX_BUF_SIZE. */
					received_len = recv(s, data_buf, RSR_len);         /* read the received data */
					send(s, data_buf, RSR_len);         /* sent the received data */
					disconnect(s);
					ch_status[s] = 0;
					return 2;
				}				
				delay_ms(100);
        break;

    case SOCK_CLOSE_WAIT:                                 /* If the client request to close */
        printf("%d : CLOSE_WAIT\r\n", s);
        if ((RSR_len = getSn_RX_RSR(s)) > 0)         /* check Rx data */
        {
            if (RSR_len > TX_RX_MAX_BUF_SIZE) RSR_len = TX_RX_MAX_BUF_SIZE;   /* if Rx data size is lager than TX_RX_MAX_BUF_SIZE */
            /* the data size to read is MAX_BUF_SIZE. */
            received_len = recv(s, data_buf, RSR_len);         /* read the received data */
        }
				delay_ms(10);
        disconnect(s);
        ch_status[s] = 0;
				//return 2;
        break;
    
    case SOCK_CLOSED:                                               /* if a socket is closed */
				if(end_sign == 1)
				{
					if(getSn_IR(s) & Sn_IR_CON)	// Socket n interrupt register mask; TCP CON interrupt = connection with peer is successful
          {
						printf("%d:Connected to - %d.%d.%d.%d : %d\r\n",s, Chconfig_Type_Def.destip[0], Chconfig_Type_Def.destip[1], Chconfig_Type_Def.destip[2], Chconfig_Type_Def.destip[3], port);
						clearSn_IR(s, Sn_IR_CON);  // this interrupt should be write the bit cleared to '1'
          }
					end_sign = 0;
					return 2;
				}
        if(!ch_status[s])
        {
            printf("%d : Loop-Back TCP Client Started. port: %d\r\n", s, port);
            ch_status[s] = 1;
        }
        //-- socket open				
        if(ip_ver == AF_INET6)
        {
            s = socket(s,Sn_MR_TCP6,port,Sn_MR_ND);
        }
        else if(ip_ver == AF_INET_DUAL)
        {
            s = socket(s,Sn_MR_DUALT,port,Sn_MR_ND);
        }
        else
        {   
						s = socket(s,Sn_MR_TCP,port,Sn_MR_ND);        					
        }
        
    case SOCK_INIT:     /* if a socket is initiated */        
				printf("[SOCK_INIT]\r\n");
        listen(s);
				printf("%x :LISTEN socket %d\r\n",getSn_SR(s), s);
        break;

    default:
        break;
    }
		return 0;
}
int TCP_SERVER_CON_PASSIVE_DISCON_test(SOCKET s, uint16 port, uint8 * data_buf, uint8 ip_ver )
{
    uint16 RSR_len;
    uint16 received_len;
		uint8 end_sign;
//		uint8_t test_buf[]={"hello i'm W6100"};
//		uint32_t loop_cnt;

    switch (getSn_SR(s))
    {
     case SOCK_ESTABLISHED:                 /* if connection is established */
        if(ch_status[s]==1)
        {
            
            printf("%d : Connected\r\n",s);					
						//send(s, test_buf, received_len);         /* sent the received data */				
						if ((RSR_len = getSn_RX_RSR(s)) > 0)         /* check Rx data */
						{
							if (RSR_len > TX_RX_MAX_BUF_SIZE) RSR_len = TX_RX_MAX_BUF_SIZE;   /* if Rx data size is lager than TX_RX_MAX_BUF_SIZE */
							/* the data size to read is MAX_BUF_SIZE. */
							received_len = recv(s, data_buf, RSR_len);         /* read the received data */
						}					
						//send(s, test_buf, test_buf_len);         /* sent the received data */
            ch_status[s] = 2;
        }
				//send(s, test_buf, received_len);         /* sent the received data */	
				if ((RSR_len = getSn_RX_RSR(s)) > 0)         /* check Rx data */
				{					
					if (RSR_len > TX_RX_MAX_BUF_SIZE) RSR_len = TX_RX_MAX_BUF_SIZE;   /* if Rx data size is lager than TX_RX_MAX_BUF_SIZE */
					/* the data size to read is MAX_BUF_SIZE. */
					received_len = recv(s, data_buf, RSR_len);         /* read the received data */
				}				
				delay_ms(100);
        break;

    case SOCK_CLOSE_WAIT:                                 /* If the client request to close */
        printf("%d : CLOSE_WAIT\r\n", s);
        if ((RSR_len = getSn_RX_RSR(s)) > 0)         /* check Rx data */
        {
            if (RSR_len > TX_RX_MAX_BUF_SIZE) RSR_len = TX_RX_MAX_BUF_SIZE;   /* if Rx data size is lager than TX_RX_MAX_BUF_SIZE */
            /* the data size to read is MAX_BUF_SIZE. */
            received_len = recv(s, data_buf, RSR_len);         /* read the received data */
        }
				delay_ms(10);
        disconnect(s);
        ch_status[s] = 0;
				return 2;
    
    case SOCK_CLOSED:                                               /* if a socket is closed */
				if(end_sign == 1)
				{
					if(getSn_IR(s) & Sn_IR_CON)	// Socket n interrupt register mask; TCP CON interrupt = connection with peer is successful
          {
						printf("%d:Connected to - %d.%d.%d.%d : %d\r\n",s, Chconfig_Type_Def.destip[0], Chconfig_Type_Def.destip[1], Chconfig_Type_Def.destip[2], Chconfig_Type_Def.destip[3], port);
						clearSn_IR(s, Sn_IR_CON);  // this interrupt should be write the bit cleared to '1'
          }
					end_sign = 0;
					return 2;
				}
        if(!ch_status[s])
        {
            printf("%d : Loop-Back TCP Client Started. port: %d\r\n", s, port);
            ch_status[s] = 1;
        }
        //-- socket open
        if(ip_ver == AF_INET6)
        {
            s = socket(s,Sn_MR_TCP6,port,Sn_MR_ND);
        }
        else if(ip_ver == AF_INET_DUAL)
        {
            s = socket(s,Sn_MR_DUALT,port,Sn_MR_ND);
        }
        else
        {
            s = socket(s,Sn_MR_TCP,port,Sn_MR_ND);        
        }
        
    case SOCK_INIT:     /* if a socket is initiated */        
				printf("[SOCK_INIT]\r\n");
        listen(s);
        printf("[REQUEST_CONNECT]\r\n");
        break;

    default:
        break;
    }
		return 0;
}
int TCP_CLIENT_CON_ACTIVE_DISCON_test(SOCKET s, uint16 port, uint8 * data_buf, uint8 ip_ver, uint8 con_ver )
{
    uint16 RSR_len;
    uint16 received_len;
		uint8 end_sign;
//		uint8_t test_buf[]={"hello i'm W6100"};
//		uint32_t loop_cnt;

    switch (getSn_SR(s))
    {
     case SOCK_ESTABLISHED:                 /* if connection is established */
        if(ch_status[s]==1)
        {
            
            printf("%d : Connected\r\n",s);
					
						if ((RSR_len = getSn_RX_RSR(s)) > 0)         /* check Rx data */
						{
							if (RSR_len > TX_RX_MAX_BUF_SIZE) RSR_len = TX_RX_MAX_BUF_SIZE;   /* if Rx data size is lager than TX_RX_MAX_BUF_SIZE */
							/* the data size to read is MAX_BUF_SIZE. */
							received_len = recv(s, data_buf, RSR_len);         /* read the received data */
						}											
						//send(s, test_buf, test_buf_len);         /* sent the received data */				
					
            ch_status[s] = 2;
        }
				if ((RSR_len = getSn_RX_RSR(s)) > 0)         /* check Rx data */
				{
					if (RSR_len > TX_RX_MAX_BUF_SIZE) RSR_len = TX_RX_MAX_BUF_SIZE;   /* if Rx data size is lager than TX_RX_MAX_BUF_SIZE */
					/* the data size to read is MAX_BUF_SIZE. */
					received_len = recv(s, data_buf, RSR_len);         /* read the received data */
					disconnect(s);
					ch_status[s] = 0;
					return 2;
				}
				//send(s, test_buf, received_len);         /* sent the received data */												
				delay_ms(100);
        break;

    case SOCK_CLOSE_WAIT:                                 /* If the client request to close */
        printf("%d : CLOSE_WAIT\r\n", s);
        if ((RSR_len = getSn_RX_RSR(s)) > 0)         /* check Rx data */
        {
            if (RSR_len > TX_RX_MAX_BUF_SIZE) RSR_len = TX_RX_MAX_BUF_SIZE;   /* if Rx data size is lager than TX_RX_MAX_BUF_SIZE */
            /* the data size to read is MAX_BUF_SIZE. */
            received_len = recv(s, data_buf, RSR_len);         /* read the received data */
        }				
				delay_ms(10);
        disconnect(s);
        ch_status[s] = 0;
				return 2;
    
    case SOCK_CLOSED:                                               /* if a socket is closed */
				if(end_sign == 1)
				{
					end_sign = 0;
					return 2;
				}
        if(!ch_status[s])
        {
            printf("%d : Loop-Back TCP Client Started. port: %d\r\n", s, port);
            ch_status[s] = 1;
        }
        //-- socket open
        if(ip_ver == AF_INET6)
        {
            s = socket(s,Sn_MR_TCP6,any_port++,Sn_MR_ND);
        }
        else if(ip_ver == AF_INET_DUAL)
        {
            s = socket(s,Sn_MR_DUALT,any_port++,Sn_MR_ND);
        }
        else
        {						
            s = socket(s,Sn_MR_TCP,any_port++,Sn_MR_ND);        
        }
        
    case SOCK_INIT:     /* if a socket is initiated */
        if((time_return() - presentTime) >= (tick_second * 3)) /* For TCP client's connection request delay : 3 sec */
        {
            if(ip_ver == AF_INET6){
                connect(s, Chconfig_Type_Def.destipv6, port, 16); /* Try to connect to TCP server(Socket, DestIP, DestPort) */
						}
						else if(ip_ver == AF_INET_DUAL){
							if(con_ver == AF_INET6)
                connect(s, Chconfig_Type_Def.destipv6, port, 16); /* Try to connect to TCP server(Socket, DestIP, DestPort) */
							else if(con_ver == AF_INET)
								connect(s, Chconfig_Type_Def.destip, port,4);
							else return 0;
						}
            else
                connect(s, Chconfig_Type_Def.destip, port,4);

            presentTime = time_return();						
        }
        break;

    default:
        break;
    }
		return 0;
}
int TCP_CLIENT_CON_PASSIVE_DISCON_test(SOCKET s, uint16 port, uint8 * data_buf, uint8 ip_ver, uint8 con_ver  )
{
    uint16 RSR_len;
    uint16 received_len=16;
		uint8 end_sign;
//		uint8_t test_buf[]={"hello i'm W6100"};

    switch (getSn_SR(s))
    {
     case SOCK_ESTABLISHED:                 /* if connection is established */
        if(ch_status[s]==1)
        {
            
            printf("%d : Connected\r\n",s);
					
						if ((RSR_len = getSn_RX_RSR(s)) > 0)         /* check Rx data */
						{
							if (RSR_len > TX_RX_MAX_BUF_SIZE) RSR_len = TX_RX_MAX_BUF_SIZE;   /* if Rx data size is lager than TX_RX_MAX_BUF_SIZE */
							/* the data size to read is MAX_BUF_SIZE. */
							received_len = recv(s, data_buf, RSR_len);         /* read the received data */
						}
						//send(s, test_buf, received_len);         /* sent the received data */				
					
            ch_status[s] = 2;
        }
				if ((RSR_len = getSn_RX_RSR(s)) > 0)         /* check Rx data */
				{
					if (RSR_len > TX_RX_MAX_BUF_SIZE) RSR_len = TX_RX_MAX_BUF_SIZE;   /* if Rx data size is lager than TX_RX_MAX_BUF_SIZE */
					/* the data size to read is MAX_BUF_SIZE. */
					received_len = recv(s, data_buf, RSR_len);         /* read the received data */
				}
				//send(s, test_buf, received_len);         /* sent the received data */								
				delay_ms(100);
        break;

    case SOCK_CLOSE_WAIT:                                 /* If the client request to close */
        printf("%d : CLOSE_WAIT\r\n", s);
        if ((RSR_len = getSn_RX_RSR(s)) > 0)         /* check Rx data */
        {
            if (RSR_len > TX_RX_MAX_BUF_SIZE) RSR_len = TX_RX_MAX_BUF_SIZE;   /* if Rx data size is lager than TX_RX_MAX_BUF_SIZE */
            /* the data size to read is MAX_BUF_SIZE. */
            received_len = recv(s, data_buf, RSR_len);         /* read the received data */
        }				
				delay_ms(10);
        disconnect(s);
        ch_status[s] = 0;
				return 2;
    
    case SOCK_CLOSED:                                               /* if a socket is closed */
				if(end_sign == 1)
				{
					end_sign = 0;
					return 2;
				}
        if(!ch_status[s])
        {
            printf("%d : Loop-Back TCP Client Started. port: %d\r\n", s, port);
            ch_status[s] = 1;
        }
        //-- socket open
        if(ip_ver == AF_INET6)
        {
            s = socket(s,Sn_MR_TCP6,any_port++,Sn_MR_ND);
        }
        else if(ip_ver == AF_INET_DUAL)
        {
            s = socket(s,Sn_MR_DUALT,any_port++,Sn_MR_ND);
        }
        else
        {
            s = socket(s,Sn_MR_TCP,any_port++,Sn_MR_ND);        
        }
        
    case SOCK_INIT:     /* if a socket is initiated */
        if((time_return() - presentTime) >= (tick_second * 3)) /* For TCP client's connection request delay : 3 sec */
        {
            if(ip_ver == AF_INET6)
                connect(s, Chconfig_Type_Def.destipv6, port, 16); /* Try to connect to TCP server(Socket, DestIP, DestPort) */
						else if(ip_ver == AF_INET_DUAL){
							if(con_ver == AF_INET6)
                connect(s, Chconfig_Type_Def.destipv6, port, 16); /* Try to connect to TCP server(Socket, DestIP, DestPort) */
							else if(con_ver == AF_INET)
								connect(s, Chconfig_Type_Def.destip, port,4);
							else return 0;
						}
            else
                connect(s, Chconfig_Type_Def.destip, port,4);

            presentTime = time_return();						
        }
        break;

    default:
        break;
    }
		return 0;
}
int TCP_CLIENT_PORT_REUSED_test(SOCKET s, uint16 port, uint8 * data_buf, uint8 ip_ver, uint8 con_ver )
{
    uint16 RSR_len;
    uint16 received_len, test_buf_len=16;
		uint8 end_sign;
		uint8_t test_buf[]={"hello i'm W6100"};
		uint32_t loop_cnt;
				
    switch (getSn_SR(s))
    {
     case SOCK_ESTABLISHED:                 /* if connection is established */
        if(ch_status[s]==1)
        {
            
            printf("%d : Connected\r\n",s);
					
						//send(s, test_buf, test_buf_len);         /* sent the received data */				
					
            ch_status[s] = 2;
        }
				if(ch_status[s]==2)
        {
					for(loop_cnt = 0;loop_cnt<3;loop_cnt++)
					{					
//						while (!(RSR_len = getSn_RX_RSR(s)))         /* check Rx data */					
						while ((RSR_len = getSn_RX_RSR(s)) == 0)         /* check Rx data */					
						{
							if(getSn_SR(s) != SOCK_ESTABLISHED) return 1;							
						}
						if (RSR_len > TX_RX_MAX_BUF_SIZE) RSR_len = TX_RX_MAX_BUF_SIZE;   /* if Rx data size is lager than TX_RX_MAX_BUF_SIZE */
						/* the data size to read is MAX_BUF_SIZE. */
						received_len = recv(s, data_buf, RSR_len);        /* read the received data */
						send(s, test_buf, test_buf_len);         /* sent the received data */
					}
					ch_status[s] = 3;
				}
				//close(s);
				//delay_ms(5000);
				//return 1;
        break;

    case SOCK_CLOSE_WAIT:                                 /* If the client request to close */
        printf("%d : CLOSE_WAIT\r\n", s);
        if ((RSR_len = getSn_RX_RSR(s)) > 0)         /* check Rx data */
        {
            if (RSR_len > TX_RX_MAX_BUF_SIZE) RSR_len = TX_RX_MAX_BUF_SIZE;   /* if Rx data size is lager than TX_RX_MAX_BUF_SIZE */
            /* the data size to read is MAX_BUF_SIZE. */
            received_len = recv(s, data_buf, RSR_len);         /* read the received data */
        }				
				delay_ms(10);
        //disconnect(s);
        ch_status[s] = 0;
				//return 2;
				break;				
    
    case SOCK_CLOSED:                                               /* if a socket is closed */
				if(end_sign == 1)
				{
					end_sign = 0;
					return 2;
				}
        if(!ch_status[s])
        {
            printf("%d : Loop-Back TCP Client Started. port: %d\r\n", s, port);
            ch_status[s] = 1;
        }
        //-- socket open
        if(ip_ver == AF_INET6)
        {
            s = socket(s,Sn_MR_TCP6,any_port++,Sn_MR_ND);
        }
        else if(ip_ver == AF_INET_DUAL)
        {
            s = socket(s,Sn_MR_DUALT,any_port++,Sn_MR_ND);
        }
        else
        {
            s = socket(s,Sn_MR_TCP,any_port++,Sn_MR_ND);        
        }
        
    case SOCK_INIT:     /* if a socket is initiated */
        if((time_return() - presentTime) >= (tick_second * 3)) /* For TCP client's connection request delay : 3 sec */
        {
            if(ip_ver == AF_INET6)
                connect(s, Chconfig_Type_Def.destipv6, port, 16); /* Try to connect to TCP server(Socket, DestIP, DestPort) */
						else if(ip_ver == AF_INET_DUAL){
							if(con_ver == AF_INET6)
                connect(s, Chconfig_Type_Def.destipv6, port, 16); /* Try to connect to TCP server(Socket, DestIP, DestPort) */
							else if(con_ver == AF_INET)
								connect(s, Chconfig_Type_Def.destip, port,4);
							else return 0;
						}
            else
                connect(s, Chconfig_Type_Def.destip, port,4);

            presentTime = time_return();						
        }
        break;

    default:
        break;
    }
		return 0;
}
int TCP_CLIENT_KA_Half_Close_test(SOCKET s, uint16 port, uint8 * data_buf, uint8 ip_ver, uint8 con_ver )
{
//    uint16 RSR_len;
    uint16 received_len=16;
		uint8 end_sign;
		uint8_t test_buf[]={"hello i'm W6100"};
		uint32_t loop_cnt;

    switch (getSn_SR(s))
    {
     case SOCK_ESTABLISHED:                 /* if connection is established */
        if(ch_status[s]==1)
        {
            
            printf("%d : Connected\r\n",s);
					
						send(s, test_buf, received_len);         /* sent the received data */				
					
            ch_status[s] = 2;
        }
				//send(s, test_buf, received_len);         /* sent the received data */								
				delay_ms(100);
        break;

    case SOCK_CLOSE_WAIT:                                 /* If the client request to close */
        printf("%d : CLOSE_WAIT\r\n", s);
//        if ((RSR_len = getSn_RX_RSR(s)) > 0)         /* check Rx data */
//        {
//            if (RSR_len > TX_RX_MAX_BUF_SIZE) RSR_len = TX_RX_MAX_BUF_SIZE;   /* if Rx data size is lager than TX_RX_MAX_BUF_SIZE */
//            /* the data size to read is MAX_BUF_SIZE. */
//            received_len = recv(s, data_buf, RSR_len);         /* read the received data */
//        }
				for(loop_cnt=0;loop_cnt<5;loop_cnt++)
				{
						printf("[SEND_KEEPALIVE]\r\n");
						delay_ms(1000);
						WIZCHIP_WRITE(Sn_CR(s), Sn_CR_SEND_KEEP);
				}		
					//printf("test1");
				delay_ms(10);
        disconnect(s);
        ch_status[s] = 0;
				return 2;
    
    case SOCK_CLOSED:                                               /* if a socket is closed */
				if(end_sign == 1)
				{
					end_sign = 0;
					return 2;
				}
        if(!ch_status[s])
        {
            printf("%d : Loop-Back TCP Client Started. port: %d\r\n", s, port);
            ch_status[s] = 1;
        }
        //-- socket open
        if(ip_ver == AF_INET6)
        {
            s = socket(s,Sn_MR_TCP6,any_port++,Sn_MR_ND);
        }
        else if(ip_ver == AF_INET_DUAL)
        {
            s = socket(s,Sn_MR_DUALT,any_port++,Sn_MR_ND);
        }
        else
        {
            s = socket(s,Sn_MR_TCP,any_port++,Sn_MR_ND);        
        }
        
    case SOCK_INIT:     /* if a socket is initiated */
        if((time_return() - presentTime) >= (tick_second * 3)) /* For TCP client's connection request delay : 3 sec */
        {
            if(ip_ver == AF_INET6)
                connect(s, Chconfig_Type_Def.destipv6, port, 16); /* Try to connect to TCP server(Socket, DestIP, DestPort) */
						else if(ip_ver == AF_INET_DUAL){
							if(con_ver == AF_INET6)
                connect(s, Chconfig_Type_Def.destipv6, port, 16); /* Try to connect to TCP server(Socket, DestIP, DestPort) */
							else if(con_ver == AF_INET)
								connect(s, Chconfig_Type_Def.destip, port,4);
							else return 0;
						}
            else
                connect(s, Chconfig_Type_Def.destip, port,4);

            presentTime = time_return();						
        }
        break;

    default:
        break;
    }
		return 0;
}
int TCP_CLIENT_KA_HC_KPALVTR_test(SOCKET s, uint16 port, uint8 * data_buf, uint8 ip_ver, uint8 con_ver )
{
//    uint16 RSR_len;
    uint16 received_len=16;
		uint8 end_sign;
		uint8_t test_buf[]={"hello i'm W6100"};
		uint32_t timer_delay=1;

    switch (getSn_SR(s))
    {
     case SOCK_ESTABLISHED:                 /* if connection is established */
        if(ch_status[s]==1)
        {
            
            printf("%d : Connected\r\n",s);
					
						send(s, test_buf, received_len);         /* sent the received data */				
					
            ch_status[s] = 2;
        }
				//send(s, test_buf, received_len);         /* sent the received data */								
				delay_ms(100);
        break;

    case SOCK_CLOSE_WAIT:                                 /* If the client request to close */
        printf("%d : CLOSE_WAIT\r\n", s);
//        if ((RSR_len = getSn_RX_RSR(s)) > 0)         /* check Rx data */
//        {
//            if (RSR_len > TX_RX_MAX_BUF_SIZE) RSR_len = TX_RX_MAX_BUF_SIZE;   /* if Rx data size is lager than TX_RX_MAX_BUF_SIZE */
//            /* the data size to read is MAX_BUF_SIZE. */
//            received_len = recv(s, data_buf, RSR_len);         /* read the received data */
//        }								
				printf("[SEND_KEEPALIVE]\r\n");
				delay_ms(20000);
				
        disconnect(s);
        ch_status[s] = 0;
				return 2;
    
    case SOCK_CLOSED:                                               /* if a socket is closed */
				if(end_sign == 1)
				{
					end_sign = 0;
					return 2;
				}
        if(!ch_status[s])
        {
            printf("%d : Loop-Back TCP Client Started. port: %d\r\n", s, port);
            ch_status[s] = 1;
        }
        //-- socket open
				WIZCHIP_WRITE(Sn_KPALVTR(s), timer_delay);
        if(ip_ver == AF_INET6)
        {
            s = socket(s,Sn_MR_TCP6,any_port++,Sn_MR_ND);
        }
        else if(ip_ver == AF_INET_DUAL)
        {
            s = socket(s,Sn_MR_DUALT,any_port++,Sn_MR_ND);
        }
        else
        {
            s = socket(s,Sn_MR_TCP,any_port++,Sn_MR_ND);        
        }
        
    case SOCK_INIT:     /* if a socket is initiated */
        if((time_return() - presentTime) >= (tick_second * 3)) /* For TCP client's connection request delay : 3 sec */
        {
            if(ip_ver == AF_INET6)
                connect(s, Chconfig_Type_Def.destipv6, port, 16); /* Try to connect to TCP server(Socket, DestIP, DestPort) */
						else if(ip_ver == AF_INET_DUAL){
							if(con_ver == AF_INET6)
                connect(s, Chconfig_Type_Def.destipv6, port, 16); /* Try to connect to TCP server(Socket, DestIP, DestPort) */
							else if(con_ver == AF_INET)
								connect(s, Chconfig_Type_Def.destip, port,4);
							else return 0;
						}
            else
                connect(s, Chconfig_Type_Def.destip, port,4);

            presentTime = time_return();						
        }
        break;

    default:
        break;
    }
		return 0;
}
int TCP_CLIENT_0BYTE_SEND_test(SOCKET s, uint16 port, uint8 * data_buf, uint8 ip_ver, uint8 con_ver )
{
    uint16 RSR_len;
    uint16 received_len=16;
		uint8 end_sign, tx_wr[2];
		uint8_t test_buf[]={"hello i'm W6100"};
		uint8_t test_buf1[]={"hello i'm w6100"};
		uint32_t loop_cnt;

    switch (getSn_SR(s))
    {
     case SOCK_ESTABLISHED:                 /* if connection is established */
        if(ch_status[s]==1)
        {
            
            printf("%d : Connected\r\n",s);
						
						//send(s, test_buf, received_len);         /* sent the received data */
						//delay_ms(100);
						
						//setSn_CR(sn,Sn_CR_SEND);
						/* wait to process the command... */
            while(WIZCHIP_READ(Sn_CR(s)));					
						for(loop_cnt=0;loop_cnt<1;loop_cnt++)
						{
							WIZCHIP_READ_BUF(Sn_TX_WR(s) ,tx_wr,2);
							printf("%x %x\n",tx_wr[0], tx_wr[1]);
							//WIZCHIP_WRITE(WZ_Sn_CR(s), Sn_CR_SEND);			
							send(s, test_buf, 0);         /* sent the received data */
							delay_ms(1000);
							/* wait to process the command... */
							while(WIZCHIP_READ(Sn_CR(s)));
							
						}						
						
						setSn_MR(s,Sn_MR_FPSH|getSn_MR(s));
						delay_ms(100);
						for(loop_cnt=0;loop_cnt<1;loop_cnt++)
						{
							//WIZCHIP_WRITE(WZ_Sn_CR(s), Sn_CR_SEND);			
							send(s, test_buf, 0);         /* sent the received data */
							delay_ms(1000);
							/* wait to process the command... */
							while(WIZCHIP_READ(Sn_CR(s)));
						}
						
						send(s, test_buf1, received_len);         /* sent the received data */
						delay_ms(1000);
						
						//setSn_CR(sn,Sn_CR_SEND);
						/* wait to process the command... */
						setSn_MR(s,(~(1<<6))&getSn_MR(s));
            while(WIZCHIP_READ(Sn_CR(s)));					
						for(loop_cnt=0;loop_cnt<1;loop_cnt++)
						{
							//WIZCHIP_WRITE(WZ_Sn_CR(s), Sn_CR_SEND);			
							send(s, test_buf, 0);         /* sent the received data */
							delay_ms(100);
							/* wait to process the command... */
							while(WIZCHIP_READ(Sn_CR(s)));
						}						
						setSn_MR(s,Sn_MR_FPSH|getSn_MR(s));
						delay_ms(10);
						for(loop_cnt=0;loop_cnt<1;loop_cnt++)
						{
							//WIZCHIP_WRITE(WZ_Sn_CR(s), Sn_CR_SEND);			
							send(s, test_buf, 0);         /* sent the received data */
							delay_ms(100);
							/* wait to process the command... */
							while(WIZCHIP_READ(Sn_CR(s)));
						}
					
            ch_status[s] = 2;
        }
				//send(s, test_buf, received_len);         /* sent the received data */												
				disconnect(s);
				ch_status[s] = 0;
				delay_ms(100);
				return 2;

    case SOCK_CLOSE_WAIT:                                 /* If the client request to close */
        printf("%d : CLOSE_WAIT\r\n", s);
        if ((RSR_len = getSn_RX_RSR(s)) > 0)         /* check Rx data */
        {
            if (RSR_len > TX_RX_MAX_BUF_SIZE) RSR_len = TX_RX_MAX_BUF_SIZE;   /* if Rx data size is lager than TX_RX_MAX_BUF_SIZE */
            /* the data size to read is MAX_BUF_SIZE. */
            received_len = recv(s, data_buf, RSR_len);         /* read the received data */
        }								
        disconnect(s);
        ch_status[s] = 0;
				return 2;
    
    case SOCK_CLOSED:                                               /* if a socket is closed */
				if(end_sign == 1)
				{
					end_sign = 0;
					return 2;
				}
        if(!ch_status[s])
        {
            printf("%d : Loop-Back TCP Client Started. port: %d\r\n", s, port);
            ch_status[s] = 1;
        }
        //-- socket open
        if(ip_ver == AF_INET6)
        {
            s = socket(s,Sn_MR_TCP6,any_port++,Sn_MR_ND);
        }
        else if(ip_ver == AF_INET_DUAL)
        {
            s = socket(s,Sn_MR_DUALT,any_port++,Sn_MR_ND);
        }
        else
        {
            s = socket(s,Sn_MR_TCP,any_port++,Sn_MR_ND);        
        }
        
    case SOCK_INIT:     /* if a socket is initiated */
				WIZCHIP_READ_BUF(Sn_TX_WR(s) ,tx_wr,2);
				printf("%x %x\n",tx_wr[0], tx_wr[1]);
        if((time_return() - presentTime) >= (tick_second * 3)) /* For TCP client's connection request delay : 3 sec */
        {
            if(ip_ver == AF_INET6)
                connect(s, Chconfig_Type_Def.destipv6, port, 16); /* Try to connect to TCP server(Socket, DestIP, DestPort) */
						else if(ip_ver == AF_INET_DUAL){
							if(con_ver == AF_INET6)
                connect(s, Chconfig_Type_Def.destipv6, port, 16); /* Try to connect to TCP server(Socket, DestIP, DestPort) */
							else if(con_ver == AF_INET)
								connect(s, Chconfig_Type_Def.destip, port,4);
							else return 0;
						}
            else
                connect(s, Chconfig_Type_Def.destip, port,4);

            presentTime = time_return();						
        }
				
        break;

    default:
        break;
    }
		return 0;
}
int TCP_CLIENT_0BYTE_SEND_inKEEPALIVE_test(SOCKET s, uint16 port, uint8 * data_buf, uint8 ip_ver, uint8 con_ver )
{
    uint16 RSR_len;
    uint16 received_len=16;
		uint8 end_sign, tx_wr[2];
		uint8_t test_buf[]={"hello i'm W6100"};
		uint32_t loop_cnt;

    switch (getSn_SR(s))
    {
     case SOCK_ESTABLISHED:                 /* if connection is established */
        if(ch_status[s]==1)
        {
            
            printf("%d : Connected\r\n",s);
						
						//send(s, test_buf, received_len);         /* sent the received data */
						//delay_ms(100);
						
						//setSn_CR(sn,Sn_CR_SEND);
						/* wait to process the command... */
            //while(WIZCHIP_READ(WZ_Sn_CR(s)));					
						for(loop_cnt=0;loop_cnt<4;loop_cnt++)
						{
							WIZCHIP_READ_BUF(Sn_TX_WR(s) ,tx_wr,2);
							printf("%x %x\n",tx_wr[0], tx_wr[1]);
							//WIZCHIP_WRITE(WZ_Sn_CR(s), Sn_CR_SEND);			
							send(s, test_buf, received_len);         /* sent the received data */
							delay_ms(1000);
							/* wait to process the command... */
							while(WIZCHIP_READ(Sn_CR(s)));
							delay_ms(60000); //wait window10 keepalivetime
						}						
					
            ch_status[s] = 2;
        }
				send(s, test_buf, 0);         /* sent the received data */			
				delay_ms(2000); //zerobye send interval
				return 1;

    case SOCK_CLOSE_WAIT:                                 /* If the client request to close */
        printf("%d : CLOSE_WAIT\r\n", s);
        if ((RSR_len = getSn_RX_RSR(s)) > 0)         /* check Rx data */
        {
            if (RSR_len > TX_RX_MAX_BUF_SIZE) RSR_len = TX_RX_MAX_BUF_SIZE;   /* if Rx data size is lager than TX_RX_MAX_BUF_SIZE */
            /* the data size to read is MAX_BUF_SIZE. */
            received_len = recv(s, data_buf, RSR_len);         /* read the received data */
        }								
        disconnect(s);
        ch_status[s] = 0;
				return 2;
    
    case SOCK_CLOSED:                                               /* if a socket is closed */
				if(end_sign == 1)
				{
					end_sign = 0;
					return 2;
				}
        if(!ch_status[s])
        {
            printf("%d : Loop-Back TCP Client Started. port: %d\r\n", s, port);
            ch_status[s] = 1;
        }
        //-- socket open
        if(ip_ver == AF_INET6)
        {
            s = socket(s,Sn_MR_TCP6,any_port++,Sn_MR_ND);
        }
        else if(ip_ver == AF_INET_DUAL)
        {
            s = socket(s,Sn_MR_DUALT,any_port++,Sn_MR_ND);
        }
        else
        {
            s = socket(s,Sn_MR_TCP,any_port++,Sn_MR_ND);        
        }
        
    case SOCK_INIT:     /* if a socket is initiated */
				WIZCHIP_READ_BUF(Sn_TX_WR(s) ,tx_wr,2);
				printf("%x %x\n",tx_wr[0], tx_wr[1]);
        if((time_return() - presentTime) >= (tick_second * 3)) /* For TCP client's connection request delay : 3 sec */
        {
            if(ip_ver == AF_INET6)
                connect(s, Chconfig_Type_Def.destipv6, port, 16); /* Try to connect to TCP server(Socket, DestIP, DestPort) */
						else if(ip_ver == AF_INET_DUAL){
							if(con_ver == AF_INET6)
                connect(s, Chconfig_Type_Def.destipv6, port, 16); /* Try to connect to TCP server(Socket, DestIP, DestPort) */
							else if(con_ver == AF_INET)
								connect(s, Chconfig_Type_Def.destip, port,4);
							else return 0;
						}
            else
                connect(s, Chconfig_Type_Def.destip, port,4);

            presentTime = time_return();						
        }
				
        break;

    default:
        break;
    }
		return 0;
}
int TCP_CLIENT_0BYTE_SEND_asACK_test(SOCKET s, uint16 port, uint8 * data_buf, uint8 ip_ver, uint8 con_ver )
{
    uint16 RSR_len;
    uint16 received_len=16;
		uint8 end_sign, tx_wr[2];
		uint8_t test_buf[]={"hello i'm W6100"};
		//uint32_t loop_cnt;
		uint16_t size = 0;
//		uint32_t timer_delay=1;

    switch (getSn_SR(s))
    {
     case SOCK_ESTABLISHED:                 /* if connection is established */
        if(ch_status[s]==1)
        {
            
            printf("%d : Connected\r\n",s);
						
						//send(s, test_buf, received_len);         /* sent the received data */
						//delay_ms(100);
						
						//setSn_CR(sn,Sn_CR_SEND);
						/* wait to process the command... */
            //while(WIZCHIP_READ(WZ_Sn_CR(s)));												
						delay_ms(100);														
            ch_status[s] = 2;
        }
				if((size = getSn_RX_RSR(0)) > 0) // Don't need to check SOCKERR_BUSY because it doesn't not occur. 
				{ 
					send(s, test_buf, 0);         /* sent the received data */				
				}
				
				delay_ms(100); //zerobye send interval
				return 1;

    case SOCK_CLOSE_WAIT:                                 /* If the client request to close */
        printf("%d : CLOSE_WAIT\r\n", s);
        if ((RSR_len = getSn_RX_RSR(s)) > 0)         /* check Rx data */
        {
            if (RSR_len > TX_RX_MAX_BUF_SIZE) RSR_len = TX_RX_MAX_BUF_SIZE;   /* if Rx data size is lager than TX_RX_MAX_BUF_SIZE */
            /* the data size to read is MAX_BUF_SIZE. */
            received_len = recv(s, data_buf, RSR_len);         /* read the received data */
        }								
        disconnect(s);
        ch_status[s] = 0;
				return 2;
    
    case SOCK_CLOSED:                                               /* if a socket is closed */
				if(end_sign == 1)
				{
					end_sign = 0;
					return 2;
				}
        if(!ch_status[s])
        {
            printf("%d : Loop-Back TCP Client Started. port: %d\r\n", s, port);
            ch_status[s] = 1;
        }
        //-- socket open
        if(ip_ver == AF_INET6)
        {
            s = socket(s,Sn_MR_TCP6,any_port++,Sn_MR_ND);
        }
        else if(ip_ver == AF_INET_DUAL)
        {
            s = socket(s,Sn_MR_DUALT,any_port++,Sn_MR_ND);
        }
        else
        {
            s = socket(s,Sn_MR_TCP,any_port++,Sn_MR_ND);        
        }
        
    case SOCK_INIT:     /* if a socket is initiated */
				WIZCHIP_READ_BUF(Sn_TX_WR(s) ,tx_wr,2);
				printf("%x %x\n",tx_wr[0], tx_wr[1]);
        if((time_return() - presentTime) >= (tick_second * 3)) /* For TCP client's connection request delay : 3 sec */
        {
            if(ip_ver == AF_INET6)
                connect(s, Chconfig_Type_Def.destipv6, port, 16); /* Try to connect to TCP server(Socket, DestIP, DestPort) */
						else if(ip_ver == AF_INET_DUAL){
							if(con_ver == AF_INET6)
                connect(s, Chconfig_Type_Def.destipv6, port, 16); /* Try to connect to TCP server(Socket, DestIP, DestPort) */
							else if(con_ver == AF_INET)
								connect(s, Chconfig_Type_Def.destip, port,4);
							else return 0;
						}
            else
                connect(s, Chconfig_Type_Def.destip, port,4);

            presentTime = time_return();						
        }
				
        break;

    default:
        break;
    }
		return 0;
}
int TCP_CLIENT_ACK_PACKET_TX_test(SOCKET s, uint16 port, uint8 * data_buf, uint8 ip_ver, uint8 ndack, uint8 con_ver )
{
    uint16 RSR_len;
    uint16 received_len;
//		uint16 test_buf_len=16;    
		uint8 end_sign=0;
//		uint8_t test_buf[]={"hello i'm W6100"};
//		uint32_t loop_cnt;

    switch (getSn_SR(s))
    {
     case SOCK_ESTABLISHED:                 /* if connection is established */
        if(ch_status[s]==1)
        {
            
            printf("%d : Connected\r\n",s);
						//send(s, test_buf, test_buf_len);         /* sent the received data */									
            ch_status[s] = 2;
        }				
//				if ((RSR_len = getSn_RX_RSR(s)) > 8192 - getSn_MSSR(s))         /* check Rx data */
				if ((RSR_len = getSn_RX_RSR(s)) >= 2048 - getSn_MSSR(s) - 200)         /* check Rx data */
				{
					//if (RSR_len > TX_RX_MAX_BUF_SIZE) RSR_len = TX_RX_MAX_BUF_SIZE;   /* if Rx data size is lager than TX_RX_MAX_BUF_SIZE */
					/* the data size to read is MAX_BUF_SIZE. */
					received_len = recv(s, data_buf, 5);         /* read the received data */
				}
				//send(s, test_buf, test_buf_len);         /* sent the received data */								
				delay_ms(100);
        break;

    case SOCK_CLOSE_WAIT:                                 /* If the client request to close */
        printf("%d : CLOSE_WAIT\r\n", s);
        if ((RSR_len = getSn_RX_RSR(s)) > 0)         /* check Rx data */
        {
            if (RSR_len > TX_RX_MAX_BUF_SIZE) RSR_len = TX_RX_MAX_BUF_SIZE;   /* if Rx data size is lager than TX_RX_MAX_BUF_SIZE */
            /* the data size to read is MAX_BUF_SIZE. */
            received_len = recv(s, data_buf, RSR_len);         /* read the received data */
        }				
				delay_ms(10);
        disconnect(s);
        ch_status[s] = 0;
				return 2;
//        break;
    
    case SOCK_CLOSED:                                               /* if a socket is closed */
				if(end_sign == 1)
				{
					end_sign = 0;
					return 2;
				}
        if(!ch_status[s])
        {
            printf("%d : Loop-Back TCP Client Started. port: %d\r\n", s, port);
            ch_status[s] = 1;
        }
        //-- socket open
				WIZCHIP_WRITE( (Sn_RX_MSR(s)), 2);
				setSn_MSSR(s,1000);
				
        if(ip_ver == AF_INET6)
        {
            s = socket(s,Sn_MR_TCP6,any_port++,Sn_MR_ND);
        }
        else if(ip_ver == AF_INET_DUAL)
        {
            s = socket(s,Sn_MR_DUALT,any_port++,Sn_MR_ND);
        }
        else
        {
            s = socket(s,Sn_MR_TCP,any_port++,Sn_MR_ND);        
        }
        
    case SOCK_INIT:     /* if a socket is initiated */		
				setSn_RTR(s,2000);
				if(!ndack)				setSn_MR(s,getSn_MR(s)& ~Sn_MR_ND);
				printf("%d\r\n",getSn_MSSR(s));
//				printf("%x\r\n",getSn_MR(s));				
//				printf("%d\r\n",getRTR());	
//				printf("%d\r\n",(WIZCHIP_READ(WZ_Sn_RTR0(s))<<8)|WIZCHIP_READ(WZ_Sn_RTR1(s)));
        if((time_return() - presentTime) >= (tick_second * 3)) /* For TCP client's connection request delay : 3 sec */
        {
            if(ip_ver == AF_INET6)
                connect(s, Chconfig_Type_Def.destipv6, port, 16); /* Try to connect to TCP server(Socket, DestIP, DestPort) */
						else if(ip_ver == AF_INET_DUAL){
							if(con_ver == AF_INET6)
                connect(s, Chconfig_Type_Def.destipv6, port, 16); /* Try to connect to TCP server(Socket, DestIP, DestPort) */
							else if(con_ver == AF_INET)
								connect(s, Chconfig_Type_Def.destip, port,4);
							else return 0;
						}
            else
                connect(s, Chconfig_Type_Def.destip, port,4);

            presentTime = time_return();						
        }
        break;

    default:
        break;
    }
		return 0;
}
int TCP_CLIENT_SYN_TX_withoutARP_test(SOCKET s, uint16 port, uint8 * data_buf, uint8 ip_ver, uint8 * peer_mac, uint8 * virtual_mac, uint8 con_ver )
{
    uint16 RSR_len;
    uint16 received_len, test_buf_len=16;
		uint8 end_sign;
		uint8_t test_buf[]={"hello i'm W6100"};
		uint32_t loop_cnt;

    switch (getSn_SR(s))
    {
     case SOCK_ESTABLISHED:                 /* if connection is established */
        if(ch_status[s]==1)
        {
            
            printf("%d : Connected\r\n",s);
					
						send(s, test_buf, test_buf_len);         /* sent the received data */				
					
            ch_status[s] = 2;
						delay_ms(100);
						break;
        }
				if(ch_status[s]==2)
				{
            
            printf("%d : Connected\r\n",s);
						setSn_DHAR(s,virtual_mac);
						for (loop_cnt=0;loop_cnt<2;loop_cnt++)
						{
							send(s, test_buf, test_buf_len);         /* sent the received data */				
							delay_ms(100);
						}
					
            ch_status[s] = 3;
        }
				//send(s, test_buf, test_buf_len);         /* sent the received data */								
				delay_ms(100);
        break;

    case SOCK_CLOSE_WAIT:                                 /* If the client request to close */
        printf("%d : CLOSE_WAIT\r\n", s);
        if ((RSR_len = getSn_RX_RSR(s)) > 0)         /* check Rx data */
        {
            if (RSR_len > TX_RX_MAX_BUF_SIZE) RSR_len = TX_RX_MAX_BUF_SIZE;   /* if Rx data size is lager than TX_RX_MAX_BUF_SIZE */
            /* the data size to read is MAX_BUF_SIZE. */
            received_len = recv(s, data_buf, RSR_len);         /* read the received data */
        }				
				delay_ms(10);
        disconnect(s);
        ch_status[s] = 0;
				return 2;
    
    case SOCK_CLOSED:                                               /* if a socket is closed */
				if(end_sign == 1)
				{
					end_sign = 0;
					return 2;
				}
        if(!ch_status[s])
        {
            printf("%d : Loop-Back TCP Client Started. port: %d\r\n", s, port);
            ch_status[s] = 1;
        }
        //-- socket open
				
				setSn_DHAR(s,peer_mac);
				setSn_MR2(s,(1<<1 | 0));
        if(ip_ver == AF_INET6)
        {
            s = socket(s,Sn_MR_TCP6,any_port++,Sn_MR_ND);
        }
        else if(ip_ver == AF_INET_DUAL)
        {
            s = socket(s,Sn_MR_DUALT,any_port++,Sn_MR_ND);
        }
        else
        {
            s = socket(s,Sn_MR_TCP,any_port++,Sn_MR_ND);        
        }
        
    case SOCK_INIT:     /* if a socket is initiated */
        if((time_return() - presentTime) >= (tick_second * 3)) /* For TCP client's connection request delay : 3 sec */
        {
            if(ip_ver == AF_INET6)
                connect(s, Chconfig_Type_Def.destipv6, port, 16); /* Try to connect to TCP server(Socket, DestIP, DestPort) */
						else if(ip_ver == AF_INET_DUAL){
							if(con_ver == AF_INET6)
                connect(s, Chconfig_Type_Def.destipv6, port, 16); /* Try to connect to TCP server(Socket, DestIP, DestPort) */
							else if(con_ver == AF_INET)
								connect(s, Chconfig_Type_Def.destip, port,4);
							else return 0;
						}
            else
                connect(s, Chconfig_Type_Def.destip, port,4);

            presentTime = time_return();						
        }
        break;

    default:
        break;
    }
		return 0;
}
int TCP_CLIENT_WINDOW_FULL_test(SOCKET s, uint16 port, uint8 * data_buf, uint8 ip_ver, uint8 con_ver )
{
    uint16 RSR_len;
    uint16 received_len, test_buf_len=16;
		uint8 end_sign;
//		uint8_t test_buf[]={"hello i'm W6100"};
//		uint32_t loop_cnt;

    switch (getSn_SR(s))
    {
     case SOCK_ESTABLISHED:                 /* if connection is established */
        if(ch_status[s]==1)
        {
            
            printf("%d : Connected\r\n",s);
					
						//send(s, test_buf, test_buf_len);         /* sent the received data */										
						while(getSn_RX_RSR(s) != 1024*getSn_RxMAX(s))         /* check Rx data */
            ch_status[s] = 2;
						delay_ms(20000);
        }
				if ((RSR_len = getSn_RX_RSR(s)) > 0)         /* check Rx data */
        {
            if (RSR_len > TX_RX_MAX_BUF_SIZE) RSR_len = TX_RX_MAX_BUF_SIZE;   /* if Rx data size is lager than TX_RX_MAX_BUF_SIZE */
            /* the data size to read is MAX_BUF_SIZE. */
            received_len = recv(s, data_buf, RSR_len);         /* read the received data */
        }
				//send(s, test_buf, test_buf_len);         /* sent the received data */								
				delay_ms(100);
        break;

    case SOCK_CLOSE_WAIT:                                 /* If the client request to close */
        printf("%d : CLOSE_WAIT\r\n", s);
        if ((RSR_len = getSn_RX_RSR(s)) > 0)         /* check Rx data */
        {
            if (RSR_len > TX_RX_MAX_BUF_SIZE) RSR_len = TX_RX_MAX_BUF_SIZE;   /* if Rx data size is lager than TX_RX_MAX_BUF_SIZE */
            /* the data size to read is MAX_BUF_SIZE. */
            received_len = recv(s, data_buf, RSR_len);         /* read the received data */
        }				
				delay_ms(10);
        disconnect(s);
        ch_status[s] = 0;
				return 2;
    
    case SOCK_CLOSED:                                               /* if a socket is closed */
				if(end_sign == 1)
				{
					end_sign = 0;
					return 2;
				}
        if(!ch_status[s])
        {
            printf("%d : Loop-Back TCP Client Started. port: %d\r\n", s, port);
            ch_status[s] = 1;
        }
        //-- socket open
        if(ip_ver == AF_INET6)
        {
            s = socket(s,Sn_MR_TCP6,any_port++,Sn_MR_ND);
        }
        else if(ip_ver == AF_INET_DUAL)
        {
            s = socket(s,Sn_MR_DUALT,any_port++,Sn_MR_ND);
        }
        else
        {
            s = socket(s,Sn_MR_TCP,any_port++,Sn_MR_ND);        
        }
        
    case SOCK_INIT:     /* if a socket is initiated */
        if((time_return() - presentTime) >= (tick_second * 3)) /* For TCP client's connection request delay : 3 sec */
        {
            if(ip_ver == AF_INET6)
                connect(s, Chconfig_Type_Def.destipv6, port, 16); /* Try to connect to TCP server(Socket, DestIP, DestPort) */
						else if(ip_ver == AF_INET_DUAL){
							if(con_ver == AF_INET6)
                connect(s, Chconfig_Type_Def.destipv6, port, 16); /* Try to connect to TCP server(Socket, DestIP, DestPort) */
							else if(con_ver == AF_INET)
								connect(s, Chconfig_Type_Def.destip, port,4);
							else return 0;
						}
            else
                connect(s, Chconfig_Type_Def.destip, port,4);

            presentTime = time_return();						
        }
        break;

    default:
        break;
    }
		return 0;
}
int TCP_CLIENT_ARP_RETRANSMISSION_test(SOCKET s, uint16 port, uint8 * data_buf, uint8 ip_ver, uint8 con_ver )
{
    uint16 RSR_len;
    uint16 received_len, test_buf_len=16;
		uint8 end_sign;
		uint8_t test_buf[]={"hello i'm W6100"};
//		uint32_t loop_cnt;

    switch (getSn_SR(s))
    {
     case SOCK_ESTABLISHED:                 /* if connection is established */
        if(ch_status[s]==1)
        {
            
            printf("%d : Connected\r\n",s);
					
						send(s, test_buf, test_buf_len);         /* sent the received data */				
					
            ch_status[s] = 2;
        }
				//send(s, test_buf, test_buf_len, ip_ver);         /* sent the received data */								
				delay_ms(100);
        break;

    case SOCK_CLOSE_WAIT:                                 /* If the client request to close */
        printf("%d : CLOSE_WAIT\r\n", s);
        if ((RSR_len = getSn_RX_RSR(s)) > 0)         /* check Rx data */
        {
            if (RSR_len > TX_RX_MAX_BUF_SIZE) RSR_len = TX_RX_MAX_BUF_SIZE;   /* if Rx data size is lager than TX_RX_MAX_BUF_SIZE */
            /* the data size to read is MAX_BUF_SIZE. */
            received_len = recv(s, data_buf, RSR_len);         /* read the received data */
        }				
				delay_ms(10);
        disconnect(s);
        ch_status[s] = 0;
				return 2;
    
    case SOCK_CLOSED:                                               /* if a socket is closed */
				if(end_sign == 1)
				{
					end_sign = 0;
					return 2;
				}
        if(!ch_status[s])
        {
            printf("%d : Loop-Back TCP Client Started. port: %d\r\n", s, port);
            ch_status[s] = 1;
        }
        //-- socket open
        if(ip_ver == AF_INET6)
        {
            s = socket(s,Sn_MR_TCP6,any_port++,Sn_MR_ND);
        }
        else if(ip_ver == AF_INET_DUAL)
        {
            s = socket(s,Sn_MR_DUALT,any_port++,Sn_MR_ND);
        }
        else
        {
            s = socket(s,Sn_MR_TCP,any_port++,Sn_MR_ND);        
        }
        
    case SOCK_INIT:     /* if a socket is initiated */
        if((time_return() - presentTime) >= (tick_second * 3)) /* For TCP client's connection request delay : 3 sec */
        {
            if(ip_ver == AF_INET6)
                connect(s, virtual_ip.destipv6, port, 16); /* Try to connect to TCP server(Socket, DestIP, DestPort) */
						else if(ip_ver == AF_INET_DUAL){
							if(con_ver == AF_INET6)
                connect(s, virtual_ip.destipv6, port, 16); /* Try to connect to TCP server(Socket, DestIP, DestPort) */
							else if(con_ver == AF_INET)
								connect(s, virtual_ip.destip, port,4);
							else return 0;
						}
            else
                connect(s, virtual_ip.destip, port,4);
						
						if(getSn_IR(s) & Sn_IR_TIMEOUT) 
						{
							printf("TIME_OUT INT\r\n");
							return 2;
						}
            presentTime = time_return();
        }
				
        break;

    default:
        break;
    }
		return 0;
}
uint32_t send_16k(uint8_t s, const uint8_t * buf, uint32_t len, uint8_t flags)
{
	uint8_t status=0;
	uint8_t smode=0;
	uint8_t cmd_send=0;
	uint32_t freesize=0;
	uint32_t ret=0;
//	uint16_t loop_cnt;
//	uint8_t val[2]={0, };
//	uint32_t ptr = 0;

	// 0. check the Sn_MR  
	smode = getSn_MR(s);
    //printf("smode: %d \n",smode);
	if( smode&0x01 )
	{
		// 1. check IP version
		switch(flags)
		{
			case AF_INET :
				cmd_send  = Sn_CR_SEND;	
				break;	
			case AF_INET6:
				cmd_send  = Sn_CR_SEND6;	
				break;
//            case AF_INET_DUAL :
//				cmd_send  = Sn_CR_SEND;	
//				break;	
			default: // do not support flags
				return 0;
		}
	}
	else
		return 0;

	// 2. check the Socket free buffer size
//	if (len > getIINCHIP_TxMAX(s)) ret = getIINCHIP_TxMAX(s); 
//	else ret = len;
	//-- if freebuf is available, start.
	do
	{
		freesize = getSn_TX_FSR(s);
        //printf("freesize0: %d\r\n",freesize);
        //printf("length : %d \r\n",ret);
		status = WIZCHIP_READ(Sn_SR(s));
		if ((status != SOCK_ESTABLISHED) && (status != SOCK_CLOSE_WAIT))
		{
			ret = 0;
			break;
		}
	} while (freesize < ret);
    //printf("break\r\n");
    // 3. copy data
		wiz_send_data(s, (uint8_t *)buf, 16000);//ret);
		
	// 4. Excute Send Command and check the Command Clear
	WIZCHIP_WRITE( Sn_CR(s) ,cmd_send);
	while( WIZCHIP_READ(Sn_CR(s) ) );	
	
	// 5. Check the Send ok!
    while ( (WIZCHIP_READ(Sn_IR(s) ) & Sn_IR_SENDOK) != Sn_IR_SENDOK )
	{
        //printf("freesize1: %d\r\n",freesize);
		status = WIZCHIP_READ(Sn_SR(s));
		if ((status != SOCK_ESTABLISHED) && (status != SOCK_CLOSE_WAIT) )
		{
			printf("SEND_OK Problem!!\r\n");
			close(s);
			return 0;
		}
	}
	
	WIZCHIP_WRITE( Sn_IRCLR(s) , Sn_IR_SENDOK);
    //printf("Send OK\r\n");
    //freesize = getSn_TX_FSR(s);
    //printf("freesize2: %d\r\n",freesize);
    //printf("length : %d \r\n",ret);

#ifdef __DEF_IINCHIP_INT__
	putISR(s, getISR(s) & (~Sn_IR_SEND_OK));
#else
	clearSn_IR( s, Sn_IR_SENDOK );
#endif

	return ret;
}
int TCP_CLIENT_DATA_RETRANSMISSION_test(SOCKET s, uint16 port, uint8 * data_buf, uint8 ip_ver, uint8 con_ver )
{
    uint16 RSR_len;
    uint16 received_len, test_buf_len=16;
		uint8 end_sign;
		uint8_t test_buf[]={"hello i'm W6100"};
//		uint32_t loop_cnt;
//		uint8 test_txsize[MAX_SOCK_NUM] = {16, 0, 0, 0};//, 0, 0};
//		uint8 test_rxsize[MAX_SOCK_NUM] = {16, 0, 0, 0};//, 0, 0};
		unsigned char test_AdrSet[2][8] = {{16,0,0,0,0,0,0,0},{16,0,0,0,0,0,0,0}};
		
    switch (getSn_SR(s))
    {
     case SOCK_ESTABLISHED:                 /* if connection is established */
        if(ch_status[s]==1)
        {
            
            printf("%d : Connected\r\n",s);						
						delay_ms(5000);
						//send_16k(s, test_buf, test_buf_len, ip_ver);         /* sent the received data */				
						send(s, test_buf, test_buf_len);         /* sent the received data */										
					
            ch_status[s] = 2;
        }
				//send(s, test_buf, test_buf_len);         /* sent the received data */								
				delay_ms(100);
        break;

    case SOCK_CLOSE_WAIT:                                 /* If the client request to close */
        printf("%d : CLOSE_WAIT\r\n", s);
        if ((RSR_len = getSn_RX_RSR(s)) > 0)         /* check Rx data */
        {
            if (RSR_len > TX_RX_MAX_BUF_SIZE) RSR_len = TX_RX_MAX_BUF_SIZE;   /* if Rx data size is lager than TX_RX_MAX_BUF_SIZE */
            /* the data size to read is MAX_BUF_SIZE. */
            received_len = recv(s, data_buf, RSR_len);         /* read the received data */
        }				
				delay_ms(10);
        disconnect(s);
        ch_status[s] = 0;
				return 2;
    
    case SOCK_CLOSED:                                               /* if a socket is closed */
				if(end_sign == 1)
				{
					end_sign = 0;
					return 2;
				}
        if(!ch_status[s])
        {
            printf("%d : Loop-Back TCP Client Started. port: %d\r\n", s, port);
            ch_status[s] = 1;
        }
        //-- socket open
//				sysinit( test_txsize, test_rxsize );
				sysinit( test_AdrSet);
        if(ip_ver == AF_INET6)
        {
            s = socket(s,Sn_MR_TCP6,any_port++,Sn_MR_ND);
        }
        else if(ip_ver == AF_INET_DUAL)
        {
            s = socket(s,Sn_MR_DUALT,any_port++,Sn_MR_ND);
        }
        else
        {
            s = socket(s,Sn_MR_TCP,any_port++,Sn_MR_ND);        
        }
        
    case SOCK_INIT:     /* if a socket is initiated */
        if((time_return() - presentTime) >= (tick_second * 3)) /* For TCP client's connection request delay : 3 sec */
        {
            if(ip_ver == AF_INET6)
                connect(s, Chconfig_Type_Def.destipv6, port, 16); /* Try to connect to TCP server(Socket, DestIP, DestPort) */
						else if(ip_ver == AF_INET_DUAL){
							if(con_ver == AF_INET6)
                connect(s, Chconfig_Type_Def.destipv6, port, 16); /* Try to connect to TCP server(Socket, DestIP, DestPort) */
							else if(con_ver == AF_INET)
								connect(s, Chconfig_Type_Def.destip, port,4);
							else return 0;
						}
            else
                connect(s, Chconfig_Type_Def.destip, port,4);

            presentTime = time_return();						
        }
        break;

    default:
        break;
    }
		return 0;
}
int TCP_SERVER_3ACK_test(SOCKET s, uint16 port, uint8 * data_buf, uint8 ip_ver )
{
    uint16 RSR_len;
    uint16 received_len, test_buf_len=16;
		uint8 end_sign;
		uint8_t test_buf[]={"hello i'm W6100"};		
//		uint32_t loop_cnt;

    switch (getSn_SR(s))
    {
     case SOCK_ESTABLISHED:                 /* if connection is established */
        if(ch_status[s]==1)
        {
            
            printf("%d : Connected\r\n",s);
						delay_ms(5000);
						send(s, test_buf, test_buf_len);         /* sent the received data */				
					
            ch_status[s] = 2;
        }
				//send(s, test_buf, test_buf_len);         /* sent the received data */			
				if ((RSR_len = getSn_RX_RSR(s)) > 0)         /* check Rx data */
        {
            if (RSR_len > TX_RX_MAX_BUF_SIZE) RSR_len = TX_RX_MAX_BUF_SIZE;   /* if Rx data size is lager than TX_RX_MAX_BUF_SIZE */
            /* the data size to read is MAX_BUF_SIZE. */
            received_len = recv(s, data_buf, RSR_len);         /* read the received data */												
						send(s, data_buf, received_len);         /* sent the received data */										
        }
				delay_ms(100);
        break;

    case SOCK_CLOSE_WAIT:                                 /* If the client request to close */
        printf("%d : CLOSE_WAIT\r\n", s);
        if ((RSR_len = getSn_RX_RSR(s)) > 0)         /* check Rx data */
        {
            if (RSR_len > TX_RX_MAX_BUF_SIZE) RSR_len = TX_RX_MAX_BUF_SIZE;   /* if Rx data size is lager than TX_RX_MAX_BUF_SIZE */
            /* the data size to read is MAX_BUF_SIZE. */
            received_len = recv(s, data_buf, RSR_len);         /* read the received data */
        }
				delay_ms(10);
        disconnect(s);
        ch_status[s] = 0;
				return 2;
    
    case SOCK_CLOSED:                                               /* if a socket is closed */
				if(end_sign == 1)
				{
					end_sign = 0;
					return 2;
				}
				setSn_RTR(s,60000);
				WIZCHIP_WRITE(Sn_RCR(s),5);
        if(!ch_status[s])
        {
            printf("%d : Loop-Back TCP Client Started. port: %d\r\n", s, port);
            ch_status[s] = 1;
        }
        //-- socket open
        if(ip_ver == AF_INET6)
        {
            s = socket(s,Sn_MR_TCP6,port,Sn_MR_ND);
        }
        else if(ip_ver == AF_INET_DUAL)
        {
            s = socket(s,Sn_MR_DUALT,port,Sn_MR_ND);
        }
        else
        {
            s = socket(s,Sn_MR_TCP,port,Sn_MR_ND);        
        }
        
    case SOCK_INIT:     /* if a socket is initiated */        
				printf("[SOCK_INIT]\r\n");
        listen(s);
        printf("[REQUEST_CONNECT]\r\n");
        break;

    default:
        break;
    }
		return 0;
}
int TCP_CLIENT_3ACK_SEND_test(SOCKET s, uint16 port, uint8 * data_buf, uint8 ip_ver )
{
    uint16 RSR_len;
    uint16 received_len=16;
		uint8 end_sign, tx_wr[2];
		uint8_t test_buf[]={"hello i'm W6100"};
		uint8_t uart_cmd;
		//uint32_t loop_cnt;
//		uint16_t size = 0;
//		uint32_t timer_delay=1;

    switch (getSn_SR(s))
    {
     case SOCK_ESTABLISHED:                 /* if connection is established */
        if(ch_status[s]==1)
        {
            
            printf("%d : Connected\r\n",s);
						
						//send(s, test_buf, received_len);         /* sent the received data */
						//delay_ms(100);
						
						//setSn_CR(sn,Sn_CR_SEND);
						/* wait to process the command... */
            //while(WIZCHIP_READ(WZ_Sn_CR(s)));												
						delay_ms(100);														
            ch_status[s] = 2;
        }
				while(1)
        {
            uart_cmd = UartGetc();
            if( uart_cmd == 'j')
                break;
        }				
				//while(!(WIZCHIP_READ(WZ_PHYSR)&1));					
				//while(1) printf("%x\r\n",WIZCHIP_READ(WZ_PHYSR));
				send(s, test_buf, 0);         /* sent the received data */
				send(s, test_buf, 0);         /* sent the received data */
				send(s, test_buf, 0);         /* sent the received data */
				uart_cmd = 'a';
				
				delay_ms(100); //zerobye send interval
				return 1;

    case SOCK_CLOSE_WAIT:                                 /* If the client request to close */
        printf("%d : CLOSE_WAIT\r\n", s);
        if ((RSR_len = getSn_RX_RSR(s)) > 0)         /* check Rx data */
        {
            if (RSR_len > TX_RX_MAX_BUF_SIZE) RSR_len = TX_RX_MAX_BUF_SIZE;   /* if Rx data size is lager than TX_RX_MAX_BUF_SIZE */
            /* the data size to read is MAX_BUF_SIZE. */
            received_len = recv(s, data_buf, RSR_len);         /* read the received data */
        }								
        disconnect(s);
        ch_status[s] = 0;
				return 2;
    
    case SOCK_CLOSED:                                               /* if a socket is closed */
				if(end_sign == 1)
				{
					end_sign = 0;
					return 2;
				}
        if(!ch_status[s])
        {
            printf("%d : Loop-Back TCP Client Started. port: %d\r\n", s, port);
            ch_status[s] = 1;
        }
        //-- socket open
        if(ip_ver == AF_INET6)
        {
            s = socket(s,Sn_MR_TCP6,any_port++,Sn_MR_ND);
        }
        else if(ip_ver == AF_INET_DUAL)
        {
            s = socket(s,(Sn_MR_DUALT),any_port++,Sn_MR_ND);
        }
        else
        {
            s = socket(s,(Sn_MR_TCP),any_port++,Sn_MR_ND);        
        }
        
    case SOCK_INIT:     /* if a socket is initiated */
				WIZCHIP_READ_BUF(Sn_TX_WR(s) ,tx_wr,2);
				printf("%x %x\n",tx_wr[0], tx_wr[1]);
        if((time_return() - presentTime) >= (tick_second * 3)) /* For TCP client's connection request delay : 3 sec */
        {
            if(ip_ver == AF_INET6)
                connect(s, Chconfig_Type_Def.destipv6, port, 16); /* Try to connect to TCP server(Socket, DestIP, DestPort) */
            else
                connect(s, Chconfig_Type_Def.destip, port,4);

            presentTime = time_return();						
        }
				
        break;

    default:
        break;
    }
		
		return 0;
}
int TCP_SERVER_RSTBLK_test(SOCKET s, uint16 port, uint8 * data_buf, uint8 ip_ver, uint8 rstblk )
{
    uint16 RSR_len;
    uint16 received_len;
		uint8 end_sign;
//		uint8_t test_buf[]={"hello i'm W6100"};
//		uint32_t loop_cnt;

    switch (getSn_SR(s))
    {
     case SOCK_ESTABLISHED:                 /* if connection is established */
        if(ch_status[s]==1)
        {
            
            printf("%d : Connected\r\n",s);					
						//send(s, test_buf, test_buf_len);         /* sent the received data */				
						if ((RSR_len = getSn_RX_RSR(s)) > 0)         /* check Rx data */
						{
							if (RSR_len > TX_RX_MAX_BUF_SIZE) RSR_len = TX_RX_MAX_BUF_SIZE;   /* if Rx data size is lager than TX_RX_MAX_BUF_SIZE */
							/* the data size to read is MAX_BUF_SIZE. */
							received_len = recv(s, data_buf, RSR_len);         /* read the received data */
						}					
						//send(s, test_buf, test_buf_len);         /* sent the received data */
            ch_status[s] = 2;
        }
				//send(s, test_buf, test_buf_len);         /* sent the received data */					
				if ((RSR_len = getSn_RX_RSR(s)) > 0)         /* check Rx data */
				{
					if (RSR_len > TX_RX_MAX_BUF_SIZE) RSR_len = TX_RX_MAX_BUF_SIZE;   /* if Rx data size is lager than TX_RX_MAX_BUF_SIZE */
					/* the data size to read is MAX_BUF_SIZE. */
					received_len = recv(s, data_buf, RSR_len);         /* read the received data */
					send(s, data_buf, RSR_len);         /* sent the received data */
					disconnect(s);
					ch_status[s] = 0;
					return 2;
				}				
				delay_ms(100);
        break;

    case SOCK_CLOSE_WAIT:                                 /* If the client request to close */
        printf("%d : CLOSE_WAIT\r\n", s);
        if ((RSR_len = getSn_RX_RSR(s)) > 0)         /* check Rx data */
        {
            if (RSR_len > TX_RX_MAX_BUF_SIZE) RSR_len = TX_RX_MAX_BUF_SIZE;   /* if Rx data size is lager than TX_RX_MAX_BUF_SIZE */
            /* the data size to read is MAX_BUF_SIZE. */
            received_len = recv(s, data_buf, RSR_len);         /* read the received data */
        }
				delay_ms(10);
        disconnect(s);
        ch_status[s] = 0;
				//return 2;
        break;
    
    case SOCK_CLOSED:                                               /* if a socket is closed */
				if(end_sign == 1)
				{
					if(getSn_IR(s) & Sn_IR_CON)	// Socket n interrupt register mask; TCP CON interrupt = connection with peer is successful
          {
						printf("%d:Connected to - %d.%d.%d.%d : %d\r\n",s, Chconfig_Type_Def.destip[0], Chconfig_Type_Def.destip[1], Chconfig_Type_Def.destip[2], Chconfig_Type_Def.destip[3], port);
						clearSn_IR(s, Sn_IR_CON);  // this interrupt should be write the bit cleared to '1'
          }
					end_sign = 0;
					return 2;
				}
        if(!ch_status[s])
        {
            printf("%d : Loop-Back TCP Client Started. port: %d\r\n", s, port);
            ch_status[s] = 1;
        }
        //-- socket open
				setNET4MR(rstblk);
				setNET6MR(rstblk);
        if(ip_ver == AF_INET6)
        {
            s = socket(s,Sn_MR_TCP6,port,Sn_MR_ND);
        }
        else if(ip_ver == AF_INET_DUAL)
        {
            s = socket(s,Sn_MR_DUALT,port,Sn_MR_ND);
        }
        else
        {   
						s = socket(s,Sn_MR_TCP,port,Sn_MR_ND);        					
        }
        
    case SOCK_INIT:     /* if a socket is initiated */        
				printf("[SOCK_INIT]\r\n");
        listen(s);
				printf("%x :LISTEN socket %d\r\n",getSn_SR(s), s);
        break;

    default:
        break;
    }
		return 0;
}






































//int TCP_Keep_Alive_Timer_Test(SOCKET s, uint16_t port, uint8_t *data_buf, uint8_t ip_ver, uint8_t timer_delay)
//{
//    uint16 RSR_len;
//    uint16 received_len;
//    uint8 tmp;

//    switch (getSn_SR(s))
//    {
//        case SOCK_ESTABLISHED:              /* if connection is established */
//            if(ch_status[s]==1)
//            {
//                printf("[SOCK_ESTABLISHED]\r\n");
//                ch_status[s] = 2;
//            }
//            if ((RSR_len = getSn_RX_RSR(s)) > 0)        /* check Rx data */
//            {
//                if (RSR_len > TX_RX_MAX_BUF_SIZE) RSR_len = TX_RX_MAX_BUF_SIZE;   /* if Rx data size is lager than TX_RX_MAX_BUF_SIZE */ /* the data size to read is MAX_BUF_SIZE. */
//                received_len = recv(s, data_buf, RSR_len);      /* read the received data */
//                data_buf[received_len] = '\0';
//                printf("[RECVED]%s\r\n",data_buf);
//                send(s, data_buf, received_len);         /* sent the received data */
//                
//                delay_ms(timer_delay * 10);

//                end_signal[s] = 1;
//                printf("[REQUEST_CLOSE]\r\n");
//            }
//            
//            break;
//            
//        case SOCK_CLOSE_WAIT:                              /* If the client request to close */
//            printf("[SOCK_CLOSE_WAIT]\r\n");
//            if ((RSR_len = getSn_RX_RSR(s)) > 0)     /* check Rx data */
//            {
//                if (RSR_len > TX_RX_MAX_BUF_SIZE) RSR_len = TX_RX_MAX_BUF_SIZE;  /* if Rx data size is lager than TX_RX_MAX_BUF_SIZE *//* the data size to read is MAX_BUF_SIZE. */
//                recv(s, data_buf, RSR_len);     /* read the received data */
//            }
//            disconnect(s);
//            ch_status[s] = 0;
//            break;
//        
//        case SOCK_CLOSED:                                       /* if a socket is closed */
//             if(end_signal[s] == 1)
//             {
//                 end_signal[s] = 0;
//                 printf("[TEST_FINISHED]\r\n");
//                 return 2;
//             }
//        
//            if(!ch_status[s])
//            {
//                printf("[SOCK_CLOSED]\r\n");
//                ch_status[s] = 1;
//            }
//            
//            WIZCHIP_WRITE(WZ_Sn_KPALVTR(s), timer_delay);

//            //-- socket open
//            if(ip_ver == AF_INET6)
//            {
//                tmp = socket(s,(Sn_MR_ND|Sn_MR_TCP|Sn_MR_TCP6),port,0x00);
//            }
//            else if(ip_ver == AF_INET_DUAL)
//            {
//                tmp = socket(s,(Sn_MR_ND|Sn_MR_TCP|Sn_MR_DUALT),port,0x00);
//            }
//            else
//            {
//                tmp = socket(s,(Sn_MR_ND|Sn_MR_TCP),port,0x00);        
//            }

//            if(tmp == 0)    /* reinitialize the socket */
//            {
//                printf("[ERROR]: %d : Fail to create socket.",s);
//                ch_status[s] = 0;
//            }
//            break;
//        
//        case SOCK_INIT:   /* if a socket is initiated */
//            printf("[SOCK_INIT]\r\n");
//            listen(s);
//            printf("[REQUEST_CONNECT]\r\n");
//            break;

//        default:
//            break;
//    }
//    
//    return 0;    
//}

//int TCP_Keep_Alive_Send_Test(SOCKET s, uint16_t port, uint8_t *data_buf, uint8_t ip_ver)
//{
//    uint16 RSR_len;
//    uint16 received_len;
//    uint8 tmp;
//    uint32_t loop_cnt;

//    switch (getSn_SR(s))
//    {
//        case SOCK_ESTABLISHED:              /* if connection is established */
//            if(ch_status[s]==1)
//            {
//                printf("[SOCK_ESTABLISHED]\r\n");
//                ch_status[s] = 2;
//            }
//            if ((RSR_len = getSn_RX_RSR(s)) > 0)        /* check Rx data */
//            {
//                if (RSR_len > TX_RX_MAX_BUF_SIZE) RSR_len = TX_RX_MAX_BUF_SIZE;   /* if Rx data size is lager than TX_RX_MAX_BUF_SIZE */ /* the data size to read is MAX_BUF_SIZE. */
//                received_len = recv(s, data_buf, RSR_len);      /* read the received data */
//                data_buf[received_len] = '\0';
//                printf("[RECVED]%s\r\n",data_buf);
//                send(s, data_buf, received_len);         /* sent the received data */


//                for(loop_cnt=0;loop_cnt<10;loop_cnt++)
//                {
//                    printf("[SEND_KEEPALIVE]\r\n");
//                    delay_ms(1000);
//                    WIZCHIP_WRITE(WZ_Sn_CR(s), Sn_CR_SEND_KEEP);
//                }

//                end_signal[s] = 1;
//                printf("[REQUEST_CLOSE]\r\n");
//            }
//            
//            break;
//            
//        case SOCK_CLOSE_WAIT:                              /* If the client request to close */
//            printf("[SOCK_CLOSE_WAIT]\r\n");
//            if ((RSR_len = getSn_RX_RSR(s)) > 0)     /* check Rx data */
//            {
//                if (RSR_len > TX_RX_MAX_BUF_SIZE) RSR_len = TX_RX_MAX_BUF_SIZE;  /* if Rx data size is lager than TX_RX_MAX_BUF_SIZE *//* the data size to read is MAX_BUF_SIZE. */
//                recv(s, data_buf, RSR_len);     /* read the received data */
//            }
//            disconnect(s);
//            ch_status[s] = 0;
//            break;
//        
//        case SOCK_CLOSED:                                       /* if a socket is closed */
//             if(end_signal[s] == 1)
//             {
//                 end_signal[s] = 0;
//                 printf("[TEST_FINISHED]\r\n");                 
//                 return 2;
//             }
//        
//            if(!ch_status[s])
//            {
//                printf("[SOCK_CLOSED]\r\n");
//                ch_status[s] = 1;
//            }

//            //-- socket open
//            if(ip_ver == AF_INET6)
//            {
//                tmp = socket(s,(Sn_MR_ND|Sn_MR_TCP|Sn_MR_TCP6),port,0x00);
//            }
//            else if(ip_ver == AF_INET_DUAL)
//            {
//                tmp = socket(s,(Sn_MR_ND|Sn_MR_TCP|Sn_MR_DUALT),port,0x00);
//            }
//            else
//            {
//                tmp = socket(s,(Sn_MR_ND|Sn_MR_TCP),port,0x00);        
//            }

//            if(tmp == 0)    /* reinitialize the socket */
//            {
//                printf("[ERROR]: %d : Fail to create socket.",s);
//                ch_status[s] = 0;
//            }
//            break;
//        
//        case SOCK_INIT:   /* if a socket is initiated */
//            printf("[SOCK_INIT]\r\n");
//            listen(s);
//            printf("[REQUEST_CONNECT]\r\n");
//            break;

//        default:
//            break;
//    }
//    
//    return 0;    
//}



//int TCP_Transmit_Test(SOCKET s, uint16_t port, uint8_t *data_buf, uint32_t size, uint32_t send_cnt, uint8_t ip_ver)
//{
//    uint16 RSR_len;
//    uint8 tmp;
//    uint32_t loop_cnt;

//    switch (getSn_SR(s))
//    {
//        case SOCK_ESTABLISHED:              /* if connection is established */
//            if(ch_status[s]==1)
//            {
//                printf("[SOCK_ESTABLISHED]\r\n");
//                ch_status[s] = 2;

//                if(size == 0)
//                {
//                    for(loop_cnt=0;loop_cnt<send_cnt;loop_cnt++)
//                    {
//                        printf("[SEND_DATA,SIZE:%d]\r\n",size);
//                        send(s, data_buf, size);         /* sent the received data */
//                    }
//                    end_signal[s] = 1;
//                    printf("[REQUEST_CLOSE]\r\n");
//                }
//                else
//                {
//                    for(loop_cnt=1;loop_cnt<=size;loop_cnt++)
//                    {
//                        printf("[SEND_DATA,SIZE:%d]\r\n",loop_cnt);
//                        send(s, data_buf, loop_cnt);         /* sent the received data */
//                    }
//                    end_signal[s] = 1;
//                    printf("[REQUEST_CLOSE]\r\n");
//                }
//            }
//            break;
//            
//        case SOCK_CLOSE_WAIT:                              /* If the client request to close */
//            printf("[SOCK_CLOSE_WAIT]\r\n");
//            if ((RSR_len = getSn_RX_RSR(s)) > 0)     /* check Rx data */
//            {
//                if (RSR_len > TX_RX_MAX_BUF_SIZE) RSR_len = TX_RX_MAX_BUF_SIZE;  /* if Rx data size is lager than TX_RX_MAX_BUF_SIZE *//* the data size to read is MAX_BUF_SIZE. */
//                recv(s, data_buf, RSR_len);     /* read the received data */
//            }
//            disconnect(s);
//            ch_status[s] = 0;
//            break;
//        
//        case SOCK_CLOSED:                                       /* if a socket is closed */
//             if(end_signal[s] == 1)
//             {
//                 end_signal[s] = 0;
//                 printf("[TEST_FINISHED]\r\n");
//                 return 2;
//             }
//        
//            if(!ch_status[s])
//            {
//                printf("[SOCK_CLOSED]\r\n");
//                ch_status[s] = 1;
//            }

//            //-- socket open
//            if(ip_ver == AF_INET6)
//            {
//                tmp = socket(s,(Sn_MR_ND|Sn_MR_TCP|Sn_MR_TCP6),port,0x00);
//            }
//            else if(ip_ver == AF_INET_DUAL)
//            {
//                tmp = socket(s,(Sn_MR_ND|Sn_MR_TCP|Sn_MR_DUALT),port,0x00);
//            }
//            else
//            {
//                tmp = socket(s,(Sn_MR_ND|Sn_MR_TCP),port,0x00);        
//            }

//            if(tmp == 0)    /* reinitialize the socket */
//            {
//                printf("[ERROR]: %d : Fail to create socket.",s);
//                ch_status[s] = 0;
//            }
//            break;
//        
//        case SOCK_INIT:   /* if a socket is initiated */
//            printf("[SOCK_INIT]\r\n");
//            listen(s);
//            printf("[REQUEST_CONNECT]\r\n");
//            break;

//        default:
//            break;
//    }
//    
//    return 0;    
//}


//int TCP_Status_Check(SOCKET s, uint16_t port, uint8_t *data_buf, uint8_t ip_ver)
//{
//    uint16 RSR_len;
//    uint16 received_len;
//    uint8 tmp;

//    
//    switch (getSn_SR(s))
//    {
//        case SOCK_ESTABLISHED:              /* if connection is established */
//            if(ch_status[s]==1)
//            {
//                printf("[SOCK_ESTABLISHED]\r\n");
//                ch_status[s] = 2;
//                printf("[REQUEST_SEND]\r\n");
//                
//            }
//            if ((RSR_len = getSn_RX_RSR(s)) > 0)        /* check Rx data */
//            {
//                if (RSR_len > TX_RX_MAX_BUF_SIZE) RSR_len = TX_RX_MAX_BUF_SIZE;   /* if Rx data size is lager than TX_RX_MAX_BUF_SIZE */ /* the data size to read is MAX_BUF_SIZE. */
//                received_len = recv(s, data_buf, RSR_len);      /* read the received data */
//                data_buf[received_len] = '\0';
//                printf("[RECVED]%s\r\n",data_buf);
//                send(s, data_buf, received_len);         /* sent the received data */
//                end_signal[s] = 1;
//                printf("[REQUEST_CLOSE]\r\n");
//            }
//            break;
//            
//        case SOCK_CLOSE_WAIT:                              /* If the client request to close */
//            printf("[SOCK_CLOSE_WAIT]\r\n");
//            if ((RSR_len = getSn_RX_RSR(s)) > 0)     /* check Rx data */
//            {
//                if (RSR_len > TX_RX_MAX_BUF_SIZE) RSR_len = TX_RX_MAX_BUF_SIZE;  /* if Rx data size is lager than TX_RX_MAX_BUF_SIZE *//* the data size to read is MAX_BUF_SIZE. */
//                received_len = recv(s, data_buf, RSR_len);     /* read the received data */
//            }
//            disconnect(s);
//            ch_status[s] = 0;
//            break;
//        
//        case SOCK_CLOSED:                                       /* if a socket is closed */
//             if(end_signal[s] == 1)
//             {
//                 end_signal[s] = 0;
//                 printf("[TEST_FINISHED]\r\n");
//                 return 2;
//             }
//        
//            if(!ch_status[s])
//            {
//                printf("[SOCK_CLOSED]\r\n");
//                ch_status[s] = 1;
//            }

//            //-- socket open
//            if(ip_ver == AF_INET6)
//            {
//                tmp = socket(s,(Sn_MR_ND|Sn_MR_TCP|Sn_MR_TCP6),port,0x00);
//            }
//            else if(ip_ver == AF_INET_DUAL)
//            {
//                tmp = socket(s,(Sn_MR_ND|Sn_MR_TCP|Sn_MR_STREAMD),port,0x00);
//            }
//            else
//            {
//                tmp = socket(s,(Sn_MR_ND|Sn_MR_TCP),port,0x00);        
//            }

//            if(tmp == 0)    /* reinitialize the socket */
//            {
//                printf("[ERROR]: %d : Fail to create socket.",s);
//                ch_status[s] = 0;
//            }
//            break;
//        
//        case SOCK_INIT:   /* if a socket is initiated */
//            printf("[SOCK_INIT]\r\n");
//            listen(s);
//            printf("[REQUEST_CONNECT]\r\n");
//            break;

//        default:
//            break;
//    }
//    
//    return 0;
//}



//int TCP_TXRX_RDWR_Pointer_Check(SOCKET sn, uint16_t port, uint8_t *data_buf, uint8_t ip_ver)
//{
//    static uint8_t is_first[8] = {0,};
//    uint8_t val[2]={0,};
//    int32_t rx_wr_ptr;
//    int32_t rx_rd_ptr;
//    int32_t tx_wr_ptr;
//    static uint32_t tx_wr_random[8], tx_rd_random[8];
//    int32_t tx_rd_ptr;
//    uint8 tmp;
//    uint16 RSR_len, received_len;

//    switch (getSn_SR(sn))
//    {
//        case SOCK_ESTABLISHED:
//            if(is_first[sn] == 1)
//            {
//                is_first[sn] = 2;
//                WIZCHIP_READ_BUF( WZ_Sn_RXRDR0(sn),val ,2 ) ;
//                rx_rd_ptr = (uint16_t)(val[0]<<8)+ val[1];
//                
//                WIZCHIP_READ_BUF( WZ_Sn_RXWR0(sn),val ,2 ) ;
//                rx_wr_ptr = (uint16_t)(val[0]<<8)+ val[1];
//                
//                WIZCHIP_READ_BUF( WZ_Sn_TXRDR0(sn),val ,2 ) ;
//                tx_rd_random[sn] = (uint16_t)(val[0]<<8)+ val[1];
//                WIZCHIP_READ_BUF( WZ_Sn_TXWR0(sn),val ,2 ) ;
//                tx_wr_random[sn] = (uint16_t)(val[0]<<8)+ val[1];
//                
//                if( (rx_rd_ptr == 0) && (rx_wr_ptr == 0) && (tx_rd_random[sn] != 0) && (tx_wr_random[sn] != 0) )
//                    printf("[REQUEST_CLOSE] : rx_rd:%x,rx_wr:%x,tx_rd:%x,tx_wr:%x\r\n",rx_rd_ptr, rx_wr_ptr, tx_rd_random[sn], tx_wr_random[sn]);
//                else
//                {
//                    printf("[ESTABLISHED]ERROR : rx_rd : %x, rx_wr : %x, tx_rd : %x, tx_wr : %x\r\n",rx_rd_ptr, rx_wr_ptr, tx_rd_random[sn], tx_wr_random[sn]);
//                    return -1;
//                }
//            }
//            if((RSR_len = getSn_RX_RSR(sn)) > 0)
//            {
//                if (RSR_len > TX_RX_MAX_BUF_SIZE) RSR_len = TX_RX_MAX_BUF_SIZE;   /* if Rx data size is lager than TX_RX_MAX_BUF_SIZE */ /* the data size to read is MAX_BUF_SIZE. */
//                received_len = recv(sn, data_buf, RSR_len);      /* read the received data */
//                data_buf[received_len] = '\0';
//                printf("%s",data_buf);
//                if( strncmp((char*)data_buf,"[END_TEST]",received_len) == 0)
//                {    
//                    end_signal[sn] = 1;
//                }
//            }

//            break;
//        case SOCK_CLOSE_WAIT:
//                disconnect(sn);
//                is_first[sn] = 1;
//                break;

//        case SOCK_CLOSED:
//            if(end_signal[sn] == 1)
//            {
//                end_signal[sn] = 0;
//                printf("[TEST_FINISHED]\r\n");
//                return 2;
//            }

//            if (is_first[sn] == 0)
//            {
//                is_first[sn] = 1;
//                WIZCHIP_READ_BUF( WZ_Sn_RXRDR0(sn),val ,2 ) ;
//                rx_rd_ptr = (uint16_t)(val[0]<<8)+ val[1];
//                
//                WIZCHIP_READ_BUF( WZ_Sn_RXWR0(sn),val ,2 ) ;
//                rx_wr_ptr = (uint16_t)(val[0]<<8)+ val[1];
//                
//                WIZCHIP_READ_BUF( WZ_Sn_TXRDR0(sn),val ,2 ) ;
//                tx_rd_ptr = (uint16_t)(val[0]<<8)+ val[1];

//                WIZCHIP_READ_BUF( WZ_Sn_TXWR0(sn),val ,2 ) ;
//                tx_wr_ptr = (uint16_t)(val[0]<<8)+ val[1];
//                
//                if( (rx_rd_ptr == 0) && (rx_wr_ptr == 0) && (tx_rd_ptr == 0) && (tx_wr_ptr == 0))
//                {
//                    printf("[REQUEST_CONNECT] : rx_rd :%x,rx_wd:%x,tx_rd:%x,tx_wd:%x\r\n",rx_rd_ptr,rx_wr_ptr,tx_rd_ptr,tx_wr_ptr);
//                }
//                else
//                {
//                    printf("[TEST ERROR1] : rx_rd:%x,rx_wr:%x,tx_rd:%x,tx_wr:%x\r\n",rx_rd_ptr, rx_wr_ptr, tx_rd_ptr, tx_wr_ptr);
//                    return -1;
//                }
//            }
//            else
//            {
//                WIZCHIP_READ_BUF( WZ_Sn_RXRDR0(sn),val ,2 ) ;
//                rx_rd_ptr = (uint16_t)(val[0]<<8)+ val[1];
//                
//                WIZCHIP_READ_BUF( WZ_Sn_RXWR0(sn),val ,2 ) ;
//                rx_wr_ptr = (uint16_t)(val[0]<<8)+ val[1];

//                WIZCHIP_READ_BUF( WZ_Sn_TXRDR0(sn),val ,2 ) ;
//                tx_rd_ptr = (uint16_t)(val[0]<<8)+ val[1];

//                WIZCHIP_READ_BUF( WZ_Sn_TXWR0(sn),val ,2 ) ;
//                tx_wr_ptr = (uint16_t)(val[0]<<8)+ val[1];
//                
//                if( (rx_rd_ptr == 0) && (rx_wr_ptr == 0) && (tx_rd_ptr == tx_rd_random[sn]) && (tx_wr_ptr == tx_wr_random[sn]))
//                {
//                    printf("[TEST SUCCESS] : rx_rd:%x,rx_wr:%x,tx_rd[%x]:%x,tx_wr[%x]:%x\r\n",rx_rd_ptr, rx_wr_ptr, tx_rd_random[sn], tx_rd_ptr, tx_wr_random[sn], tx_wr_ptr);
//                    printf("[REQUEST_CONNECT] : rx_rd :%x,rx_wd:%x,tx_rd:%x,tx_wd:%x\r\n",rx_rd_ptr,rx_wr_ptr,tx_rd_ptr,tx_wr_ptr);
//                }
//                else
//                {
//                    printf("[TEST ERROR2] : rx_rd:%x,rx_wr:%x,tx_rd[%x]:%x,tx_wr[%x]:%x\r\n",rx_rd_ptr, rx_wr_ptr, tx_rd_random[sn], tx_rd_ptr, tx_wr_random[sn], tx_wr_ptr);
//                    printf("[TEST] : tx_rd_randon[%d]:%x, tx_rd_ptr:%x\r\n",sn,tx_rd_random[sn],tx_rd_ptr);
//                    printf("[TEST] : tx_wr_randon[%d]:%x, tx_wr_ptr:%x\r\n",sn,tx_wr_random[sn],tx_wr_ptr);
//                    return -1;
//                }
//            }
//            //-- socket open
//            if(ip_ver == AF_INET6)
//            {
//                tmp = socket(sn,(Sn_MR_ND|Sn_MR_TCP|Sn_MR_TCP6),port,0x00);
//            }
//            else if(ip_ver == AF_INET_DUAL)
//            {
//                tmp = socket(sn,(Sn_MR_ND|Sn_MR_TCP|Sn_MR_STREAMD),port,0x00);
//            }
//            else
//            {
//                tmp = socket(sn,(Sn_MR_ND|Sn_MR_TCP),port,0x00);        
//            }

//            if(tmp == 0)    /* reinitialize the socket */
//            {
//                printf("\r\n%d : Fail to create socket.",sn);
//                end_signal[sn] = 0;
//            }
//            break;
//        case SOCK_INIT:
//            listen(sn);
//            break;
//            
//        default:
//            break;
//    }    

//     return 0;
//}
