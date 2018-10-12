//****************************************************************************/ 
//!
//! \file wizchip_conf.c
//! \brief WIZCHIP Config Header File.
//! \version 1.0.1
//! \date 2018/09/20
//! \par  Revision history
//!       <2018/09/20> 1st Release
//! \author MidnightCow
//! \copyright
//!
//! Copyright (c)  2018, WIZnet Co., LTD.
//! All rights reserved.
//! 
//! Redistribution and use in source and binary forms, with or without 
//! modification, are permitted provided that the following conditions 
//! are met: 
//! 
//!     * Redistributions of source code must retain the above copyright 
//! notice, this list of conditions and the following disclaimer. 
//!     * Redistributions in binary form must reproduce the above copyright
//! notice, this list of conditions and the following disclaimer in the
//! documentation and/or other materials provided with the distribution. 
//!     * Neither the name of the <ORGANIZATION> nor the names of its 
//! contributors may be used to endorse or promote products derived 
//! from this software without specific prior written permission. 
//! 
//! THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//! AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
//! IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//! ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
//! LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
//! CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
//! SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
//! INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
//! CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
//! ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
//! THE POSSIBILITY OF SUCH DAMAGE.
//
//*****************************************************************************/
//A20140501 : for use the type - ptrdiff_t
#include <stddef.h>
//

#include "wizchip_conf.h"
//#include "w6100.h"
/////////////
//M20150401 : Remove ; in the default callback function such as wizchip_cris_enter(), wizchip_cs_select() and etc.
/////////////

/**
 * @brief Default function to enable interrupt.
 * @note This function help not to access wrong address. If you do not describe this function or register any functions,
 * null function is called.
 */
//void 	  wizchip_cris_enter(void)           {};
void 	  wizchip_cris_enter(void)           {}

/**
 * @brief Default function to disable interrupt.
 * @note This function help not to access wrong address. If you do not describe this function or register any functions,
 * null function is called.
 */
//void 	  wizchip_cris_exit(void)          {};
void 	  wizchip_cris_exit(void)          {}

/**
 * @brief Default function to select chip.
 * @note This function help not to access wrong address. If you do not describe this function or register any functions,
 * null function is called.
 */
//void 	wizchip_cs_select(void)            {};
void 	wizchip_cs_select(void)            {}

/**
 * @brief Default function to deselect chip.
 * @note This function help not to access wrong address. If you do not describe this function or register any functions,
 * null function is called.
 */
//void 	wizchip_cs_deselect(void)          {};
void 	wizchip_cs_deselect(void)          {}

/**
 * @brief Default function to read in direct or indirect interface.
 * @note This function help not to access wrong address. If you do not describe this function or register any functions,
 * null function is called.
 */
 //M20150601 : Rename the function for integrating with W5300
//uint8_t wizchip_bus_readbyte(uint32_t AddrSel) { return * ((volatile uint8_t *)((ptrdiff_t) AddrSel)); }
iodata_t wizchip_bus_readdata(uint32_t AddrSel) { return * ((volatile iodata_t *)((ptrdiff_t) AddrSel)); }

/**
 * @brief Default function to write in direct or indirect interface.
 * @note This function help not to access wrong address. If you do not describe this function or register any functions,
 * null function is called.
 */
//M20150601 : Rename the function for integrating with W5300
//void 	wizchip_bus_writebyte(uint32_t AddrSel, uint8_t wb)  { *((volatile uint8_t*)((ptrdiff_t)AddrSel)) = wb; }
void 	wizchip_bus_writedata(uint32_t AddrSel, iodata_t wb)  { *((volatile iodata_t*)((ptrdiff_t)AddrSel)) = wb; }

/**
 * @brief Default function to read in SPI interface.
 * @note This function help not to access wrong address. If you do not describe this function or register any functions,
 * null function is called.
 */
//uint8_t wizchip_spi_readbyte(void)        {return 0;};
uint8_t wizchip_spi_readbyte(void)        {return 0;}

/**
 * @brief Default function to write in SPI interface.
 * @note This function help not to access wrong address. If you do not describe this function or register any functions,
 * null function is called.
 */
//void 	wizchip_spi_writebyte(uint8_t wb) {};
void 	wizchip_spi_writebyte(uint8_t wb) {}

/**
 * @brief Default function to burst read in SPI interface.
 * @note This function help not to access wrong address. If you do not describe this function or register any functions,
 * null function is called.
 */
//void 	wizchip_spi_readburst(uint8_t* pBuf, uint16_t len) 	{}; 
void 	wizchip_spi_readburst(uint8_t* pBuf, uint16_t len) 	{}

/**
 * @brief Default function to burst write in SPI interface.
 * @note This function help not to access wrong address. If you do not describe this function or register any functions,
 * null function is called.
 */
//void 	wizchip_spi_writeburst(uint8_t* pBuf, uint16_t len) {};
void 	wizchip_spi_writeburst(uint8_t* pBuf, uint16_t len) {}

/**
 * @\ref _WIZCHIP instance
 */      
_WIZCHIP  WIZCHIP =
      {
      _WIZCHIP_IO_MODE_,
      _WIZCHIP_ID_ ,
      wizchip_cris_enter,
      wizchip_cris_exit,
      wizchip_cs_select,
      wizchip_cs_deselect,
      //M20150601 : Rename the function 
      //wizchip_bus_readbyte,
      //wizchip_bus_writebyte
      wizchip_bus_readdata,
      wizchip_bus_writedata,
//    wizchip_spi_readbyte,
//    wizchip_spi_writebyte
      };


static uint8_t    _DNS_[4];      // DNS server ip address
static dhcp_mode  _DHCP_;        // DHCP mode

void reg_wizchip_cris_cbfunc(void(*cris_en)(void), void(*cris_ex)(void))
{
   if(!cris_en || !cris_ex)
   {
      WIZCHIP.CRIS._enter = wizchip_cris_enter;
      WIZCHIP.CRIS._exit  = wizchip_cris_exit;
   }
   else
   {
      WIZCHIP.CRIS._enter = cris_en;
      WIZCHIP.CRIS._exit  = cris_ex;
   }
}

void reg_wizchip_cs_cbfunc(void(*cs_sel)(void), void(*cs_desel)(void))
{
   if(!cs_sel || !cs_desel)
   {
      WIZCHIP.CS._select   = wizchip_cs_select;
      WIZCHIP.CS._deselect = wizchip_cs_deselect;
   }
   else
   {
      WIZCHIP.CS._select   = cs_sel;
      WIZCHIP.CS._deselect = cs_desel;
   }
}

void reg_wizchip_bus_cbfunc(iodata_t(*bus_rb)(uint32_t addr), void (*bus_wb)(uint32_t addr, iodata_t wb))
{
   while(!(WIZCHIP.if_mode & _WIZCHIP_IO_MODE_BUS_));   
   if(!bus_rb || !bus_wb)
   {
      WIZCHIP.IF.BUS._read_data   = wizchip_bus_readdata;
      WIZCHIP.IF.BUS._write_data  = wizchip_bus_writedata;
   }
   else
   {
      WIZCHIP.IF.BUS._read_data   = bus_rb;
      WIZCHIP.IF.BUS._write_data  = bus_wb;
   }
}

void reg_wizchip_spi_cbfunc(uint8_t (*spi_rb)(void), void (*spi_wb)(uint8_t wb))
{
   while(!(WIZCHIP.if_mode & _WIZCHIP_IO_MODE_SPI_));
   
   if(!spi_rb || !spi_wb)
   {
      WIZCHIP.IF.SPI._read_byte   = wizchip_spi_readbyte;
      WIZCHIP.IF.SPI._write_byte  = wizchip_spi_writebyte;
   }
   else
   {
      WIZCHIP.IF.SPI._read_byte   = spi_rb;
      WIZCHIP.IF.SPI._write_byte  = spi_wb;
   }
}

// 20140626 Eric Added for SPI burst operations
void reg_wizchip_spiburst_cbfunc(void (*spi_rb)(uint8_t* pBuf, uint16_t len), void (*spi_wb)(uint8_t* pBuf, uint16_t len))
{
   while(!(WIZCHIP.if_mode & _WIZCHIP_IO_MODE_SPI_));

   if(!spi_rb || !spi_wb)
   {
      WIZCHIP.IF.SPI._read_burst   = wizchip_spi_readburst;
      WIZCHIP.IF.SPI._write_burst  = wizchip_spi_writeburst;
   }
   else
   {
      WIZCHIP.IF.SPI._read_burst   = spi_rb;
      WIZCHIP.IF.SPI._write_burst  = spi_wb;
   }
}

int8_t ctlwizchip(ctlwizchip_type cwtype, void* arg)
{
   uint8_t tmp = 0;

   uint8_t* ptmp[2] = {0,0};
   switch(cwtype)
   {
      case CW_RESET_WIZCHIP:
         wizchip_sw_reset();
         break;
      case CW_INIT_WIZCHIP:
         if(arg != 0) 
         {
            ptmp[0] = (uint8_t*)arg;
            ptmp[1] = ptmp[0] + _WIZCHIP_SOCK_NUM_;
         }
         return wizchip_init(ptmp[0], ptmp[1]);
      case CW_CLR_INTERRUPT:
         wizchip_clrinterrupt(*((intr_kind*)arg));
         break;
      case CW_GET_INTERRUPT:
        *((intr_kind*)arg) = wizchip_getinterrupt();
         break;
      case CW_SET_INTRMASK:
         wizchip_setinterruptmask(*((intr_kind*)arg));
         break;         
      case CW_GET_INTRMASK:
         *((intr_kind*)arg) = wizchip_getinterruptmask();
         break;   
      case CW_SET_INTRTIME:
         setINTPTMR(*(uint16_t*)arg);
         break;
      case CW_GET_INTRTIME:
         *(uint16_t*)arg = getINTPTMR();
         break;   
      case CW_GET_ID:
         ((uint8_t*)arg)[0] = WIZCHIP.id[0];
         ((uint8_t*)arg)[1] = WIZCHIP.id[1];
         ((uint8_t*)arg)[2] = WIZCHIP.id[2];
         ((uint8_t*)arg)[3] = WIZCHIP.id[3];
         ((uint8_t*)arg)[4] = WIZCHIP.id[4];
         ((uint8_t*)arg)[5] = 0;
         break;   
      case CW_RESET_PHY:
         wizphy_reset();
         break;
      case CW_SET_PHYCONF:
         wizphy_setphyconf((wiz_PhyConf*)arg);
         break;
      case CW_GET_PHYCONF:
         wizphy_getphyconf((wiz_PhyConf*)arg);
         break;
      case CW_GET_PHYSTATUS:
         break;
      case CW_SET_PHYPOWMODE:
         return wizphy_setphypmode(*(uint8_t*)arg);   
      case CW_GET_PHYLINK:
         tmp = wizphy_getphylink();
         if((int8_t)tmp == -1) return -1;
         *(uint8_t*)arg = tmp;
         break;
      default:
         return -1;
   }
   return 0;
}


int8_t ctlnetwork(ctlnetwork_type cntype, void* arg)
{
   
   switch(cntype)
   {
      case CN_SET_NETINFO:
         wizchip_setnetinfo((wiz_NetInfo*)arg);
         break;
      case CN_GET_NETINFO:
         wizchip_getnetinfo((wiz_NetInfo*)arg);
         break;
      case CN_SET_NETMODE:
         return wizchip_setnetmode(*(netmode_type*)arg);
      case CN_GET_NETMODE:
         *(netmode_type*)arg = wizchip_getnetmode();
         break;
      case CN_SET_TIMEOUT:
         wizchip_settimeout((wiz_NetTimeout*)arg);
         break;
      case CN_GET_TIMEOUT:
         wizchip_gettimeout((wiz_NetTimeout*)arg);
         break;
      default:
         return -1;
   }
   return 0;
}

void wizchip_sw_reset(void)
{
   uint8_t gw[4], sn[4], sip[4], gw6[16], sn6[16], lla[16], gua[16];
   uint8_t mac[6];

   getSHAR(mac);
   getGAR(gw);  getSUBR(sn);  getSIPR(sip);	getGA6R(gw6); getSUB6R(sn6); getLLAR(lla); getGUAR(gua);
   setSYCR0(SYCR0_RST);
   getSYCR0(); // for delay

   setSHAR(mac);
   setGAR(gw);
   setSUBR(sn);
   setSIPR(sip);
   setGA6R(gw6); 
	 setSUB6R(sn6); 
	 setLLAR(lla); 
	 setGUAR(gua);
}

int8_t wizchip_init(uint8_t* txsize, uint8_t* rxsize)
{
   int8_t i;
   int8_t tmp = 0;
   wizchip_sw_reset();
   if(txsize)
   {
      tmp = 0;
  
			for(i = 0 ; i < _WIZCHIP_SOCK_NUM_; i++)
			{
				tmp += txsize[i];		
				if(tmp > 16) return -1;		
			}
			for(i = 0 ; i < _WIZCHIP_SOCK_NUM_; i++)
				setSn_TXBUF_SIZE(i, txsize[i]);			
   }
   if(rxsize)
   {
      tmp = 0;
   
			for(i = 0 ; i < _WIZCHIP_SOCK_NUM_; i++)
			{
				tmp += rxsize[i];		
				if(tmp > 16) return -1;		
			}
			for(i = 0 ; i < _WIZCHIP_SOCK_NUM_; i++)
				setSn_RXBUF_SIZE(i, txsize[i]);			
   }
   return 0;
}

void wizchip_clrinterrupt(intr_kind intr)
{
   uint8_t ir  = (uint8_t)intr;
   uint8_t sir = (uint8_t)((uint16_t)intr >> 8);
	 int i;
	
   setIRCLR(ir);
   for(i=0;i<8;i++)
   	if(sir&(1<<i))   	setSn_IRCLR(i,0x0F);
}

intr_kind wizchip_getinterrupt(void)
{
   uint8_t ir  = 0;
   uint8_t sir = 0;
   uint16_t ret = 0;

   ir  = getIR();
   sir = getSIR();

   ret = sir;
   ret = (ret << 8) + ir;
	
   return (intr_kind)ret;
}

void wizchip_setinterruptmask(intr_kind intr)
{
   uint8_t imr  = (uint8_t)intr;
   uint8_t simr = (uint8_t)((uint16_t)intr >> 8);
   
   setIMR(imr);
   setSIMR(simr);
}

intr_kind wizchip_getinterruptmask(void)
{
   uint8_t imr  = 0;
   uint8_t simr = 0;
   uint16_t ret = 0;

   imr  = getIMR();
   simr = getSIMR();

   ret = simr;
   ret = (ret << 8) + imr;
	
   return (intr_kind)ret;
}

int8_t wizphy_getphylink(void)
{
   int8_t tmp = PHY_LINK_OFF;

   if(getPHYSR() & PHYSR_LNK)
      tmp = PHY_LINK_ON;

   return tmp;
}

#if _WIZCHIP_ == W6100
void wizphy_reset(void)
{
	uint16_t tmp = wiz_mdio_read(PHYMDIO_BMCR);
	tmp |= BMCR_RESET;
	wiz_mdio_write(PHYMDIO_BMCR, tmp);
	while(wiz_mdio_read(PHYMDIO_BMCR)&BMCR_RESET){}
}

void wizphy_setphyconf(wiz_PhyConf* phyconf)
{
   uint16_t tmp = wiz_mdio_read(PHYMDIO_BMCR);
   if(phyconf->mode == PHY_MODE_AUTONEGO)
      tmp |= BMCR_AUTONEGO;
   else
   {
	  tmp &= ~BMCR_AUTONEGO;
      if(phyconf->duplex == PHY_DUPLEX_FULL)
      {
    	  tmp |= BMCR_DUP;
      }
      else
      {
    	  tmp &= ~BMCR_DUP;
      }
      if(phyconf->speed == PHY_SPEED_100)
      {
    	  tmp |= BMCR_SPEED;
      }
      else
      {
    	  tmp &= ~BMCR_SPEED;
      }
   }
   wiz_mdio_write(PHYMDIO_BMCR, tmp);
}

void wizphy_getphyconf(wiz_PhyConf* phyconf)
{
   uint16_t tmp = 0;
   tmp = wiz_mdio_read(PHYMDIO_BMCR);
   phyconf->by   = PHY_CONFBY_SW;
   if(tmp & BMCR_AUTONEGO)
   {
	   phyconf->mode = PHY_MODE_AUTONEGO;
   }
   else
   {
	   phyconf->mode = PHY_MODE_MANUAL;
	   if(tmp&BMCR_DUP) phyconf->duplex = PHY_DUPLEX_FULL;
	   else phyconf->duplex = PHY_DUPLEX_HALF;
	   if(tmp&BMCR_SPEED) phyconf->speed = PHY_SPEED_100;
	   else phyconf->speed = PHY_SPEED_10;
   }
}

int8_t wizphy_setphypmode(uint8_t pmode)
{
   uint16_t tmp = 0;
   tmp = wiz_mdio_read(PHYMDIO_BMCR);
   if( pmode == PHY_POWER_DOWN)
   {
      tmp |= BMCR_PWDN;
   }
   else
   {
	   tmp &= ~BMCR_PWDN;
   }
   wiz_mdio_write(PHYMDIO_BMCR, tmp);
   tmp = wiz_mdio_read(PHYMDIO_BMCR);
   if( pmode == PHY_POWER_DOWN)
   {
      if(tmp & BMCR_PWDN) return 0;
   }
   else
   {
      if((tmp & BMCR_PWDN) != BMCR_PWDN) return 0;
   }
   return -1;
}
#endif




void wizchip_setnetinfo(wiz_NetInfo* pnetinfo)
{
//	int i,j,k;
   setSHAR(pnetinfo->mac);

   setGAR(pnetinfo->gw);
   setSUBR(pnetinfo->sn);
   setSIPR(pnetinfo->ip);   
   setGA6R(pnetinfo->gw6);
   setSUB6R(pnetinfo->sn6);
   setLLAR(pnetinfo->lla);
   setGUAR(pnetinfo->gua);

   _DNS_[0] = pnetinfo->dns[0];
   _DNS_[1] = pnetinfo->dns[1];
   _DNS_[2] = pnetinfo->dns[2];
   _DNS_[3] = pnetinfo->dns[3];
   _DHCP_   = pnetinfo->dhcp;
}

void wizchip_getnetinfo(wiz_NetInfo* pnetinfo)
{
   getSHAR(pnetinfo->mac);

   getGAR(pnetinfo->gw);
   getSUBR(pnetinfo->sn);
   getSIPR(pnetinfo->ip);
   
   getGA6R(pnetinfo->gw6);
   getSUB6R(pnetinfo->sn6);
   getLLAR(pnetinfo->lla);
   getGUAR(pnetinfo->gua);

   pnetinfo->dns[0]= _DNS_[0];
   pnetinfo->dns[1]= _DNS_[1];
   pnetinfo->dns[2]= _DNS_[2];
   pnetinfo->dns[3]= _DNS_[3];
   pnetinfo->dhcp  = _DHCP_;
}

int8_t wizchip_setnetmode(netmode_type netmode)
{
   uint8_t tmp = 0;

   if(netmode & ~(NM_WAKEONLAN)) return -1;

   tmp = getNETMR();
   tmp |= (uint8_t)netmode;
   setNETMR(tmp);

   return 0;
}

netmode_type wizchip_getnetmode(void)
{
   return (netmode_type) getNETMR();
}

void wizchip_settimeout(wiz_NetTimeout* nettime)
{
   setRCR(nettime->retry_cnt);
   setRTR(nettime->time_100us);
}

void wizchip_gettimeout(wiz_NetTimeout* nettime)
{
   nettime->retry_cnt = getRCR();
   nettime->time_100us = getRTR();
}

