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


#include "xparameters.h"
#include "xstatus.h"
#include "stdio.h"
#include "xrtcpsu.h"
#include <unistd.h>

#define RTC_DEVICE_ID              XPAR_XRTCPSU_0_DEVICE_ID

XRtcPsu Rtc_Psu;
XRtcPsu_Config *Config;


int main(void)
{

	printf("UltraZed Edition RTC A53 Example\r\n");
	printf("www.ultrazedchronicles.com\r\n");

	int Status;
	u32 CurrentTime, DesiredTime,LastSetTime;
	u32 OscillatorFreq;
	XRtcPsu_DT dt1,dt2,dt3;

	Config = XRtcPsu_LookupConfig(RTC_DEVICE_ID);
	if (NULL == Config) {
		printf("Look up failed\r\n");
		return XST_FAILURE;
	}

	Status = XRtcPsu_CfgInitialize(&Rtc_Psu, Config, Config->BaseAddr);
	if (Status != XST_SUCCESS) {
		printf("Initialise failed\r\n");
		return XST_FAILURE;
	}

	/* Check hardware build. */
	Status = XRtcPsu_SelfTest(&Rtc_Psu);
	if (Status != XST_SUCCESS) {
		printf("Self Test failed\r\n");
		return XST_FAILURE;
	}

	//printf("Day Convention : 0-Fri, 1-Sat, 2-Sun, 3-Mon, 4-Tue, 5-Wed, 6-Thur\n\r");
	printf("Last RTC set time is \n\r");
	LastSetTime = XRtcPsu_GetLastSetTime(&Rtc_Psu);
	XRtcPsu_SecToDateTime(LastSetTime,&dt1);
	printf("YEAR:MM:DD HR:MM:SS \t %04d:%02d:%02d %02d:%02d:%02d\t Day = %d\n\r",
			dt1.Year,dt1.Month,dt1.Day,dt1.Hour,dt1.Min,dt1.Sec,dt1.WeekDay);

	printf("Current RTC time is..\n\r");
	CurrentTime = XRtcPsu_GetCurrentTime(&Rtc_Psu);
	XRtcPsu_SecToDateTime(CurrentTime,&dt2);
	printf("YEAR:MM:DD HR:MM:SS \t %04d:%02d:%02d %02d:%02d:%02d\t Day = %d\n\r",
			dt2.Year,dt2.Month,dt2.Day,dt2.Hour,dt2.Min,dt2.Sec,dt2.WeekDay);

	printf("Enter Desired Current Time YEAR MM DD HR MM SS : ");
	scanf("%d %d %d %d %d %d",&dt3.Year,&dt3.Month,&dt3.Day,&dt3.Hour,&dt3.Min,&dt3.Sec);
	//printf("%d %d %d %d %d %d\n\r",dt3.Year,dt3.Month,dt3.Day,dt3.Hour,dt3.Min,dt3.Sec);

	DesiredTime = XRtcPsu_DateTimeToSec(&dt3);
	XRtcPsu_SetTime(&Rtc_Psu,DesiredTime);

	printf("Enter Crystal oscillator frequency : ");
	scanf("%d",&OscillatorFreq);
	printf("\n\rOld Calibration value : %08x\tCrystal Frequency : %08x\n\r",
			Rtc_Psu.CalibrationValue,Rtc_Psu.OscillatorFreq);

	XRtcPsu_CalculateCalibration(&Rtc_Psu,DesiredTime,OscillatorFreq);

	Status = XRtcPsu_CfgInitialize(&Rtc_Psu, Config, Config->BaseAddr);
	if (Status != XST_SUCCESS) {
		printf("Initialise failed following Calibration\r\n");
		return XST_FAILURE;
	}

	printf("New Calibration value : %08x\tCrystal Frequency : %08x\n\r",
			Rtc_Psu.CalibrationValue,Rtc_Psu.OscillatorFreq);

	printf("Current RTC time is..\n\r");
	CurrentTime = XRtcPsu_GetCurrentTime(&Rtc_Psu);
	XRtcPsu_SecToDateTime(CurrentTime,&dt2);
	printf("YEAR:MM:DD HR:MM:SS \t %04d:%02d:%02d %02d:%02d:%02d\t Day = %d\n\r",
			dt2.Year,dt2.Month,dt2.Day,dt2.Hour,dt2.Min,dt2.Sec,dt2.WeekDay);

	while(1){
		usleep(1000000);
		CurrentTime = XRtcPsu_GetCurrentTime(&Rtc_Psu);
		XRtcPsu_SecToDateTime(CurrentTime,&dt2);
		printf("YEAR:MM:DD HR:MM:SS \t %04d:%02d:%02d %02d:%02d:%02d\t Day = %d\n\r",
					dt2.Year,dt2.Month,dt2.Day,dt2.Hour,dt2.Min,dt2.Sec,dt2.WeekDay);

	}


}


