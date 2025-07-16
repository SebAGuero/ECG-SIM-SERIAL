#include <Arduino.h>

// Parámetros PWM
const int pwmPin = 2;        // GPIO para salida PWM
const int channel = 0;
const int resolution = 8;    // 8 bits (0-255)
const int freqPWM = 20000;   // 20 kHz PWM para mejor filtrado

const int waveformLength = 500;
uint8_t ecgWaveform[waveformLength];

// Variables de control
int indexWav = 0;
unsigned long lastUpdate = 0;
int bpm = 60;
unsigned long tiempoMuestreo = 0;
unsigned long intervaloLatido = 0;
unsigned long duracionReposo = 0;
bool enReposo = false;
unsigned long inicioReposo = 0;
int patologia = 0;  // 0 a 9

// Prototipos
void generarECG(int pat);
float ondaP(float t);
float ondaQ(float t);
float ondaR(float t);
float ondaS(float t);
float ondaT(float t);

// Generadores de patologías
void generarPatologiaNormal();
void generarPatologiaArritmia();
void generarPatologiaBradicarida();
void generarPatologiaTaquicardia();
void generarPatologiaExtrasistolica();
void generarPatologiaFibrilacion();
void generarPatologiaFlutter();
void generarPatologiaBloqueoAV();
void generarPatologiaIsquemia();
void generarPatologiaInfarto();

void calcularDuracionReposo();
void mostrarMenu();
void procesarComando(String cmd);

void setup() {
  Serial.begin(115200);
  ledcSetup(channel, freqPWM, resolution);
  ledcAttachPin(pwmPin, channel);

  calcularDuracionReposo();
  generarECG(patologia);

  Serial.println(">> Simulador ECG PWM con patologías");
  mostrarMenu();
}

void loop() {
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    if (cmd.length() > 0) procesarComando(cmd);
  }

  unsigned long now = millis();

  if (!enReposo) {
    if (now - lastUpdate >= tiempoMuestreo) {
      ledcWrite(channel, ecgWaveform[indexWav]);
      lastUpdate = now;
      indexWav++;
      if (indexWav >= waveformLength) {
        enReposo = true;
        inicioReposo = now;
        indexWav = 0;
        ledcWrite(channel, 128);  // línea base
        Serial.println(">> Latido generado");
      }
    }
  } else {
    if (now - inicioReposo >= duracionReposo) {
      enReposo = false;
    }
  }
}

// -------- FUNCIONES ONDAS BÁSICAS --------

float ondaP(float t) {
  if (t >= 0.05 && t < 0.15) {
    float x = (t - 0.05) / 0.1;
    return 0.1 * sin(PI * x);
  }
  return 0;
}

float ondaQ(float t) {
  if (t >= 0.18 && t < 0.21) {
    return -0.15;
  }
  return 0;
}

float ondaR(float t) {
  if (t >= 0.21 && t < 0.25) {
    float x = (t - 0.21) / 0.04;
    return 1.0 * sin(PI * x);
  }
  return 0;
}

float ondaS(float t) {
  if (t >= 0.25 && t < 0.28) {
    float x = (t - 0.25) / 0.03;
    return -0.25 * (1 - x);
  }
  return 0;
}

float ondaT(float t) {
  if (t >= 0.35 && t < 0.45) {
    float x = (t - 0.35) / 0.1;
    return 0.2 * sin(PI * x);
  }
  return 0;
}

// -------- PATOLOGÍAS --------

void generarECG(int pat) {
  switch(pat) {
    case 0: generarPatologiaNormal(); break;
    case 1: generarPatologiaArritmia(); break;
    case 2: generarPatologiaBradicarida(); break;
    case 3: generarPatologiaTaquicardia(); break;
    case 4: generarPatologiaExtrasistolica(); break;
    case 5: generarPatologiaFibrilacion(); break;
    case 6: generarPatologiaFlutter(); break;
    case 7: generarPatologiaBloqueoAV(); break;
    case 8: generarPatologiaIsquemia(); break;
    case 9: generarPatologiaInfarto(); break;
    default: generarPatologiaNormal(); break;
  }
}

void generarPatologiaNormal() {
  for (int i = 0; i < waveformLength; i++) {
    float t = (float)i / waveformLength;
    float val = ondaP(t) + ondaQ(t) + ondaR(t) + ondaS(t) + ondaT(t);
    ecgWaveform[i] = constrain(128 + val * 100, 0, 255);
  }
}

void generarPatologiaArritmia() {
  for (int i = 0; i < waveformLength; i++) {
    float t = (float)i / waveformLength;
    float val = ondaQ(t) + ondaR(t)*0.9 + ondaS(t) + ondaT(t)*0.9;
    ecgWaveform[i] = constrain(128 + val * 100, 0, 255);
  }
}

void generarPatologiaBradicarida() {
  for (int i = 0; i < waveformLength; i++) {
    float t = min((float)i / waveformLength * 0.75, 1.0);
    float val = ondaP(t) + ondaQ(t) + ondaR(t) + ondaS(t) + ondaT(t);
    ecgWaveform[i] = constrain(128 + val * 100, 0, 255);
  }
}

void generarPatologiaTaquicardia() {
  for (int i = 0; i < waveformLength; i++) {
    float t = min((float)i / waveformLength * 1.25, 1.0);
    float val = ondaP(t) + ondaQ(t) + ondaR(t) + ondaS(t) + ondaT(t);
    ecgWaveform[i] = constrain(128 + val * 100, 0, 255);
  }
}

void generarPatologiaExtrasistolica() {
  for (int i = 0; i < waveformLength; i++) {
    float t = (float)i / waveformLength;
    float val = ondaP(t) + ondaQ(t) + ondaR(t) + ondaS(t) + ondaT(t);
    if (t > 0.6 && t < 0.8) val += 0.3 * sin(20 * PI * (t - 0.6));
    ecgWaveform[i] = constrain(128 + val * 100, 0, 255);
  }
}

void generarPatologiaFibrilacion() {
  for (int i = 0; i < waveformLength; i++) {
    float ruido = random(-20, 20) / 100.0;
    ecgWaveform[i] = constrain(128 + ruido * 100, 0, 255);
  }
}

void generarPatologiaFlutter() {
  for (int i = 0; i < waveformLength; i++) {
    float t = (float)i / waveformLength;
    float val = ondaR(t) + 0.1 * sin(20 * PI * t);
    ecgWaveform[i] = constrain(128 + val * 100, 0, 255);
  }
}

void generarPatologiaBloqueoAV() {
  for (int i = 0; i < waveformLength; i++) {
    float t = (float)i / waveformLength;
    float val = ondaQ(t) + ondaR(t) + ondaS(t) + ondaT(t);
    ecgWaveform[i] = constrain(128 + val * 100, 0, 255);
  }
}

void generarPatologiaIsquemia() {
  for (int i = 0; i < waveformLength; i++) {
    float t = (float)i / waveformLength;
    float val = ondaP(t) + ondaQ(t) + ondaR(t) + ondaS(t) + ondaT(t) - 0.1;
    ecgWaveform[i] = constrain(128 + val * 100, 0, 255);
  }
}

void generarPatologiaInfarto() {
  for (int i = 0; i < waveformLength; i++) {
    float t = (float)i / waveformLength;
    float val = ondaP(t) + ondaQ(t) + ondaR(t) + ondaS(t) + ondaT(t) + 0.1;
    ecgWaveform[i] = constrain(128 + val * 100, 0, 255);
  }
}

// -------- UTILIDADES --------

void calcularDuracionReposo() {
  intervaloLatido = 60000 / bpm;
  tiempoMuestreo = intervaloLatido / waveformLength;
  duracionReposo = intervaloLatido - tiempoMuestreo * waveformLength;
  if (duracionReposo < 0) duracionReposo = 0;
}

void mostrarMenu() {
  Serial.println("\n=== MENU SIMULADOR ECG ===");
  Serial.println("Comandos:");
  Serial.println("bpm <valor>     - Cambia frecuencia (30-180 bpm)");
  Serial.println("pat <0-9>       - Cambia patología:");
  Serial.println("   0  Normal");
  Serial.println("   1  Arritmia");
  Serial.println("   2  Bradicardia");
  Serial.println("   3  Taquicardia");
  Serial.println("   4  Extrasistolia");
  Serial.println("   5  Fibrilación auricular");
  Serial.println("   6  Flutter auricular");
  Serial.println("   7  Bloqueo AV");
  Serial.println("   8  Isquemia (ST ↓)");
  Serial.println("   9  Infarto (ST ↑)");
  Serial.println("help            - Muestra este menu");
  Serial.println("=========================\n");
}

void procesarComando(String cmd) {
  cmd.toLowerCase();
  if (cmd.startsWith("bpm ")) {
    int val = cmd.substring(4).toInt();
    if (val >= 30 && val <= 180) {
      bpm = val;
      calcularDuracionReposo();
      Serial.printf(">> BPM actualizado a %d\n", bpm);
    } else {
      Serial.println(">> BPM fuera de rango (30-180)");
    }
  }
  else if (cmd.startsWith("pat ")) {
    int val = cmd.substring(4).toInt();
    if (val >= 0 && val <= 9) {
      patologia = val;
      generarECG(patologia);
      Serial.printf(">> Patología cambiada a %d\n", patologia);
    } else {
      Serial.println(">> Patología inválida (0-9)");
    }
  }
  else if (cmd == "help") {
    mostrarMenu();
  }
  else {
    Serial.println(">> Comando desconocido. Escriba 'help' para ayuda.");
  }
}

