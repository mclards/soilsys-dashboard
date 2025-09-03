#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <WiFiManager.h>     // https://github.com/tzapu/WiFiManager
#include <HardwareSerial.h>  // For RS485 sensor (future use)

// HiveMQ Cloud
const char *mqtt_server = "c33fda18bbcd4c9998277dfe166c7c4a.s1.eu.hivemq.cloud";
const int mqtt_port = 8883; // TLS
const char *mqtt_user = "gwapooo";
const char *mqtt_pass = "_Mclards23";
const char *clientID = "SoilMon0002";

// Topics
const char *topicSprinkler = "Sprinkler";
const char *topicInterval = "Interval";
const char *topicRuntime = "Runtime";
const char *topicSprinklerLong = "SprinklerLong";
const char *topicTemperature = "Temperature";
const char *topicLastwill = "Lastwill";
const char *topicJSON = "SoilSystem"; // JSON for MIT App

// Objects
WiFiClientSecure espClient;
PubSubClient client(espClient);

// Global variables
bool pumpRunning = false;
bool manualMode = false;
unsigned long pumpStartTime = 0;
int pumpRuntime = 0;
int intervalSeconds = 0;

// Pins
int sensorPin = 34; // Analog soil moisture (temporary for testing)
int pumpPin = 2;    // Relay

// Task handles
TaskHandle_t mqttTaskHandle;
TaskHandle_t sensorTaskHandle;

// ================== Queue for MQTT messages ==================
struct MqttMessage {
  char topic[64];
  char payload[256];
};
QueueHandle_t mqttQueue;

// ================== WiFi setup with WiFiManager ==================
void setup_wifi() {
  WiFiManager wm;
  if (!wm.autoConnect("SoilSystem_Setup")) {
    Serial.println("Failed to connect, restarting...");
    ESP.restart();
  }
  Serial.println("WiFi connected: " + WiFi.SSID());
}

// ================== MQTT callback ==================
void callback(char *topic, byte *message, unsigned int length) {
  String msg;
  for (int i = 0; i < length; i++) msg += (char)message[i];

  if (String(topic) == topicSprinkler) {
    if (msg == "1" && !manualMode) {
      digitalWrite(pumpPin, HIGH);
      pumpRunning = true;
      pumpStartTime = millis();
      Serial.println("Pump ON (Runtime Mode)");
    }
  } else if (String(topic) == topicRuntime) {
    pumpRuntime = msg.toInt();
    Serial.printf("Runtime set: %d sec\n", pumpRuntime);
  } else if (String(topic) == topicInterval) {
    intervalSeconds = msg.toInt();
    Serial.printf("Interval set: %d sec\n", intervalSeconds);
  } else if (String(topic) == topicSprinklerLong) {
    if (msg == "1") {
      manualMode = true;
      digitalWrite(pumpPin, HIGH);
      pumpRunning = true;
      Serial.println("Pump ON (Manual Mode)");
    } else if (msg == "0") {
      manualMode = false;
      digitalWrite(pumpPin, LOW);
      pumpRunning = false;
      Serial.println("Pump OFF (Manual Mode)");
    }
  }
}

// ================== Check runtime expiry OR soil threshold ==================
void checkPumpRuntime(int moistureValue) {
  if (pumpRunning && !manualMode) {
    unsigned long elapsed = (millis() - pumpStartTime) / 1000;

    if (pumpRuntime > 0 && elapsed >= pumpRuntime) {
      digitalWrite(pumpPin, LOW);
      pumpRunning = false;

      // ✅ Queue MQTT instead of direct publish
      MqttMessage msg;
      snprintf(msg.topic, sizeof(msg.topic), "%s", topicSprinkler);
      snprintf(msg.payload, sizeof(msg.payload), "0");
      xQueueSend(mqttQueue, &msg, portMAX_DELAY);

      Serial.println("Pump OFF (Runtime complete)");
    }

    if (moistureValue >= 85) {
      digitalWrite(pumpPin, LOW);
      pumpRunning = false;

      // ✅ Queue MQTT instead of direct publish
      MqttMessage msg;
      snprintf(msg.topic, sizeof(msg.topic), "%s", topicSprinkler);
      snprintf(msg.payload, sizeof(msg.payload), "0");
      xQueueSend(mqttQueue, &msg, portMAX_DELAY);

      Serial.println("Pump OFF (Soil reached 85% wetness)");
    }
  }
}

// ================== MQTT reconnect with Lastwill ==================
void reconnect() {
  while (!client.connected()) {
    if (client.connect(clientID, mqtt_user, mqtt_pass, topicLastwill, 0, true, "System Disconnected")) {
      Serial.println("Connected to MQTT");
      client.publish(topicLastwill, "System Online", true);

      client.subscribe(topicSprinkler);
      client.subscribe(topicRuntime);
      client.subscribe(topicInterval);
      client.subscribe(topicSprinklerLong);
    } else {
      Serial.println("MQTT reconnect failed, retrying...");
      delay(2000);
    }
  }
}

// ================== Task 1: MQTT handling ==================
void mqttTask(void *pvParameters) {
  MqttMessage msg;
  for (;;) {
    if (!client.connected()) reconnect();
    client.loop();

    if (xQueueReceive(mqttQueue, &msg, 0) == pdTRUE) {
      client.publish(msg.topic, msg.payload, true);
      Serial.printf("MQTT Published → %s : %s\n", msg.topic, msg.payload);
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

// ================== For now: Simulated temperature ==================
float readTemperatureSimulated() {
  return 20.0 + (analogRead(sensorPin) / 4095.0) * 10.0; // ~20–30°C
}

// ================== Task 2: Sensor reading & publishing ==================
void sensorTask(void *pvParameters) {
  for (;;) {
    int moistureValue = (int)((analogRead(sensorPin) / 4095.0) * 100);
    float temperatureValue = readTemperatureSimulated();

    MqttMessage msg;

    snprintf(msg.topic, sizeof(msg.topic), "%s", topicJSON);
    snprintf(msg.payload, sizeof(msg.payload),
             "{\"moisture\":%d,\"temperature\":%.2f,\"runtime\":%d,\"interval\":%d,\"pumpRunning\":%d}",
             moistureValue, temperatureValue, pumpRuntime, intervalSeconds, pumpRunning);
    xQueueSend(mqttQueue, &msg, portMAX_DELAY);

    snprintf(msg.topic, sizeof(msg.topic), "%s", topicTemperature);
    snprintf(msg.payload, sizeof(msg.payload), "%.2f", temperatureValue);
    xQueueSend(mqttQueue, &msg, portMAX_DELAY);

    snprintf(msg.topic, sizeof(msg.topic), "%s", topicLastwill);
    snprintf(msg.payload, sizeof(msg.payload), "%s", "System Online");
    xQueueSend(mqttQueue, &msg, portMAX_DELAY);

    checkPumpRuntime(moistureValue);

    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

// ================== Setup ==================
void setup() {
  Serial.begin(115200);
  pinMode(pumpPin, OUTPUT);
  digitalWrite(pumpPin, LOW);

  setup_wifi();

  espClient.setInsecure();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  client.setKeepAlive(10);

  mqttQueue = xQueueCreate(10, sizeof(MqttMessage));
  if (mqttQueue == NULL) {
    Serial.println("❌ Failed to create MQTT queue!");
    ESP.restart();
  }

  xTaskCreatePinnedToCore(mqttTask, "MQTT Task", 4096, NULL, 1, &mqttTaskHandle, 0);
  xTaskCreatePinnedToCore(sensorTask, "Sensor Task", 4096, NULL, 1, &sensorTaskHandle, 1);
}

// ================== Loop ==================
void loop() {
  // nothing here, tasks handle everything
}
