/* ###################################################################
**     Filename    : main.c
**     Project     : roboty_mobilne
**     Processor   : MK60FX512VLQ15
**     Version     : Driver 01.01
**     Compiler    : GNU C Compiler
**     Date/Time   : 2016-05-24, 12:32, # CodeGen: 0
**     Abstract    :
**         Main module.
**         This module contains user's application code.
**     Settings    :
**     Contents    :
**         No public methods
**
** ###################################################################*/
/*!
** @file main.c
** @version 01.01
** @brief
**         Main module.
**         This module contains user's application code.
*/         
/*!
**  @addtogroup main_module main module documentation
**  @{
*/         
/* MODULE main */


/* Including needed modules to compile this module/procedure */
#include "Cpu.h"
#include "Events.h"
#include "NC1.h"
#include "AdcLdd1.h"
#include "SIL1_L.h"
#include "BitIoLdd1.h"
#include "SIL1_P.h"
#include "BitIoLdd2.h"
#include "SIL1.h"
#include "PwmLdd1.h"
#include "TU1.h"
/* Including shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"

/* User includes (#include below this line is not maintained by Processor Expert) */
word pom_u = 0;
word pom_d = 0;
int16 pom1 = 0;
int16 zadana1 = 0;
int16 ster1 = 0;

/*lint -save  -e970 Disable MISRA rule (6.3) checking. */
int main(void)
/*lint -restore Enable MISRA rule (6.3) checking. */
{
  

  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  PE_low_level_init();
  /*** End of Processor Expert internal initialization.                    ***/

  // zmienne do regulatora PID
  signed long uchyb=0;
  signed long P=0, I=0, D=0;
  static signed long uchyb_old=0, uchyb_sum=0;
  static signed long Kp=0, Kd=0, Ki=0; // <- do regulacji
  static float offset=0; // <- do regulacji
  
  // zmienne do histerezy
  signed long h = 0; // <- do regulacji

  for(;;){
	  // pobranie odczytu z czujnika gornego
	  (void)NC1_MeasureChan(TRUE, 0);
	  (void)NC1_Measure(TRUE);
	  (void)NC1_GetValue16(&pom_u);
	  // pobranie odczytu z czujnika dolnego
	  (void)NC1_MeasureChan(TRUE, 1);
	  (void)NC1_Measure(TRUE);
	  (void)NC1_GetValue16(&pom_d);
	  
	  // regulator PID
	  #define granica_sterowania (100) // 100% - maksymalne sterowanie
	  #define granica_calkowania (granica_sterowania*10000/Ki)
	  pom1 = (int16)pom_u - (int16)pom_d;
	  uchyb = (zadana1-pom1);
	  
	  // petla histerezy
	  if(uchyb >= -h/2 && uchyb <= h/2){
		  uchyb = 0;
		  SIL1_L_PutVal(FALSE);
		  SIL1_P_PutVal(FALSE);
	  }
	  
	  // regulator PID
	  P = uchyb;
	  uchyb_sum += uchyb;
	  if (uchyb_sum > granica_calkowania) uchyb = granica_calkowania;
	  if (uchyb_sum < -granica_calkowania) uchyb = -granica_calkowania;
	  I = uchyb_sum;
	  D = uchyb - uchyb_old;
	  uchyb_old = uchyb;
	  ster1 = ((float)(Kp*P + Kd*D + Ki*I))/10000;
	  // offset po to by wyeliminowac tarcie napedu
	  if (ster1 > 0) ster1+=offset; else ster1-=offset;
	  // ograniczenie sterowania
	  if (ster1 > granica_sterowania) ster1 = granica_sterowania;
	  if (ster1 < -granica_sterowania) ster1 = -granica_sterowania;
	  
	  // sterowanie mostkiem
	  if (ster1 < 0){
		  SIL1_L_PutVal(TRUE);
		  SIL1_P_PutVal(FALSE);
	  }
	  if (ster1 > 0){
		  SIL1_L_PutVal(FALSE);
		  SIL1_P_PutVal(TRUE);
	  }
	  SIL1_SetRatio16(ster1);
  }

  /*** Don't write any code pass this line, or it will be deleted during code generation. ***/
  /*** RTOS startup code. Macro PEX_RTOS_START is defined by the RTOS component. DON'T MODIFY THIS CODE!!! ***/
  #ifdef PEX_RTOS_START
    PEX_RTOS_START();                  /* Startup of the selected RTOS. Macro is defined by the RTOS component. */
  #endif
  /*** End of RTOS startup code.  ***/
  /*** Processor Expert end of main routine. DON'T MODIFY THIS CODE!!! ***/
  for(;;){}
  /*** Processor Expert end of main routine. DON'T WRITE CODE BELOW!!! ***/
} /*** End of main routine. DO NOT MODIFY THIS TEXT!!! ***/

/* END main */
/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.3 [05.09]
**     for the Freescale Kinetis series of microcontrollers.
**
** ###################################################################
*/
