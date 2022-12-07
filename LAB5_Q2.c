#include <msp430.h>

#define CALADC_15V_30C  *((unsigned int *)0x1A1A)

#define CALADC_15V_85C  *((unsigned int *)0x1A1C)

volatile long temp1;
volatile float IntDegC1;

volatile long temp2;
volatile float IntDegC2;

char result[100];
int count;

void ConfigureAdc_temp1(){
        ADCCTL0 |= ADCSHT_8 | ADCON;
        ADCCTL1 |= ADCSHP;
        ADCCTL2 &= ~ADCRES;
        ADCCTL2 |= ADCRES_2;
        ADCMCTL0 |= ADCSREF_1 | ADCINCH_12;
        ADCIE |= ADCIE0;
}

void ConfigureAdc_temp2(){
        ADCCTL0 &= ~ADCENC;
        ADCCTL0 |= ADCSHT_8 | ADCON;
        ADCCTL1 |= ADCSHP|ADCCONSEQ_1;
        ADCCTL2 &= ~ADCRES;
        ADCCTL2 |= ADCRES_2;
        ADCMCTL0 |= ADCSREF_1 | ADCINCH_6;
        ADCIE |= ADCIE0;
}

void initialize_Adc(){
     ADCCTL0 &= ~ADCIFG;//CLEAR FLAG
     ADCMEM0=0x00000000;
     ADCCTL0=0x0000;
     ADCCTL1=0x0000;
}

void main(void)
{
    WDTCTL = WDTPW + WDTHOLD; // Stop watchdog timer

    PM5CTL0 &= ~LOCKLPM5;
    P1DIR &= ~(BIT1);

    // Configure the ADC pin
    P1SEL1 |=  BIT6;
    P1SEL1 |=  BIT7;

    TB0CCTL0 |= CCIE;
    TB0CCR0 = 65535;
    TB0CTL = TBSSEL__ACLK | MC__UP;

    // Configure reference
    PMMCTL0_H = PMMPW_H;
    PMMCTL2 |= INTREFEN | TSENSOREN | REFVSEL_0;
    //__delay_cycles(400);

    int m=0;

    if(m==0){
      initialize_Adc();
      ConfigureAdc_temp1();
      ADCCTL0 |= ADCENC + ADCSC +ADCMSC;
      while((ADCCTL0 & ADCIFG) == 0);
      _delay_cycles(200000);
      temp1 = ADCMEM0;
      ADCCTL0 &= ~ADCIFG;
      ADCIE &= ~ADCIE0;
      IntDegC1 = (temp1-CALADC_15V_30C)*(85-30)/(CALADC_15V_85C-CALADC_15V_30C)+30;
      //__delay_cycles(400);
      m=1;
    }

    // Configure reference
    PMMCTL0_H = PMMPW_H;
    PMMCTL2 |= INTREFEN | TSENSOREN | REFVSEL_0;
    //__delay_cycles(400);

    if(m==1){
      initialize_Adc();
      ConfigureAdc_temp2();
      ADCCTL0 |= ADCENC + ADCSC +ADCMSC;
      while((ADCCTL0 & ADCIFG) == 0);
      _delay_cycles(20000000);
      temp2 = ADCMEM0;
      ADCCTL0 &= ~ADCIFG;
      ADCIE &= ~ADCIE0;
      IntDegC2 = (temp2-CALADC_15V_30C)*(85-30)/(CALADC_15V_85C-CALADC_15V_30C)+30;
      //__delay_cycles(400);
      m=0;
    }
}
