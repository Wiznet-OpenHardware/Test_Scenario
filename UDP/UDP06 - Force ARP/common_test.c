#include "common_test.h"


uint32_t recv_wol_test(){
	printf("wake on lan interrupt test!");
	setIMR2(0x01);
	setMR2(MR2_WOL|getMR2());
	printf("wait\r\n");
	while(getIR2() ==0);
	printf("\r\nreceive the magic packet!\r\n");
	printf("IR2 : %.2x\r\n", getIR2());

	setIR2(0x01);

	return 2;
}

uint32_t mdc_mdio_test(){
	PHYUNLOCK();

	printf("Address 0 : %.4x (is expected 0x3100)\r\n", wiz_mdio_read(0x00)); //
	printf("Address 4 : %.4x (is expected 0x01E1)\r\n", wiz_mdio_read(0x04)); //

	printf("------------after write----------");
	wiz_mdio_write(0x0, 0xEFFF);
	wiz_mdio_write(0x4, 0xFFFF);

	printf("Address 0 : %.4x (is expected 0x3000)\r\n", wiz_mdio_read(0x00));
	printf("Address 4 : %.4x (is expected 0x01E1)\r\n", wiz_mdio_read(0x04));


	printf("------------after reset-----------------");
	wiz_mdio_write(0x0,0x8000 | wiz_mdio_read(0x0)); //reset high


	printf("Address 0 : %.4x (is expected 0x3000)\r\n", wiz_mdio_read(0x00));
	printf("Address 4 : %.4x (is expected 0x01E1)\r\n", wiz_mdio_read(0x04));
	return 2;
}

uint32_t ping_block_test(){
	setMR(getMR() | 0x10);
	return 2;
}


uint32_t ip_conflict_test(){
	setIMR(getIMR() | 0x80);
	return 2;
}
