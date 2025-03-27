#include <LiquidCrystal.h>

// LCD
const int rs = 9, en = 8, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Semáforo
const int verdePin = 13;
const int amarilloPin = 12;
const int rojoPin = 11;
const int buzzer = 7;
const int botonPin = 6;

// Sensores de proximidad (HC-SR04)
const int trigPin1 = A0;  // Sensor 1 Trigger
const int echoPin1 = A1;  // Sensor 1 Echo
const int trigPin2 = A2;  // Sensor 2 Trigger
const int echoPin2 = A3;  // Sensor 2 Echo

// Variables de estado
int state = 0;  // 0: Verde, 1: Amarillo, 2: Rojo
int secondsRemaining = 10;
unsigned long previousMillis = 0;
unsigned long buzzerMillis = 0;
unsigned long proximityMillis = 0;
bool botonPresionado = false;
bool proximityAlarmActive = false;
bool cruzando = false;  // Nueva variable para controlar el estado de cruce

void setup() {
  lcd.begin(16, 2);
  pinMode(verdePin, OUTPUT);
  pinMode(amarilloPin, OUTPUT);
  pinMode(rojoPin, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(botonPin, INPUT);
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);

  // Estado inicial: Verde encendido
  digitalWrite(verdePin, HIGH);
  digitalWrite(amarilloPin, LOW);
  digitalWrite(rojoPin, LOW);
  
  lcd.setCursor(0, 0);
  lcd.print("Estado: VERDE");
  lcd.setCursor(0, 1);
  lcd.print("SIGUIENTE: 10s");
}

void loop() {
  unsigned long currentMillis = millis();

  // Detectar proximidad con ambos sensores
  long distance1 = getDistance(trigPin1, echoPin1);
  long distance2 = getDistance(trigPin2, echoPin2);

  // Alarma de proximidad (solo si no se ha presionado el botón y no está cruzando)
  if (((distance1 > 0 && distance1 <= 20) || (distance2 > 0 && distance2 <= 20)) && !botonPresionado && !cruzando) {
    if (!proximityAlarmActive) {
      proximityAlarmActive = true;
      proximityMillis = currentMillis;
    }
    if (currentMillis - proximityMillis >= 500) {  // Alarma cada 0.5s
      tone(buzzer, 2000, 100);  // Tono alto y corto para proximidad
      proximityMillis = currentMillis;
    }
  } else {
    proximityAlarmActive = false;  // Desactivar si no hay movimiento o está cruzando
  }

  // Detectar si se presiona el botón
  if (digitalRead(botonPin) == HIGH) {
    if (!botonPresionado) {  // Solo cambia estado al presionarlo por primera vez
      botonPresionado = true;
      proximityAlarmActive = false;  // Desactivar alarma de proximidad
      lcd.setCursor(0, 1);
      lcd.print("Esperando...    ");
    }
  }

  // Control del semáforo
  if (currentMillis - previousMillis >= 1000) {
    previousMillis = currentMillis;

    if (secondsRemaining > 0) {
      secondsRemaining--;
    } else {
      cambiarEstado();
    }
    updateLCD();
  }

  // Alarma de cruce (solo en rojo y si el botón fue presionado)
  if (state == 2 && botonPresionado && currentMillis - buzzerMillis >= 1000) {
    cruzando = true;  // Activar estado de cruce
    buzzerMillis = currentMillis;
    tone(buzzer, 1000, 200);  // Tono diferente para cruce
  } else if (state != 2) {
    cruzando = false;  // Desactivar estado de cruce cuando no está en rojo
  }
}

// Función para medir distancia con un sensor ultrasónico
long getDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  long duration = pulseIn(echoPin, HIGH);
  long distance = duration * 0.034 / 2;  // Convertir a cm
  return distance;
}

void cambiarEstado() {
  if (state == 0) {  // Verde → Amarillo
    digitalWrite(verdePin, LOW);
    digitalWrite(amarilloPin, HIGH);
    lcd.setCursor(0, 0);
    lcd.print("Estado: AMARILLO   ");
    secondsRemaining = 5;
    state = 1;
  } 
  else if (state == 1) {  // Amarillo → Rojo
    digitalWrite(amarilloPin, LOW);
    digitalWrite(rojoPin, HIGH);
    lcd.setCursor(0, 0);
    lcd.print("AVANCE AHORA!    ");
    secondsRemaining = 10;
    state = 2;
  } 
  else if (state == 2) {  // Rojo → Verde
    digitalWrite(rojoPin, LOW);
    digitalWrite(verdePin, HIGH);
    lcd.setCursor(0, 0);
    lcd.print("Estado: VERDE    ");
    secondsRemaining = 10;
    state = 0;
    botonPresionado = false;  // Resetear el estado del botón
    proximityAlarmActive = false;
    cruzando = false;  // Resetear estado de cruce
  }
}

void updateLCD() {
  lcd.setCursor(0, 1);
  if (state == 2) {
    lcd.print("Solo Peaton     ");
  } else {
    lcd.print("SIGUIENTE: ");
    if (secondsRemaining > 0) {
      lcd.print(secondsRemaining);
      lcd.print("s      ");
    } else {
      lcd.print("       ");
    }
  }
}
