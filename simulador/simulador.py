import paho.mqtt.client as mqtt
import time
import random
from collections import deque

from secrets import broker, port, username, password

topico_vazao = "casa/agua/vazao"
topico_controle = "casa/agua/controle"

# Parâmetros de simulação
intervalo_pub = 5  # segundos
threshold = 0.5      # L/min
duracao_alerta = 600 # segundos (10 minutos)
max_leituras = duracao_alerta // intervalo_pub

leituras = deque(maxlen=max_leituras)

def on_connect(client, userdata, flags, reasonCode, properties=None):
    print(f"Conectado com código {reasonCode}")
    client.subscribe(topico_controle)

def on_message(client, userdata, msg):
    print(f"Mensagem recebida em {msg.topic}: {msg.payload.decode()}")
    if msg.payload.decode() == "FECHAR_VALVULA":
        print("[ATUADOR] Válvula acionada por 10 segundos.")
        time.sleep(10)  # Simula o fechamento da válvula por 10s
        print("[ATUADOR] Válvula desligada.")  # Ao finalizar, retorna ao loop principal para continuar publicando

        print("[ATUADOR] Válvula desligada.")

# Configuração do cliente MQTT
client = mqtt.Client(protocol=mqtt.MQTTv5, callback_api_version=mqtt.CallbackAPIVersion.VERSION2)
client.username_pw_set(username, password)
client.tls_set()
client.on_connect = on_connect
client.on_message = on_message

client.connect(broker, port)
client.loop_start()

try:
    while True:
        # Simula leitura do sensor de vazão
        vazao = round(random.uniform(0.2, 2.0), 2)
        leituras.append(vazao)

        # Publica leitura no broker
        payload = f'{{"vazao": {vazao}}}'
        client.publish(topico_vazao, payload)
        print(f"Publicado: {payload}")

        # Verifica condição de alerta
        if len(leituras) == max_leituras and all(v > threshold for v in leituras):
            print("[SISTEMA] Vazão acima do limite por 10 minutos: acionando válvula.")
            client.publish(topico_controle, "FECHAR_VALVULA")
            # reseta fila para evitar múltiplos disparos
            leituras.clear()

        time.sleep(intervalo_pub)

except KeyboardInterrupt:
    client.loop_stop()
    client.disconnect()