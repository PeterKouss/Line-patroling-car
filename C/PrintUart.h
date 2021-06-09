/*
	//////////////////////////////////////////////////////////////////////////////////
	
	Copyright PeterYang 2017K8009926001
	All Rights reserved.
	
	//////////////////////////////////////////////////////////////////////////////////
	
	Reference data:
		Manual Chapter 39, Chapter 12
		Example source code of MKL25Z
	
	//////////////////////////////////////////////////////////////////////////////////
		
	Check your uart device settings before use.
	
	//////////////////////////////////////////////////////////////////////////////////
*/

#ifndef __PrintUART_H__
#define __PrintUART_H__

#include <stdio.h>
#include <stdbool.h>
#include <MKL25Z4.h>


#define BSP_CLK_CORECLK_HZ            (48000000U) //record CLK
#define BSP_UART_DEBUG_BAUDRATE    (115200U)	// record baudrate


typedef struct
{
    uint32_t BusClkHz;
    uint32_t Baudrate;
} UART_Config_T;


const UART_Config_T mDbgUartConfigStruct =
{
    BSP_CLK_CORECLK_HZ,      /* BusClkHz. */
    BSP_UART_DEBUG_BAUDRATE /* Baudrate. */
};

bool UART0_ConfigTransfer(const UART_Config_T *configPtr)
{
    uint16_t sbr_val;

    /* Disable rx and tx. */
    UART0->C2 &= ~(UART0_C2_TE_MASK | UART0_C2_RE_MASK);
    
    /* Configure uart1 for 8-bit mode */
    UART0->C1 = 0U;

    /* Calculate the sbr value. */
    sbr_val = (configPtr->BusClkHz >> 4)/configPtr->Baudrate;
    UART0->BDH = (uint8_t)(((0x1F00 & sbr_val) >> 8)&UART0_BDH_SBR_MASK);
    UART0->BDL = (uint8_t)(sbr_val & UART0_BDL_SBR_MASK);
    
		/* Initial registers */
    UART0->C3 = 0U;
    UART0->S1 = 0x1FU;
    UART0->S2 = 0U;
    
    /* Enable tx and rx  */
    UART0->C2 |= (UART0_C2_TE_MASK | UART0_C2_RE_MASK);
    
    return true;
}

bool init_board(void)
{
    /* Clock set. */
		SIM->SOPT2 |= SIM_SOPT2_PLLFLLSEL_MASK;
		uint32_t tmp = SIM->SOPT2 & (~SIM_SOPT2_UART0SRC_MASK);
    tmp |= SIM_SOPT2_UART0SRC((uint32_t)1U);
    SIM->SOPT2 = tmp;
    
		/*Enable clock gate. */
		SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
    //SIM_EnableClockForUART(BSP_UART_DEBUG_INSTANCE, true);
		SIM->SCGC4 |= SIM_SCGC4_UART0_MASK;
    
    /* Set rx and tx pins*/
	  PORTA->PCR[1] = PORT_PCR_MUX(2); /* UART0_RX. */
    PORTA->PCR[2] = PORT_PCR_MUX(2); /* UART0_TX. */
    
    /* Init UART0 */
    UART0_ConfigTransfer(&mDbgUartConfigStruct);
		

    return true;
}

//put char
void UART0_PutTxData(uint8_t txData)
{
    UART0->D = txData;
}

//buff empty
bool UART0_IsTxBufferEmpty(void)
{
    return ( 0U != (UART0->S1 & UART0_S1_TDRE_MASK) );
}

//verify empty
void UART0_PutTxDataBlocking(uint8_t txData)
{
    while (!UART0_IsTxBufferEmpty()) {}
    UART0_PutTxData(txData);
}

//get char
uint8_t UART0_GetRxData(void)
{
    return (uint8_t)(UART0->D);
}

//buff full
bool UART0_IsRxBufferFull(void)
{
    return (0U != (UART0->S1 & UART0_S1_RDRF_MASK) );
}

//verify full
uint8_t UART0_GetRxDataBlocking(void)
{
    while (!UART0_IsRxBufferFull() ) {}
    return UART0_GetRxData();
}

//change the fputc of printf
int fputc(int c, FILE *f)
{
    UART0_PutTxDataBlocking((uint8_t)c);
    return c;
}

// change the fgetc of getchar, not necessary.
int fgetc(FILE *f)
{
    return (UART0_GetRxDataBlocking());
}


#endif