#include "main.h"
#include "VCNL_4010.h"

/******************************************************************************/
/** @file VCNL_4010
  * @brief The VCNL4010 sensor library implemented using STM32.

  * The library includes a VCNL_Struct structure, 
	which allows support for multiple sensors' readings if the host 
	device has multiple I2C interfaces. This structure needs to be 
	invoked in most functions, as the sensor data will be stored in 
	the structure for easy access. This library is entirely developed in C language.

  * @author ST.Ye, (ykinfe777@gmail.com)
  * @date 2023/08/03 */
/******************************************************************************/

/** @brief Intialize the communicate handle of the vcnl4010 sensor.

  * @param VCNLx: The pointer of the sensor structure.    
  * @param hi2c: The pointer of the I2C handler use in the sensor.  
  * @return None. */
void VCNL4010_Init_Handle(VCNL_Struct* VCNLx, I2C_HandleTypeDef *hi2c){
	VCNLx->VCNL_hi2c = hi2c;
}

/** @brief Intialize the vcnl4010 sensor.
  * 
  * @param VCNLx: The pointer of the sensor structure.  
  * @return None. */
void VCNL4010_Init_SET(VCNL_Struct* VCNLx){
	while(VCNL4010_R_PRODUCT(VCNLx) != 0x21){;}
	VCNL4010_W_CMD(VCNLx, 0x00);
	VCNL4010_R_CMD(VCNLx);
	VCNL4010_W_Proximity_Rate(VCNLx, VCNL4010_Prox_Hz_250);
	VCNL4010_R_Proximity_Rate(VCNLx);
	VCNL4010_W_IRLEDCurrent(VCNLx, 200);
	VCNL4010_W_Ambient_Para(VCNLx, 0x78);
	VCNL4010_W_Proximity_Timing(VCNLx, VCNL4010_Prox_Timing_390K);
	
	VCNL4010_W_INT_Control(VCNLx, 0x00);
	VCNL4010_W_CMD(VCNLx, VCNLx->CMD_REG.ALL_SETTINGS | 0x18);
}

/** @brief Write data into vcnl4010 sensor.
  * 
  * @param VCNLx: The pointer of the sensor structure.  
  * @param Register: The register of the sensor you want to write something.  
  * @param TxData: The data you want to write.  
  * @return None. */
static void Write_Data(VCNL_Struct* VCNLx, uint8_t Register, uint8_t TxData){
	HAL_I2C_Mem_Write(VCNLx->VCNL_hi2c, VCNL4010_SlaveAddress|VCNL4010_WRITE, Register, I2C_MEMADD_SIZE_8BIT, &TxData, 1, 10);
	// HAL_I2C_Master_Transmit(VCNLx->VCNL_hi2c, VCNL4010_SlaveAddress|VCNL4010_WRITE, TxData, 2, 10);
}

/** @brief Read data from vcnl4010 sensor.
  * 
  * @param VCNLx: The pointer of the sensor structure.  
  * @param Register: The register of the sensor you want to read something.  
  * @param RxData: The pointer store data from vcnl4010.  
  * @param Rxlen: The length of data you want to read.  
  * @return None. */
static void Read_Data(VCNL_Struct* VCNLx, uint8_t Register, uint8_t* RxData, int8_t Rxlen){
	// HAL_I2C_Mem_Write(VCNLx->VCNL_hi2c, VCNL4010_SlaveAddress|VCNL4010_WRITE)
	HAL_I2C_Master_Transmit(VCNLx->VCNL_hi2c, VCNL4010_SlaveAddress|VCNL4010_WRITE, &Register, 1, 10);
	// HAL_I2C_Master_Read(VCNLx->VCNL_hi2c, VCNL4010_SlaveAddress|VCNL4010_READ, RxData, Rxlen, 10);
	HAL_I2C_Mem_Read(VCNLx->VCNL_hi2c, VCNL4010_SlaveAddress|VCNL4010_READ, Register, I2C_MEMADD_SIZE_8BIT, RxData, Rxlen, 10);
	Decode_Data(VCNLx, Register, RxData);
}

/** @brief Decode data from vcnl4010 and store the data into sensor structure.
  * 
  * @param VCNLx: The pointer of the sensor structure.  
  * @param Register: The register of the sensor you want to read something.  
  * @param RxData: The pointer store data from vcnl4010.  
  * @return None. */
static void Decode_Data(VCNL_Struct* VCNLx, uint8_t Register, uint8_t *RxData){
	switch (Register) {
		case VCNL4010_CMD_REG:
			VCNLx->CMD_REG.ALL_SETTINGS = RxData[0];
			// printData("CMD REG: %x\n", VCNLx->CMD_REG.ALL_SETTINGS);
			break;
		case VCNL4010_Ambient_Result_REG:
			VCNLx->RAW_Ambient_Result = ((RxData[0]<<8)|RxData[1]);
			break;
		case VCNL4010_Proximity_Result_REG:
			VCNLx->RAW_Proximity_Result = ((RxData[0]<<8)|RxData[1]);
			break;
		default: 
			break;
	}
}

/** @brief Write data to command register of vcnl4010..
  * 
  * @param VCNLx: The pointer of the sensor structure.  
  * @param CMD: Command that write into vcnl4010.
  * @return None. */
void VCNL4010_W_CMD(VCNL_Struct* VCNLx, uint8_t CMD){
	Write_Data(VCNLx, VCNL4010_CMD_REG, CMD);
}

/** @brief Read data from command register of vcnl4010..
  * 
  * @param VCNLx: The pointer of the sensor structure.  
  * @return Command register's data from vcnl4010. */
uint8_t VCNL4010_R_CMD(VCNL_Struct* VCNLx){
	uint8_t RxData[1];
	Read_Data(VCNLx, VCNL4010_CMD_REG, RxData, 1);
	return RxData[0];
}

/** @brief Read data from product register of vcnl4010..
  * 
  * @param VCNLx: The pointer of the sensor structure.  
  * @return Product register's data from vcnl4010. */
uint8_t VCNL4010_R_PRODUCT(VCNL_Struct* VCNLx){
	uint8_t RxData[1];
	Read_Data(VCNLx, VCNL4010_PRODUCT_REG, RxData, 1);
	return RxData[0];
}

/** @brief Write data to proximity rate register of vcnl4010..
  * 
  * @param VCNLx: The pointer of the sensor structure.  
  * @param Rate: Proximity measurement rate that write into vcnl4010.
  * @return None. */
void VCNL4010_W_Proximity_Rate(VCNL_Struct* VCNLx, vcnl4010_Prox_Hz Rate){
	uint8_t original_set = VCNL4010_R_CMD(VCNLx);
	if (VCNLx->CMD_REG.COMMAND_SETTING.selftimed_en){
		VCNL4010_W_CMD(VCNLx, VCNLx->CMD_REG.ALL_SETTINGS & ~0x01);
		// printData("SETTING1:%d\n", VCNLx->CMD_REG.COMMAND_SETTING.selftimed_en);
	}
	Write_Data(VCNLx, VCNL4010_Proximity_Rate_REG, Rate);
	VCNL4010_R_CMD(VCNLx);
	if ((VCNLx->CMD_REG.COMMAND_SETTING.selftimed_en)!=original_set){
		VCNL4010_W_CMD(VCNLx, original_set);
		// printData("SETTING2:%d\n", VCNLx->CMD_REG.COMMAND_SETTING.selftimed_en);
	}
}

/** @brief Read data from proximity rate register of vcnl4010..
  * 
  * @param VCNLx: The pointer of the sensor structure.  
  * @return Proximity measurement rate from vcnl4010. */
uint8_t VCNL4010_R_Proximity_Rate(VCNL_Struct* VCNLx){
	uint8_t RxData[1];
	Read_Data(VCNLx, VCNL4010_Proximity_Rate_REG, RxData, 1);
	return RxData[0];
}

/** @brief Write data to IR LED current register of vcnl4010..
  * 
  * @param VCNLx: The pointer of the sensor structure.  
  * @param mA: Current that write into vcnl4010. 
  * Should be 0 to 200mA, and it will corresponding to 0 ~ 20 and Writeing to the device.
  * @return None. */
void VCNL4010_W_IRLEDCurrent(VCNL_Struct* VCNLx, int mA){
	// mA should be 0 to 200, and it will corresponding to 0 ~ 20 and Writeing to the device
	// printData("%x\n", mA/10);
	Write_Data(VCNLx, VCNL4010_IRLEDCurrent_REG, (uint8_t)(mA/10));
}

/** @brief Read data from IR LED current register of vcnl4010..
  * 
  * @param VCNLx: The pointer of the sensor structure.  
  * @return IR LED current from vcnl4010. */
uint8_t VCNL4010_R_IRLEDCurrent(VCNL_Struct* VCNLx){
	uint8_t RxData[1];
	Read_Data(VCNLx, VCNL4010_IRLEDCurrent_REG, RxData, 1);
	return RxData[0];
}

/** @brief Write data to ambient light parameter register of vcnl4010..
  * 
  * @param VCNLx: The pointer of the sensor structure.  
  * @param SetUP: Data that write into vcnl4010. 
  * @return None. */
void VCNL4010_W_Ambient_Para(VCNL_Struct* VCNLx, uint8_t SetUP){
	uint8_t original_set = VCNL4010_R_CMD(VCNLx);
	if (VCNLx->CMD_REG.COMMAND_SETTING.selftimed_en){
		VCNL4010_W_CMD(VCNLx, VCNLx->CMD_REG.ALL_SETTINGS & ~0x01);
	}
	Write_Data(VCNLx, VCNL4010_CMD_REG, SetUP);
	VCNL4010_W_CMD(VCNLx, original_set);
}

/** @brief Read data from ambient light parameter register of vcnl4010..
  * 
  * @param VCNLx: The pointer of the sensor structure.  
  * @return Ambient light parameter from vcnl4010. */
uint8_t VCNL4010_R_Ambient_Para(VCNL_Struct* VCNLx){
	uint8_t RxData[1];
	Read_Data(VCNLx, VCNL4010_Ambient_Para_REG, RxData, 1);
	return RxData[0];
}

/** @brief Read ambient light measurement from vcnl4010..
  * 
  * @param VCNLx: The pointer of the sensor structure.  
  * @return Raw Ambient light measurement data from vcnl4010. */
uint16_t VCNL4010_R_RAWAmbient(VCNL_Struct* VCNLx){
	uint8_t RxData[2];
	uint8_t original_set = VCNL4010_R_CMD(VCNLx);
	if (VCNLx->CMD_REG.COMMAND_SETTING.selftimed_en){
		VCNL4010_W_CMD(VCNLx, VCNLx->CMD_REG.ALL_SETTINGS & ~0x01);
	}
	while(!(VCNLx->CMD_REG.COMMAND_SETTING.als_data_rdy)){VCNL4010_R_CMD(VCNLx);HAL_Delay(1);}
	Read_Data(VCNLx, VCNL4010_Ambient_Result_REG, RxData, 2);
	VCNL4010_W_CMD(VCNLx, original_set | 0x18);
	return VCNLx->RAW_Ambient_Result;
}

/** @brief Read proximity measurement from vcnl4010..
  * 
  * @param VCNLx: The pointer of the sensor structure.  
  * @return Proximity measurement data from vcnl4010. */
uint16_t VCNL4010_R_RAWProximity(VCNL_Struct* VCNLx){
	// uint8_t* original_INTSTATUS = Read_Data(VCNLx, VCNL4010_Interrupt_STATUS_REG, 1);
	// Write_Data(VCNLx, VCNL4010_Interrupt_STATUS_REG, original_INTSTATUS[0] & ~0x80);
	uint8_t RxData[2];
	uint8_t original_set = VCNL4010_R_CMD(VCNLx);
	if (VCNLx->CMD_REG.COMMAND_SETTING.selftimed_en){
		VCNL4010_W_CMD(VCNLx, VCNLx->CMD_REG.ALL_SETTINGS & ~0x01);
	}
	while(!(VCNLx->CMD_REG.COMMAND_SETTING.prox_data_rdy)){VCNL4010_R_CMD(VCNLx);HAL_Delay(1);}
	Read_Data(VCNLx, VCNL4010_Proximity_Result_REG, RxData, 2);
	VCNL4010_W_CMD(VCNLx, original_set | 0x18);
	return VCNLx->RAW_Proximity_Result;
}

/** @brief Write data to interrupt control register of vcnl4010..
  * 
  * @param VCNLx: The pointer of the sensor structure.  
  * @param SetUP: Data that write into vcnl4010. 
  * @return None. */
void VCNL4010_W_INT_Control(VCNL_Struct* VCNLx, uint8_t SetUP){
	Write_Data(VCNLx, VCNL4010_Interrupt_Control_REG, SetUP);
}

/** @brief Read data from interrupt control register of vcnl4010..
  * 
  * @param VCNLx: The pointer of the sensor structure.  
  * @return Interrupt control register's data of vcnl4010. */
uint8_t VCNL4010_R_INT_Control(VCNL_Struct* VCNLx){
	uint8_t RxData[1];
	Read_Data(VCNLx, VCNL4010_Interrupt_Control_REG, RxData, 1);
	return RxData[0];
}

/** @brief Write data to interrupt status register of vcnl4010..
  * 
  * @param VCNLx: The pointer of the sensor structure.  
  * @param status: Data that write into vcnl4010. 
  * @return None. */
void VCNL4010_W_INT_STATUS(VCNL_Struct* VCNLx, uint8_t status){
	Write_Data(VCNLx, VCNL4010_Interrupt_STATUS_REG, status);
}

/** @brief Read data from interrupt status register of vcnl4010..
  * 
  * @param VCNLx: The pointer of the sensor structure.  
  * @return Interrupt status register's data of vcnl4010. */
uint8_t VCNL4010_R_INT_STATUS(VCNL_Struct* VCNLx){
	uint8_t RxData[1];
	Read_Data(VCNLx, VCNL4010_Interrupt_STATUS_REG, RxData, 1);
	return RxData[0];
}

/** @brief Write data to proximity modulator timing register of vcnl4010..
  * 
  * @param VCNLx: The pointer of the sensor structure.  
  * @param Timing: Proximity IR test frequency that write into vcnl4010. 
	*	00: 390.625 kHz
		01: 781.25  kHz
		10: 1.5625  MHz
		11: 3.125   MHz
	
  * @return None. */
void VCNL4010_W_Proximity_Timing(VCNL_Struct* VCNLx, vcnl4010_Prox_Timing Timing){
	uint8_t original_set = VCNL4010_R_Proximity_Timing(VCNLx);
	Write_Data(VCNLx, VCNL4010_Proximity_MODTiming_REG, original_set&0xE7 | ((Timing)<<3));
}

/** @brief Read data from proximity modulator timing register of vcnl4010..
  * 
  * @param VCNLx: The pointer of the sensor structure.  
  * @return Proximity modulator timing register's data of vcnl4010. */
uint8_t VCNL4010_R_Proximity_Timing(VCNL_Struct* VCNLx){
	uint8_t RxData[1];
	Read_Data(VCNLx, VCNL4010_Proximity_MODTiming_REG, RxData, 1);
	return RxData[0];
}

