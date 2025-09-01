# ☀️ IoTricity_S2 Solar Panel Monitor

![IoT](https://img.shields.io/badge/IoT-Project-blue?style=for-the-badge)
![Hackathon](https://img.shields.io/badge/IoTricity-S2-orange?style=for-the-badge)
![Cloud](https://img.shields.io/badge/Cloud-Integrated-brightgreen?style=for-the-badge)
![ML](https://img.shields.io/badge/Machine-Learning-purple?style=for-the-badge)
![Arduino](https://img.shields.io/badge/Arduino-C++-lightblue?style=for-the-badge)

<p align="center">
  <a href="https://github.com/rajdeep13-coder/iotricity-solar">
    <img src="./data/Solar Panel Monitor.png" />
  </a>
</p>

---

## 📌 Overview
**IoTricity Solar Panel Monitor** is an IoT + ML powered project designed for smart solar tracking.  
It monitors **light intensity, panel orientation, and environmental parameters** to automatically optimize the tilt of solar panels for maximum efficiency.  

We implemented **two approaches**:
- 🔹 **Rule-Based Local Tracking** → Uses LDR sensors and servo logic (Arduino).  
- 🔹 **Cloud-Integrated ML Tracking** → Uses ESP32 + FastAPI ML API to predict the best panel angles in real time.  

---

## ✨ Features
- Real-time LDR sensor data acquisition.  
- Dual-mode: **Rule-based fallback** & **ML-based cloud tracking**.  
- Servo-based alignment of solar panels.  
- Cloud-hosted **FastAPI** model inference.  
- IoT dashboard ready (ThingSpeak integration possible).  
- Sweeping fallback in low-light conditions.  

---

## 🛠️ Tech Stack

**Hardware:**  
- ESP32 / Arduino UNO + Wi-Fi module  
- LDR Sensors × 4  
- Servo Motors × 2  
- Breadboard, Jumper Wires  

**Software:**  
- Arduino IDE (C++)  
- FastAPI (Python)  
- Scikit-Learn (ML models)  
- Joblib (model storage)  
- Cloud Deployment → Render 
- IoT Dashboard → ThingSpeak 

---

## 📊 SWOT Analysis  

**Strengths**  
- Dual tracking logic (rule-based + ML-based).  
- Low-cost prototype (~₹1500–2200).  
- Cloud integration for remote monitoring.  

**Weaknesses**  
- Relies on Wi-Fi availability for ML mode.  
- Prototype scale, not yet industrial grade.  
- Accuracy limited by LDRs (could use better sensors).  

**Opportunities**  
- Scale for **large solar farms**.  
- Integrate with **predictive weather APIs**.  
- Use **AI-based energy optimization** for smart grids.  

**Threats**  
- Market competition with commercial trackers.  
- Harsh weather can damage low-cost sensors.  
- Dependency on cloud costs for scaling.  

---

## 🔮 Future Scope
- Upgrade LDRs → **Pyranometer / Lux sensors** for higher accuracy.  
- Add **battery storage monitoring** and **MPPT logic**.  
- Integrate **edge ML models** (run directly on ESP32).  
- Develop **mobile app dashboard** for users.  
- Scale system for **smart city solar farms**.  

---

## ⚡ How It Works
1. LDR sensors detect light levels from four quadrants.  
2. Data is sent to:  
   - **Local Logic (Arduino)** → compares differences → moves servos.  
   - **Cloud ML API (ESP32)** → predicts best angles → servos align.  
3. IoT dashboard shows real-time panel data.  

---

📢 Developed for IoTricity-S2 Hackathon by Team Loading…