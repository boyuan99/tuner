#include <Arduino.h>

// =====================================================================
// TRIGGER TEST - Simple TDT Trigger Detection
// When trigger received on Pin 9, prints "1" to Serial Monitor
// =====================================================================

#define TRIGGER_PIN 9       // TTL trigger input from TDT
#define LED_PIN 8           // Visual indicator LED

// --------------------- State Variables ----------------------
unsigned long triggerCount = 0;         // Total triggers received
int previousTriggerState = LOW;         // Previous state of trigger pin for edge detection

// =====================================================================
// SETUP
// =====================================================================
void setup() {
    Serial.begin(115200);
    delay(2000);  // Wait 2 seconds for serial monitor to connect

    // Initialize GPIO pins
    pinMode(LED_PIN, OUTPUT);
    pinMode(TRIGGER_PIN, INPUT);  // Regular INPUT - TDT drives the signal
    digitalWrite(LED_PIN, LOW);

    Serial.println("\n==============================================");
    Serial.println("===   TDT TRIGGER TEST                    ===");
    Serial.println("==============================================");
    Serial.println("Waiting for triggers on Pin 9...");
    Serial.println("Will print '1' when trigger detected\n");

    // Flash LED 3 times to show system is ready
    for (int i = 0; i < 3; i++) {
        digitalWrite(LED_PIN, HIGH);
        delay(200);
        digitalWrite(LED_PIN, LOW);
        delay(200);
    }
    Serial.println(">>> SYSTEM READY - LED should have blinked 3 times <<<\n");
}

// =====================================================================
// MAIN LOOP
// =====================================================================
void loop() {
    static unsigned long lastPrintTime = 0;

    // Read current pin state
    int currentTriggerState = digitalRead(TRIGGER_PIN);

    // Print pin state every second for debugging
    if (millis() - lastPrintTime >= 1000) {
        Serial.print("Pin 9 state: ");
        Serial.print(currentTriggerState);
        Serial.print(" (");
        Serial.print(currentTriggerState == HIGH ? "HIGH" : "LOW");
        Serial.print(") | Triggers received: ");
        Serial.println(triggerCount);
        lastPrintTime = millis();
    }

    // Detect rising edge (LOW to HIGH transition) - like button detection
    if (currentTriggerState == HIGH && previousTriggerState == LOW) {
        delay(5);  // Small debounce delay

        // Confirm the pin is still HIGH
        if (digitalRead(TRIGGER_PIN) == HIGH) {
            triggerCount++;

            // Print "1" to serial
            Serial.println("*** TRIGGER DETECTED: 1 ***");

            // Flash LED briefly as visual confirmation
            digitalWrite(LED_PIN, HIGH);
            delay(50);
            digitalWrite(LED_PIN, LOW);
        }
    }

    // Update previous state for next iteration
    previousTriggerState = currentTriggerState;
}
