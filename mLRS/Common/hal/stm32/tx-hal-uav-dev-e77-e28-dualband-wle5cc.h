//*******************************************************
// Copyright (c) MLRS project
// GPL3
// https://www.gnu.org/licenses/gpl-3.0.de.html
//*******************************************************
// hal
//*******************************************************

//#define MLRS_FEATURE_E77_XTAL // must be defined high up, not here, affects main !

//-------------------------------------------------------
// TX UAV-Dev USB TX, E77 E28 dualband, STM32WLE5CC
//
// Fixed pinout:
//   PA2  = UART2 TX    CRSF TX → handset
//   PA3  = UART2 RX    CRSF RX ← handset
//   PA9  = SWUART TX   debug UART
//   PB0  = SX2_RESET   E28 reset
//   PB6  = UART1 TX    serial TX → FC
//   PB7  = UART1 RX    serial RX ← FC
//-------------------------------------------------------

// CRSF full-duplex on UART2 PA2/PA3, no inversion
#define DEVICE_HAS_JRPIN5
#define JRPIN5_FULL_DUPLEX        // separate TX/RX pins: do not disable RX ISR during TX
#define DEVICE_HAS_COM_ON_SERIAL  // serial or com selected by BUTTON during power on
#define DEVICE_HAS_DEBUG_SWUART


//-- Timers, Timing, EEPROM, and such stuff

#define DELAY_USE_DWT

#define EE_START_PAGE             120 // 256 kB flash, 2 kB page

#define MICROS_TIMx               TIM16
#define MICROS_TIM_NAMEPREFIX     TIM16_


//-- UARTS
// UARTB = serial port
// UARTC = COM (CLI)
// UART  = JR bay pin5 / CRSF full-duplex

#define UARTB_USE_UART1_PB6PB7    // serial or com
#define UARTB_BAUD                TX_SERIAL_BAUDRATE
#define UARTB_USE_TX
#define UARTB_TXBUFSIZE           TX_SERIAL_TXBUFSIZE
#define UARTB_USE_TX_ISR
#define UARTB_USE_RX
#define UARTB_RXBUFSIZE           TX_SERIAL_RXBUFSIZE

/* #define UARTC_USE_UART1_PB6PB7 // com
#define UARTC_BAUD                TX_COM_BAUDRATE
#define UARTC_USE_TX
#define UARTC_TXBUFSIZE           TX_COM_TXBUFSIZE_LARGE
#define UARTC_USE_TX_ISR
#define UARTC_USE_RX
#define UARTC_RXBUFSIZE           TX_COM_RXBUFSIZE */

#define UART_USE_UART2_PA2PA3     // CRSF full-duplex, no inversion
#define UART_BAUD                 400000
#define UART_USE_TX
#define UART_TXBUFSIZE            512
#define UART_USE_TX_ISR
#define UART_USE_RX
#define UART_RXBUFSIZE            512
// No JRPIN5_FULL_INTERNAL_ON_TX: standard full-duplex UART, no inversion

#define SWUART_USE_TIM17          // debug UART on PA9
#define SWUART_TX_IO              IO_PA9
#define SWUART_BAUD               115200
#define SWUART_USE_TX
#define SWUART_TXBUFSIZE          512


//-- SX12xx & SPI (SubGHz internal = SX126x)

#define SPI_USE_SUBGHZSPI
#define SPI_USE_CLOCKSPEED_12MHZ

#define SX_BUSY                   0
#define SX_HAS_NO_RESET

#define SX_RX_EN                  IO_PA7
#define SX_TX_EN                  IO_PA6

#define SX_DIO_EXTI_IRQn              SUBGHZ_Radio_IRQn
#define SX_DIO_EXTI_IRQHandler        SUBGHZ_Radio_IRQHandler

#ifdef MLRS_FEATURE_E77_XTAL
#define SX_USE_CRYSTALOSCILLATOR
#endif

void sx_init_gpio(void)
{
    gpio_init(SX_TX_EN, IO_MODE_OUTPUT_PP_LOW, IO_SPEED_VERYFAST);
    gpio_init(SX_RX_EN, IO_MODE_OUTPUT_PP_LOW, IO_SPEED_VERYFAST);
}

bool sx_busy_read(void) { return subghz_is_busy(); }
void sx_reset(void) {}

void sx_amp_transmit(void)
{
    gpio_low(SX_RX_EN);
    gpio_high(SX_TX_EN);
}

void sx_amp_receive(void)
{
    gpio_low(SX_TX_EN);
    gpio_high(SX_RX_EN);
}

void sx_dio_init_exti_isroff(void)
{
    NVIC_SetPriority(SX_DIO_EXTI_IRQn, SX_DIO_EXTI_IRQ_PRIORITY);
}

void sx_dio_enable_exti_isr(void) { NVIC_EnableIRQ(SX_DIO_EXTI_IRQn); }
void sx_dio_exti_isr_clearflag(void) {}


//-- SX12xx II & SPIB (SPI1 = SX1280/E28)

#define SPIB_USE_SPI1
#define SPIB_USE_SCK_IO           IO_PA5
#define SPIB_USE_MISO_IO          IO_PA11
#define SPIB_USE_MOSI_IO          IO_PA12
#define SPIB_CS_IO                IO_PB2
#define SPIB_USE_CLK_LOW_1EDGE
#define SPIB_USE_CLOCKSPEED_18MHZ

#define SX2_RESET                 IO_PB0   // relocated from PA1 to free ADC_IN1
#define SX2_DIO1                  IO_PB8
#define SX2_BUSY                  IO_PA0
#define SX2_RX_EN                 IO_PB12
#define SX2_TX_EN                 IO_PA4

#define SX2_DIO1_SYSCFG_EXTI_PORTx    LL_SYSCFG_EXTI_PORTB
#define SX2_DIO1_SYSCFG_EXTI_LINEx    LL_SYSCFG_EXTI_LINE8
#define SX2_DIO_EXTI_LINE_x           LL_EXTI_LINE_8
#define SX2_DIO_EXTI_IRQn             EXTI9_5_IRQn
#define SX2_DIO_EXTI_IRQHandler       EXTI9_5_IRQHandler

void sx2_init_gpio(void)
{
    gpio_init(SX2_RESET, IO_MODE_OUTPUT_PP_HIGH, IO_SPEED_VERYFAST);
    gpio_init(SX2_DIO1, IO_MODE_INPUT_PD, IO_SPEED_VERYFAST);
    gpio_init(SX2_BUSY, IO_MODE_INPUT_PU, IO_SPEED_VERYFAST);
    gpio_init(SX2_TX_EN, IO_MODE_OUTPUT_PP_LOW, IO_SPEED_VERYFAST);
    gpio_init(SX2_RX_EN, IO_MODE_OUTPUT_PP_LOW, IO_SPEED_VERYFAST);
}

bool sx2_busy_read(void) { return (gpio_read_activehigh(SX2_BUSY)) ? true : false; }

void sx2_amp_transmit(void)
{
    gpio_low(SX2_RX_EN);
    gpio_high(SX2_TX_EN);
}

void sx2_amp_receive(void)
{
    gpio_low(SX2_TX_EN);
    gpio_high(SX2_RX_EN);
}

void sx2_dio_init_exti_isroff(void)
{
    LL_SYSCFG_SetEXTISource(SX2_DIO1_SYSCFG_EXTI_PORTx, SX2_DIO1_SYSCFG_EXTI_LINEx);
    LL_EXTI_DisableEvent_0_31(SX2_DIO_EXTI_LINE_x);
    LL_EXTI_DisableIT_0_31(SX2_DIO_EXTI_LINE_x);
    LL_EXTI_DisableFallingTrig_0_31(SX2_DIO_EXTI_LINE_x);
    LL_EXTI_EnableRisingTrig_0_31(SX2_DIO_EXTI_LINE_x);
    NVIC_SetPriority(SX2_DIO_EXTI_IRQn, SX2_DIO_EXTI_IRQ_PRIORITY);
    NVIC_EnableIRQ(SX2_DIO_EXTI_IRQn);
}

void sx2_dio_enable_exti_isr(void)
{
    LL_EXTI_ClearFlag_0_31(SX2_DIO_EXTI_LINE_x);
    LL_EXTI_EnableIT_0_31(SX2_DIO_EXTI_LINE_x);
}

void sx2_dio_exti_isr_clearflag(void)
{
    LL_EXTI_ClearFlag_0_31(SX2_DIO_EXTI_LINE_x);
}


//-- Button

#define BUTTON                    IO_PA15

void button_init(void)
{
    gpio_init(BUTTON, IO_MODE_INPUT_PU, IO_SPEED_DEFAULT);
}

bool button_pressed(void)
{
    return gpio_read_activelow(BUTTON);
}


//-- LEDs

#define LED_GREEN                 IO_PB4
#define LED_RED                   IO_PB3

void leds_init(void)
{
    gpio_init(LED_GREEN, IO_MODE_OUTPUT_PP_LOW, IO_SPEED_DEFAULT);
    gpio_init(LED_RED, IO_MODE_OUTPUT_PP_LOW, IO_SPEED_DEFAULT);
}

void led_green_off(void) { gpio_low(LED_GREEN); }
void led_green_on(void) { gpio_high(LED_GREEN); }
void led_green_toggle(void) { gpio_toggle(LED_GREEN); }

void led_red_off(void) { gpio_low(LED_RED); }
void led_red_on(void) { gpio_high(LED_RED); }
void led_red_toggle(void) { gpio_toggle(LED_RED); }


//-- Serial or Com Switch
// use com if BUTTON is pressed during power up, else use serial

bool uavdev_ser_or_com_serial = true;

bool ser_or_com_init(void) // return true if is_serial
{
    gpio_init(BUTTON, IO_MODE_INPUT_PU, IO_SPEED_DEFAULT);
    uint8_t cnt = 0;
    for (uint8_t i = 0; i < 16; i++) {
        if (gpio_read_activelow(BUTTON)) cnt++;
    }
    uavdev_ser_or_com_serial = !(cnt > 8);
    return uavdev_ser_or_com_serial;
}

bool ser_or_com_serial(void)
{
    return uavdev_ser_or_com_serial;
}


//-- Buzzer
// has none


//-- POWER

#define POWER_PA_NONE_SX126X
#include "../hal-power-pa.h"

#define POWER2_GAIN_DBM           27
#define POWER2_SX1280_MAX         SX1280_POWER_0_DBM
#define POWER2_USE_DEFAULT_RFPOWER_CALC


//-- TEST

uint32_t porta[] = {
    LL_GPIO_PIN_0, LL_GPIO_PIN_1, LL_GPIO_PIN_2, LL_GPIO_PIN_3, LL_GPIO_PIN_4, LL_GPIO_PIN_5,
    LL_GPIO_PIN_9, LL_GPIO_PIN_10, LL_GPIO_PIN_11, LL_GPIO_PIN_12,
    LL_GPIO_PIN_15,
};

uint32_t portb[] = {
    LL_GPIO_PIN_0, LL_GPIO_PIN_2, LL_GPIO_PIN_3, LL_GPIO_PIN_4, LL_GPIO_PIN_6, LL_GPIO_PIN_7,
    LL_GPIO_PIN_8, LL_GPIO_PIN_12,
};

uint32_t portc[] = {
    //LL_GPIO_PIN_13,
};
