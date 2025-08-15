#ifndef INC_PARSER_H_
#define INC_PARSER_H_

#include "FreeRTOS.h"
#include "main.h"
#include "usart.h"
#include "cmsis_os.h"
#include <commands.h>

#define UART_BUF_SIZE 128
#define MAX_CMD_LENGTH 64
#define MSG_BUFFER_SIZE 128

typedef struct {
    char command[MAX_CMD_LENGTH];
    uint8_t length;
} UART_Command_t;

extern uint8_t rxBuffer;
extern uint8_t rxBuffer6;
extern char receivedData[MSG_BUFFER_SIZE];
extern char receivedData6[MSG_BUFFER_SIZE];
extern uint8_t receivedIndex;
extern uint8_t receivedIndex6;
extern uint8_t complete;
extern uint8_t complete6;

void parseCommand(char *input);
int validateCommand(const char *cmd);

#endif /* INC_PARSER_H_ */
