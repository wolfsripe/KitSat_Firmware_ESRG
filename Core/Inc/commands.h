#ifndef COMMANDS_H
#define COMMANDS_H

#include "main.h"
#include "cmsis_os.h"
#include <stdint.h>
#include <string.h>
#include "utils.h"

typedef void (*CommandHandler)(const char *target, const char *cmd_id, const char *params);

typedef struct {
    const char *cmd;
    CommandHandler handler;
} Command_t;

extern Command_t commandTable[];


void handle_beep(const char *target, const char *cmd_id, const char *params);
void handle_beep_stop(const char *target, const char *cmd_id, const char *params);
void handle_morse(const char *target, const char *cmd_id, const char *params);
void handle_WhoAmI(const char *target, const char *cmd_id, const char *params);
void handle_ping_local(const char *target, const char *cmd_id, const char *params);
void handle_ping(const char *target, const char *cmd_id, const char *params);
void handle_reset(const char *target, const char *cmd_id, const char *params);
void handle_get_version(const char *target, const char *cmd_id, const char *params);
void handle_get_status(const char *target, const char *cmd_id, const char *params);

// Cam
void handle_cam_list(const char *target, const char *cmd_id, const char *params);
void handle_cam_num_blocks(const char *target, const char *cmd_id, const char *params);
void handle_cam_get_blocks(const char *target, const char *cmd_id, const char *params);
void handle_cam_take_pic(const char *target, const char *cmd_id, const char *params);
void handle_cam_stream(const char *target, const char *cmd_id, const char *params);
void handle_cam_get_latest(const char *target, const char *cmd_id, const char *params);

// GPS
void handle_gps_get_time(const char *target, const char *cmd_id, const char *params);
void handle_gps_get_location(const char *target, const char *cmd_id, const char *params);
void handle_gps_get_velocity(const char *target, const char *cmd_id, const char *params);
void handle_gps_get_altitude(const char *target, const char *cmd_id, const char *params);
void handle_gps_fix(const char *target, const char *cmd_id, const char *params);
void handle_gps_get_all(const char *target, const char *cmd_id, const char *params);

// BMP
void handle_bmp_get_temp(const char *target, const char *cmd_id, const char *params);
void handle_bmp_get_pres(const char *target, const char *cmd_id, const char *params);
void handle_bmp_get_all(const char *target, const char *cmd_id, const char *params);

// IMU
void handle_imu_get_mag(const char *target, const char *cmd_id, const char *params);
void handle_imu_get_gyr(const char *target, const char *cmd_id, const char *params);
void handle_imu_get_acc(const char *target, const char *cmd_id, const char *params);
void handle_imu_calibrate(const char *target, const char *cmd_id, const char *params);
void handle_imu_get_all(const char *target, const char *cmd_id, const char *params);

// RF
void handle_rf_get_temp(const char *target, const char *cmd_id, const char *params);
void handle_rf_get_rssi(const char *target, const char *cmd_id, const char *params);

// LED
void handle_led_off(const char *target, const char *cmd_id, const char *params);
void handle_led_on(const char *target, const char *cmd_id, const char *params);

// RESET
void handle_reset_local(const char *target, const char *cmd_id, const char *params);

// Ground Station
void handle_gs_set_nodeid(const char *target, const char *cmd_id, const char *params);
void handle_gs_get_nodeid(const char *target, const char *cmd_id, const char *params);
void handle_gs_get_RSSI(const char *target, const char *cmd_id, const char *params);
void handle_gs_set_network(const char *target, const char *cmd_id, const char *params);
void handle_reset_gs(const char *target, const char *cmd_id, const char *params);

// EPS
void handle_eps_get_voltage(const char *target, const char *cmd_id, const char *params);
void handle_eps_measure(const char *target, const char *cmd_id, const char *params);
void handle_eps_get_sp_voltage(const char *target, const char *cmd_id, const char *params);
void handle_eps_get_sp_current(const char *target, const char *cmd_id, const char *params);
void handle_sd_card_connected(const char *target, const char *cmd_id, const char *params);
void handle_rbf_connected(const char *target, const char *cmd_id, const char *params);
void handle_eps_charging(const char *target, const char *cmd_id, const char *params);

#endif // COMMANDS_H
