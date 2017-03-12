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

#include "xsysmonpsu.h"
#include "xsysmonpsu_hw.h"
#include "xparameters.h"
#include "xstatus.h"
#include "stdio.h"

/************************** Constant Definitions ****************************/

/*
 * The following constants map to the XPAR parameters created in the
 * xparameters.h file. They are defined here such that a user can easily
 * change all the needed parameters in one place.
 */
#define SYSMON_BASEADDR		XPAR_XSYSMONPSU_0_BASEADDR

/**************************** Type Definitions ******************************/

/***************** Macros (Inline Functions) Definitions ********************/
#define XSysMonPsu_RawToVoltagePS(AdcData) 					\
	((((float)(AdcData))* (6.0f))/65536.0f)
/************************** Function Prototypes *****************************/

int SysMonPsuLowLevelExample(u32 BaseAddress);

int main(void)
{

	printf("UltraZed Edition Sysmon PS Example using R5\r\n");
	SysMonPsuLowLevelExample(SYSMON_BASEADDR);


	return XST_SUCCESS;
}


int SysMonPsuLowLevelExample(u32 BaseAddress)
{
	u32 TempData;
	u32 VccauxData;
	u32 int_lp;
	u32 int_fp;
	u32 ddr;
	u32 io3;
	u32 io0;
	u32 io1;
	u32 io2;
	float TempData_Real;
	float VccauxData_Real;
	float int_lp_Real;
	float int_fp_Real;
	float ddr_Real;
	float io3_Real;
	float io0_Real;
	float io1_Real;
	float io2_Real;

	XSysMonPsu_Config *config;
	XSysMonPsu *InstancePtr;

	config = XSysMonPsu_LookupConfig(XPAR_PSU_AMS_DEVICE_ID);

	XSysMonPsu_CfgInitialize(InstancePtr, config, config->BaseAddress);

	XSysMonPsu_Reset(InstancePtr);

	XSysMonPsu_SetSequencerMode(InstancePtr, XSM_SEQ_MODE_SAFE,XSYSMON_PS);

	XSysMonPsu_SetAlarmEnables(InstancePtr, XSYSMONPSU_CFG_REG1_ALRM_ALL_MASK, XSYSMON_PS);

	XSysMonPsu_SetSeqChEnables(InstancePtr,
			XSYSMONPSU_SEQ_CH0_TEMP_MASK |
			XSYSMONPSU_SEQ_CH0_SUP1_MASK |
			XSYSMONPSU_SEQ_CH0_SUP2_MASK |
			XSYSMONPSU_SEQ_CH0_SUP3_MASK |
			XSYSMONPSU_SEQ_CH0_SUP4_MASK |
			XSYSMONPSU_SEQ_CH0_SUP5_MASK |
			XSYSMONPSU_SEQ_CH0_SUP6_MASK |
			XSYSMONPSU_SEQ_CH2_SUP7_MASK |
			XSYSMONPSU_SEQ_CH2_SUP8_MASK ,
			XSYSMON_PS);

	XSysMonPsu_SetAdcClkDivisor(InstancePtr,32,XSYSMON_PS);

	XSysMonPsu_SetSequencerMode(InstancePtr,XSM_SEQ_MODE_CONTINPASS,XSYSMON_PS);


	//while (1) { //to read continually include this line

		TempData = XSysMonPsu_GetAdcData(InstancePtr,XSM_CH_TEMP, XSYSMON_PS);
		printf("Temp Data %x\r\n",TempData);
		TempData_Real = XSysMonPsu_RawToTemperature_OnChip(TempData);
		printf("Temp  %f\r\n",TempData_Real);

		VccauxData = XSysMonPsu_GetAdcData(InstancePtr,XSM_CH_SUPPLY3, XSYSMON_PS);
		printf("VCCAUX %x\r\n",VccauxData);
		VccauxData_Real = XSysMonPsu_RawToVoltage(VccauxData);
		printf("VCCAUX  %f\r\n",VccauxData_Real);

		int_lp = XSysMonPsu_GetAdcData(InstancePtr,XSM_CH_SUPPLY1, XSYSMON_PS);
		printf("VCC Int LP %x\r\n",int_lp);
		int_lp_Real = XSysMonPsu_RawToVoltage(int_lp);
		printf("VCC Int LP  %f\r\n",int_lp_Real);

		int_fp = XSysMonPsu_GetAdcData(InstancePtr,XSM_CH_SUPPLY2, XSYSMON_PS);
		printf("VCC Int FP %x\r\n",int_fp);
		int_fp_Real = XSysMonPsu_RawToVoltage(int_fp);
		printf("VCC Int FP  %f\r\n",int_fp_Real);

		ddr = XSysMonPsu_GetAdcData(InstancePtr,XSM_CH_SUPPLY4, XSYSMON_PS);
		printf("VCC DDR %x\r\n",ddr);
		ddr_Real = XSysMonPsu_RawToVoltage(ddr);
		printf("VCC DDR %f\r\n",ddr_Real);

		io3 = XSysMonPsu_GetAdcData(InstancePtr,XSM_CH_SUPPLY5, XSYSMON_PS);
		printf("VCC IO 3 %x\r\n",io3);
		io3_Real = XSysMonPsu_RawToVoltagePS(io3);
		printf("VCC IO 3 %f\r\n",io3_Real);

		io0 = XSysMonPsu_GetAdcData(InstancePtr,XSM_CH_SUPPLY6, XSYSMON_PS);
		printf("VCC IO 0 %x\r\n",io0);
		io0_Real = XSysMonPsu_RawToVoltagePS(io0);
		printf("VCC IO 0 %f\r\n",io0_Real);

		io1 = XSysMonPsu_GetAdcData(InstancePtr,XSM_CH_SUPPLY7, XSYSMON_PS);
		printf("VCC IO 1 %x\r\n",io1);
		io1_Real = XSysMonPsu_RawToVoltagePS(io1);
		printf("VCC IO 1 %f\r\n",io1_Real);

		io2 = XSysMonPsu_GetAdcData(InstancePtr,XSM_CH_SUPPLY8, XSYSMON_PS);
		printf("VCC IO 2 %x\r\n",io2);
		io2_Real = XSysMonPsu_RawToVoltagePS(io2);
		printf("VCC IO 2 %f\r\n",io2_Real);


	//} //include for continous loop
	return XST_SUCCESS;
}
