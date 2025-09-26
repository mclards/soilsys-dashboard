# 🌱 Soil Moisture Dashboard (IoT Project)

The **Soil Moisture Dashboard** is an **Internet of Things (IoT)** project for **real-time soil monitoring and smart sprinkler control**.  
At its core is an **ESP32 microcontroller** connected to soil and temperature sensors, communicating through the **MQTT protocol** (a lightweight IoT messaging system).  

You can access the system in three ways:  
1. 📱 As a **Mobile App (APK)** – installable directly on Android phones.  
2. 💻 As a **Windows Executable (.exe)** – installable desktop program.  
3. 🌐 As a **Progressive Web App (PWA)** – accessible through any modern browser.  

---

## 📥 Releases

- 📱 **Android App (APK):** [SoilSysApp.apk](https://github.com/mclards/soilsys-dashboard/releases/download/v5.0/SoilSysDashboard.apk)
- 💻 **Windows Executable (.exe):** [SoilSysDashboard.Setup.exe](https://github.com/mclards/soilsys-dashboard/releases/download/v5.1/SoilSysDashboard.Setup.exe)  
- 🌐 **Web Dashboard (PWA):** [SoilSys Dashboard](https://mclards.github.io/soilsys-dashboard/)  

---

## 🚀 Features

### 📊 IoT Dashboard Display
- **Soil Moisture Gauge** → live % reading from sensor  
- **Temperature Bar** → real-time soil temperature (°C)  
- **System Status** → ESP32 device online/offline  
- **Server Status** → IoT broker connected/disconnected  
- **Sprinkler Status** → Pump ON / OFF feedback  
- **Runtime Display** → how long the pump runs (sec)  
- **Threshold Value** → soil moisture % where watering stops  
- **Client ID** → identifies each connected client in the IoT network  

### 🎛 Remote Controls (via App / Dashboard)
- **Runtime Input** → set pump runtime (seconds)  
- **Threshold Input** → set moisture stop % (1–100%)  
- **Sprinkler Button**  
  - Short press → toggle pump ON/OFF (auto mode)  
  - Long hold → manual override (keeps ON while held)  

### 🔘 Local Controls (on ESP32 Device Button)
- **1 click** → toggle pump (auto mode)  
- **Long press** → enable/disable manual override  
- **5 quick clicks** → reset WiFi credentials (ESP32 enters AP setup mode)  

---

## 🛠️ IoT Technology Stack

- **Hardware:** ESP32 microcontroller with soil & temperature sensors  
- **Connectivity:** WiFi-enabled ESP32 → MQTT broker  
- **Communication:** MQTT (Message Queuing Telemetry Transport) → lightweight IoT messaging protocol  
- **Apps:** HTML, CSS, JavaScript dashboards (PWA, APK, Windows .exe)  
- **Broker Service:** HiveMQ Cloud (IoT message exchange)  
- **Deployment:**  
  - Android App (**APK**)  
  - Windows Desktop App (**.exe**)  
  - Installable **Web Dashboard (PWA)** via GitHub Pages  

---

## 🔄 How It Works (IoT Flow)

1. **ESP32 Device (Edge IoT Node)**  
   - Reads soil moisture & temperature from sensors  
   - Publishes data to the **IoT broker (MQTT)**  
   - Listens for commands to control the **sprinkler**  

2. **IoT Broker (MQTT)**  
   - Acts as the central hub for messaging  
   - Relays data from ESP32 → Apps, and commands back to ESP32  

3. **User Dashboard (App / Web / PC)**  
   - Subscribes to real-time data streams  
   - Sends commands to control the sprinkler remotely  

---

## 📦 Installation

### 📱 Mobile App (APK)
👉 [Download SoilSysApp.apk](https://github.com/mclards/soilsys-dashboard/releases/download/v1.0.0/SoilSysApp.apk)  

### 💻 Windows Executable
👉 [Download SoilSysDashboard.Setup.exe](https://github.com/mclards/soilsys-dashboard/releases/download/v5.1/SoilSysDashboard.Setup.exe)  

### 🌐 Web Dashboard (PWA)
1. Open in your browser:  
   👉 [SoilSys Dashboard](https://mclards.github.io/soilsys-dashboard/)  
2. Install prompt:  
   - On **Android/iOS** → *Add to Home Screen*  
   - On **Desktop (Chrome/Edge)** → *Install as App*  

---

## 📷 Screenshots

<p align="center">
  <img src="https://github.com/user-attachments/assets/32e81c14-8fe5-42d2-9e2a-d27e03efa926" alt="Mobile App (APK)" width="250" />
  &nbsp;&nbsp;&nbsp;
  <img src="https://github.com/user-attachments/assets/f2d54dc7-aeec-4afc-a4b1-a28ba8f0bb1d" alt="Web Dashboard (Mobile PWA)" width="250" />
   &nbsp;&nbsp;&nbsp;
  <img src="https://github.com/user-attachments/assets/ffdb51e4-82c2-4ee4-962f-be2bcfffa8f8" alt="Web Dashboard (Desktop)" width="600" />
</p>

---

## 👨‍💻 Creator  

<p align="center">
  <img src="https://avatars.githubusercontent.com/u/00000000?v=4" alt="Clariden Montaño" width="150" style="border-radius:50%;" />
</p>

<p align="center">
  <b>Engr. Clariden Montaño</b><br/>
  Licensed Electrical Engineer ⚡<br/>
  IoT & Arduino Developer 🤖<br/>
  Web Programming Enthusiast 💻<br/>
  <a href="https://www.facebook.com/engr.clard">📘 Facebook Profile</a>
</p>

---
