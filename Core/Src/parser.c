#include <parser.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint8_t rxBuffer;
uint8_t rxBuffer6;
char receivedData[MSG_BUFFER_SIZE];
char receivedData6[MSG_BUFFER_SIZE];
uint8_t receivedIndex = 0;
uint8_t receivedIndex6 = 0;
uint8_t complete=0;
uint8_t complete6=0;


int validateCommand(const char *cmd) {
    return (cmd != NULL && strlen(cmd) > 0);
}

void parseCommand(char *input) {
    char *command = strtok(input, ",");
    char *targetID_str = strtok(NULL, ",");
    char *commandID_str = strtok(NULL, ",");
    char *parameters = strtok(NULL, ",");
    char *explanation = strtok(NULL, ",");
    char *parameter_explanation = strtok(NULL, ",");

    if (!command) return;

    int targetID = targetID_str ? atoi(targetID_str) : -1;
    int commandID = commandID_str ? atoi(commandID_str) : -1;

    char msg[100];

    snprintf(msg, sizeof(msg), "CMD: %s\r\n", command);
    HAL_UART_Transmit(&huart3, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);

    snprintf(msg, sizeof(msg), "TargetID: %d\r\n", targetID);
    HAL_UART_Transmit(&huart3, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);

    snprintf(msg, sizeof(msg), "CommandID: %d\r\n", commandID);
    HAL_UART_Transmit(&huart3, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);

    if(parameters) {
        snprintf(msg, sizeof(msg), "Parameters: %s\r\n", parameters);
        HAL_UART_Transmit(&huart3, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
    }

    if(explanation) {
        snprintf(msg, sizeof(msg), "Explanation: %s\r\n", explanation);
        HAL_UART_Transmit(&huart3, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
    }

    if(parameter_explanation) {
        snprintf(msg, sizeof(msg), "Param Explanation: %s\r\n", parameter_explanation);
        HAL_UART_Transmit(&huart3, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
    }

    for (int i = 0; commandTable[i].cmd != NULL; i++) {
        if (strcmp(command, commandTable[i].cmd) == 0) {
        	char targetStr[12], cmdStr[12];
        	snprintf(targetStr, sizeof(targetStr), "%d", targetID);
        	snprintf(cmdStr, sizeof(cmdStr), "%d", commandID);
        	commandTable[i].handler(targetStr, cmdStr, parameters);
            return;
        }
    }
}

