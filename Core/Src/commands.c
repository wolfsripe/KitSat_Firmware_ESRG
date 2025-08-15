#include "commands.h"
#include <stdio.h>
#include "main.h"
#include "usart.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


Command_t commandTable[] = {
    { "beep", handle_beep },
    { "beep_stop", handle_beep_stop },
    { "morse", handle_morse },
    { "WhoAmI", handle_WhoAmI },
    { "ping_local", handle_ping_local },
    { "ping", handle_ping },
    { "reset", handle_reset },
    { "get_version", handle_get_version },
    { "get_status", handle_get_status },
    { "cam_list", handle_cam_list },
    { "cam_num_blocks", handle_cam_num_blocks },
    { "cam_get_blocks", handle_cam_get_blocks },
    { "cam_take_pic", handle_cam_take_pic },
    { "cam_stream", handle_cam_stream },
    { "cam_get_latest", handle_cam_get_latest },
    { "gps_get_time", handle_gps_get_time },
    { "gps_get_location", handle_gps_get_location },
    { "gps_get_velocity", handle_gps_get_velocity },
    { "gps_get_altitude", handle_gps_get_altitude },
    { "gps_fix", handle_gps_fix },
    { "gps_get_all", handle_gps_get_all },
    { "env_get_temp", handle_bmp_get_temp },
    { "env_get_pres", handle_bmp_get_pres },
	{ "env_get_all", handle_bmp_get_all },
    { "imu_get_mag", handle_imu_get_mag },
    { "imu_get_gyr", handle_imu_get_gyr },
    { "imu_get_acc", handle_imu_get_acc },
    { "imu_calibrate", handle_imu_calibrate },
    { "imu_get_all", handle_imu_get_all },
    { "rf_get_temp", handle_rf_get_temp },
    { "rf_get_rssi", handle_rf_get_rssi },
    { "led_off", handle_led_off },
    { "led_on", handle_led_on },
    { "reset_local", handle_reset_local },
    { "gs_set_nodeid", handle_gs_set_nodeid },
    { "gs_get_nodeid", handle_gs_get_nodeid },
    { "gs_get_RSSI", handle_gs_get_RSSI },
    { "gs_set_network", handle_gs_set_network },
    { "reset_gs", handle_reset_gs },
    { "eps_get_voltage", handle_eps_get_voltage },
    { "eps_measure", handle_eps_measure },
    { "eps_get_sp_voltage", handle_eps_get_sp_voltage },
    { "eps_get_sp_current", handle_eps_get_sp_current },
    { "sd_card_connected", handle_sd_card_connected },
    { "rbf_connected", handle_rbf_connected },
    { "eps_charging", handle_eps_charging },
    { NULL, NULL }
};

// handlers com placeholders
void handle_beep(const char *target, const char *cmd_id, const char *params) {
	int targetID = atoi(target);
	int commandID = atoi(cmd_id);
	int n = atoi(params);
	Buzzer_PWM_Init(&htim1, TIM_CHANNEL_1);
	init_buzzer(targetID, commandID, params, n);
}

void handle_beep_stop(const char *target, const char *cmd_id, const char *params) {
	int targetID = atoi(target);
	int commandID = atoi(cmd_id);
	stop_buzzer(targetID,commandID, params);
}

void handle_morse(const char *target, const char *cmd_id, const char *params) {
	morse(params);
}



void handle_WhoAmI(const char *target, const char *cmd_id, const char *params) {
    printf("WHOAMI: Node ID\n");
}

void handle_ping_local(const char *target, const char *cmd_id, const char *params) {
    printf("PING LOCAL OK\n");
}

void handle_ping(const char *target, const char *cmd_id, const char *params) {
    printf("PING TO %s\n", params);
}

void handle_reset(const char *target, const char *cmd_id, const char *params) {
    printf("SYSTEM RESET\n");
}

void handle_get_version(const char *target, const char *cmd_id, const char *params) {
    printf("VERSION 1.0\n");
}

void handle_get_status(const char *target, const char *cmd_id, const char *params) {
    printf("STATUS: OK\n");
}


// ARDUCam
void handle_cam_list(const char *target, const char *cmd_id, const char *params) {}
void handle_cam_num_blocks(const char *target, const char *cmd_id, const char *params) {}
void handle_cam_get_blocks(const char *target, const char *cmd_id, const char *params) {}
void handle_cam_take_pic(const char *target, const char *cmd_id, const char *params) {}
void handle_cam_stream(const char *target, const char *cmd_id, const char *params) {}
void handle_cam_get_latest(const char *target, const char *cmd_id, const char *params) {}

// GPS
void handle_gps_get_time(const char *target, const char *cmd_id, const char *params) {}
void handle_gps_get_location(const char *target, const char *cmd_id, const char *params) {}
void handle_gps_get_velocity(const char *target, const char *cmd_id, const char *params) {}
void handle_gps_get_altitude(const char *target, const char *cmd_id, const char *params) {}
void handle_gps_fix(const char *target, const char *cmd_id, const char *params) {}
void handle_gps_get_all(const char *target, const char *cmd_id, const char *params) {}


// BMP
void handle_bmp_get_temp(const char *target, const char *cmd_id, const char *params) {
	int targetID = atoi(target);
	init_bmp280(targetID);
	temperature = read_temp();
}

void handle_bmp_get_pres(const char *target, const char *cmd_id, const char *params) {
	int targetID = atoi(target);
	init_bmp280(targetID);
	pressure = read_pressure();
}
void handle_bmp_get_all(const char *target, const char *cmd_id, const char *params) {
	int targetID = atoi(target);
	init_bmp280(targetID);
	pressure = read_pressure();
	temperature = read_temp();
}

// IMU
void handle_imu_get_mag(const char *target, const char *cmd_id, const char *params) {
	int targetID = atoi(target);
	init_imu(targetID);
	//EM FALTAAAAAAAAAAAA
}
void handle_imu_get_gyr(const char *target, const char *cmd_id, const char *params) {
	int targetID = atoi(target);
	init_imu(targetID);
	if (ICM20608_ReadAll(&imu) == HAL_OK){
		int len = snprintf(debugMsg, sizeof(debugMsg), "Gx: %.2f Gy: %.2f Gz: %.2f\n", imu.gyro_x, imu.gyro_y, imu.gyro_z);
		HAL_UART_Transmit_IT(&huart3, (uint8_t*)debugMsg, len);
	}
}
void handle_imu_get_acc(const char *target, const char *cmd_id, const char *params) {
	int targetID = atoi(target);
	init_imu(targetID);
	if (ICM20608_ReadAll(&imu) == HAL_OK){
		int len = snprintf(debugMsg, sizeof(debugMsg),"Ax: %.2f Ay: %.2f Az: %.2f\n", imu.accel_x, imu.accel_y, imu.accel_z);
		HAL_UART_Transmit_IT(&huart3, (uint8_t*)debugMsg, len);
	}
}
void handle_imu_calibrate(const char *target, const char *cmd_id, const char *params) {
	int targetID = atoi(target);
	init_imu(targetID);
	printf("Calibrate IMU...\n");
	ICM20608_Calibrate(&imu_calib, 100);
}

void handle_imu_get_all(const char *target, const char *cmd_id, const char *params) {
	int targetID = atoi(target);
	init_imu(targetID);
	read_imu();
}

// RF
void handle_rf_get_temp(const char *target, const char *cmd_id, const char *params) {}
void handle_rf_get_rssi(const char *target, const char *cmd_id, const char *params) {}

// LED
void handle_led_off(const char *target, const char *cmd_id, const char *params) {}
void handle_led_on(const char *target, const char *cmd_id, const char *params) {}

// RESET
void handle_reset_local(const char *target, const char *cmd_id, const char *params) {}

// GROUND STATION
void handle_gs_set_nodeid(const char *target, const char *cmd_id, const char *params) {}
void handle_gs_get_nodeid(const char *target, const char *cmd_id, const char *params) {}
void handle_gs_get_RSSI(const char *target, const char *cmd_id, const char *params) {}
void handle_gs_set_network(const char *target, const char *cmd_id, const char *params) {}
void handle_reset_gs(const char *target, const char *cmd_id, const char *params) {}

// EPS
void handle_eps_get_voltage(const char *target, const char *cmd_id, const char *params) {}
void handle_eps_measure(const char *target, const char *cmd_id, const char *params) {}
void handle_eps_get_sp_voltage(const char *target, const char *cmd_id, const char *params) {}
void handle_eps_get_sp_current(const char *target, const char *cmd_id, const char *params) {}
void handle_sd_card_connected(const char *target, const char *cmd_id, const char *params) {}
void handle_rbf_connected(const char *target, const char *cmd_id, const char *params) {}
void handle_eps_charging(const char *target, const char *cmd_id, const char *params) {}
