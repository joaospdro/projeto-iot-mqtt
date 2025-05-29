#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include "secrets.h"

// Configurações de rede (Wokwi)
const char* ssid = SECRET_WIFI_SSID;
const char* password = SECRET_WIFI_PASSWORD;

// Configurações MQTT - HiveMQ Cloud
const char* mqtt_server     = SECRET_MQTT_SERVER;
const int   mqtt_port       = SECRET_MQTT_PORT;
const char* mqtt_user       = SECRET_MQTT_USER;
const char* mqtt_password   = SECRET_MQTT_PASSWORD;
const char* mqtt_topic_vazao   = SECRET_MQTT_TOPIC_VAZAO;
const char* mqtt_topic_alerta  = SECRET_MQTT_TOPIC_ALERTA;
const char* mqtt_topic_comando = SECRET_MQTT_TOPIC_COMANDO;

// Pinos
const int POT_PIN    = 34;
const int RELAY_PIN  = 4;

// Controle de vazão e alertas
float vazaoAtual               = 0.0;
const float LIMIAR_VAZAO       = 0.5;         // L/min para detectar vazamento
const unsigned long TEMPO_ALERTA   = 600000;  // 10 minutos em ms

unsigned long lastPublish      = 0;
const unsigned long PUBLISH_INTERVAL = 5000;   // 5 segundos

bool vazando                   = false;
unsigned long vazamentoStart   = 0;

WiFiClientSecure espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  Serial.println("=== Sistema de Monitoramento de Água ===");

  pinMode(POT_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW); // Válvula inicialmente aberta

  setupWiFi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(mqttCallback);

  Serial.println("Sistema inicializado com sucesso!");
}

void loop() {
  if (!client.connected()) {
    reconnectMQTT();
  }
  client.loop();

  unsigned long now = millis();

  // Publicação de dados
  if (now - lastPublish >= PUBLISH_INTERVAL) {
    lastPublish = now;

    // Leitura do potenciômetro com suavização
    int potValue = analogRead(POT_PIN);
    float novaVazao = map(potValue, 0, 4095, 0, 300) / 100.0; // 0.00–3.00 L/min

    // Suavização da leitura (filtro passa-baixa simples)
    vazaoAtual = (vazaoAtual * 0.7) + (novaVazao * 0.3);

    // Publica vazão e checa vazamento
    publishVazao(vazaoAtual);
    checkLeak(now);

    // Debug no Serial
    Serial.printf("Vazão: %.2f L/min | Válvula: %s | Pot: %d\n",
                  vazaoAtual,
                  digitalRead(RELAY_PIN) ? "FECHADA" : "ABERTA",
                  potValue);
  }
}

void publishVazao(float v) {
  char msg[20];
  snprintf(msg, sizeof(msg), "{\"vazao\":%.2f}", v);

  if (client.publish(mqtt_topic_vazao, msg)) {
    Serial.printf("📡 Vazão publicada: %s\n", msg);
  } else {
    Serial.println("❌ Falha ao publicar vazão");
  }
}

void checkLeak(unsigned long now) {
  bool valvulaFechada = digitalRead(RELAY_PIN);

  if (vazaoAtual > LIMIAR_VAZAO && !valvulaFechada) {
    if (!vazando) {
      vazando = true;
      vazamentoStart = now;
      Serial.printf("⚠️ Vazamento iniciado - Vazão: %.2f L/min\n", vazaoAtual);

      // Publica alerta imediato
      client.publish(mqtt_topic_alerta, "{\"status\":\"VAZAMENTO_DETECTADO\"}");
    } else if (now - vazamentoStart >= TEMPO_ALERTA) {
      // Vazamento prolongado - fecha válvula
      client.publish(mqtt_topic_alerta, "{\"status\":\"VAZAMENTO_PROLONGADO\"}");
      fecharValvula();
      vazando = false;
    } else {
      // Mostra tempo restante
      unsigned long tempoRestante = (TEMPO_ALERTA - (now - vazamentoStart)) / 1000;
      Serial.printf("⏱️ Vazamento em curso - Restam %lu segundos\n", tempoRestante);
    }
  } else {
    if (vazando) {
      Serial.println("✅ Vazamento cessou");
    }
    vazando = false;
  }
}

void fecharValvula() {
  digitalWrite(RELAY_PIN, HIGH);
  client.publish(mqtt_topic_comando, "{\"acao\":\"VALVULA_FECHADA\"}");
  Serial.println("🔒 Válvula fechada automaticamente");
}

void abrirValvula() {
  digitalWrite(RELAY_PIN, LOW);
  client.publish(mqtt_topic_comando, "{\"acao\":\"VALVULA_ABERTA\"}");
  Serial.println("🔓 Válvula aberta");
}

// Configura e conecta WiFi
void setupWiFi() {
  delay(10);
  Serial.printf("Conectando ao WiFi '%s'...", ssid);
  WiFi.begin(ssid, password);

  int tentativas = 0;
  while (WiFi.status() != WL_CONNECTED && tentativas < 20) {
    delay(500);
    Serial.print(".");
    tentativas++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ WiFi conectado!");
    Serial.printf("📍 IP: %s\n", WiFi.localIP().toString().c_str());
    Serial.printf("📶 RSSI: %d dBm\n", WiFi.RSSI());
  } else {
    Serial.println("\n❌ Falha na conexão WiFi");
  }

  // Aceita qualquer certificado (simulação Wokwi)
  espClient.setInsecure();
}

// Conecta MQTT
void reconnectMQTT() {
  while (!client.connected()) {
    Serial.print("🔄 Conectando ao MQTT...");
    String clientId = "ESP32-MonitorAgua-" + String(random(0xffff), HEX);

    if (client.connect(clientId.c_str(), mqtt_user, mqtt_password)) {
      Serial.println("✅ MQTT conectado!");

      // Subscribe nos tópicos de comando
      if (client.subscribe(mqtt_topic_comando)) {
        Serial.printf("📨 Subscrito em: %s\n", mqtt_topic_comando);
      }

      // Publica status de conexão
      client.publish(mqtt_topic_comando, "{\"status\":\"SISTEMA_ONLINE\"}");

    } else {
      Serial.printf("❌ Falha MQTT, rc=%d. Tentando em 5s...\n", client.state());
      delay(5000);
    }
  }
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String msg = "";
  for (unsigned int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }

  Serial.printf("📥 Comando recebido [%s]: %s\n", topic, msg.c_str());

  if (msg == "FECHAR_VALVULA" || msg.indexOf("FECHAR") != -1) {
    fecharValvula();
  } else if (msg == "ABRIR_VALVULA" || msg.indexOf("ABRIR") != -1) {
    abrirValvula();
  } else if (msg == "STATUS") {
    char status[100];
    snprintf(status, sizeof(status),
             "{\"vazao\":%.2f,\"valvula\":\"%s\",\"vazando\":%s}",
             vazaoAtual,
             digitalRead(RELAY_PIN) ? "FECHADA" : "ABERTA",
             vazando ? "true" : "false");
    client.publish(mqtt_topic_comando, status);
  }
}
