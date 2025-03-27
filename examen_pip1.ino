#include <LiquidCrystal.h>
 
int counter = 0;
const int botonPin = 6;
int botonEstado = HIGH;
int estadoReciente;

LiquidCrystal lcd_1(9, 8, 5, 4, 3, 2);

// Pin assignments for LEDs and Buzzer
const int greenLedPin = 13;
const int yellowLedPin = 12;
const int redLedPin = 11;
const int blueLedPin = 10;
const int buzzerPin = 7;

String message = "Press the button!!!!!!             ";  // Add spaces for smooth looping
int scrollIndex = 0;
unsigned long lastScrollTime = 0;
const int scrollDelay = 50; // Time between scroll updates

void setup() {
  lcd_1.begin(16, 2);

  pinMode(botonPin, INPUT_PULLUP);
  pinMode(greenLedPin, OUTPUT);
  pinMode(yellowLedPin, OUTPUT);
  pinMode(redLedPin, OUTPUT);
  pinMode(blueLedPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  // Show the message normally for 1 second
  lcd_1.clear();
  lcd_1.setCursor(0, 0);
  lcd_1.print("Press the button!");
  lcd_1.setCursor(0, 1);
  lcd_1.print(counter); // Show counter initially
  delay(1000); // Wait before scrolling starts
}

void loop() {
  // Handle scrolling independently
  if (millis() - lastScrollTime >= scrollDelay) {
    lastScrollTime = millis();
    lcd_1.setCursor(0, 0);
    lcd_1.print(message.substring(scrollIndex, scrollIndex + 16)); // Show 16 chars

    scrollIndex++;  
    if (scrollIndex > message.length() - 16) {
      scrollIndex = 0; // Restart scrolling
    }
  }

  // Blink LEDs
  digitalWrite(greenLedPin, millis() / 500 % 2);  // Green LED blinks every 500ms
  digitalWrite(yellowLedPin, millis() / 750 % 2); // Yellow LED blinks every 750ms
  digitalWrite(redLedPin, millis() / 1000 % 2);   // Red LED blinks every 1000ms
  digitalWrite(blueLedPin, millis() / 1250 % 2);  // Blue LED blinks every 1250ms

  // Check button press in real-time
  botonEstado = digitalRead(botonPin);
  if (botonEstado == LOW && estadoReciente == HIGH) {
    counter++;  

    // Make the buzzer sound for 0.5 seconds
    tone(buzzerPin, 1000);  // Play sound at 1 kHz
    delay(50);              // Duration of sound (500ms)
    noTone(buzzerPin);       // Stop the sound

    // Update counter display
    lcd_1.setCursor(0, 1);  
    lcd_1.print("       "); // Clear previous number
    lcd_1.setCursor(0, 1);
    lcd_1.print(counter);
    
    delay(200); // Debounce to avoid multiple counts on a single press
  }

  estadoReciente = botonEstado;
}
