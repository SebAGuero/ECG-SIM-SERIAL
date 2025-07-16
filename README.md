# 🫀 ESP32 ECG Simulator with Pathologies

Este proyecto implementa un **simulador de señales de ECG** sobre un ESP32 utilizando PWM. Permite simular 10 condiciones cardíacas distintas y controlar la frecuencia de latido (BPM) por comandos seriales.

---

## ⚙️ Características

- ✔️ Generación de señal ECG por PWM en GPIO2
- ✔️ Simulación de 10 patologías cardíacas comunes
- ✔️ Frecuencia cardíaca configurable (30–180 bpm)
- ✔️ Menú interactivo por puerto serie
- ✔️ Código organizado y fácilmente extensible
- ✔️ Ideal para pruebas de monitores médicos o prácticas docentes

---

## 📡 Conexión

Usar un **filtro RC pasa bajos** a la salida GPIO2 para obtener una señal analógica desde el PWM.
de C=1uF y R=5k6
