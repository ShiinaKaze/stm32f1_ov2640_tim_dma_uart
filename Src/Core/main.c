#include "main.h"
#include "ov2640.h"
#include "oled.h"
#include "systick.h"
#include "uart.h"

#define JPEG_BUFFER_LENGTH 16 * 1024
#define OUTPUT_IMAGE_WIDTH 320
#define OUTPUT_IMAGE_HEIGHT 240
#define OV2640_CLOCK 4
#define OV2640_DVP_CLOCK 48

uint8_t jpeg_buffer[JPEG_BUFFER_LENGTH] = {0};
int main(void)
{
    // Clock
    RCC_ClocksTypeDef Clock;
    RCC_GetClocksFreq(&Clock);

    // Initialization
    OLED_Init();
    UART1_Init(460800);
    OV2640_Init();

    // OV2640 Setting
    OV2640_SetClockDivision(OV2640_CLOCK, OV2640_DVP_CLOCK);
    OV2640_SetOutputJPEG();
    OV2640_SetOutputSize(OUTPUT_IMAGE_WIDTH, OUTPUT_IMAGE_HEIGHT);

    // OLED_ShowString(1, 1, "SYSCLK:");
    // OLED_ShowNum(1, 8, Clock.SYSCLK_Frequency, 8);
    // OLED_ShowString(2, 1, "PCLK2:");
    // OLED_ShowNum(2, 7, Clock.PCLK2_Frequency, 8);
    OLED_ShowString(1, 1, "CAM PID:");
    OLED_ShowHexNum(1, 10, OV2640_GetPID(), 4);
    OV2640_TIM_DMA_Init(jpeg_buffer, JPEG_BUFFER_LENGTH);
    OV2640_TIM_DMA_Start();
    while (1)
    {
        OV2640_TransmitFrame_JPEG_UART1_TIM_DMA(jpeg_buffer, JPEG_BUFFER_LENGTH);
    }
}
