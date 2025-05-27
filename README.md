# projeto-iot-mqtt

## Descrição

Este projeto implementa um sistema IoT para monitoramento e prevenção de desperdício de água, utilizando sensores de vazão, atuadores (relé) e comunicação via protocolo MQTT. O sistema detecta vazamentos prolongados e aciona automaticamente uma válvula para interromper o fluxo de água.

> **Nota:** Este código é um protótipo de simulação, não requer componentes físicos conectados. Todas as leituras e comandos são simulados em software.

## Funcionamento e Uso

1. O simulador (`simulador.py`) publica leituras simuladas de vazão de água a cada 5 segundos em um broker MQTT.
2. Se a vazão permanecer acima de 0,5 L/min por 10 minutos, o sistema publica um comando para acionar o atuador (relé), simulando o fechamento da válvula.
3. O sistema pode ser reproduzido executando o simulador em Python, com as credenciais do broker MQTT configuradas no arquivo `secrets.py`.

### Como executar

1. Instale as dependências:
   ```
   pip install paho-mqtt
   ```
2. Configure o arquivo `secrets.py` com as informações do broker MQTT.
3. Execute o simulador:
   ```
   python simulador/simulador.py
   ```

## Hardware Proposto

- **Plataforma de Prototipagem:** NodeMCU ESP8266 (Wi-Fi, 32-bit, 4MB Flash)
- **Sensor de Vazão:** YF-S201 (1–30 L/min, saída digital)
- **Atuador:** Módulo Relé 5V (controle de válvula)
- **Conexões:** Sensor conectado ao GPIO do NodeMCU; relé conectado para controle de válvula.
- **Alimentação:** 5V para sensor e relé.

## Documentação das Interfaces e Protocolos

- **Protocolo de Comunicação:** MQTT sobre TCP/IP (TLS opcional)
- **Broker:** HiveMQ (ou outro broker MQTT compatível)
- **Tópicos utilizados:**
  - `casa/agua/vazao` (publicação de leituras de vazão)
  - `casa/agua/controle` (comandos para o atuador)
- **Formato das mensagens:** JSON para leituras de vazão, texto simples para comandos.