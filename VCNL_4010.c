#include "main.h"
#include "VCNL_4010.h"

/******************************************************************************/

/******************************************************************************/
/******************************************************************************/

/******************************************************************************/
/******************************************************************************/
void VCNL4010_Init_Handle(VCNL_Struct* VCNLx, I2C_HandleTypeDef *hi2c){
	VCNLx->VCNL_hi2c = hi2c;
}
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
static void Write_Data(VCNL_Struct* VCNLx, uint8_t Register, uint8_t TxData){
	HAL_I2C_Mem_Write(VCNLx->VCNL_hi2c, VCNL4010_SlaveAddress|VCNL4010_WRITE, Register, I2C_MEMADD_SIZE_8BIT, &TxData, 1, 10);
	// HAL_I2C_Master_Transmit(VCNLx->VCNL_hi2c, VCNL4010_SlaveAddress|VCNL4010_WRITE, TxData, 2, 10);
}
static void Read_Data(VCNL_Struct* VCNLx, uint8_t Register, uint8_t* RxData, int8_t Rxlen){
	// HAL_I2C_Mem_Write(VCNLx->VCNL_hi2c, VCNL4010_SlaveAddress|VCNL4010_WRITE)
	HAL_I2C_Master_Transmit(VCNLx->VCNL_hi2c, VCNL4010_SlaveAddress|VCNL4010_WRITE, &Register, 1, 10);
	// HAL_I2C_Master_Read(VCNLx->VCNL_hi2c, VCNL4010_SlaveAddress|VCNL4010_READ, RxData, Rxlen, 10);
	HAL_I2C_Mem_Read(VCNLx->VCNL_hi2c, VCNL4010_SlaveAddress|VCNL4010_READ, Register, I2C_MEMADD_SIZE_8BIT, RxData, Rxlen, 10);
	Decode_Data(VCNLx, Register, RxData);
}
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
void VCNL4010_W_CMD(VCNL_Struct* VCNLx, uint8_t CMD){
	Write_Data(VCNLx, VCNL4010_CMD_REG, CMD);
}
uint8_t VCNL4010_R_CMD(VCNL_Struct* VCNLx){
	uint8_t RxData[1];
	Read_Data(VCNLx, VCNL4010_CMD_REG, RxData, 1);
	return RxData[0];
}
uint8_t VCNL4010_R_PRODUCT(VCNL_Struct* VCNLx){
	uint8_t RxData[1];
	Read_Data(VCNLx, VCNL4010_PRODUCT_REG, RxData, 1);
	return RxData[0];
}
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
uint8_t VCNL4010_R_Proximity_Rate(VCNL_Struct* VCNLx){
	uint8_t RxData[1];
	Read_Data(VCNLx, VCNL4010_Proximity_Rate_REG, RxData, 1);
	return RxData[0];
}
void VCNL4010_W_IRLEDCurrent(VCNL_Struct* VCNLx, int mA){
	// mA should be 0 to 200, and it will corresponding to 0 ~ 20 and Writeing to the device
	// printData("%x\n", mA/10);
	Write_Data(VCNLx, VCNL4010_IRLEDCurrent_REG, (uint8_t)(mA/10));
}
uint8_t VCNL4010_R_IRLEDCurrent(VCNL_Struct* VCNLx){
	uint8_t RxData[1];
	Read_Data(VCNLx, VCNL4010_IRLEDCurrent_REG, RxData, 1);
	return RxData[0];
}
void VCNL4010_W_Ambient_Para(VCNL_Struct* VCNLx, uint8_t SetUP){
	uint8_t original_set = VCNL4010_R_CMD(VCNLx);
	if (VCNLx->CMD_REG.COMMAND_SETTING.selftimed_en){
		VCNL4010_W_CMD(VCNLx, VCNLx->CMD_REG.ALL_SETTINGS & ~0x01);
	}
	Write_Data(VCNLx, VCNL4010_CMD_REG, SetUP);
	VCNL4010_W_CMD(VCNLx, original_set);
}
uint8_t VCNL4010_R_Ambient_Para(VCNL_Struct* VCNLx){
	uint8_t RxData[1];
	Read_Data(VCNLx, VCNL4010_Ambient_Para_REG, RxData, 1);
	return RxData[0];
}

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

void VCNL4010_W_INT_Control(VCNL_Struct* VCNLx, uint8_t SetUP){
	Write_Data(VCNLx, VCNL4010_Interrupt_Control_REG, SetUP);
}
uint8_t VCNL4010_R_INT_Control(VCNL_Struct* VCNLx){
	uint8_t RxData[1];
	Read_Data(VCNLx, VCNL4010_Interrupt_Control_REG, RxData, 1);
	return RxData[0];
}
void VCNL4010_W_INT_STATUS(VCNL_Struct* VCNLx, uint8_t status){
	Write_Data(VCNLx, VCNL4010_Interrupt_STATUS_REG, status);
}
uint8_t VCNL4010_R_INT_STATUS(VCNL_Struct* VCNLx){
	uint8_t RxData[1];
	Read_Data(VCNLx, VCNL4010_Interrupt_STATUS_REG, RxData, 1);
	return RxData[0];
}
void VCNL4010_W_Proximity_Timing(VCNL_Struct* VCNLx, vcnl4010_Prox_Timing Timing){
	/*
		00: 390.625 kHz
		01: 781.25  kHz
		10: 1.5625  MHz
		11: 3.125   MHz
	*/
	uint8_t original_set = VCNL4010_R_Proximity_Timing(VCNLx);
	Write_Data(VCNLx, VCNL4010_Proximity_MODTiming_REG, original_set&0xE7 | ((Timing)<<3));
}
uint8_t VCNL4010_R_Proximity_Timing(VCNL_Struct* VCNLx){
	uint8_t RxData[1];
	Read_Data(VCNLx, VCNL4010_Proximity_MODTiming_REG, RxData, 1);
	return RxData[0];
}

