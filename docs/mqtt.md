# Documentação do Protocolo MQTT

## Visão Geral

O sistema utiliza o protocolo MQTT para comunicação entre sensores, atuadores e o sistema de monitoramento. O MQTT é um protocolo leve, baseado em TCP/IP, ideal para aplicações IoT.

## Broker

- **Broker recomendado:** HiveMQ (público ou privado)
- **Porta:** 8883 (TLS) ou 1883 (sem TLS)

## Tópicos Utilizados

- `casa/agua/vazao`: Leituras de vazão publicadas pelo sensor.
- `casa/agua/controle`: Comandos para o atuador (relé/válvula).

## Payloads

### Leituras de Vazão

Enviadas em formato JSON:
```json
{
  "vazao": 1.23
}
```
- **vazao:** Valor da vazão em L/min.

### Comando de Controle

Enviado como texto simples:
- `"FECHAR_VALVULA"`: Aciona o relé para fechar a válvula.

## Fluxo de Comunicação

1. O sensor publica leituras periódicas em `casa/agua/vazao`.
2. O sistema monitora as leituras. Se a vazão exceder o limite por tempo prolongado, publica `"FECHAR_VALVULA"` em `casa/agua/controle`.
3. O atuador escuta o tópico de controle e aciona a válvula conforme o comando.

## Segurança

- Recomenda-se uso de autenticação (usuário/senha) e TLS para comunicação segura.

## Bibliotecas

- **Python:** paho-mqtt
- **NodeMCU/ESP8266:** PubSubClient (C++/Arduino)

## Exemplo de Configuração (Python)

```python
import paho.mqtt.client as mqtt

client = mqtt.Client()
client.username_pw_set("usuario", "senha")
client.tls_set()
client.connect("broker.hivemq.com", 8883)
```
