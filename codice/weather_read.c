

#include "weather_read.h"
#include "stdint.h"
#include "stm32l0xx_hal.h"





void weather_read(float* lettura ){
	int i=0;

	uint8_t txdata='R';
	uint8_t data[70];
	int ff=0;
	for(ff=0;ff<70;ff++)
		data[ff]=0;
	MX_USART1_UART_Init_weather();
	HAL_UART_Transmit(&huart1,&txdata,1,1000);
	HAL_UART_Receive(&huart1,data,70,1000);

	while((data[i]!='R') && (i<60)){
		i++;
	}
	if (i<60){
		i++;
		uint8_t wind[3]= {data[i+1],data[i+2], data[i+3]};
		lettura [0] = (float)((wind[0]-'0')*100) + (float)((wind[1]-'0')*10) + (float)(wind[2]-'0');
		uint8_t w_sp[6]= {data[i+5],data[i+6], data[i+7], data[i+8], data[i+9], data[i+10]};
		lettura [1] = (float)((w_sp[0]-'0')*100) + (float)((w_sp[1]-'0')*10) + (float)(w_sp[2]-'0') + (float)((w_sp[4]-'0'))/10 + (float)((w_sp[5]-'0'))/100;
		uint8_t dir[3]= {data[i+12],data[i+13],data[i+14]};
		lettura [2] = (float)((dir[0]-'0')*100) + (float)((dir[1]-'0')*10) + (float)(dir[2]-'0');
		uint8_t pr[6] = {data[i+16],data[i+17], data[i+18], data[i+19], data[i+20], data[i+21]};
		lettura [3] = (float)((pr[0]-'0')*1000) + (float)((pr[1]-'0')*100) + (float)((pr[2]-'0')*10) + (float)(pr[3]-'0') + (float)((pr[5]-'0'))/10;
		uint8_t hum[3] = {data[i+23],data[i+24],data[i+25]};
		lettura [4] = (float)((hum[0]-'0')*100) + (float)((hum[1]-'0')*10) + (float)(hum[2]-'0');
		uint8_t t[5] = {data[i+28], data[i+29], data[i+30], data[i+31], data[i+32]};
		lettura [5] = (float)((t[0]-'0')*100) + (float)((t[1]-'0')*10) + (float)(t[2]-'0') + (float)((t[4]-'0'))/10;

		if(data[i+27]=='-')
			lettura[5] = -lettura[5];

		uint8_t d[5] = {data[i+35], data[i+36], data[i+37], data[i+38], data[i+39]};
		lettura [6] = (float)((d[0]-'0')*100) + (float)((d[1]-'0')*10) + (float)(d[2]-'0') + (float)((d[4]-'0'))/10;

		if(data[i+34]=='-')
			lettura[6] = -lettura[6];

		uint8_t prec [9] = {data[i+41], data[i+42], data[i+43], data[i+44], data[i+45], data[i+46], data[i+47], data[i+48], data[i+49]};
		lettura [7] = (float)((prec[0]-'0')*10000) + (float)((prec[1]-'0')*1000) + (float)((prec[2]-'0')*100) + (float)((prec[3]-'0')*10) +  (float)(prec[4]-'0') +  (float)((prec[6]-'0'))/10+  (float)((prec[7]-'0'))/100 +  (float)((prec[8]-'0'))/1000;
		uint8_t intens[7] = {data[i+51],data[i+52],data[i+53], data[i+54], data[i+55], data[i+56], data[i+57]};
		lettura [8] = (float)((intens[0]-'0')*100) + (float)((intens[1]-'0')*10) + (float)(intens[2]-'0') +  (float)((intens[4]-'0'))/10+  (float)((intens[5]-'0'))/100 +  (float)((intens[6]-'0'))/1000;
	}
}

void MX_USART1_UART_Init_weather(void)
{

  /* USER CODE BEGIN USART1_Init 0 */
////
  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */
////
  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 19200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    return;
  }
  /* USER CODE BEGIN USART1_Init 2 */
////
  /* USER CODE END USART1_Init 2 */

}



