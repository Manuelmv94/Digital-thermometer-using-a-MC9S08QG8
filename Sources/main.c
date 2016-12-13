#include <hidef.h> /* for EnableInterrupts macro */
#include "derivative.h" /* include peripheral declarations */

unsigned int minuto_en_ms;
unsigned char mensaje[]={"Temperatura de    .  °C"};
unsigned char i;

interrupt VectorNumber_Vmtim void MTIM_ISR (void){
   (void) MTIMSC;
    MTIMSC_TOF=0;
    minuto_en_ms--;
    
    if(minuto_en_ms==0)
    {
       ADCSC1=0b01000000; //Habilitador de interrupciones del ADC, Una sola conversión, canal0 (Inicia la conversión)
       minuto_en_ms=60000;
    }
}

interrupt VectorNumber_Vadc void ADC_ISR (void)
{
   unsigned int temp;
   temp=((unsigned int) ADCRL *3300)/255;
   
   mensaje[19]=temp%10 + 0x30;   //décimas de grado (1mV=0.1°C)
   temp/=10;
   mensaje[17]=temp%10 + 0x30;   //unidades de grados
   temp/=10;
   mensaje[16]=temp%10 + 0x30;   //decenas de grados
   mensaje[15]=temp/10 + 0x30;   //centenas de grados
   
   i=0;
   SCIC2_TIE=1;         //Habilitar interrupción de la comunicación serial
   SCID=mensaje[i++];   //Comenzar la transmisión
}

interrupt VectorNumber_Vscitx void SCI_Tx_Isr(void)
{
   (void) SCIS1;
   SCID=mensaje[i++];      //siguiente caracter a transmitir
     
    if(mensaje[i]==0)
    {
       SCIC2_TIE=0;        //cuando termine el mensaje, deshabilitar las interrupciones de Tx
    }
}

void main(void) {
  EnableInterrupts;
  /* include your code here */
  minuto_en_ms=60000;
  
  MTIMSC_TOIE=1;        //Habilitar interrupciones del MTIM
  MTIMCLK=0b00000100;   //Bus clock y PS de /16
  MTIMMOD=250;          //250 cuentas de 4 us equivalente a 1ms
  
  APCTL1_ADPC0=1;       //PIN ADPO ES ENTRADA ANALOGICA
  ADCSC1=0b01000000;    //Habilitador de interrupciones del ADC, Una sola conversión, canal0 (Inicia la conversión)

  SCIBD=26;             //9600 bps, busclk 4MHz
  SCIC2_TE=1;           //Habilitar transmisión

  
  
  
  for(;;) {
    __RESET_WATCHDOG();	/* feeds the dog */
  } /* loop forever */
  /* please make sure that you never leave main */
}
