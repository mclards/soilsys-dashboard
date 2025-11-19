#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <WiFiManager.h>
#include <ModbusMaster.h>
#include <EEPROM.h>
#include <OneButton.h>

const char* default_SSID = "SoilSys AP";
const char* default_PASS = "12345678";

const char* mqtt_server = "c33fda18bbcd4c9998277dfe166c7c4a.s1.eu.hivemq.cloud";
const int   mqtt_port = 8883;
const char* mqtt_user = "gwapooo";
const char* mqtt_pass = "_Mclards23";
const char* clientID = "SoilMon0002";

const char* topicSprinkler = "Sprinkler";
const char* topicRuntime = "Runtime";
const char* topicSprinklerLong = "SprinklerLong";
const char* topicTemperature = "Temperature";
const char* topicLastwill = "Lastwill";
const char* topicJSON = "SoilSystem";
const char* topicThreshold = "Threshold";
const char* topicMinThreshold = "MinThreshold";

WiFiClientSecure espClient;
PubSubClient client(espClient);

bool pumpRunning = false;
bool manualMode = false;
bool runtimeMode = false;
unsigned long pumpStartTime = 0;
int pumpRuntime = 5;
int threshold = 85;
int minThreshold = 25;
int bootCount = 0;

bool autoEnabled = true;
unsigned long lastManualOffTime = 0;
const unsigned long autoCooldown = 30000;

int pumpPin = 4;
int buttonPin = 13;
int mqttStatusPin = 2;

unsigned long waitSec = 60;

OneButton oneBtn(buttonPin, true, true);

#define RX2_PIN   16
#define TX2_PIN   17
#define RE_DE_PIN 14

ModbusMaster soilNode;

void preTransmission() { digitalWrite(RE_DE_PIN, HIGH); delayMicroseconds(10); }
void postTransmission() { digitalWrite(RE_DE_PIN, LOW); delayMicroseconds(10); }

void initRS485() {
    Serial2.begin(4800, SERIAL_8N1, RX2_PIN, TX2_PIN);
    pinMode(RE_DE_PIN, OUTPUT);
    digitalWrite(RE_DE_PIN, LOW);
    soilNode.begin(1, Serial2);
    soilNode.preTransmission(preTransmission);
    soilNode.postTransmission(postTransmission);
}

bool readSoilSensor(float& humidity, float& temperature) {
    uint8_t result = soilNode.readHoldingRegisters(0x0000, 2);
    if (result == soilNode.ku8MBSuccess) {
        humidity = soilNode.getResponseBuffer(0) * 0.1;
        temperature = (int16_t)soilNode.getResponseBuffer(1) * 0.1;
        return true;
    }
    return false;
}

#define EEPROM_SIZE     32
#define ADDR_RUNTIME    0
#define ADDR_THRESHOLD  2
#define ADDR_MINTHRESHOLD 4
#define ADDR_BOOTCOUNT  6

void setBootFlag(int value) {
    EEPROM.begin(EEPROM_SIZE);
    EEPROM.put(ADDR_BOOTCOUNT, value);
    EEPROM.commit();
    EEPROM.end();
    bootCount = value;
}

int getBootFlag() {
    EEPROM.begin(EEPROM_SIZE);
    EEPROM.get(ADDR_BOOTCOUNT, bootCount);
    EEPROM.end();
    if (bootCount == 0xFFFF) bootCount = 0;
    return bootCount;
}

void ledBlink(long offTime, long onTime) {
    digitalWrite(mqttStatusPin, HIGH); delay(onTime);
    digitalWrite(mqttStatusPin, LOW);  delay(offTime);
    digitalWrite(mqttStatusPin, HIGH); delay(onTime);
    digitalWrite(mqttStatusPin, LOW);  delay(offTime);
    digitalWrite(mqttStatusPin, HIGH); delay(onTime);
    digitalWrite(mqttStatusPin, LOW);  delay(offTime);
}

void ledEEPROM(long offTime, long onTime) {
    digitalWrite(mqttStatusPin, LOW); delay(offTime);
    digitalWrite(mqttStatusPin, HIGH); delay(onTime);
    digitalWrite(mqttStatusPin, LOW); delay(offTime);
    digitalWrite(mqttStatusPin, HIGH); delay(onTime);
    digitalWrite(mqttStatusPin, LOW); delay(offTime);
    digitalWrite(mqttStatusPin, HIGH);
}

void loadSettings() {
    EEPROM.begin(EEPROM_SIZE);
    uint16_t r, t, m;
    EEPROM.get(ADDR_RUNTIME, r);
    EEPROM.get(ADDR_THRESHOLD, t);
    EEPROM.get(ADDR_MINTHRESHOLD, m);
    EEPROM.get(ADDR_BOOTCOUNT, bootCount);
    if (r == 0xFFFF || r == 0) r = 5;
    if (t == 0xFFFF || t == 0) t = 85;
    if (m == 0xFFFF || m == 0) m = 25;
    pumpRuntime = r;
    threshold = t;
    minThreshold = m;
    EEPROM.end();
    ledEEPROM(80, 80);
}

void saveRuntime(int v) {
    pumpRuntime = v;
    EEPROM.begin(EEPROM_SIZE);
    EEPROM.put(ADDR_RUNTIME, (uint16_t)v);
    EEPROM.commit();
    EEPROM.end();
    ledEEPROM(80, 80);
}

void saveThreshold(int v) {
    threshold = v;
    EEPROM.begin(EEPROM_SIZE);
    EEPROM.put(ADDR_THRESHOLD, (uint16_t)v);
    EEPROM.commit();
    EEPROM.end();
    ledEEPROM(80, 80);
}

void saveMinThreshold(int v) {
    minThreshold = v;
    EEPROM.begin(EEPROM_SIZE);
    EEPROM.put(ADDR_MINTHRESHOLD, (uint16_t)v);
    EEPROM.commit();
    EEPROM.end();
    ledEEPROM(80, 80);
}

struct MqttMessage {
    char topic[64];
    char payload[256];
};
QueueHandle_t mqttQueueTelemetry;
QueueHandle_t mqttQueueControl;

void setup_savedWifi() {
    unsigned long waitMs = waitSec * 1000UL;
    WiFi.begin();
    unsigned long startAttempt = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startAttempt < waitMs) {
        ledBlink(200, 200);
    }
}

void setup_newWifi() {
    WiFiManager wm;
    wm.resetSettings();
    setBootFlag(0);
    delay(200);
    wm.setConfigPortalTimeout(waitSec);
    wm.setConnectTimeout(20);
    wm.startConfigPortal(default_SSID, default_PASS);
    ledBlink(100, 100);
}

void callback(char* topic, byte* message, unsigned int length) {
    String msg;
    for (unsigned int i = 0; i < length; i++) msg += (char)message[i];

    if (String(topic) == topicSprinkler) {
        if (manualMode) return;
        if (msg == "1") {
            if (!pumpRunning) {
                digitalWrite(pumpPin, LOW);
                pumpRunning = true;
                runtimeMode = true;
                pumpStartTime = millis();
            }
        } else if (msg == "0") {
            if (pumpRunning && runtimeMode) {
                digitalWrite(pumpPin, HIGH);
                pumpRunning = false;
                runtimeMode = false;
            }
        }
    } else if (String(topic) == topicRuntime) {
        int v = msg.toInt();
        if (v > 0) saveRuntime(v);
    } else if (String(topic) == topicSprinklerLong) {
        if (msg == "1") {
            manualMode = true;
            pumpRunning = true;
            digitalWrite(pumpPin, LOW);
        } else {
            manualMode = false;
            pumpRunning = false;
            digitalWrite(pumpPin, HIGH);
        }
    } else if (String(topic) == topicThreshold) {
        int t = msg.toInt();
        if (t >= 1 && t <= 100) saveThreshold(t);
    } else if (String(topic) == topicMinThreshold) {
        int t = msg.toInt();
        if (t >= 1 && t < threshold) saveMinThreshold(t);
    }
}

void checkPumpRuntime(float moistureValue) {
    if (pumpRunning && !manualMode && runtimeMode) {
        unsigned long runtimeMs = pumpRuntime * 1000UL;
        if (millis() - pumpStartTime >= runtimeMs) {
            digitalWrite(pumpPin, HIGH);
            pumpRunning = false;
            runtimeMode = false;
            if (WiFi.isConnected() && client.connected()) {
                MqttMessage msg;
                snprintf(msg.topic, sizeof(msg.topic), "%s", topicSprinkler);
                snprintf(msg.payload, sizeof(msg.payload), "0");
                xQueueOverwrite(mqttQueueControl, &msg);
            }
        }
    }

    if (pumpRunning && !manualMode && !runtimeMode) {
        if (moistureValue >= threshold) {
            digitalWrite(pumpPin, HIGH);
            pumpRunning = false;
            if (WiFi.isConnected() && client.connected()) {
                MqttMessage msg;
                snprintf(msg.topic, sizeof(msg.topic), "%s", topicSprinkler);
                snprintf(msg.payload, sizeof(msg.payload), "0");
                xQueueOverwrite(mqttQueueControl, &msg);
            }
        }
    }
}

bool reconnectOnce() {
    if (client.connected()) return true;
    bool ok = client.connect(clientID, mqtt_user, mqtt_pass, topicLastwill, 0, true, "System Disconnected");
    if (ok) {
        client.publish(topicLastwill, "System Online", true);
        client.subscribe(topicSprinkler);
        client.subscribe(topicRuntime);
        client.subscribe(topicSprinklerLong);
        client.subscribe(topicThreshold);
        client.subscribe(topicMinThreshold);
        digitalWrite(mqttStatusPin, HIGH);
    } else {
        ledBlink(200, 1000);
    }
    return ok;
}

void mqttTask(void* pvParameters) {
    (void)pvParameters;
    MqttMessage msg;
    unsigned long lastAttempt = 0;

    for (;;) {
        if (!client.connected()) {
            unsigned long now = millis();
            if (now - lastAttempt >= 10000) {
                lastAttempt = now;
                reconnectOnce();
            }
        } else {
            client.loop();
        }

        if (xQueueReceive(mqttQueueControl, &msg, 0) == pdTRUE) {
            if (WiFi.isConnected() && client.connected()) {
                client.publish(msg.topic, msg.payload, false);
            }
        }

        if (xQueueReceive(mqttQueueTelemetry, &msg, 0) == pdTRUE) {
            if (WiFi.isConnected() && client.connected()) {
                client.publish(msg.topic, msg.payload, false);
            }
        }

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void sensorTask(void* pvParameters) {
    (void)pvParameters;
    for (;;) {
        float humidity = 0, temperature = 0;
        if (readSoilSensor(humidity, temperature)) {
            if (WiFi.isConnected() && client.connected()) {
                MqttMessage msg;
                snprintf(msg.topic, sizeof(msg.topic), "%s", topicJSON);
                snprintf(msg.payload, sizeof(msg.payload),
                         "{\"moisture\":%.1f,\"temperature\":%.2f,\"runtime\":%d,\"pumpRunning\":%d,\"threshold\":%d,\"minThreshold\":%d}",
                         humidity, temperature, pumpRuntime, pumpRunning, threshold, minThreshold);
                xQueueSend(mqttQueueTelemetry, &msg, 0);

                snprintf(msg.topic, sizeof(msg.topic), "%s", topicTemperature);
                snprintf(msg.payload, sizeof(msg.payload), "%.2f", temperature);
                xQueueSend(mqttQueueTelemetry, &msg, 0);
            }

            if (!autoEnabled && (millis() - lastManualOffTime >= autoCooldown)) autoEnabled = true;

            if (autoEnabled && !pumpRunning && !manualMode && humidity <= minThreshold) {
                digitalWrite(pumpPin, LOW);
                pumpRunning = true;
                runtimeMode = false;
                pumpStartTime = millis();
                if (WiFi.isConnected() && client.connected()) {
                    MqttMessage msg;
                    snprintf(msg.topic, sizeof(msg.topic), "%s", topicSprinkler);
                    snprintf(msg.payload, sizeof(msg.payload), "1");
                    xQueueOverwrite(mqttQueueControl, &msg);
                }
            }
            checkPumpRuntime(humidity);
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void onClick() {
    if (manualMode) return;
    if (pumpRunning) {
        digitalWrite(pumpPin, HIGH);
        pumpRunning = false;
        if (WiFi.isConnected() && client.connected()) {
            MqttMessage msg = { "Sprinkler", "0" };
            xQueueOverwrite(mqttQueueControl, &msg);
        }
        return;
    }

    digitalWrite(pumpPin, LOW);
    pumpRunning = true;
    runtimeMode = true;
    pumpStartTime = millis();

    if (WiFi.isConnected() && client.connected()) {
        MqttMessage msg = { "Sprinkler", "1" };
        xQueueOverwrite(mqttQueueControl, &msg);
    }
}

void onLongPressStart() {
    if (!manualMode) {
        manualMode = true;
        autoEnabled = false;
        digitalWrite(pumpPin, LOW);
        pumpRunning = true;
        if (WiFi.isConnected() && client.connected()) {
            MqttMessage msg = { "SprinklerLong", "1" };
            xQueueOverwrite(mqttQueueControl, &msg);
        }
    } else {
        manualMode = false;
        digitalWrite(pumpPin, HIGH);
        pumpRunning = false;
        lastManualOffTime = millis();
        if (WiFi.isConnected() && client.connected()) {
            MqttMessage msg = { "SprinklerLong", "0" };
            xQueueOverwrite(mqttQueueControl, &msg);
        }
    }
}

void onMultiClick() {
    if (oneBtn.getNumberClicks() == 5) {
        setBootFlag(1);
        ledBlink(100, 100);
        ESP.restart();
    }
}

void buttonTask(void* pvParameters) {
    (void)pvParameters;
    oneBtn.setPressMs(800);
    oneBtn.attachClick(onClick);
    oneBtn.attachLongPressStart(onLongPressStart);
    oneBtn.attachMultiClick(onMultiClick);

    for (;;) {
        oneBtn.tick();
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void setup() {
    pinMode(pumpPin, OUTPUT);
    digitalWrite(pumpPin, HIGH);
    pinMode(buttonPin, INPUT_PULLUP);
    pinMode(mqttStatusPin, OUTPUT);
    digitalWrite(mqttStatusPin, LOW);

    getBootFlag();
    if (bootCount == 0) setup_savedWifi();
    else setup_newWifi();

    loadSettings();
    initRS485();

    espClient.setInsecure();
    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(callback);
    client.setKeepAlive(30);

    mqttQueueTelemetry = xQueueCreate(10, sizeof(MqttMessage));
    mqttQueueControl = xQueueCreate(1, sizeof(MqttMessage));
    if (!mqttQueueTelemetry || !mqttQueueControl) ESP.restart();

    if (client.connect(clientID, mqtt_user, mqtt_pass, topicLastwill, 0, true, "System Disconnected")) {
        client.publish(topicSprinkler, "0", true);
        client.publish(topicSprinklerLong, "0", true);
        client.publish(topicLastwill, "System Online", true);
        delay(200);
        client.subscribe(topicSprinkler);
        client.subscribe(topicRuntime);
        client.subscribe(topicSprinklerLong);
        client.subscribe(topicThreshold);
        client.subscribe(topicMinThreshold);
        digitalWrite(mqttStatusPin, HIGH);
    }

    xTaskCreatePinnedToCore(mqttTask, "MQTT Task", 4096, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(sensorTask, "Sensor Task", 4096, NULL, 1, NULL, 1);
    xTaskCreatePinnedToCore(buttonTask, "Button Task", 2048, NULL, 1, NULL, 1);
}

void loop() {}
