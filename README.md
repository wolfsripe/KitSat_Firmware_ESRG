# KitSat_Firmware_ESRG

Este repositório contém o desenvolvimento de um sistema embebido em tempo real (RTOS) baseado no microcontrolador STM32F767, concebido para suportar múltiplos periféricos e executar comandos remotos recebidos por diferentes interfaces. O sistema foi implementado com recurso à HAL (Hardware Abstraction Layer) da STMicroelectronics e segue uma arquitetura modular orientada a tarefas, ideal para aplicações em sistemas embarcados de média e alta complexidade.

O objetivo principal do projeto é integrar sensores diversos e um módulo de comunicação RF (LoRa) num sistema resiliente e extensível, capaz de receber, interpretar e executar comandos dinamicamente, garantindo simultaneamente isolamento de falhas, escalabilidade e flexibilidade em tempo de execução.



# Arquitetura do Sistema

A arquitetura é organizada segundo o modelo Dispatcher–Worker, comum em sistemas RTOS, permitindo a gestão concorrente de múltiplos comandos com tratamento individualizado:

Tarefa de Recepção UART e LoRa
Responsável por receber mensagens através das interfaces UART e LoRa, armazenando os dados em buffers temporários com sinalização por variáveis de controlo (ex: complete, complete6). As interrupções são configuradas com HAL_UART_Receive_IT, garantindo recepção não bloqueante.

Parser de Comandos
A tarefa principal de parsing verifica a validade das mensagens e interpreta os comandos de acordo com a sintaxe estabelecida (ex: cmd,targetID,commandID,parametros...). A identificação e roteamento para os handlers é feita através de uma tabela central de comandos (commandTable[]).

Dispatcher de Execução
Uma task separada atua como gestor de execução. A partir de uma fila global de comandos (commandExecQueueHandle), instancia dinamicamente handlers especializados que processam cada comando de forma independente. Esta abordagem assegura isolamento de contexto, permitindo a alocação controlada de recursos (stack, tempo de CPU, prioridades).



# Comunicação via LoRa (SX1278)

O sistema suporta comunicação bidirecional através de LoRa SX1278, utilizando identificação de origem e destino no payload, com suporte a modos TX e RX com controle de timeouts e verificação de destinatário (MASTER_ID, SLAVE_ID, BROADCAST_ID). A receção e transmissão são feitas com:

SX1278_LoRaEntryRx() / SX1278_LoRaEntryTx()

SX1278_LoRaRxPacket() / SX1278_LoRaTxPacket()



# Sensores Integrados

O projeto integra diversos sensores, comunicando através do barramento I2C ou SPI, todos acedidos via drivers HAL. A leitura periódica e tratamento de dados são feitos em tarefas dedicadas (em desenvolvimento futuro podem ser desacopladas em handlers):

IMU (Inertial Measurement Unit) – Magnetómetro, Acelerómetro e Giroscópio para medição de orientação, aceleração e rotação.

Magnetómetro – Utilizado para orientação e heading.

Sensor de Pressão (BMP) – Mede pressão atmosférica e temperatura.

GPS – Fornece dados de localização, altitude, tempo e velocidade.



# Sistema RTOS

A aplicação utiliza CMSIS-RTOS v1, com criação de tarefas (osThreadDef), filas de mensagens (osMessageQDef), semáforos e mutexes. As principais tarefas:

Uart_Parser_Task – Recebe e interpreta mensagens UART/LoRa.

Dispatcher_Task – Encaminha comandos validados.

Handler_Task – Executa funções específicas por comando (ex: beep, reset, gps_get_time, imu_get_acc).

Recursos como stacks estáticos e alocação dinâmica são combinados para garantir segurança e performance previsível.



# Testes e Validação

O sistema foi testado em ambiente real, com receção de comandos via UART (terminal série) e via LoRa (entre master e slave). A transmissão e resposta dos sensores foram validadas através de prints via UART e logs de debug. O modelo de execução garante determinismo temporal, baixa latência de resposta e capacidade de expansão com novos comandos.



# Arquitetura de Comunicação: Satélite ↔ Estação Base

O projeto foi concebido como um sistema distribuído composto por dois módulos principais:

Nó Satélite (Embarcado STM32F767)
Responsável pela aquisição de dados sensoriais (IMU, magnetómetro, pressão, GPS) e execução local de comandos recebidos remotamente. Este nó executa um sistema RTOS completo, com suporte a múltiplas tarefas, buffers circulares, filas de mensagens e gestão de prioridades para garantir processamento eficiente e determinístico.

Estação Base (Ground Station)
Atua como terminal de comando remoto, enviando instruções e solicitando dados ao nó satélite. Pode também receber informações de estado e diagnóstico via comunicação RF. A estação pode ser implementada noutro microcontrolador, num Raspberry Pi ou num PC com terminal serial e transceptor LoRa acoplado.

Modos de Comunicação
A troca de dados entre os dois módulos ocorre por dois canais distintos, possibilitando redundância e flexibilidade:

  # UART Serial 
  Usada para testes locais, monitorização em bancada e debug. Comunicação bidirecional é feita por comandos formatados como strings delimitadas (e.g., cmd,targetID,commandID,parametros,...\r), sendo processadas    pela Uart_Parser_Task.
  
  # LoRa RF (SX1278)
  Utilizada como meio de comunicação sem fios entre o satélite e a estação base, simulando um cenário de operação remota como seria numa missão espacial ou ambiente de difícil acesso. O protocolo inclui campos     de identificação de origem e destino no payload, suportando endereçamento direto e broadcast.
  Cada mensagem é verificada no recetor e apenas processada se o destino for compatível com o ID local (MASTER_ID, SLAVE_ID, BROADCAST_ID).

Sincronização e Tolerância a Falhas
A lógica RTOS garante que cada interface (UART/LoRa) possa operar em paralelo, utilizando buffers e variáveis de controlo (complete, flag_receive) para evitar condições de corrida.

Em caso de falha numa interface, a outra pode continuar ativa sem comprometer a estabilidade do sistema.

É possível estender o protocolo para suportar reenvios, compressão ou verificação CRC de mensagens em futuras iterações.

