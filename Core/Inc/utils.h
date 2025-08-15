#ifndef INC_UTILS_H_
#define INC_UTILS_H_

#include "tim.h"
#include "i2c.h"
#include "usart.h"
#include "bmp280.h"
#include "icm20608.h"
#include "SX1278.h"
#include "spi.h"
#include "gpio.h"
#include "i2c.h"

#define SLAVE_ID       0x02   // This is the ID of the satellite
#define MASTER_ID      0x01
#define BROADCAST_ID   0xFF
#define WAITING_FOR_PING 0
#define RUNNING          1

extern uint8_t state;

extern BMP280_HandleTypedef bmp280;
extern float temperature, pressure, humidity;
extern ICM20608_Data imu;
extern IMU_CalibrationData imu_calib;
extern char debugMsg[128];
//___________________________________________SENSOR DE PRESSAO E TEMPERATURA____________________________________________



void init_bmp280(int targetID);
float read_temp();
float read_pressure();

//__________________________________________________SENSOR INERTIAL ICM20608____________________________________________
void init_imu(int targetID);
void read_imu();
//__________________________________________________GPS_________________________________________________________________
//__________________________________________________ARDUCAM 2MP PLUS____________________________________________________
//__________________________________________________RF LORA_____________________________________________________________

extern SX1278_hw_t SX1278_hw;
extern SX1278_t SX1278;

extern uint8_t tx_buffer[64];
extern uint8_t rx_buffer[64];

extern uint32_t packet_id;
extern uint8_t dest, src;

void LoRa_Transmit();
int LoRa_Receive();
void LoRa_Init();
int LoRa_Receive_chat();
void loraTransmit_chat(const char* message);
int _write(int file, char *ptr, int len);
void listen_for_ping(void);
//__________________________________________________LEDS DRIVER_________________________________________________________
//__________________________________________________ELETRONICS POWER SUPPLY_____________________________________________
//__________________________________________________BUZZER______________________________________________________________

void Buzzer_PWM_Init(TIM_HandleTypeDef *htim, uint32_t channel);
void init_buzzer(int targetID, int commandID, const char *params, int n);
void stop_buzzer(int targetID,int commandID, const char *params);
void morse(const char *params);

#endif /* INC_UTILS_H_ */
