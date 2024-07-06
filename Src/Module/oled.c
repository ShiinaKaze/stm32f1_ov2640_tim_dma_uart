#include "oled.h"
#include "oled_font.h"
#include "swi2c.h"
#include "systick.h"

// SSD1306 configuration
// The SSD1306 address already contains write/read bit
#define SSD1306_Slave_Address 0x78
#define SSD1306_Control_Command 0x00
#define SSD1306_Control_Data 0x40
#define SSD1306_RCC_APB_GPIOx RCC_APB2Periph_GPIOB
#define SSD1306_GPIOx GPIOB
#define SSD1306_SCL GPIO_Pin_6
#define SSD1306_SDA GPIO_Pin_7
#define SSD1306_RCC_APB_I2Cx RCC_APB1Periph_I2C1
#define SSD1306_I2C I2C1
/**
 * SSD1306 software I2C interface implement
 * If you want to use software I2C, you can modify these #define
 */
#define SWI2C_Init() SWI2C_Init(SSD1306_RCC_APB_GPIOx, SSD1306_GPIOx, SSD1306_SCL, SSD1306_SDA)
#define SWI2C_Start() SWI2C_Start(SSD1306_GPIOx, SSD1306_SCL, SSD1306_SDA)
#define SWI2C_Stop() SWI2C_Stop(SSD1306_GPIOx, SSD1306_SCL, SSD1306_SDA)
#define SWI2C_TransmitByte(BYTE) SWI2C_TransmitByte(SSD1306_GPIOx, SSD1306_SCL, SSD1306_SDA, BYTE)

/*************************************************************************************************/
/*SSD1306 software implement*/


void SSD1306_SWInit(void)
{
	RCC_APB2PeriphClockCmd(SSD1306_RCC_APB_GPIOx, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Pin = SSD1306_SCL | SSD1306_SDA;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SSD1306_GPIOx, &GPIO_InitStructure);
}

/**
 * @brief  	Write commands to the device using I2C
 * 			If you want to use hardware I2C, you can modify this function.
 * @param  	Command
 * @retval 	None
 */
void SSD1306_SWriteCommand(uint8_t Command)
{
	SWI2C_Start();
	SWI2C_TransmitByte(SSD1306_Slave_Address);	// slave address
	SWI2C_TransmitByte(SSD1306_Control_Command); // write command
	SWI2C_TransmitByte(Command);
	SWI2C_Stop();
}

/**
 * @brief  	Write data to the device using I2C
 * 			If you want to use hardware I2C, you can modify this function.
 * @param 	Data
 * @retval 	None
 */
void SSD1306_SWriteData(uint8_t Data)
{
	// If you want to use hardware I2C, you can modify this function.
	SWI2C_Start();
	SWI2C_TransmitByte(SSD1306_Slave_Address); // slave address
	SWI2C_TransmitByte(SSD1306_Control_Data);  // write data
	SWI2C_TransmitByte(Data);
	SWI2C_Stop();
}

/*************************************************************************************************/
/*SSD1306 hardware implement*/

void SSD1306_Init(void)
{
	RCC_APB2PeriphClockCmd(SSD1306_RCC_APB_GPIOx, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStructure.GPIO_Pin = SSD1306_SCL | SSD1306_SDA;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SSD1306_GPIOx, &GPIO_InitStructure);

	RCC_APB1PeriphClockCmd(SSD1306_RCC_APB_I2Cx, ENABLE);

	I2C_InitTypeDef I2C_InitStructure;
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_ClockSpeed = 400000;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_OwnAddress1 = 0x00;
	I2C_Init(SSD1306_I2C, &I2C_InitStructure);

	I2C_Cmd(SSD1306_I2C, ENABLE);
}

uint8_t SSD1306_WaitEvent(I2C_TypeDef *I2Cx, uint32_t I2C_EVENT)
{
	uint32_t Timeout;
	Timeout = 1000;
	while (I2C_CheckEvent(I2Cx, I2C_EVENT) == ERROR)
	{
		Timeout--;
		if (Timeout == 0)
		{
			return 1;
		}
	}
	return 0;
}

void SSD1306_WriteCommand(uint8_t Command)
{
	I2C_GenerateSTART(SSD1306_I2C, ENABLE);
	SSD1306_WaitEvent(SSD1306_I2C, I2C_EVENT_MASTER_MODE_SELECT);

	I2C_Send7bitAddress(SSD1306_I2C, SSD1306_Slave_Address, I2C_Direction_Transmitter);
	SSD1306_WaitEvent(SSD1306_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);

	I2C_SendData(SSD1306_I2C, SSD1306_Control_Command);
	SSD1306_WaitEvent(SSD1306_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTING);

	I2C_SendData(SSD1306_I2C, Command);
	SSD1306_WaitEvent(SSD1306_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED);

	I2C_GenerateSTOP(SSD1306_I2C, ENABLE);
}

void SSD1306_WriteData(uint8_t Data)
{
	I2C_GenerateSTART(SSD1306_I2C, ENABLE);
	SSD1306_WaitEvent(SSD1306_I2C, I2C_EVENT_MASTER_MODE_SELECT);

	I2C_Send7bitAddress(SSD1306_I2C, SSD1306_Slave_Address, I2C_Direction_Transmitter);
	SSD1306_WaitEvent(SSD1306_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);

	I2C_SendData(SSD1306_I2C, SSD1306_Control_Data);
	SSD1306_WaitEvent(SSD1306_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTING);

	I2C_SendData(SSD1306_I2C, Data);
	SSD1306_WaitEvent(SSD1306_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED);

	I2C_GenerateSTOP(SSD1306_I2C, ENABLE);
}

/*************************************************************************************************/
/*0.96 inch OLED implement*/

/**
 * @brief  OLED sets cursor position
 * @param  Y With the upper left corner as the origin, downward direction coordinates, range: 0-7
 * @param  X With the upper left corner as the origin, the coordinates in the right direction, range 0-127
 * @retval None
 */
void OLED_SetCursor(uint8_t Y, uint8_t X)
{
	SSD1306_WriteCommand(0xB0 | Y);								// Set Y position
	SSD1306_WriteCommand(0x10 | ((X & 0xF0) >> 4));				// Set X position 4 bits higher
	SSD1306_WriteCommand(SSD1306_Control_Command | (X & 0x0F)); // Set X position 4 bits lower
}

/**
 * @brief  OLED clear screen
 * @param  None
 * @retval None
 */
void OLED_Clear(void)
{
	uint8_t i, j;
	for (j = 0; j < 8; j++)
	{
		OLED_SetCursor(j, 0);
		for (i = 0; i < 128; i++)
		{
			SSD1306_WriteData(SSD1306_Control_Command);
		}
	}
}

/**
 * @brief  OLED displays a character
 * @param  Line Line position, value range 1-4
 * @param  Column Column position, value range 1-16
 * @param  Char A character to display, range: ASCII characters
 * @retval None
 */
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char)
{
	uint8_t i;
	OLED_SetCursor((Line - 1) * 2, (Column - 1) * 8); // Set the cursor position in the top half
	for (i = 0; i < 8; i++)
	{
		SSD1306_WriteData(Font8x16[Char - ' '][i]); // Displays the top half of the content
	}
	OLED_SetCursor((Line - 1) * 2 + 1, (Column - 1) * 8); // Set the cursor position in the bottom half
	for (i = 0; i < 8; i++)
	{
		SSD1306_WriteData(Font8x16[Char - ' '][i + 8]); // Displays the bottom half of the content
	}
}

/**
 * @brief  OLED display string
 * @param  Line Line position, value range 1-4
 * @param  Column Column position, value range 1-16
 * @param  String String to display, range: ASCII characters
 * @retval None
 */
void OLED_ShowString(uint8_t Line, uint8_t Column, char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i++)
	{
		OLED_ShowChar(Line, Column + i, String[i]);
	}
}

/**
 * @brief  OLED power function
 * @retval The return value is equal to X to the Y power
 */
uint32_t OLED_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while (Y--)
	{
		Result *= X;
	}
	return Result;
}

/**
 * @brief  OLED display numbers (decimal, positive)
 * @param  Line Line position, range 1-4
 * @param  Column Column position, range 1-16
 * @param  Number The number to display
 * @param  Length To display the length of the number, the value ranges from 1 to 10
 * @retval None
 */
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i++)
	{
		OLED_ShowChar(Line, Column + i,
					  Number / OLED_Pow(10, Length - i - 1) % 10 + '0');
	}
}

/**
 * @brief  OLED display number (decimal, signed number)
 * @param  Line Line position, range 1-4
 * @param  Column Column position, range 1-16
 * @param  Number The number to display
 * @param  Length To display the length of the number, the value ranges from 1 to 10
 * @retval None
 */
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number,
						uint8_t Length)
{
	uint8_t i;
	uint32_t Number1;
	if (Number >= 0)
	{
		OLED_ShowChar(Line, Column, '+');
		Number1 = Number;
	}
	else
	{
		OLED_ShowChar(Line, Column, '-');
		Number1 = -Number;
	}
	for (i = 0; i < Length; i++)
	{
		OLED_ShowChar(Line, Column + i + 1,
					  Number1 / OLED_Pow(10, Length - i - 1) % 10 + '0');
	}
}

/**
 * @brief  OLED display numbers (hexadecimal, positive)
 * @param  Line Line position, range 1-4
 * @param  Column Column position, range 1-16
 * @param  Number The number to display， value range：0-0xFFFFFFFF
 * @param  Length To display the number length, the value ranges from 1 to 8
 * @retval None
 */
void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number,
					 uint8_t Length)
{
	uint8_t i, SingleNumber;
	for (i = 0; i < Length; i++)
	{
		SingleNumber = Number / OLED_Pow(16, Length - i - 1) % 16;
		if (SingleNumber < 10)
		{
			OLED_ShowChar(Line, Column + i, SingleNumber + '0');
		}
		else
		{
			OLED_ShowChar(Line, Column + i, SingleNumber - 10 + 'A');
		}
	}
}

/**
 * @brief  OLED display numbers (binary, positive)
 * @param  Line Line position, range 1-4
 * @param  Column Column position, range 1-16
 * @param  Number The number to display，value range ：0-1111 1111 1111 1111
 * @param  Length To display the number length, the value ranges from 1 to 16
 * @retval None
 */
void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number,
					 uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i++)
	{
		OLED_ShowChar(Line, Column + i,
					  Number / OLED_Pow(2, Length - i - 1) % 2 + '0');
	}
}

/**
 * @brief  Initializes the OLED
 * @param  None
 * @retval None
 */
void OLED_Init(void)
{
	Delay_s(1);
	SSD1306_Init();

	// software configuration from SSD1306 Application Note
	SSD1306_WriteCommand(0xA8); // Set MUX Ratio
	SSD1306_WriteCommand(0x3F);

	SSD1306_WriteCommand(0xD3); // Set Display Offset
	SSD1306_WriteCommand(0x00);

	SSD1306_WriteCommand(0x40); // Set Display Start Line

	SSD1306_WriteCommand(0xA1); // Set Segment re-map

	SSD1306_WriteCommand(0xC8); // Set COM Output Scan Direction

	SSD1306_WriteCommand(0xDA); // Set COM Pins hardware configuration
	SSD1306_WriteCommand(0x12);

	SSD1306_WriteCommand(0x81); // Set Contrast Control
	SSD1306_WriteCommand(0xCF);

	SSD1306_WriteCommand(0xA4); // Disable Entire Display On

	SSD1306_WriteCommand(0xA6); // Set Normal Display

	SSD1306_WriteCommand(0xD5); // Set Osc Frequency
	SSD1306_WriteCommand(0x80);

	SSD1306_WriteCommand(0x8D); // Enable charge pump regulator
	SSD1306_WriteCommand(0x14);

	SSD1306_WriteCommand(0xAF); // Display On AFh

	OLED_Clear(); // OLED Clear
}
