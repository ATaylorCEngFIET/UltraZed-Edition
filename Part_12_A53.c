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
#include "xgpiops.h"
#include "xscugic.h"
#include "xil_exception.h"
#include "xplatform_info.h"
#include "unistd.h"
#include <xil_printf.h>
#include <stdio.h>

#define GPIO_DEVICE_ID		XPAR_XGPIOPS_0_DEVICE_ID
#define INTC_DEVICE_ID		XPAR_SCUGIC_SINGLE_DEVICE_ID
#define GPIO_INTERRUPT_ID	XPAR_XGPIOPS_0_INTR


#define INPUT_BANK	XGPIOPS_BANK1  /* Bank 0 of the GPIO Device */
#define OUTPUT_BANK	XGPIOPS_BANK1  /* Bank 1 of the GPIO Device */

static int GpioIntrExample(XScuGic *Intc, XGpioPs *Gpio, u16 DeviceId,
			   u16 GpioIntrId);
static void IntrHandler(void *CallBackRef, u32 Bank, u32 Status);
static int SetupInterruptSystem(XScuGic *Intc, XGpioPs *Gpio, u16 GpioIntrId);


static XGpioPs Gpio; /* The Instance of the GPIO Driver */
static XScuGic Intc; /* The Instance of the Interrupt Controller Driver */

static u32 AllButtonsPressed; /* Intr status of the bank */
static u32 Input_Pin; /* Switch button */
static u32 Output_Pin; /* LED button */
static u32 toggle = 1;

int main(void)
{
	int Status;

	printf("UltraZed Edition GPIO Interrupt Example \r\n");
	printf("http://www.ultrazedchronicles.com \r\n");

	Status = GpioIntrExample(&Intc, &Gpio, GPIO_DEVICE_ID,
				 GPIO_INTERRUPT_ID);

	if (Status != XST_SUCCESS) {
		printf("GPIO Interrupt Example Test Failed\r\n");
		return XST_FAILURE;
	}

	printf("Successfully ran GPIO Interrupt Example Test\r\n");
	return XST_SUCCESS;
}

int GpioIntrExample(XScuGic *Intc, XGpioPs *Gpio, u16 DeviceId, u16 GpioIntrId)
{
	XGpioPs_Config *ConfigPtr;
	int Status;
	Input_Pin = 26;
	Output_Pin = 31;


	/* Initialize the Gpio driver. */
	ConfigPtr = XGpioPs_LookupConfig(DeviceId);
	if (ConfigPtr == NULL) {
		return XST_FAILURE;
	}
	XGpioPs_CfgInitialize(Gpio, ConfigPtr, ConfigPtr->BaseAddr);

	/* Run a self-test on the GPIO device. */
	Status = XGpioPs_SelfTest(Gpio);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/* Set the direction for the specified pin to be input */
	XGpioPs_SetDirectionPin(Gpio, Input_Pin, 0x0);

	/* Set the direction for the specified pin to be output. */
	XGpioPs_SetDirectionPin(Gpio, Output_Pin, 1);
	XGpioPs_SetOutputEnablePin(Gpio, Output_Pin, 1);
	XGpioPs_WritePin(Gpio, Output_Pin, 0x0);

	Status = SetupInterruptSystem(Intc, Gpio, GPIO_INTERRUPT_ID);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	printf("\n\rPush Switch to Toggle PS LED\n\r");
	AllButtonsPressed = FALSE;

	/*
	 * Loop forever while the button changes are handled by the interrupt
	 * level processing.
	 */
	while(1);

	return XST_SUCCESS;
}

static void IntrHandler(void *CallBackRef, u32 Bank, u32 Status)
{
	XGpioPs *Gpio = (XGpioPs *)CallBackRef;
	u32 DataRead;

	/* Push the switch button */
	DataRead = XGpioPs_ReadPin(Gpio, Input_Pin);

	if (DataRead != 0) {
		XGpioPs_SetDirectionPin(Gpio, Output_Pin, 1);
		XGpioPs_SetOutputEnablePin(Gpio, Output_Pin, 1);
		XGpioPs_WritePin(Gpio, Output_Pin, toggle);
		toggle = ~toggle;
		//printf("%u",toggle);
		printf("\n\r Button Pressed ");
		if (toggle == 1 ){
			printf("LED OFF \n\r");
		}
		else{
			printf("LED ON \n\r");
		}
		usleep(1000000);
		//AllButtonsPressed = TRUE;
	}
}

static int SetupInterruptSystem(XScuGic *GicInstancePtr, XGpioPs *Gpio,
				u16 GpioIntrId)
{
	int Status;

	XScuGic_Config *IntcConfig; /* Instance of the interrupt controller */

	Xil_ExceptionInit();

	/*
	 * Initialize the interrupt controller driver so that it is ready to
	 * use.
	 */
	IntcConfig = XScuGic_LookupConfig(INTC_DEVICE_ID);
	if (NULL == IntcConfig) {
		return XST_FAILURE;
	}

	Status = XScuGic_CfgInitialize(GicInstancePtr, IntcConfig,
					IntcConfig->CpuBaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
				(Xil_ExceptionHandler)XScuGic_InterruptHandler,
				GicInstancePtr);

	Status = XScuGic_Connect(GicInstancePtr, GpioIntrId,
				(Xil_ExceptionHandler)XGpioPs_IntrHandler,
				(void *)Gpio);
	if (Status != XST_SUCCESS) {
		return Status;
	}

	XGpioPs_SetIntrType(Gpio, INPUT_BANK, 0x00, 0x00, 0x00);

	XGpioPs_SetCallbackHandler(Gpio, (void *)Gpio, IntrHandler);

	XGpioPs_IntrEnablePin(Gpio, Input_Pin );

	XScuGic_Enable(GicInstancePtr, GpioIntrId);

	Xil_ExceptionEnableMask(XIL_EXCEPTION_IRQ);

	return XST_SUCCESS;
}
