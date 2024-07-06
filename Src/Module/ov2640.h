#ifndef _OV2640_H
#define _OV2640_H

#include "main.h"
#include "systick.h"
#include "sccb.h"
#include "uart.h"
#include "ov2640_cfg.h"

#define OV2640_DEVICE_ADDRESS 0x60

#define RCC_APB_OV2640 (RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC)

// Control Pin
// SIO_C and SIO_D are defined in sccb.h

#define PCLK_PWDN_HREF_RESET_VSYNC_GPIO GPIOB
#define PCLK_PIN GPIO_Pin_0
#define PWDN_PIN GPIO_Pin_1
#define RESET_PIN GPIO_Pin_3
#define HREF_PIN GPIO_Pin_4
#define VSYNC_PIN GPIO_Pin_5

// Data Pin
#define DATA_GPIO GPIOA
#define DATA0_PIN GPIO_Pin_0
#define DATA1_PIN GPIO_Pin_1
#define DATA2_PIN GPIO_Pin_2
#define DATA3_PIN GPIO_Pin_3
#define DATA4_PIN GPIO_Pin_4
#define DATA5_PIN GPIO_Pin_5
#define DATA6_PIN GPIO_Pin_6
#define DATA7_PIN GPIO_Pin_7

#define OV2640_DATA DATA_GPIO->IDR & 0x00FF
#define OV2640_PCLK (PCLK_PWDN_HREF_RESET_VSYNC_GPIO->IDR & PCLK_PIN ? 1 : 0)
#define OV2640_HREF (PCLK_PWDN_HREF_RESET_VSYNC_GPIO->IDR & HREF_PIN ? 1 : 0)
#define OV2640_VSYNC (PCLK_PWDN_HREF_RESET_VSYNC_GPIO->IDR & VSYNC_PIN ? 1 : 0)

#define SCCB_Write(sub_address, data) SWSCCB_WriteReg(OV2640_DEVICE_ADDRESS, sub_address, data)
#define SCCB_Read(sub_address) SWSCCB_ReadReg(OV2640_DEVICE_ADDRESS, sub_address)

void OV2640_HW_Reset(void);
uint16_t OV2640_GetPID(void);
uint16_t OV2640_GetMID(void);

void OV2640_Init(void);
void OV2640_SetClockDivision(uint8_t Clock, uint8_t DVP_Clock);
void OV2640_SetOutputJPEG(void);
void OV2640_SetOutputSize(uint16_t width, uint16_t height);
void OV2640_TransmitFrame_JPEG_UART(uint8_t *jpeg_buffer, uint16_t jpeg_buffer_length);
void OV2640_TransmitFrame_JPEG_UART1_TIM_DMA(uint8_t *jpeg_buffer, uint16_t jpeg_buffer_length);
void OV2640_TIM_DMA_Init(uint8_t *jpeg_buffer, uint16_t buffer_length);
void OV2640_TIM_DMA_Start(void);
#endif // _OV2640_H
