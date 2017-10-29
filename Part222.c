/*Copyright (c) 2017, Adam Taylor
All rights reserved.
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the FreeBSD Project*/

#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "xil_io.h"
#include "xwdtps.h"

#define WDT_DEVICE_ID  		XPAR_XWDTPS_1_DEVICE_ID
#define EN_FPD_SWDT		   (0x1 << 13)
#define RST_count		    0xFFD80050
XWdtPs Watchdog;
XWdtPs_Config *ConfigPtr;

int main()
{
	u32 count,Reg;
    init_platform();

    Xil_Out32(0xFFD80570,EN_FPD_SWDT);
    Xil_Out32(0xFFD805A0,0x00000000);

    Reg = Xil_In32(0xFFD80530);
    printf("Error Status Reg %x\n\r",Reg);
    Reg |= EN_FPD_SWDT;
    Xil_Out32(0xFFD80530,EN_FPD_SWDT);
    Reg = Xil_In32(0xFFD80530);
    printf("Error Status Reg %x\n\r",Reg);
    Reg = Xil_In32(0xFFD80534);
    printf("Error mask Reg %x\n\r",Reg);


	//Xil_Out32(0xFFD80530,Reg);
	//Xil_Out32(0xFFD80570,EN_FPD_SWDT);

    ConfigPtr = XWdtPs_LookupConfig(WDT_DEVICE_ID);
    XWdtPs_CfgInitialize(&Watchdog, ConfigPtr,ConfigPtr->BaseAddress);
    XWdtPs_Stop(&Watchdog);
	XWdtPs_SetControlValue(&Watchdog,(u8) XWDTPS_COUNTER_RESET, (u8) 256);
	count = XWdtPs_GetControlValue(&Watchdog,(u8) XWDTPS_COUNTER_RESET);
	XWdtPs_SetControlValue(&Watchdog,(u8) XWDTPS_CLK_PRESCALE,(u8) XWDTPS_CCR_PSCALE_4096);
	XWdtPs_EnableOutput(&Watchdog, XWDTPS_RESET_SIGNAL);
    printf("UltraZed Edition Part 20\n\r");
    usleep(100);
	Reg= Xil_In32(0xFF5E0220);
	printf("Reset Reason %x\n\r",Reg);
	Reg= Xil_In32(RST_count);
	printf("Reset Number %x\n\r",Reg);
	usleep(10000);
	Reg++;
	Xil_Out32(RST_count,Reg);

	Xil_Out32(0xFFD8056C,EN_FPD_SWDT);
	Reg = Xil_In32(0xFFD805A0);
	Reg |= EN_FPD_SWDT;
	Xil_Out32(0xFFD805A0,Reg);

	usleep(10000000); // delay
	XWdtPs_Start(&Watchdog);
	XWdtPs_RestartWdt(&Watchdog);
	usleep(10000);

while(1){
	Reg= Xil_In32(RST_count);
	if (Reg == 6){ //By the time we get here we will have performed 6 resets 0 to 5,
		//reg is updated before reset occurs
		XWdtPs_RestartWdt(&Watchdog);
		printf("Resetting WatchDog to Run properly");
		usleep(100000);
	}

}
    cleanup_platform();
    return 0;
}
