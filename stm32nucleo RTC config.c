#include "contiki.h"
#include "project-conf.h"
#include <stdio.h>
#include "stm32cube_hal_init.h"
#include "stm32l1xx_hal.h"
#include "string.h"
#include "lib/random.h"
#include "sys/ctimer.h"
#include "sys/etimer.h"
#include "simple-udp.h"

#define HELLO_INTERVAL 1 * CLOCK_SECOND
RTC_DateTypeDef sdatestructure;
RTC_TimeTypeDef stimestructure;
RTC_HandleTypeDef RtcHandle;
UART_HandleTypeDef huart3;
uint8_t aShowTime[50] = {0}, aShowTimeStamp[50] = {0};
uint8_t aShowDate[50] = {0}, aShowDateStamp[50] = {0};
uint8_t byte;
char GPS_msg[11] = {0};
char GPS_header[6] = {0};
uint8_t header_gps = 0;
uint8_t msg_gps = 0;
uint8_t count_header_gps = 0;
uint8_t count_msg_gps = 0;
char GPS_msg_to_compare[6] = "GPGGA,";
char hour[2]="";
char min[2]="";
char sec[2]="";
char msec[3]="";
int flag = 1;

/* Private function prototypes -----------------------------------------------*/
static void RTC_TimeConfig(uint8_t a, uint8_t b, uint8_t c);
int RTC_GPS_time_difference(char GPS_msg[]);
void  RTC_sync(char GPS_msg[]);

/* UART3 Interrupt Service Routine */
void USART3_IRQHandler(void)
{
HAL_UART_IRQHandler(&huart3);
}

void uart_gpio_init()
{
GPIO_InitTypeDef GPIO_InitStruct;

__GPIOC_CLK_ENABLE();

/**USART2 GPIO Configuration
PC10     ------> USART3_TX
PC11     ------> USART3_RX
*/
GPIO_InitStruct.Pin = GPIO_PIN_10 | GPIO_PIN_11;
GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
GPIO_InitStruct.Pull = GPIO_PULLUP;
GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;//originale LoW
GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);


}
void uart_init()
{

__USART3_CLK_ENABLE();

huart3.Instance = USART3;
huart3.Init.BaudRate = 9600;
huart3.Init.WordLength = UART_WORDLENGTH_8B;
huart3.Init.StopBits = UART_STOPBITS_1;
huart3.Init.Parity = UART_PARITY_NONE;
huart3.Init.Mode = UART_MODE_TX_RX;
huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
huart3.Init.OverSampling = UART_OVERSAMPLING_16;
HAL_UART_Init(&huart3);

HAL_NVIC_SetPriority(USART3_IRQn, 0, 0);
HAL_NVIC_EnableIRQ(USART3_IRQn);


if (HAL_UART_Receive_IT(&huart3, &byte, 1) != HAL_OK)
{
while(1); // This deserves better error handling....
}


}

/* This callback is called by the HAL_UART_IRQHandler when the given number of bytes are received */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){

if (huart->Instance == USART3)     //current UART
{

HAL_UART_Receive_IT(&huart3, &byte, 1); //activate UART receive interrupt every time
//printf("%c",(char) byte);

if (msg_gps == 1){
GPS_msg [count_msg_gps] = byte;
count_msg_gps++;
if(count_msg_gps == 10){
msg_gps = 0;
count_msg_gps = 0;
//printf("gps msg: %s\n",GPS_msg);

hour[0]=GPS_msg[0];
hour[1]=GPS_msg[1];
min[0]=GPS_msg[2];
min[1]=GPS_msg[3];
sec[0]=GPS_msg[4];
sec[1]=GPS_msg[5];
msec[0]=GPS_msg[7];
msec[1]=GPS_msg[8];
msec[2]=GPS_msg[9];

if (flag == 1){
RTC_sync(GPS_msg);
printf("gps syncro msg: %s\n",GPS_msg);
flag = 0;
}
else{
printf("%d,",RTC_GPS_time_difference(GPS_msg));
printf("%s\n",GPS_msg);
}
}
}
if (header_gps == 1){
GPS_header[count_header_gps] = byte;
count_header_gps++;
if (count_header_gps==6){
count_header_gps = 0;
header_gps = 0;
if (strcmp(GPS_header,GPS_msg_to_compare) == 0){
msg_gps = 1;
}
}
}

if (byte == 36){
header_gps = 1;
}
}
}

/*---------------------------------------------------------------------------*/
PROCESS(hello_world_process, "Hello world process");
AUTOSTART_PROCESSES(&hello_world_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(hello_world_process, ev, data)
{

static struct etimer etimer;
PROCESS_BEGIN();

/*##-1- Configure the RTC peripheral #######################################*/
/* Configure RTC prescaler and RTC data registers */
/* RTC configured as follow:
- Hour Format    = Format 12
- Asynch Prediv  = Value according to source clock
- Synch Prediv   = Value according to source clock
- OutPut         = Output Disable
- OutPutPolarity = High Polarity
- OutPutType     = Open Drain */
__HAL_RTC_RESET_HANDLE_STATE(&RtcHandle);
RtcHandle.Instance = RTC;
RtcHandle.Init.HourFormat     = RTC_HOURFORMAT_24;
RtcHandle.Init.AsynchPrediv   = RTC_ASYNCH_PREDIV;
RtcHandle.Init.SynchPrediv    = RTC_SYNCH_PREDIV;
RtcHandle.Init.OutPut         = RTC_OUTPUT_DISABLE;
RtcHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
RtcHandle.Init.OutPutType     = RTC_OUTPUT_TYPE_OPENDRAIN;

if(HAL_RTC_Init(&RtcHandle) != HAL_OK)
{
/* Initialization Error */
Error_Handler();
}

uart_gpio_init();
uart_init();
HAL_NVIC_DisableIRQ(USART3_IRQn);



while (1){
etimer_set(&etimer, 29 * HELLO_INTERVAL);
PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&etimer));
HAL_NVIC_EnableIRQ(USART3_IRQn);
HAL_UART_Receive_IT(&huart3, &byte, 1);
etimer_set(&etimer, 1*HELLO_INTERVAL);
PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&etimer));
HAL_NVIC_DisableIRQ(USART3_IRQn);
}

PROCESS_END();
}

void Error_Handler(void)
{
while(1)
{
printf("error\n");
}
}

static void RTC_TimeConfig(uint8_t o, uint8_t m, uint8_t s)
{

char tmp_ora[2] ;
char tmp_minuti[2] ;
char tmp_secondi[2] ;
const char ora[4];
const char minuti[4];
const char secondi[4];

unsigned long oo ;
unsigned long mm ;
unsigned long ss ;

/*##-2- Configure the Date #################################################*/
sdatestructure.Year    = 0x15;
sdatestructure.Month   = RTC_MONTH_MARCH;
sdatestructure.Date    = 0x13;
sdatestructure.WeekDay = RTC_WEEKDAY_FRIDAY;

HAL_RTC_SetDate(&RtcHandle,&sdatestructure,FORMAT_BIN);

/*##-3- Configure the Time #################################################*/
stimestructure.Hours          = o;
stimestructure.Minutes        = m;
stimestructure.Seconds        = s;
stimestructure.SubSeconds     = 0x00;
stimestructure.TimeFormat     = RTC_HOURFORMAT_24;
stimestructure.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
stimestructure.StoreOperation = RTC_STOREOPERATION_RESET;

HAL_RTC_SetTime(&RtcHandle,&stimestructure,FORMAT_BIN);
}

void  RTC_sync(char GPS_msg[])
{
long int ora=0;
long int secondi=0;
long int  minuti;
char *ptr;
ora = strtol(hour, &ptr, 10);
minuti = strtol(min, &ptr, 10);
secondi = strtol(sec, &ptr, 10);

RTC_TimeConfig(ora,minuti,secondi);

}

int RTC_GPS_time_difference(char GPS_msg[]){

RTC_DateTypeDef sdatestructureget;
RTC_TimeTypeDef stimestructureget;
int sec_diff=0;
int tot_diff = 0;
int msec_rtc = 0;
char *ptr;
int msec_gps =  0;

/* Get the RTC current Time */
HAL_RTC_GetTime(&RtcHandle, &stimestructureget, FORMAT_BIN);
/* Get the RTC current Date */
HAL_RTC_GetDate(&RtcHandle, &sdatestructureget, FORMAT_BIN);

sec_diff = strtol(sec, &ptr, 10) - stimestructureget.Seconds;
msec_rtc = 1000 - (stimestructureget.SubSeconds * 1000 / RTC_SYNCH_PREDIV);////da vedere!!!!!!
msec_gps = strtol(msec, &ptr, 10);
tot_diff = (sec_diff*1000) + msec_gps - msec_rtc;

/* Display time Format : hh:mm:ss */
sprintf((char*)aShowTime,"%.2d:%.2d:%.2d:%.2d", stimestructureget.Hours, stimestructureget.Minutes, stimestructureget.Seconds,msec_rtc);
/* Display date Format : mm-dd-yy */
sprintf((char*)aShowDate,"%.2d-%.2d-%.2d", sdatestructureget.Month, sdatestructureget.Date, 2000 + sdatestructureget.Year);

return tot_diff;

}
