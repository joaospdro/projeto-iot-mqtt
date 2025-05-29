# Documentação do Protocolo MQTT

## Visão Geral

O sistema utiliza o protocolo MQTT para comunicação entre sensores, atuadores e o sistema de monitoramento. O MQTT é um protocolo leve, baseado em TCP/IP, ideal para aplicações IoT.

## Broker

- **Broker recomendado:** HiveMQ (público ou privado)
- **Porta:** 8883 (TLS) ou 1883 (sem TLS)

## Tópicos Utilizados

- `agua/vazao`: Leituras de vazão publicadas pelo sensor.
- `agua/alerta`: Alertas de vazamento detectado ou prolongado.
- `agua/comando`: Comandos para o atuador (relé/válvula) e status do sistema.

## Payloads

### Leituras de Vazão

Enviadas em formato JSON:
```json
{
  "vazao": 1.23
}
```
- **vazao:** Valor da vazão em L/min.

### Alertas

Enviados em formato JSON:
```json
{ "status": "VAZAMENTO_DETECTADO" }
{ "status": "VAZAMENTO_PROLONGADO" }
```

### Comandos

Enviados como texto simples ou JSON:
- `"FECHAR_VALVULA"` ou mensagem contendo `"FECHAR"`: Aciona o relé para fechar a válvula.
- `"ABRIR_VALVULA"` ou mensagem contendo `"ABRIR"`: Aciona o relé para abrir a válvula.
- `"STATUS"`: Solicita status atual do sistema.

#### Status publicado em resposta ao comando "STATUS":
```json
{
  "vazao": 1.23,
  "valvula": "ABERTA",
  "vazando": true
}
```

## Fluxo de Comunicação

1. O sensor publica leituras periódicas em `agua/vazao`.
2. O sistema monitora as leituras. Se a vazão exceder o limite por tempo prolongado, publica alerta em `agua/alerta` e comando em `agua/comando`.
3. O atuador escuta o tópico de comando e aciona a válvula conforme o comando recebido.

## Segurança

- Recomenda-se uso de autenticação (usuário/senha) e TLS para comunicação segura.

## Bibliotecas

- **ESP32/Arduino:** PubSubClient

## Exemplo de Configuração (ESP32/Arduino)

```cpp
#include <PubSubClient.h>
client.setServer("e3df973f3f1f43788538f171061f43a7.s1.eu.hivemq.cloud", 8883);
client.setCallback(mqttCallback);
client.connect("clientId", "usuario", "senha");
```
