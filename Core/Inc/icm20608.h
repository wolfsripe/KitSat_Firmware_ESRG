
#ifndef ICM20608_H
#define ICM20608_H

#include "stm32f7xx_hal.h"

typedef struct {
    float accel_x;
    float accel_y;
    float accel_z;
    float gyro_x;
    float gyro_y;
    float gyro_z;
    float temperature;
} ICM20608_Data;

typedef struct {
    float gyro_offset_x;
    float gyro_offset_y;
    float gyro_offset_z;
    float accel_offset_x;
    float accel_offset_y;
    float accel_offset_z;
} IMU_CalibrationData;

HAL_StatusTypeDef ICM20608_Init(I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef ICM20608_ReadAll(ICM20608_Data *data);
HAL_StatusTypeDef ICM20608_Calibrate(IMU_CalibrationData *calib, uint32_t samples);
/*
typedef struct {
    float mag_x;
    float mag_y;
    float mag_z;
    float heading;
} HMC5883L_Data;

HAL_StatusTypeDef HMC5883L_Init(I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef HMC5883L_Read(HMC5883L_Data *data);
*/
#endif // ICM20608_H

