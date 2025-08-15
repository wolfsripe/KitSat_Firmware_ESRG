#include <utils.h>
#include <ctype.h>
#include <stdio.h>

BMP280_HandleTypedef bmp280 = {
    .i2c = &hi2c2,
    .addr = BMP280_I2C_ADDRESS_0
};

ICM20608_Data imu;
IMU_CalibrationData imu_calib;
float temperature, pressure,humidity;
char debugMsg[128];

SX1278_hw_t SX1278_hw;
SX1278_t SX1278;

uint8_t tx_buffer[64];
uint8_t rx_buffer[64];

uint32_t packet_id;
uint8_t dest, src;

uint8_t state = WAITING_FOR_PING;

//___________________________________________SENSOR DE PRESSAO E TEMPERATURA____________________________________________

void init_bmp280(int targetID){
	if (targetID == 4) {
		printf("\r\nBMP280 Test Application\r\n");

		bmp280_params_t params;
		bmp280_init_default_params(&params);
		params.mode = BMP280_MODE_NORMAL;
		params.filter = BMP280_FILTER_OFF;
		params.oversampling_pressure = BMP280_STANDARD;
		params.oversampling_temperature = BMP280_STANDARD;
		params.oversampling_humidity = BMP280_STANDARD;
		params.standby = BMP280_STANDBY_250;

		if (!bmp280_init(&bmp280, &params)) {
			int len = snprintf(debugMsg, sizeof(debugMsg), "Erro ao inicializar o BMP280\r\n");
			HAL_UART_Transmit_IT(&huart3, (uint8_t*)debugMsg, len);
			HAL_Delay(100);

			while (1);
		}

		int len = snprintf(debugMsg, sizeof(debugMsg), "Sensor inicializado com sucesso\r\n");
		HAL_UART_Transmit_IT(&huart3, (uint8_t*)debugMsg, len);
		HAL_Delay(100);
		if (bmp280.id == BMP280_CHIP_ID) {
			int len = snprintf(debugMsg, sizeof(debugMsg), "Sensor detectado: BMP280\r\n");
			HAL_UART_Transmit_IT(&huart3, (uint8_t*)debugMsg, len);
		} else {
			printf("ERROR\r\n");
		}
	}
}

float read_temp(int targetID){
	if (targetID == 4) {
		if (bmp280_read_float(&bmp280, &temperature, &pressure, &humidity)) {
			int len = snprintf(debugMsg, sizeof(debugMsg), "Temperature: %.2f C\r\n", temperature);
			HAL_UART_Transmit_IT(&huart3, (uint8_t*)debugMsg, len);
		} else {
			printf("Error\r\n");
		}
		return temperature;
	}
}

float read_pressure(int targetID){
	if (targetID == 4) {
		int len;
		if (bmp280_read_float(&bmp280, &temperature, &pressure, &humidity)) {
			len = snprintf(debugMsg, sizeof(debugMsg),"Pressure: %.2f Pa\r\n", pressure);
			HAL_UART_Transmit_IT(&huart3, (uint8_t*)debugMsg, len);

			if (bmp280.id == BMP280_CHIP_ID) {
				len = snprintf(debugMsg, sizeof(debugMsg),"Humity: %.2f %%\r\n", humidity);
				HAL_UART_Transmit_IT(&huart3, (uint8_t*)debugMsg, len);
			}
		} else {
			printf("Error\r\n");
		}
		return pressure;
	}
}


//__________________________________________________SENSOR INERTIAL ICM20608____________________________________________

void init_imu(int targetID) {
	if (targetID == 5) {
		if (ICM20608_Init(&hi2c1) == HAL_OK) {
			int len = snprintf(debugMsg, sizeof(debugMsg), "IMU initialized\n");
			HAL_UART_Transmit_IT(&huart3, (uint8_t*)debugMsg, len);
		} else {
			printf("Error Read IMU\n");
		}
	}
}

void read_imu(int targetID) {
	if (targetID == 5) {
		if (ICM20608_ReadAll(&imu) == HAL_OK) {
			int len = snprintf(debugMsg, sizeof(debugMsg), "Ax: %.2f Ay: %.2f Az: %.2f\n", imu.accel_x, imu.accel_y, imu.accel_z);
			HAL_UART_Transmit_IT(&huart3, (uint8_t*)debugMsg, len);
			HAL_Delay(100);
			int len1 = snprintf(debugMsg, sizeof(debugMsg), "Gx: %.2f Gy: %.2f Gz: %.2f\n", imu.gyro_x, imu.gyro_y, imu.gyro_z);
			HAL_UART_Transmit_IT(&huart3, (uint8_t*)debugMsg, len1);
		}
	}
}


//__________________________________________________GPS_________________________________________________________________
//__________________________________________________ARDUCAM 2MP PLUS____________________________________________________
//__________________________________________________RF LORA_____________________________________________________________

void LoRa_Transmit(){
	tx_buffer[0] = SLAVE_ID;
	tx_buffer[1] = MASTER_ID;
	tx_buffer[2] = 35;
	int len = 3;
	int tx_enter = SX1278_LoRaEntryTx(&SX1278, len , 2000);
	if(!(SX1278_LoRaEntryTx(&SX1278, 64, 2000))){
		printf("RX Mode Failed!\n");
		while (1);  // Halt or retry
	}
	printf("RX Mode Successfully Set!\n");
	if (SX1278_LoRaEntryTx(&SX1278, len , 2000)) {
	  SX1278_LoRaTxPacket(&SX1278, (uint8_t*)tx_buffer, len , 2000);
	  printf("[DATA] Sent packet %lu: To 0x%02X From 0x%02X Payload: %s\n",
			 packet_id, tx_buffer[0], tx_buffer[1], &tx_buffer[2]);
	} else {
	  printf("Failed to enter TX mode\n");
	}
	packet_id++;
}

int LoRa_Receive(){
	if(!(SX1278_LoRaEntryRx(&SX1278, 64, 2000))){
			printf("RX Mode Failed!\n");
		    while (1);  // Halt or retry
		}
	printf("RX Mode Successfully Set!\n");
	int ret = SX1278_LoRaRxPacket(&SX1278);
	if (ret >= 0){
		SX1278_read(&SX1278, rx_buffer, ret);
		dest = rx_buffer[0];
		src  = rx_buffer[1];
		rx_buffer[ret] = '\0';
		char* payload = (char*)&rx_buffer[2];

		if (dest == SLAVE_ID || dest == BROADCAST_ID){
			printf("[DATA] From 0x%02X: %s\r\n", src, payload);
		}
		HAL_Delay(10);
	}
	return ret;
}

void LoRa_Init(){
	printf("sx1278 Example...\n");

	  //initialize LoRa module
	  SX1278_hw.dio0.port = SX1278_DIO0_GPIO_Port;
	  SX1278_hw.dio0.pin = SX1278_DIO0_Pin;
	  SX1278_hw.nss.port = SX1278_CS_GPIO_Port;
	  SX1278_hw.nss.pin = SX1278_CS_Pin;
	  SX1278_hw.reset.port = SX1278_RESET_GPIO_Port;
	  SX1278_hw.reset.pin = SX1278_RESET_Pin;
	  SX1278_hw.spi = &hspi4;

	  SX1278.hw = &SX1278_hw;

	  if (!(SX1278_checkDevice(&SX1278) == SX1278_CHIP)){
		    printf("SX1278 not detected. Halting.\n");
		    while (1);  // Or retry logic
		} else {
		    printf("SX1278 detected successfully.\n");
		}

	  printf("Configuring LoRa module...\r\n");
	  SX1278_init(&SX1278,
			  	  433000000,
				  SX1278_POWER_17DBM,
				  SX1278_LORA_SF_7,
				  SX1278_LORA_BW_125KHZ,
				  SX1278_LORA_CR_4_5,
				  SX1278_LORA_CRC_EN,
				  10);
}

int LoRa_Receive_chat() {
    int ret = -1;

    // Enter RX mode first
    if (SX1278_LoRaEntryRx(&SX1278, 64, 2000)) {
        ret = SX1278_LoRaRxPacket(&SX1278);
        if (ret >= 0) {
            SX1278_read(&SX1278, rx_buffer, ret);
            dest = rx_buffer[0];
            src  = rx_buffer[1];
            rx_buffer[ret] = '\0';

            char* payload = (char*)&rx_buffer[2];

            // Print only if destined to us or broadcast
            if (dest == MASTER_ID || dest == BROADCAST_ID) {
                printf("[DATA] From 0x%02X: %s\r\n", src, payload);
            }
        } else {
            printf("No packet received.\n");
        }
    } else {
        printf("Failed to enter RX mode!\n");
    }

    HAL_Delay(10);  // prevent tight loop
    return ret;
}

void loraTransmit_chat(const char* message) {
    tx_buffer[0] = SLAVE_ID;     // Destination
    tx_buffer[1] = MASTER_ID;    // Source
    int msg_len = snprintf((char*)&tx_buffer[2], sizeof(tx_buffer) - 2, "%s", message);
    int len = msg_len + 2;

    // Enter TX mode and transmit
    if (SX1278_LoRaEntryTx(&SX1278, len, 2000)) {
        SX1278_LoRaTxPacket(&SX1278, (uint8_t*)tx_buffer, len, 2000);
        printf("[DATA] Sent packet %lu: To 0x%02X From 0x%02X Payload: %s\n",
               packet_id, tx_buffer[0], tx_buffer[1], &tx_buffer[2]);
    } else {
        printf("Failed to enter TX mode\n");
    }

    packet_id++;
    HAL_Delay(10);  // Optional delay after TX
}

int _write(int file, char *ptr, int len){
	(void)file;
	int DataIdx;

	for (DataIdx = 0; DataIdx < len; DataIdx++)
		HAL_UART_Transmit(&huart3, (uint8_t*) &ptr[DataIdx], 1, 100);

	return len;
}

void listen_for_ping(void) {

    printf("Waiting for PING from master...\r\n");

    while (state == WAITING_FOR_PING) {
        int ret = SX1278_LoRaRxPacket(&SX1278);
        if (ret > 0) {
            SX1278_read(&SX1278, (uint8_t*)rx_buffer, ret);
            rx_buffer[ret] = '\0';

            uint8_t dest = rx_buffer[0];
            uint8_t src  = rx_buffer[1];
            char* msg    = (char*)&rx_buffer[2];

            if ((dest == SLAVE_ID || dest == BROADCAST_ID) && strncmp(msg, "PING", 4) == 0) {
                printf("[PING] Received from 0x%02X\n", src);

                // Reply with PING
                tx_buffer[0] = src;        // destination = master
                tx_buffer[1] = SLAVE_ID;  // source = this slave
                int len = sprintf((char*)&tx_buffer[2], "PING");

                if (SX1278_LoRaEntryTx(&SX1278, len + 2, 2000)) {
                    SX1278_LoRaTxPacket(&SX1278, (uint8_t*)tx_buffer, len + 2, 2000);
                    printf("[PING REPLY] Sent to 0x%02X\n", src);
                }

                state = RUNNING;  // Ping received & replied, proceed to main loop
            }
        }
        HAL_Delay(10);
    }
}

//__________________________________________________LEDS DRIVER_________________________________________________________
//__________________________________________________ELETRONICS POWER SUPPLY_____________________________________________
//__________________________________________________BUZZER______________________________________________________________

// Global variables
TIM_HandleTypeDef *buzzer_tim;
uint32_t buzzer_channel;
volatile uint8_t beep_active = 0;
volatile uint16_t beep_count = 0;
volatile uint16_t beep_duration = 100;
volatile uint16_t beep_pause = 100;

void Buzzer_PWM_Init(TIM_HandleTypeDef *htim, uint32_t channel) {
    HAL_TIM_PWM_Start(htim, channel);
    __HAL_TIM_SET_COMPARE(htim, channel, 0);
    HAL_Delay(100);
    int len = snprintf(debugMsg, sizeof(debugMsg), "Buzzer Init\n");
    HAL_UART_Transmit(&huart3, (uint8_t*)debugMsg, len, HAL_MAX_DELAY); // Bloqueante
}

void init_buzzer(int targetID, int commandID, const char *params, int n) {
    if (targetID == 7) {
        for(uint8_t i = 0; i < n; i++) {
            HAL_UART_Transmit(&huart3, (uint8_t*)"Start Buzzer\n", strlen("Start Buzzer\n"), HAL_MAX_DELAY);
            HAL_Delay(100);
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 5000);
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
        }
    } else {
        int len = snprintf(debugMsg, sizeof(debugMsg), "Invalid Target");
        HAL_UART_Transmit(&huart3, (uint8_t*)debugMsg, len, HAL_MAX_DELAY);
    }
}

void stop_buzzer(int targetID,int commandID, const char *params){
	if (targetID == 7) {
		int len = snprintf(debugMsg, sizeof(debugMsg), "Stop BEEP: Target=%d | CmdID=%d | Params=%s\n",targetID, commandID, params);
		HAL_UART_Transmit_IT(&huart3, (uint8_t*)debugMsg, len);
		HAL_Delay(100);
		HAL_UART_Transmit_IT(&huart3, (uint8_t*)"Stop Buzzer\n", strlen("Stop Buzzer\n"));
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
	}
}

void morse(const char *params){
	 static const char* morseCodes[] = {
			/* A */ ".-",     /* B */ "-...",   /* C */ "-.-.",   /* D */ "-..",
			/* E */ ".",      /* F */ "..-.",   /* G */ "--.",    /* H */ "....",
			/* I */ "..",     /* J */ ".---",   /* K */ "-.-",    /* L */ ".-..",
			/* M */ "--",     /* N */ "-.",     /* O */ "---",    /* P */ ".--.",
			/* Q */ "--.-",   /* R */ ".-.",    /* S */ "...",    /* T */ "-",
			/* U */ "..-",    /* V */ "...-",   /* W */ ".--",    /* X */ "-..-",
			/* Y */ "-.--",   /* Z */ "--..",
			/* 0 */ "-----",  /* 1 */ ".----",  /* 2 */ "..---",  /* 3 */ "...--",
			/* 4 */ "....-",  /* 5 */ ".....",  /* 6 */ "-....",  /* 7 */ "--...",
			/* 8 */ "---..",  /* 9 */ "----."
		};
	const int dotDuration = 100;
	const int dashDuration = 300;
	const int gapDuration = 100;
	const int letterGap = 300;
	const int wordGap = 700;

	for (const char *p = params; *p; p++) {
		char c = toupper(*p);

		if (c == ' ') {
			HAL_Delay(wordGap);
			continue;
		}
		if ((c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9')) {
			const char *morse = morseCodes[(c >= 'A') ? (c - 'A') : (26 + c - '0')];
			for (const char *s = morse; *s; s++) {
				__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 5000);
				HAL_Delay((*s == '.') ? dotDuration : dashDuration);
				HAL_UART_Transmit_IT(&huart3, (uint8_t*)"beep\n", strlen("beep\n"));
				__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
				if (*(s+1)) HAL_Delay(gapDuration);
				HAL_UART_Transmit_IT(&huart3, (uint8_t*)"-\n", strlen("-"));
			}

			if (*(p+1) && *(p+1) != ' ') HAL_Delay(letterGap);
		}
	}
}
