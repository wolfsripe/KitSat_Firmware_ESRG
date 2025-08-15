
#include "ICM20608.h"

#define ICM20608_ADDR_LOW   (0x68 << 1)
#define ICM20608_ADDR_HIGH  (0x69 << 1)
#define WHO_AM_I_REG        0x75
#define WHO_AM_I_VAL        0xAF

#define PWR_MGMT_1          0x6B
#define GYRO_CONFIG         0x1B
#define ACCEL_CONFIG        0x1C
#define DATA_START_REG      0x3B

static I2C_HandleTypeDef *icm_i2c_handle;
static uint8_t icm_address = ICM20608_ADDR_LOW;

HAL_StatusTypeDef ICM20608_Init(I2C_HandleTypeDef *hi2c)
{
    icm_i2c_handle = hi2c;

    uint8_t id = 0;
    if (HAL_I2C_Mem_Read(icm_i2c_handle, ICM20608_ADDR_LOW, WHO_AM_I_REG, 1, &id, 1, HAL_MAX_DELAY) == HAL_OK && id == WHO_AM_I_VAL) {
        icm_address = ICM20608_ADDR_LOW;
    } else if (HAL_I2C_Mem_Read(icm_i2c_handle, ICM20608_ADDR_HIGH, WHO_AM_I_REG, 1, &id, 1, HAL_MAX_DELAY) == HAL_OK && id == WHO_AM_I_VAL) {
        icm_address = ICM20608_ADDR_HIGH;
    } else {
        return HAL_ERROR;
    }

    uint8_t pwr = 0x00;
    if (HAL_I2C_Mem_Write(icm_i2c_handle, icm_address, PWR_MGMT_1, 1, &pwr, 1, HAL_MAX_DELAY) != HAL_OK)
        return HAL_ERROR;

    uint8_t gyro_cfg = 0x00;
    if (HAL_I2C_Mem_Write(icm_i2c_handle, icm_address, GYRO_CONFIG, 1, &gyro_cfg, 1, HAL_MAX_DELAY) != HAL_OK)
        return HAL_ERROR;

    uint8_t accel_cfg = 0x00;
    if (HAL_I2C_Mem_Write(icm_i2c_handle, icm_address, ACCEL_CONFIG, 1, &accel_cfg, 1, HAL_MAX_DELAY) != HAL_OK)
        return HAL_ERROR;

    HAL_Delay(100);

    return HAL_OK;
}

HAL_StatusTypeDef ICM20608_ReadAll(ICM20608_Data *data)
{
    uint8_t buf[14];
    if (HAL_I2C_Mem_Read(icm_i2c_handle, icm_address, DATA_START_REG, 1, buf, 14, HAL_MAX_DELAY) != HAL_OK)
        return HAL_ERROR;

    int16_t ax = (buf[0] << 8) | buf[1];
    int16_t ay = (buf[2] << 8) | buf[3];
    int16_t az = (buf[4] << 8) | buf[5];
    int16_t temp = (buf[6] << 8) | buf[7];
    int16_t gx = (buf[8] << 8) | buf[9];
    int16_t gy = (buf[10] << 8) | buf[11];
    int16_t gz = (buf[12] << 8) | buf[13];

    data->accel_x = ax / 16384.0f;
    data->accel_y = ay / 16384.0f;
    data->accel_z = az / 16384.0f;

    data->gyro_x = gx / 131.0f;
    data->gyro_y = gy / 131.0f;
    data->gyro_z = gz / 131.0f;

    data->temperature = (temp / 326.8f) + 25.0f;

    return HAL_OK;
}

HAL_StatusTypeDef ICM20608_Calibrate(IMU_CalibrationData *calib, uint32_t samples) {
    if(samples == 0) return HAL_ERROR;

    ICM20608_Data data;
    float gx_sum = 0, gy_sum = 0, gz_sum = 0;
    float ax_sum = 0, ay_sum = 0, az_sum = 0;

    for(uint32_t i = 0; i < samples; i++) {
        if(ICM20608_ReadAll(&data) != HAL_OK) return HAL_ERROR;

        gx_sum += data.gyro_x;
        gy_sum += data.gyro_y;
        gz_sum += data.gyro_z;

        HAL_Delay(10);
    }

    calib->gyro_offset_x = gx_sum / samples;
    calib->gyro_offset_y = gy_sum / samples;
    calib->gyro_offset_z = gz_sum / samples;

    for(uint32_t i = 0; i < samples; i++) {
        if(ICM20608_ReadAll(&data) != HAL_OK) return HAL_ERROR;

        ax_sum += data.accel_x;
        ay_sum += data.accel_y;
        az_sum += (data.accel_z - 1.0f);

        HAL_Delay(10);
    }

    calib->accel_offset_x = ax_sum / samples;
    calib->accel_offset_y = ay_sum / samples;
    calib->accel_offset_z = az_sum / samples;

    return HAL_OK;
}

/*//MAGNTOMETRO
 *
#include <math.h>

#define HMC5883L_ADDR       (0x1E << 1)  // Endereço I2C
#define HMC5883L_REG_CONFIG_A 0x00
#define HMC5883L_REG_CONFIG_B 0x01
#define HMC5883L_REG_MODE     0x02
#define HMC5883L_REG_DATA     0x03

// Estrutura para dados do magnetômetro
typedef struct {
    float mag_x;
    float mag_y;
    float mag_z;
    float heading;
} HMC5883L_Data;

static I2C_HandleTypeDef *hmc_i2c_handle;

// Inicialização do magnetômetro
HAL_StatusTypeDef HMC5883L_Init(I2C_HandleTypeDef *hi2c) {
    hmc_i2c_handle = hi2c;

    uint8_t config_a = 0x70;  // 8 amostras médias, 15Hz
    uint8_t config_b = 0x20;  // Ganho ±1.3Ga
    uint8_t mode = 0x00;      // Modo contínuo

    if(HAL_I2C_Mem_Write(hmc_i2c_handle, HMC5883L_ADDR, HMC5883L_REG_CONFIG_A, 1, &config_a, 1, HAL_MAX_DELAY) != HAL_OK)
        return HAL_ERROR;

    if(HAL_I2C_Mem_Write(hmc_i2c_handle, HMC5883L_ADDR, HMC5883L_REG_CONFIG_B, 1, &config_b, 1, HAL_MAX_DELAY) != HAL_OK)
        return HAL_ERROR;

    if(HAL_I2C_Mem_Write(hmc_i2c_handle, HMC5883L_ADDR, HMC5883L_REG_MODE, 1, &mode, 1, HAL_MAX_DELAY) != HAL_OK)
        return HAL_ERROR;

    HAL_Delay(50);
    return HAL_OK;
}

// Leitura dos dados
HAL_StatusTypeDef HMC5883L_Read(HMC5883L_Data *data) {
    uint8_t buf[6];

    if(HAL_I2C_Mem_Read(hmc_i2c_handle, HMC5883L_ADDR, HMC5883L_REG_DATA, 1, buf, 6, HAL_MAX_DELAY) != HAL_OK)
        return HAL_ERROR;

    int16_t mx = (buf[0] << 8) | buf[1];
    int16_t mz = (buf[2] << 8) | buf[3];
    int16_t my = (buf[4] << 8) | buf[5];

    // Conversão para Gauss (depende do ganho configurado)
    // Para ±1.3Ga: 0.92 mG/LSB → 0.00092G/LSB
    data->mag_x = mx * 0.00092f;
    data->mag_y = my * 0.00092f;
    data->mag_z = mz * 0.00092f;

    // Cálculo do ângulo de heading (em radianos)
    data->heading = atan2f(data->mag_y, data->mag_x);

    return HAL_OK;
}

typedef struct {
    ICM20608_Data imu;
    HMC5883L_Data mag;
} IMU_CompleteData;

// Função que lê todos os sensores
HAL_StatusTypeDef IMU_ReadAll(IMU_CompleteData *data) {
    if(ICM20608_ReadAll(&data->imu) != HAL_OK)
        return HAL_ERROR;

    if(HMC5883L_Read(&data->mag) != HAL_OK)
        return HAL_ERROR;

    return HAL_OK;
}
 */
