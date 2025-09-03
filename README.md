# 🌱 Soil Moisture Dashboard

The **Soil Moisture Dashboard** is an IoT project for **real-time soil monitoring and smart sprinkler control**.
It works in **two ways**:

1. 📱 As a **Mobile App (APK)** – installable directly on Android phones.
2. 🌐 As a **Progressive Web App (PWA)** – installable on any device (desktop, Android, iOS) through the browser.

Both versions connect to an **ESP32 microcontroller** via an **MQTT broker** to fetch soil and temperature readings, and to control the sprinkler remotely.

---

## 🚀 Features

* 📊 **Soil moisture gauge** with smooth gradient (dry → wet)
* 🌡️ **Temperature bar display**
* ⚡ **Sprinkler control**: toggle ON/OFF or hold with long press
* ⏱️ **Custom runtime setting** for sprinkler activation
* 🔔 **System & MQTT connection indicators**
* 📱 **Two deployment options**:

  * Native-like **Mobile App (APK)**
  * Installable **Web Dashboard (PWA)**
* 🌐 Lightweight hosting via **GitHub Pages**

---

## 🛠️ Technology Stack

* **Hardware:** ESP32 (sensor readings + sprinkler control)
* **App:** HTML, CSS, JavaScript
* **Communication:** MQTT.js over WebSocket
* **Broker:** HiveMQ Cloud
* **Deployment:**

  * Mobile App – packaged as **APK**
  * Web App – **PWA** with Service Worker + Manifest

---

## 🔄 How It Works

1. **ESP32 Microcontroller**

   * Reads soil moisture & temperature
   * Publishes data to the **MQTT broker**
   * Listens for commands to control the **sprinkler**

2. **MQTT Broker**

   * Middle layer between ESP32 and app (message exchange)

3. **Dashboard (Mobile App or PWA)**

   * Displays real-time sensor data
   * Sends sprinkler commands (ON/OFF, runtime, hold)

---

## 📷 Screenshots

<p align="center">
  <img src="https://github.com/user-attachments/assets/32e81c14-8fe5-42d2-9e2a-d27e03efa926" alt="Mobile App (APK)" width="250" />
  &nbsp;&nbsp;&nbsp;
  <img src="https://github.com/user-attachments/assets/ffdb51e4-82c2-4ee4-962f-be2bcfffa8f8" alt="Web Dashboard (Desktop)" width="600" />
  &nbsp;&nbsp;&nbsp;
  <img src="https://github.com/user-attachments/assets/f2d54dc7-aeec-4afc-a4b1-a28ba8f0bb1d" alt="Web Dashboard (Mobile PWA)" width="250" />
</p>

---

## 📦 Installation

### 📱 Mobile App (APK)

* Download and install:
  👉 [SoilSysApp.apk](https://github.com/mclards/soilsys-dashboard/releases/download/v1.0.0/SoilSysApp.apk)

### 🌐 Web Dashboard (PWA)

1. Open in your browser:
   👉 [SoilSys Dashboard](https://mclards.github.io/soilsys-dashboard/)

2. Install prompt:

   * On **Android/iOS** → *Add to Home Screen*
   * On **Desktop (Chrome/Edge)** → *Install as App*

---

## 👨‍💻 Creator  

<p align="center">
  <img src="https://avatars.githubusercontent.com/u/00000000?v=4" alt="Clariden Montaño" width="150" style="border-radius:50%;" />
</p>

<p align="center">
  <b>Engr. Clariden Montaño</b><br/>
  Licensed Electrical Engineer ⚡<br/>
  IoT & Arduino Developer 🤖<br/>
  Web Programming Enthusiast 💻
</p>

---

📌 **Interests:**  
- IoT systems (ESP32, Arduino, MQTT)  
- Electricity & Electronics  
- Web technologies (HTML, CSS, JavaScript)  

---
