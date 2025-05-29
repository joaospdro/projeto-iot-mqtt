# Sistema IoT de Monitoramento e Prevenção de Desperdício de Água

Este projeto simula um sistema IoT para detecção de vazamentos de água usando ESP32, MQTT (HiveMQ Cloud), potenciômetro (sensor de vazão) e relé (atuador de válvula), totalmente integrado ao simulador Wokwi.

## Estrutura

- `sketch.ino`: Código principal para ESP32 (Arduino).
- `diagram.json`: Diagrama de montagem para Wokwi.
- `secrets.h`: Arquivo de credenciais e tópicos MQTT.
- `docs/mqtt.md`: Documentação detalhada do protocolo MQTT utilizado.

## Como rodar no Wokwi

1. Acesse [Wokwi](https://wokwi.com/).
2. Importe os arquivos `sketch.ino`, `diagram.json` e `secrets.h`.
3. O sistema conecta ao WiFi Wokwi-GUEST e ao broker MQTT HiveMQ Cloud.
4. O potenciômetro simula a vazão de água (0–3 L/min).
5. O relé simula a válvula de fechamento.

## Tópicos MQTT

- `agua/vazao`: Publica leituras de vazão (JSON).
- `agua/alerta`: Publica alertas de vazamento detectado/prolongado.
- `agua/comando`: Recebe comandos para abrir/fechar válvula e publica status.

## Comandos aceitos via MQTT

- `"FECHAR_VALVULA"` ou mensagem contendo `"FECHAR"`: Fecha a válvula.
- `"ABRIR_VALVULA"` ou mensagem contendo `"ABRIR"`: Abre a válvula.
- `"STATUS"`: Publica status atual (vazão, estado da válvula, vazamento).

## Observações

- O sistema publica a vazão a cada 5 segundos.
- Se a vazão permanecer acima de 0.5 L/min por 10 minutos, a válvula é fechada automaticamente.
- O código aceita certificados inseguros para simulação no Wokwi.

## Documentação MQTT

Veja detalhes dos tópicos, payloads e exemplos em [`docs/mqtt.md`](docs/mqtt.md).

## Diagrama

Veja o arquivo `diagram.json` para detalhes da montagem.