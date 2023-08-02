#ifndef VCNL_4010_H
#define VCNL_4010_H

#ifdef __cplusplus
extern "C" {
#endif
/******************************************************************************/
typedef union{
    uint8_t ALL_SETTINGS;
    struct {
        uint8_t selftimed_en : 1;
        uint8_t prox_en : 1;
        uint8_t als_en : 1;
        uint8_t prox_od : 1;
        uint8_t als_od : 1;
        uint8_t prox_data_rdy : 1;
        uint8_t als_data_rdy : 1;
        uint8_t config_lock : 1;
    } COMMAND_SETTING;
} Command_Register;

typedef struct {
	// i2c handle
	I2C_HandleTypeDef *VCNL_hi2c;
	/* Interrupt PIN */ 
	GPIO_TypeDef *INT_GPIO;
	uint16_t INT_GPIO_PIN;

	uint16_t RAW_Proximity_Result;
	uint16_t RAW_Ambient_Result;
	Command_Register CMD_REG;
} VCNL_Struct;

typedef enum {
  VCNL4010_Prox_Hz_1_95 = 0,    // 1.95     measurements/sec (Default)
  VCNL4010_Prox_Hz_3_90625 = 1, // 3.90625  measurements/sec
  VCNL4010_Prox_Hz_7_8125 = 2,  // 7.8125   measurements/sec
  VCNL4010_Prox_Hz_16_625 = 3,  // 16.625   measurements/sec
  VCNL4010_Prox_Hz_31_25 = 4,   // 31.25    measurements/sec
  VCNL4010_Prox_Hz_62_5 = 5,    // 62.5     measurements/sec
  VCNL4010_Prox_Hz_125 = 6,     // 125      measurements/sec
  VCNL4010_Prox_Hz_250 = 7,     // 250      measurements/sec
} vcnl4010_Prox_Hz;

typedef enum {
	VCNL4010_Prox_Timing_390K = 0,
	VCNL4010_Prox_Timing_781K = 1,
	VCNL4010_Prox_Timing_1_56M = 2,
	VCNL4010_Prox_Timing_3_13M = 3,
} vcnl4010_Prox_Timing;

#define VCNL4010_SlaveAddress 0x13<<1
#define VCNL4010_WRITE 0x00
#define VCNL4010_READ 0x01

#define VCNL4010_CMD_REG 0x80
#define VCNL4010_PRODUCT_REG 0x81
#define VCNL4010_Proximity_Rate_REG 0x82
#define VCNL4010_IRLEDCurrent_REG 0x83
#define VCNL4010_Ambient_Para_REG 0x84

#define VCNL4010_Ambient_Result_REG 0x85
#define VCNL4010_Ambient_ResultL_REG 0x86
#define VCNL4010_Proximity_Result_REG 0x87
#define VCNL4010_Proximity_ResultL_REG 0x88

#define VCNL4010_Interrupt_Control_REG 0x89
#define VCNL4010_LOW_ThreshouldH_REG 0x8A
#define VCNL4010_LOW_ThreshouldH_REG 0x8B
#define VCNL4010_HIGH_ThreshouldH_REG 0x8C
#define VCNL4010_HIGH_ThreshouldH_REG 0x8D
#define VCNL4010_Interrupt_STATUS_REG 0x8E
#define VCNL4010_Proximity_MODTiming_REG 0x8F

/******************************************************************************/
void VCNL4010_Init_Handle(VCNL_Struct* VCNLx, I2C_HandleTypeDef *hi2c);
void VCNL4010_Init_SET(VCNL_Struct* VCNLx);
static void Write_Data(VCNL_Struct* VCNLx, uint8_t Register, uint8_t TxData);
static void Read_Data(VCNL_Struct* VCNLx, uint8_t Register, uint8_t* RxData, int8_t Rxlen);
static void Decode_Data(VCNL_Struct* VCNLx, uint8_t Register, uint8_t *RxData);

void VCNL4010_W_CMD(VCNL_Struct* VCNLx, uint8_t CMD);
uint8_t VCNL4010_R_CMD(VCNL_Struct* VCNLx);
uint8_t VCNL4010_R_PRODUCT(VCNL_Struct* VCNLx);
void VCNL4010_W_Proximity_Rate(VCNL_Struct* VCNLx, vcnl4010_Prox_Hz Rate);
uint8_t VCNL4010_R_Proximity_Rate(VCNL_Struct* VCNLx);
void VCNL4010_W_IRLEDCurrent(VCNL_Struct* VCNLx, int mA);
uint8_t VCNL4010_R_IRLEDCurrent(VCNL_Struct* VCNLx);
void VCNL4010_W_Ambient_Para(VCNL_Struct* VCNLx, uint8_t SetUP);
uint8_t VCNL4010_R_Ambient_Para(VCNL_Struct* VCNLx);

uint16_t VCNL4010_R_RAWAmbient(VCNL_Struct* VCNLx);
uint16_t VCNL4010_R_RAWProximity(VCNL_Struct* VCNLx);

void VCNL4010_W_INT_Control(VCNL_Struct* VCNLx, uint8_t SetUP);
uint8_t VCNL4010_R_INT_Control(VCNL_Struct* VCNLx);
void VCNL4010_W_INT_STATUS(VCNL_Struct* VCNLx, uint8_t status);
uint8_t VCNL4010_R_INT_STATUS(VCNL_Struct* VCNLx);
void VCNL4010_W_Proximity_Timing(VCNL_Struct* VCNLx, vcnl4010_Prox_Timing Timing);
uint8_t VCNL4010_R_Proximity_Timing(VCNL_Struct* VCNLx);
// void IMU_Init_SET(IMU_Struct *IMUx, int frequency, uint8_t MagIO, uint8_t GyroIO, uint8_t AccIO);
// void IMU_Calibration(IMU_Struct *IMUx);
// void IMU_ContinuousReadData(IMU_Struct *IMUx);
// void IMU_handleData(IMU_Struct *IMUx);
/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif