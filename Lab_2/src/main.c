/*
 * Copyright 2022 NXP
 * NXP confidential.
 * This software is owned or controlled by NXP and may only be used strictly
 * in accordance with the applicable license terms.  By expressly accepting
 * such terms or by downloading, installing, activating and/or otherwise using
 * the software, you are agreeing that you have read, and that you agree to
 * comply with and are bound by, such license terms.  If you do not agree to
 * be bound by the applicable license terms, then you may not retain, install,
 * activate or otherwise use the software.
 */

#ifdef __USE_CMSIS
#include "LPC17xx.h"  // Incluye la biblioteca CMSIS específica para LPC17xx
#endif

#include <cr_section_macros.h>
#include <NXP/crp.h>
#include <stdint.h>
#include "lpc17xx_gpio.h"   // Biblioteca para la configuración y manejo de GPIO
#include "lpc17xx_pinsel.h" // Biblioteca para la configuración de los pines

// Definiciones de puertos y pines
#define PUERTO_0 	    (uint8_t) 0   // Puerto 0
#define PUERTO_2 	    (uint8_t) 2   // Puerto 2

// Definiciones de dirección de pines
#define ENTRADA  	    (uint8_t) 0   // Configuración de pin como entrada
#define SALIDA   	    (uint8_t) 1   // Configuración de pin como salida

// Definición de tipos de borde para las interrupciones
#define RISING_EDGE     (uint8_t) 0   // Interrupción en flanco ascendente

// Definiciones de pines específicos
#define PIN_S_US 	    ( (uint32_t) (1 << 21) )  // Pin 21 del puerto 0, sensor ultrasónico
#define PIN_S_LUZ	    ( (uint32_t) (1 << 23) )  // Pin 23 del puerto 0, sensor de luz
#define PIN_BOTON	    ( (uint32_t) (1 << 6)  )  // Pin 6 del puerto 2, botón
#define PIN_PUERTA	    ( (uint32_t) (1 << 7)  )  // Pin 7 del puerto 2, puerta/LED

#define SYSTICK_TIME	1000

// Variable global para almacenar el estado de la puerta
uint8_t Flag_Puerta = 0;  // Inicializada a 0, indica que la puerta está cerrada

// Prototipos de funciones
void Lectura_sensores(void);  // Función para leer los sensores
void Configurar_pins(void);   // Función para configurar los pines

/**
 * @brief Configura los pines del microcontrolador según sus funciones específicas.
 *
 * Esta función establece la configuración de los pines para el sensor ultrasónico,
 * sensor de luz, botón y el pin asociado a la puerta/LED. También configura las
 * direcciones de los pines y habilita las interrupciones correspondientes.
 */


// SYSTICK INTERRUPT ROUTINE

void SysTick_Handler(void)
{
   Lectura_sensores();

   SYSTICK_ClearCounterFlag();  // Limpia el flag de interrupción
}



void Configurar_pins(void)
{
    PINSEL_CFG_Type PinCfg;

    // Configuración del pin del sensor ultrasónico en el puerto 0, pin 21
    PinCfg.Portnum = PINSEL_PORT_0;
    PinCfg.Pinnum = PINSEL_PIN_21;
    PinCfg.Funcnum = PINSEL_FUNC_0;        // Configurado como GPIO
    PinCfg.Pinmode = PINSEL_PINMODE_PULLDOWN;  // Habilitar resistencia pull-down
    PinCfg.OpenDrain = PINSEL_PINMODE_NORMAL;  // Configuración normal (no open-drain)
    PINSEL_ConfigPin(&PinCfg);             // Aplicar configuración

    // Configuración del pin del sensor de luz en el puerto 0, pin 23
    PinCfg.Pinnum = PINSEL_PIN_23;
    PINSEL_ConfigPin(&PinCfg);             // Aplicar configuración

    // Configuración del pin del botón en el puerto 2, pin 6
    PinCfg.Portnum = PINSEL_PORT_2;
    PinCfg.Pinnum = PINSEL_PIN_6;
    PINSEL_ConfigPin(&PinCfg);             // Aplicar configuración

    // Configuración del pin de la puerta/LED en el puerto 2, pin 7
    PinCfg.Pinnum = PINSEL_PIN_7;
    PINSEL_ConfigPin(&PinCfg);             // Aplicar configuración

    // Configuración de las direcciones de los pines (entrada/salida)
    GPIO_SetDir(PUERTO_0, PIN_S_US, ENTRADA);   // Configurar pin del sensor ultrasónico como entrada
    GPIO_SetDir(PUERTO_0, PIN_S_LUZ, ENTRADA);  // Configurar pin del sensor de luz como entrada
    GPIO_SetDir(PUERTO_2, PIN_BOTON, ENTRADA);  // Configurar pin del botón como entrada
    GPIO_SetDir(PUERTO_2, PIN_PUERTA, SALIDA);  // Configurar pin de la puerta/LED como salida

    // Configuración de la interrupción en el pin del botón (flanco ascendente)
    GPIO_IntCmd(PUERTO_2, PIN_BOTON, RISING_EDGE);
    NVIC_EnableIRQ(EINT3_IRQn);  // Habilitar la interrupción externa 3 en el NVIC
}

/**
 * @brief Lee el estado de los sensores y activa la puerta si es necesario.
 *
 * Esta función verifica si alguno de los sensores (ultrasónico o de luz) está activo.
 * Si alguno de ellos está activo, activa el pin correspondiente para cerrar la puerta.
 */
void Lectura_sensores(void)
{
    uint32_t Flags_sensores = GPIO_ReadValue(PUERTO_0);  // Leer el valor de todos los pines del puerto 0

    // Verificar si alguno de los sensores está activo
    if (Flags_sensores & (PIN_S_US | PIN_S_LUZ)) {
        GPIO_SetValue(PUERTO_2, PIN_PUERTA);  // Activar el pin para cerrar la puerta
        Flag_Puerta = 1;  // Actualizar el flag indicando que la puerta está cerrada
    }
}

/**
 * @brief Manejador de la interrupción externa 3 (EINT3).
 *
 * Esta función se ejecuta cuando se detecta una interrupción en el puerto 2.
 * Verifica si la interrupción fue causada por el botón y, de ser así, llama
 * a la función `Lectura_sensores` para procesar los datos.
 */

void EINT3_IRQHandler(void)
{
    // Verificar si la interrupción fue causada por el pin del botón
    if (LPC_GPIOINT->IO2IntStatR & PIN_BOTON) {
        LPC_GPIOINT->IO2IntClr = PIN_BOTON;  // Limpiar la bandera de interrupción del pin del botón
        if(Flag_Puerta == 0){
            Flag_Puerta = 1;
            GPIO_SetValue(PUERTO_2, PIN_PUERTA);
        }else{
            Flag_Puerta = 0;
            GPIO_ClearValue(PUERTO_2, PIN_PUERTA);
        }
    }
}

/**
 * @brief Función principal.
 *
 * Esta es la función principal del programa, que se ejecuta indefinidamente.
 * Inicializa el sistema, configura los pines y luego entra en un bucle infinito.
 */
int main(void)
{
    SystemInit();  // Inicialización del sistema (configuración básica de reloj, etc.)

    Configurar_pins();  // Configurar los pines del microcontrolador

    SYSTICK_InternalInit(SYSTICK_TIME); // Inicia el Systick con el clock interno
                                        // SYSTICK_TIME es un valor en milisegundos (1000 ms = 1 seg)

    SYSTICK_IntCmd(ENABLE);  // activa las interrupciones por Systick

    SYSTICK_Cmd(ENABLE);  // Enable/disable Systick


    // Bucle principal del programa
    volatile static int i = 0;

    while (1) {
        i++;  // Incrementar un contador (simulación de actividad)
        __asm volatile ("nop");  // Instrucción NOP para permitir depuración precisa
    }

    return 0;  // Aunque este punto nunca se alcanza, se incluye por convención
}
