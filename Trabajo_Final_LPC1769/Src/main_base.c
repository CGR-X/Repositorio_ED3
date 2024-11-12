#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#ifdef __USE_MCUEXPRESSO
#include <cr_section_macros.h> /* The cr_section_macros is specific to the MCUXpresso delivered toolchain */
#endif

// Librerias:
#include "lpc17xx_adc.h"
#include "lpc17xx_dac.h"
#include "lpc17xx_exti.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_nvic.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_systick.h"
#include "lpc17xx_timer.h"
#include "stdio.h"
#include "system_LPC17xx.h"

// Definicionde de pines:
#define LED_CONTROL_1                                                          \
  ((uint32_t)(1 << 0)) // P2.00 LED 1 PARA CONTROL DE SYSTICK
#define LED_CONTROL_2 ((uint32_t)(1 << 1)) // P2.01 LED 2 PARA CONTROL DE TIMER
#define LED_CONTROL_3 ((uint32_t)(1 << 2)) // P2.02 LED 3 PARA CONTROL DE ADC
#define LED_CONTROL_4 ((uint32_t)(1 << 3)) // P2.03 LED 4 PARA CONTROL DE DAC
#define LED_CONTROL_5                                                          \
  ((uint32_t)(1 << 4)) // P2.04 LED 5 PARA CONTROL DE INTERRUPCION EXTERNA
#define PIN_BOTON ((uint32_t)(1 << 13))  // P2.10 BOTON
#define PIN_ADC_C0 ((uint32_t)(1 << 23)) // P0.23 ADC CANAL 0
#define PIN_ADC_C1 ((uint32_t)(1 << 24)) // P0.24 ADC CANAL 1
#define PIN_ADC_C2 ((uint32_t)(1 << 25)) // P0.25 ADC CANAL 2
#define PIN_DAC ((uint32_t)(1 << 26))    // P0.26 DAC

// Definiciones Systick:
#define SYSTICK_TIME 100 // Tiempo del Systick en ms

// Definiciones Timer:
#define TIMER0_PRESCALE_VALUE 100 // Valor del prescaler del timer en us
#define TIMER0_MATCH0_VALUE                                                    \
  2000 // Valor del match 0 del timer en cantidad de veces

// Definiciones ADC:
#define ADC_FREQ 200000 // Valor de la frecuencia de conversion del ADC en Hz

// Definiciones DAC:
#define DAC_FREQ 25000000 // Valor de la frecuencia de conversion del DAC en Hz

// Definiciones varias:
#define ON 1  // Estado del led - prender
#define OFF 0 // Estado del led - apagar

// Declaracion de variables:
volatile uint32_t DAC_Value = 0; // Valor que va a ser transferido por el DAC
volatile uint16_t
    ADC_Results[3]; // Valores obtenidos de las convversiones del ADC
volatile uint8_t Data[4];

// Declaracion de banderas:
volatile uint8_t DOOR_Flag = 0;
volatile uint8_t SYSTICK_Flag = 0;
volatile uint8_t TIMER0_Flag = 0;
volatile uint8_t ADC_Flag = 0;

// Declaracion de funciones:
void Config_GPIO();
void Config_EINT();
void Config_SYSTICK();
void Config_TIMER0();
void Config_ADC();
void Config_DAC();
void Led_Control(uint8_t estado, uint32_t PIN_led);

int main(void) {

  SystemInit();
  Config_GPIO();
  Config_EINT();
  Config_ADC();
  Config_DAC();
  Config_SYSTICK();
  Config_TIMER0();

  TIM_Cmd(LPC_TIM0, ENABLE);
  SYSTICK_Cmd(ENABLE);

  Led_Control(OFF, LED_CONTROL_1);
  Led_Control(OFF, LED_CONTROL_2);
  Led_Control(OFF, LED_CONTROL_3);
  Led_Control(OFF, LED_CONTROL_4);
  Led_Control(OFF, LED_CONTROL_5);

  while (TRUE) {
    /* code */
  }

  return 0;
}

void Config_GPIO(void) {

  PINSEL_CFG_Type Pincfg;

  // Configuracion general del pinsel para los leds:
  Pincfg.Portnum = PINSEL_PORT_2;
  Pincfg.Funcnum = PINSEL_FUNC_0;
  Pincfg.Pinmode = PINSEL_PINMODE_PULLDOWN;
  Pincfg.OpenDrain = PINSEL_PINMODE_NORMAL;

  // Configuracion pinsel led 1
  Pincfg.Pinnum = PINSEL_PIN_0;
  PINSEL_ConfigPin(&Pincfg);

  // Configuracion pinsel led 2
  Pincfg.Pinnum = PINSEL_PIN_1;
  PINSEL_ConfigPin(&Pincfg);

  // Configuracion pinsel led 3
  Pincfg.Pinnum = PINSEL_PIN_2;
  PINSEL_ConfigPin(&Pincfg);

  // Configuracion pinsel led 4
  Pincfg.Pinnum = PINSEL_PIN_3;
  PINSEL_ConfigPin(&Pincfg);

  // Configuracion pinsel led 5
  Pincfg.Pinnum = PINSEL_PIN_4;
  PINSEL_ConfigPin(&Pincfg);

  // Configuracion GPIO para los leds:
  GPIO_SetDir(PINSEL_PORT_2,
              LED_CONTROL_1 | LED_CONTROL_2 | LED_CONTROL_3 | LED_CONTROL_4 |
                  LED_CONTROL_5,
              GPIO_DIR_OUTPUT);
}

void Config_EINT(void) {

  PINSEL_CFG_Type Pincfg;

  // Configuracion pinsel del pin dela interrupcion:
  Pincfg.Portnum = PINSEL_PORT_2;
  Pincfg.Pinnum = PINSEL_PIN_13;
  Pincfg.Funcnum = PINSEL_FUNC_1;
  Pincfg.Pinmode = PINSEL_PINMODE_PULLDOWN;
  Pincfg.OpenDrain = PINSEL_PINMODE_NORMAL;
  PINSEL_ConfigPin(&Pincfg);

  // Seteamos el pin como entarda
  GPIO_SetDir(PINSEL_PORT_0, PIN_BOTON, GPIO_DIR_INPUT);

  // Habilitamos la interrupcion por flanco ascendente
  GPIO_IntCmd(PINSEL_PORT_0, PIN_BOTON, 0);

  // Inicializacion de las interrupciones externas:
  EXTI_Init();

  // Configuracion de la Iext:
  EXTI_InitTypeDef Eint3Cfg;

  Eint3Cfg.EXTI_Line = EXTI_EINT3;
  Eint3Cfg.EXTI_Mode = EXTI_MODE_LEVEL_SENSITIVE;
  Eint3Cfg.EXTI_polarity = EXTI_POLARITY_HIGH_ACTIVE_OR_RISING_EDGE;

  EXTI_Config(&Eint3Cfg);

  // Habilitacion de las interrupciones esternas del NVIC:
  NVIC_EnableIRQ(EINT3_IRQn);
}

void Config_SYSTICK(void) {

  SYSTICK_InternalInit(SYSTICK_TIME);
  SYSTICK_IntCmd(ENABLE);
}

void Config_TIMER0(void) {

  // Configuracion TIMER0:
  TIM_TIMERCFG_Type TIM0;

  TIM0.PrescaleOption = TIM_PRESCALE_USVAL;
  TIM0.PrescaleValue = TIMER0_PRESCALE_VALUE;
  TIM_Init(LPC_TIM0, TIM_TIMER_MODE, &TIM0);

  // Configuracion del match0:
  TIM_MATCHCFG_Type Match0;

  Match0.MatchChannel = 0;
  Match0.IntOnMatch = ENABLE;
  Match0.ResetOnMatch = ENABLE;
  Match0.ExtMatchOutputType = TIM_EXTMATCH_NOTHING;
  Match0.StopOnMatch = DISABLE;
  Match0.MatchValue = TIMER0_MATCH0_VALUE;
  TIM_ConfigMatch(LPC_TIM0, &Match0);

  // Habilitacion de la interrupciones externas por parte del TIMER 0:
  NVIC_EnableIRQ(TIMER0_IRQn);
}

void Config_ADC(void) {

  // Configuracion pinsel del adc:
  PINSEL_CFG_Type Pincfg;
  Pincfg.Portnum = PINSEL_PORT_0;
  Pincfg.Funcnum = PINSEL_FUNC_1;
  Pincfg.Pinmode = PINSEL_PINMODE_PULLDOWN;
  Pincfg.OpenDrain = PINSEL_PINMODE_NORMAL;

  // Configuracion pinsel ADC canal 0:
  Pincfg.Pinnum = PINSEL_PIN_23;
  PINSEL_ConfigPin(&Pincfg);

  // Configuracion pinsel ADC canal 1:
  Pincfg.Pinnum = PINSEL_PIN_24;
  PINSEL_ConfigPin(&Pincfg);

  // Configuracion pinsel ADC canal 2:
  Pincfg.Pinnum = PINSEL_PIN_25;
  PINSEL_ConfigPin(&Pincfg);

  // Inicializacion ADC:
  ADC_Init(LPC_ADC, ADC_FREQ);

  // Habilitacion de los canales:
  ADC_ChannelCmd(LPC_ADC, ADC_CHANNEL_0, ENABLE);
  ADC_ChannelCmd(LPC_ADC, ADC_CHANNEL_1, ENABLE);
  ADC_ChannelCmd(LPC_ADC, ADC_CHANNEL_2, ENABLE);

  // Habilitamos el modo burst:
  ADC_BurstCmd(LPC_ADC, ENABLE);
}

void Config_DAC(void) {

  // Configuracion pinsel del dac:
  PINSEL_CFG_Type Pincfg;

  Pincfg.Portnum = PINSEL_PORT_0;
  Pincfg.Pinnum = PINSEL_PIN_26;
  Pincfg.Funcnum = PINSEL_FUNC_2;
  Pincfg.Pinmode = PINSEL_PINMODE_PULLDOWN;
  Pincfg.OpenDrain = PINSEL_PINMODE_NORMAL;
  PINSEL_ConfigPin(&Pincfg);

  // Configuracion de la corriente de salida:
  DAC_SetBias(LPC_DAC, DAC_MAX_CURRENT_700uA);

  // Inicializacion del DAC:
  DAC_Init(LPC_DAC);
}

// Funcion encendido/apagado de los leds:

void Led_Control(uint8_t estado, uint32_t PIN_led) {
  // Maneja el estado del led rojo
  if (estado == ON) {
    GPIO_SetValue(PINSEL_PORT_2, PIN_led);
  } else {
    GPIO_ClearValue(PINSEL_PORT_2, PIN_led);
  }
}

void EINT3_IRQHandler(void) {

  uint32_t count_ciclo = 0;

  // Bucle de 30 ms:
  while (count_ciclo < 1000000) {
    count_ciclo++;
  }

  // Comprobacion del puerto
  if (GPIO_ReadValue(PINSEL_PORT_2) & PIN_BOTON) {
    // Control de led de EINT0 y seteamos el estatus de la puerta
    if (DOOR_Flag == 0) {
      Data[0] = 0xFF;
      Led_Control(ON, LED_CONTROL_5);
      DOOR_Flag = !DOOR_Flag;
    } else {
      Data[0] = 0;
      Led_Control(OFF, LED_CONTROL_5);
      DOOR_Flag = !DOOR_Flag;
    }
  }

  // Limpiamos la bandera de la EINT:
  EXTI_ClearEXTIFlag(EXTI_EINT3);
}

void SysTick_Handler(void) {
  // Calculamos el valor para enviar al DAC:
  DAC_Value = (uint32_t)((ADC_Results[0]) / 4);

  // Mandamos el valor por el DAC:
  DAC_UpdateValue(LPC_DAC, DAC_Value);

  // Control de led de control del systick:
  if (SYSTICK_Flag == 0) {
    Led_Control(ON, LED_CONTROL_1);
    SYSTICK_Flag = !SYSTICK_Flag;
  } else {
    Led_Control(OFF, LED_CONTROL_1);
    SYSTICK_Flag = !SYSTICK_Flag;
  }
  // Limpiamos la bandera del Systick:
  SYSTICK_ClearCounterFlag();
}

void TIMER0_IRQHandler(void) {

  // Obtenemos el valor de las converciones:
  ADC_Results[0] = ADC_ChannelGetData(LPC_ADC, ADC_CHANNEL_0);
  ADC_Results[1] = ADC_ChannelGetData(LPC_ADC, ADC_CHANNEL_1);
  ADC_Results[2] = ADC_ChannelGetData(LPC_ADC, ADC_CHANNEL_2);

  // Control led de control del timer 0:
  if (TIMER0_Flag == 0) {
    Led_Control(ON, LED_CONTROL_2);
    TIMER0_Flag = !TIMER0_Flag;
  } else {
    Led_Control(OFF, LED_CONTROL_2);
    TIMER0_Flag = !TIMER0_Flag;
  }
  // Limpiamos bandera del timer:
  TIM_ClearIntPending(LPC_TIM0, TIM_MR0_INT);
}
