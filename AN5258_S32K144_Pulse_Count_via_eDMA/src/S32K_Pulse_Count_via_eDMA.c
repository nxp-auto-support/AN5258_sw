/*
 * main implementation: use this 'C' sample to create your own application
 *
 */


#include "derivative.h" /* include peripheral declarations S32K144 */


/////////////////////////////
////  GLOBAl VARIABLES    ///
/////////////////////////////

uint32_t dummy1;		// Dummy variable for eDMA0
uint32_t dummy2; 		// Dummy variable for eDMA1
uint32_t dummy3;		// Dummy variable for eDMA2

uint32_t mod1 = 2800;	// FTM0 PWM frequency = 20kHz
uint32_t mod2 = 5600;	// FTM1 PWM frequency = 10kHz
uint32_t mod3 = 56000;	// FTM2 PWM frequency = 1kHz

uint32_t duty1 = 1400; 	// FTM0 PWM duty cycle = 50%
uint32_t duty2 = 2800;	// FTM1 PWM duty cycle = 50%
uint32_t duty3 = 28000;	// FTM2 PWM duty cycle = 50%

uint32_t frequency_ch1; // FTM0 measured frequency
uint32_t frequency_ch2; // FTM1 measured frequency
uint32_t frequency_ch3;	// FTM2 measured frequency

///////////////////////
////   FUNCTIONS    ///
///////////////////////

void PORT_Init()
{
	/* Enable clock for PORTA-PORTE */
	PCC->PCCn[PCC_PORTA_INDEX] = PCC_PCCn_CGC_MASK;
	PCC->PCCn[PCC_PORTB_INDEX] = PCC_PCCn_CGC_MASK;
	PCC->PCCn[PCC_PORTC_INDEX] = PCC_PCCn_CGC_MASK;
	PCC->PCCn[PCC_PORTD_INDEX] = PCC_PCCn_CGC_MASK;
	PCC->PCCn[PCC_PORTE_INDEX] = PCC_PCCn_CGC_MASK;

	/* Configure PORT pin to GPIO mode and enable eDMA request on rising/falling edge */
    PORTA->PCR[8] = PORT_PCR_MUX(1) | PORT_PCR_IRQC(1);
    PORTB->PCR[8] = PORT_PCR_MUX(1) | PORT_PCR_IRQC(1);
    PORTC->PCR[8] = PORT_PCR_MUX(1) | PORT_PCR_IRQC(1);

    /* Configure PORT pin to generate PWM */
    PORTD->PCR[15] = PORT_PCR_MUX(2);	// PTD15 -> FTM0 -> Channel0
    PORTB->PCR[2] = PORT_PCR_MUX(2);	// PTB2  -> FTM1 -> Channel0
    PORTE->PCR[10] = PORT_PCR_MUX(4);	// PTE10 -> FTM2 -> Channel4
}

void DMAMUX_Init()
{
	/* Enable clock for DMAMUX */
	PCC->PCCn[PCC_DMAMUX0_INDEX] = PCC_PCCn_CGC_MASK;

	/* Select an enable eDMA channel trigger source */
	DMAMUX->CHCFG[0] = DMAMUX_CHCFG_SOURCE(49) | DMAMUX_CHCFG_ENBL_MASK;
	/* DMA Channel Source Select - PORTB and DMA Enable */
	DMAMUX->CHCFG[1] = DMAMUX_CHCFG_SOURCE(50) | DMAMUX_CHCFG_ENBL_MASK;
	/* DMA Channel Source Select - PORTB and DMA Enable */
	DMAMUX->CHCFG[2] = DMAMUX_CHCFG_SOURCE(51) | DMAMUX_CHCFG_ENBL_MASK;
}

void DMA0_Init()
{
	/* Enable clock for DMA0 */
	PCC->PCCn[PCC_DMA0_INDEX] = PCC_PCCn_CGC_MASK;

	/* Source Configuration */
	DMA->TCD0_SADDR = &dummy1;
	DMA->TCD0_ATTR = DMA_TCD0_ATTR_SSIZE(0);
	DMA->TCD0_SOFF = 0; // no address shift after each transfer
	DMA->TCD0_SLAST = 0;

	/* Destination Configuration */
	DMA->TCD0_DADDR = &dummy1;
	DMA->TCD0_ATTR = DMA_TCD0_ATTR_DSIZE(0);
	DMA->TCD0_DOFF = 0; // no address shift after each transfer
	DMA->TCD0_DLASTSGA = 0;

	/* Set Citer and Biter to Maximum Value */
	DMA->TCD0_CITER.TCD0_CITER_ELINKNO = DMA_TCD0_CITER_ELINKNO_CITER_MASK;
	DMA->TCD0_BITER.TCD0_BITER_ELINKNO = DMA_TCD0_BITER_ELINKNO_BITER_MASK;
	DMA->TCD0_NBYTES.TCD0_NBYTES_MLNO = 1; // transfer one byte on each trigger arrived

	/* Start Transfer for Channel0 */
	DMA->SERQ = DMA_SERQ_SERQ(0);
}

void DMA1_Init()
{
	/* Source Configuration */
	DMA->TCD1_SADDR = &dummy2;
	DMA->TCD1_ATTR = DMA_TCD0_ATTR_SSIZE(0);
	DMA->TCD1_SOFF = 0; // no address shift after each transfer
	DMA->TCD1_SLAST = 0;

	/* Destination Configuration */
	DMA->TCD1_DADDR = &dummy2;
	DMA->TCD1_ATTR = DMA_TCD0_ATTR_DSIZE(0);
	DMA->TCD1_DOFF = 0; // no address shift after each transfer
	DMA->TCD1_DLASTSGA = 0;

	/* Set Citer and Biter to Maximum Value */
	DMA->TCD1_CITER.TCD1_CITER_ELINKNO = DMA_TCD0_CITER_ELINKNO_CITER_MASK;
	DMA->TCD1_BITER.TCD1_BITER_ELINKNO = DMA_TCD0_BITER_ELINKNO_BITER_MASK;
	DMA->TCD1_NBYTES.TCD1_NBYTES_MLNO = 1; // transfer one byte on each trigger arrived

	/* Start Transfer for Channel1 */
	DMA->SERQ = DMA_SERQ_SERQ(1);
}

void DMA2_Init()
{
	/* Source Configuration */
	DMA->TCD2_SADDR = &dummy3;
	DMA->TCD2_ATTR = DMA_TCD0_ATTR_SSIZE(0);
	DMA->TCD2_SOFF = 0; // no address shift after each transfer
	DMA->TCD2_SLAST = 0;

	/* Destination Configuration */
	DMA->TCD2_DADDR = &dummy3;
	DMA->TCD2_ATTR = DMA_TCD0_ATTR_DSIZE(0);
	DMA->TCD2_DOFF = 0; // no address shift after each transfer
	DMA->TCD2_DLASTSGA = 0;

	/* Set Citer and Biter to Maximum Value */
	DMA->TCD2_CITER.TCD2_CITER_ELINKNO = DMA_TCD2_CITER_ELINKNO_CITER_MASK;
	DMA->TCD2_BITER.TCD2_BITER_ELINKNO = DMA_TCD2_BITER_ELINKNO_BITER_MASK;
	DMA->TCD2_NBYTES.TCD2_NBYTES_MLNO = 1; // transfer one byte on each trigger arrived

	/* Start Transfer for Channel2 */
	DMA->SERQ = DMA_SERQ_SERQ(2);
}

 void FTM0_Init()
 {
	 /* Enable clock for FTM0 */
	 PCC->PCCn[PCC_FLEXTMR0_INDEX] = PCC_PCCn_PCS(6) | PCC_PCCn_CGC_MASK;

	 FTM0->SC=FTM_SC_CPWMS_MASK;					// Select up-down counter for Center-Align PWM
	 FTM0->CONTROLS[0].CnSC=FTM_CnSC_ELSB_MASK;		// Select high-true pulses
	 FTM0->MOD = FTM_MOD_MOD(mod1);					// Set modulo
	 FTM0->CONTROLS[0].CnV=FTM_CnV_VAL(duty1);		// Set channel Value
	 FTM0->CNT = 0;									// Counter reset
	 FTM0->SC|=FTM_SC_CLKS(1)|FTM_SC_PWMEN0_MASK;	// Select clock and enable PWM
 }

 void FTM1_Init()
 {
	 /* Enable clock for FTM1 */
	 PCC->PCCn[PCC_FLEXTMR1_INDEX] = PCC_PCCn_PCS(6) | PCC_PCCn_CGC_MASK;

	 FTM1->SC=FTM_SC_CPWMS_MASK;					// Select up-down counter for Center-Align PWM
	 FTM1->CONTROLS[0].CnSC=FTM_CnSC_ELSB_MASK;		// Select high-true pulses
	 FTM1->MOD = FTM_MOD_MOD(mod2);					// Set modulo
	 FTM1->CONTROLS[0].CnV=FTM_CnV_VAL(duty2);		// Set channel Value
	 FTM1->CNT = 0;									// Counter reset
	 FTM1->SC|=FTM_SC_CLKS(1)|FTM_SC_PWMEN0_MASK;	// Select clock and enable PWM
 }

 void FTM2_Init()
 {
	 /* Enable clock for FTM2 */
	 PCC->PCCn[PCC_FLEXTMR2_INDEX] = PCC_PCCn_PCS(6) | PCC_PCCn_CGC_MASK;

	 FTM2->SC=FTM_SC_CPWMS_MASK;					// Select up-down counter for Center-Align PWM
	 FTM2->CONTROLS[4].CnSC=FTM_CnSC_ELSB_MASK;		// Select high-true pulses
	 FTM2->MOD = FTM_MOD_MOD(mod3);					// Set modulo
	 FTM2->CONTROLS[4].CnV=FTM_CnV_VAL(duty3);		// Set channel Value
	 FTM2->CNT = 0;									// Counter reset
	 FTM2->SC|=FTM_SC_CLKS(1)|FTM_SC_PWMEN4_MASK;	// Select clock and enable PWM
 }

 void LPIT0_Init()
 {
	 /* Enable clock for LPIT0 */
	 PCC->PCCn[PCC_LPIT0_INDEX] =  PCC_PCCn_PCS(6) | PCC_PCCn_CGC_MASK;

	 LPIT0->MCR = LPIT_MCR_M_CEN_MASK;				// Enable peripheral clock
	 int wait;
	 for(wait=0;wait<4;wait++){}					// Wait 4 peripheral clock for sync
	 LPIT0->MIER=LPIT_MIER_TIE0_MASK;				// Enable timer interrupt
	 LPIT0->TVAL_CVAL_TCTRL[0].TVAL=112000000-1;	// Set timer value to one second
	 LPIT0->TVAL_CVAL_TCTRL[0].TCTRL=LPIT_TCTRL_T_EN_MASK;		// Enable timer
 }

 void NVIC_Init()
 {
	 FSL_NVIC->IP[LPIT0_IRQn] = 0x01;								// Set priority interrupt
	 FSL_NVIC->ISER[LPIT0_IRQn / 32] |= (1 << (LPIT0_IRQn % 32));	// Enable interrupt
 }

void HSRUN_Init()
{
    SMC->PMPROT=SMC_PMPROT_AHSRUN_MASK	// Allows High Speed Run
    		   |SMC_PMPROT_AVLP_MASK;	// Allows Very Low Power Modes
    SMC->PMCTRL=SMC_PMCTRL_RUNM(3);		// Entry to High Speed Run
    /* Wait for High Speed Run mode */
    while(SMC->PMSTAT != SMC_PMSTAT_PMSTAT(128))
    {
    }
}

void SCG_Init()
{
	/* SOSC Configuration
	 * OSC_OUT = 8MHz, 8MHz EXTAL */

	SCG->SOSCDIV=SCG_SOSCDIV_SOSCDIV1(1)	// System OSC DIV1=1
	      	    |SCG_SOSCDIV_SOSCDIV2(1);	// System OSC DIV2=1
	SCG->SOSCCFG=SCG_SOSCCFG_RANGE(2) 		// Medium frequency range OSC 1-8MHz
				|SCG_SOSCCFG_EREFS(1);		// Select internal OSC
	while(SCG->SOSCCSR & SCG_SOSCCSR_LK_MASK);	// Wait for SOSCCSR unlocked state
	SCG->SOSCCSR=SCG_SOSCCSR_SOSCLPEN_MASK	// Enable OSC in very low power modes
			    |SCG_SOSCCSR_SOSCEN_MASK;	// Enable OSC
	while(!(SCG->SOSCCSR & SCG_SOSCCSR_SOSCVLD_MASK));	// Wait for OSC enabling and valid output

	/* SPLL Configuration
	 * PLL_OUT = 112MHz, 8MHz EXTAL */

	SCG->SPLLDIV=SCG_SPLLDIV_SPLLDIV1(1)	// PLL DIV1=1
	 			|SCG_SPLLDIV_SPLLDIV2(1);	// PLL DIV2=1
	SCG->SPLLCFG=SCG_SPLLCFG_PREDIV(0)      // PLL PREDIV=0
	     		|SCG_SPLLCFG_MULT(12);      // PLL MULT=28
	while(SCG->SPLLCSR & SCG_SPLLCSR_LK_MASK);	// Wait for SPLLCSR unlocked state
	SCG->SPLLCSR=SCG_SPLLCSR_SPLLEN_MASK;	// Enable PLL
	while(!(SCG->SPLLCSR & SCG_SPLLCSR_SPLLVLD_MASK));	// Wait for PLL enabling and valid output

	/* HCCR Configuration
	 * PLL_OUT = 112MHz, 8MHz EXTAL */

    SCG->HCCR=SCG_HCCR_SCS(6)		// PLL source clock
			 |SCG_HCCR_DIVCORE(0)	// DIVCORE=1, Core=112MHz
			 |SCG_HCCR_DIVBUS(1)	// DIVBUS=2, BUS=56MHz
			 |SCG_HCCR_DIVSLOW(3);	// DIVSLOW=4, Flash=28MHz
}


//////////////////
////   MAIN   ////
//////////////////

int main(void)
{
	SCG_Init();
	HSRUN_Init();
	NVIC_Init();
	PORT_Init();
	FTM0_Init();
    FTM1_Init();
    FTM2_Init();
    LPIT0_Init();
	DMAMUX_Init();
	DMA0_Init();
	DMA1_Init();
	DMA2_Init();

	for(;;)
	{

	}
	
	return 0;
}

///////////////////////
////   INTERRUPTS   ///
///////////////////////

void LPIT0_IRQHandler()
{
	/* Measured FTM0 PWM Frequency */
	frequency_ch1 = (DMA->TCD0_BITER.TCD0_BITER_ELINKNO & DMA_TCD0_BITER_ELINKNO_BITER_MASK)
	    		  - (DMA->TCD0_CITER.TCD0_CITER_ELINKNO & DMA_TCD0_CITER_ELINKNO_CITER_MASK);
	/* Set channel0 Citer to Maximum Value */
	DMA->TCD0_CITER.TCD0_CITER_ELINKNO = DMA_TCD0_CITER_ELINKNO_CITER_MASK;

	/* Measured FTM1 PWM Frequency */
	frequency_ch2 = (DMA->TCD1_BITER.TCD1_BITER_ELINKNO & DMA_TCD1_BITER_ELINKNO_BITER_MASK)
			      - (DMA->TCD1_CITER.TCD1_CITER_ELINKNO & DMA_TCD1_CITER_ELINKNO_CITER_MASK);
	/* Set channel1 Citer to Maximum Value */
	DMA->TCD1_CITER.TCD1_CITER_ELINKNO = DMA_TCD1_CITER_ELINKNO_CITER_MASK;

	/* Measured FTM2 PWM Frequency */
	frequency_ch3 = (DMA->TCD2_BITER.TCD2_BITER_ELINKNO & DMA_TCD2_BITER_ELINKNO_BITER_MASK) \
	    		  -(DMA->TCD2_CITER.TCD2_CITER_ELINKNO & DMA_TCD2_CITER_ELINKNO_CITER_MASK);
	/* Set channel2 Citer to Maximum Value */
	DMA->TCD2_CITER.TCD2_CITER_ELINKNO = DMA_TCD2_CITER_ELINKNO_CITER_MASK;

	/* Clear timer overflow flag */
	LPIT0->MSR = LPIT_MSR_TIF0_MASK;
}
