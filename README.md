# 🌱 Soil Moisture Dashboard (IoT Project)

The **Soil Moisture Dashboard** is an **IoT-based smart irrigation system** for **real-time soil monitoring and automatic sprinkler control**.  
It uses an **ESP32 microcontroller** connected to soil and temperature sensors, communicating via the **MQTT protocol** for reliable IoT data exchange.

You can access the system in three ways:  
1. 📱 **Android App (APK)** – install directly on your phone  
2. 💻 **Windows App (.exe)** – install on your computer  
3. 🌐 **Web Dashboard (PWA)** – open on any modern browser  

---

## 📥 Downloads

- 📱 **Android App:** [SoilSysApp.apk](https://github.com/mclards/soilsys-dashboard/releases/download/v5.0/SOIL_MOISTURE_Original_EDITED.apk)  
- 💻 **Windows App:** [SoilSysDashboard.Setup.exe](https://github.com/mclards/soilsys-dashboard/releases/download/v5.1/SoilSysDashboard.Setup.exe)  
- 🌐 **Web Dashboard (PWA):** [SoilSys Dashboard](https://mclards.github.io/soilsys-dashboard/)  

---

## 🚀 Features

### 📊 Dashboard Display
- **Soil Moisture Gauge** → real-time percentage reading  
- **Temperature Bar** → live soil temperature (°C)  
- **System Status** → shows if ESP32 is online/offline  
- **Server Status** → MQTT broker connection status  
- **Sprinkler Status** → pump ON/OFF feedback  
- **Runtime Display** → how long the pump runs (in seconds)  
- **Upper Threshold (%max)** → moisture level to stop watering *(must be higher than Lo-Threshold)*  
- **Lower Threshold (%min)** → moisture level to start watering *(must be lower than Up-Threshold)*  
- **Client ID** → identifies each connected user/device

### 🎛 Remote Controls (App / Dashboard)
- **Runtime Input** → set pump run time (seconds)
- **Threshold (%max)** → set moisture level to stop watering *(must be higher than %min)*   
- **Threshold (%min)** → set moisture level to start watering *(must be lower than %max)*   
- **Sprinkler Button:**  
  - Short press → toggle pump (auto mode)  
  - Long press → manual override (keeps pump ON while held)  

### 🔘 Local Controls (ESP32 Button)
- **1 click** → toggle pump (auto mode)  
- **Long press** → enable/disable manual override  
- **5 quick clicks** → reset WiFi credentials (starts Access Point setup)  

---

## 🛠️ Technology Overview

- **Controller:** ESP32 with soil & temperature sensors  
- **Connectivity:** WiFi → MQTT broker (HiveMQ Cloud)  
- **Protocol:** MQTT (Message Queuing Telemetry Transport)  
- **Apps:** HTML, CSS, JavaScript dashboard (PWA, APK, Windows app)  
- **Deployment:**  
  - Android App (.apk)  
  - Windows App (.exe)  
  - Web Dashboard via GitHub Pages  

---

## 🔄 System Workflow

1. **ESP32 Device**
   - Reads soil moisture & temperature  
   - Publishes data to the MQTT broker  
   - Receives commands for the sprinkler  

2. **MQTT Broker**
   - Acts as a central hub for communication  
   - Relays data between the device and dashboard  

3. **User Dashboard**
   - Displays real-time data  
   - Sends commands to control the pump remotely  

---

## 📦 Installation

### 📱 Android App
1. Download and install:  
   👉 [SoilSysApp.apk](https://github.com/mclards/soilsys-dashboard/releases/download/v5.0/SOIL_MOISTURE_Original_EDITED.apk)

### 💻 Windows App
1. Download and run:  
   👉 [SoilSysDashboard.Setup.exe](https://github.com/mclards/soilsys-dashboard/releases/download/v5.1/SoilSysDashboard.Setup.exe)  

### 🌐 Web Dashboard (PWA)
1. Open in your browser:  
   👉 [SoilSys Dashboard](https://mclards.github.io/soilsys-dashboard/)  
2. Install prompt:  
   - On **mobile** → tap *Add to Home Screen*  
   - On **desktop** → click *Install as App* (Chrome/Edge)  

---

## 📷 Screenshots

<p align="center">
  <img src="https://github.com/user-attachments/assets/32e81c14-8fe5-42d2-9e2a-d27e03efa926" alt="Mobile App" width="250" />
  &nbsp;&nbsp;&nbsp;
  <img src="https://github.com/user-attachments/assets/f2d54dc7-aeec-4afc-a4b1-a28ba8f0bb1d" alt="Web Dashboard (Mobile)" width="250" />
  &nbsp;&nbsp;&nbsp;
  <img src="https://github.com/user-attachments/assets/ffdb51e4-82c2-4ee4-962f-be2bcfffa8f8" alt="Web Dashboard (Desktop)" width="600" />
</p>

---

## 👨‍💻 Developer

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
