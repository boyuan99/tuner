#include <Arduino.h>
#include "AD9833.h"
#include <Wire.h>
#include "PT2258.h"

// =====================================================================
// COMBINED TEST - Trigger Detection + Audio Output
// Debugging version with extensive serial output
// =====================================================================

// --------------------- Pin Definitions ----------------------
#define FNC_PIN 2           // AD9833 SPI chip select
#define TRIGGER_PIN 9       // TTL trigger input from TDT
#define LED_PIN 8           // Status LED (indicates tone playing)

// --------------------- Tone Parameters ----------------------
#define TONE_FREQ 9500      // 9500 Hz pure tone
#define TONE_DURATION 350   // 350 ms tone duration
#define VOLUME_ATTENUATION 20  // PT2258 value (0=loudest, 79=muted)

// --------------------- Hardware Objects ----------------------
PT2258 pt2258(0x8C);              // Digital volume controller (I2C)
AD9833 waveGenerator(FNC_PIN);    // DDS waveform generator (SPI)

// --------------------- State Variables ----------------------
int previousTriggerState = LOW;
bool toneActive = false;
unsigned long toneStartTime = 0;
unsigned long toneCount = 0;
unsigned long lastDebugPrint = 0;

// =====================================================================
// SETUP
// =====================================================================
void setup() {
    Serial.begin(115200);
    delay(2000);  // Wait for serial monitor

    Serial.println("\n\n");
    Serial.println("==============================================");
    Serial.println("===   COMBINED TRIGGER + AUDIO TEST       ===");
    Serial.println("===   WITH DEBUG OUTPUT                   ===");
    Serial.println("==============================================");

    // Initialize GPIO pins
    pinMode(LED_PIN, OUTPUT);
    pinMode(FNC_PIN, OUTPUT);
    pinMode(TRIGGER_PIN, INPUT);
    digitalWrite(LED_PIN, LOW);

    Serial.println("[INIT] GPIO pins configured");
    Serial.print("        LED_PIN = ");
    Serial.println(LED_PIN);
    Serial.print("        TRIGGER_PIN = ");
    Serial.println(TRIGGER_PIN);
    Serial.print("        FNC_PIN = ");
    Serial.println(FNC_PIN);

    // Initialize AD9833
    Serial.println("[INIT] Initializing AD9833...");
    waveGenerator.Begin();
    waveGenerator.EnableOutput(false);
    Serial.println("[INIT] AD9833 initialized and disabled");

    // Initialize PT2258
    Serial.println("[INIT] Initializing PT2258...");
    Wire.setClock(400000);

    int pt2258_result = pt2258.begin();
    Serial.print("[INIT] PT2258 begin() returned: ");
    Serial.println(pt2258_result);

    if (pt2258_result) {
        Serial.println("[INIT] PT2258 initialized successfully");
    } else {
        Serial.println("[ERROR] PT2258 initialization FAILED!");
    }

    // Set initial mute state
    Serial.println("[INIT] Setting PT2258 to muted state...");
    pt2258.attenuation(1, 79);
    Serial.println("[INIT] Attenuation set to 79 dB");
    pt2258.mute(true);
    Serial.println("[INIT] Mute enabled (true)");

    // Print configuration
    Serial.println("\n--- CONFIGURATION ---");
    Serial.print("Tone Frequency:   ");
    Serial.print(TONE_FREQ);
    Serial.println(" Hz");
    Serial.print("Tone Duration:    ");
    Serial.print(TONE_DURATION);
    Serial.println(" ms");
    Serial.print("Volume (atten):   ");
    Serial.print(VOLUME_ATTENUATION);
    Serial.println(" dB");

    Serial.println("\n==============================================");
    Serial.println("[READY] Waiting for triggers on Pin 9...");
    Serial.println("[DEBUG] Pin state will be printed every 2 sec");
    Serial.println("==============================================\n");
}

// =====================================================================
// MAIN LOOP
// =====================================================================
void loop() {
    unsigned long currentTime = millis();

    // ========== DEBUG: Print pin state every 2 seconds ==========
    if (currentTime - lastDebugPrint >= 2000) {
        int pinState = digitalRead(TRIGGER_PIN);
        Serial.print("[DEBUG] Pin 9 state: ");
        Serial.print(pinState);
        Serial.print(" (");
        Serial.print(pinState == HIGH ? "HIGH" : "LOW");
        Serial.print(") | toneActive: ");
        Serial.print(toneActive ? "YES" : "NO");
        Serial.print(" | Triggers received: ");
        Serial.println(toneCount);
        lastDebugPrint = currentTime;
    }

    // ========== TRIGGER DETECTION ==========
    int currentTriggerState = digitalRead(TRIGGER_PIN);

    // Detect rising edge (LOW to HIGH transition)
    if (currentTriggerState == HIGH && previousTriggerState == LOW && !toneActive) {
        Serial.println("\n>>> RISING EDGE DETECTED <<<");

        delay(5);  // Debounce

        // Confirm pin is still HIGH
        int confirmedState = digitalRead(TRIGGER_PIN);
        Serial.print(">>> Confirmed state after 5ms: ");
        Serial.println(confirmedState == HIGH ? "HIGH" : "LOW");

        if (confirmedState == HIGH) {
            toneCount++;

            Serial.println("\n*** STARTING TONE PLAYBACK ***");
            Serial.print("[");
            Serial.print(currentTime);
            Serial.print(" ms] Tone #");
            Serial.print(toneCount);
            Serial.println(" START");

            // Turn on LED
            digitalWrite(LED_PIN, HIGH);
            Serial.println("[AUDIO] LED turned ON");

            // Configure PT2258
            Serial.print("[AUDIO] Setting attenuation to ");
            Serial.print(VOLUME_ATTENUATION);
            Serial.println(" dB...");
            pt2258.attenuation(1, VOLUME_ATTENUATION);

            Serial.println("[AUDIO] Unmuting PT2258...");
            pt2258.mute(false);

            // Configure AD9833
            Serial.print("[AUDIO] Configuring AD9833 for ");
            Serial.print(TONE_FREQ);
            Serial.println(" Hz...");
            waveGenerator.ApplySignal(SINE_WAVE, REG0, TONE_FREQ);

            Serial.println("[AUDIO] Enabling AD9833 output...");
            waveGenerator.EnableOutput(true);

            Serial.println("*** TONE SHOULD BE PLAYING NOW ***\n");

            toneStartTime = currentTime;
            toneActive = true;
        } else {
            Serial.println(">>> FALSE TRIGGER (not confirmed after debounce) <<<\n");
        }
    }

    // Update previous state
    previousTriggerState = currentTriggerState;

    // ========== CHECK TONE DURATION ==========
    if (toneActive) {
        unsigned long elapsed = currentTime - toneStartTime;

        if (elapsed >= TONE_DURATION) {
            Serial.println("\n*** STOPPING TONE PLAYBACK ***");

            // Disable audio
            Serial.println("[AUDIO] Disabling AD9833 output...");
            waveGenerator.EnableOutput(false);

            Serial.println("[AUDIO] Muting PT2258...");
            pt2258.mute(true);

            Serial.println("[AUDIO] Setting attenuation to 79 dB...");
            pt2258.attenuation(1, 79);

            Serial.println("[AUDIO] Turning LED OFF...");
            digitalWrite(LED_PIN, LOW);

            Serial.print("[");
            Serial.print(currentTime);
            Serial.print(" ms] Tone #");
            Serial.print(toneCount);
            Serial.print(" END (duration: ");
            Serial.print(elapsed);
            Serial.println(" ms)");
            Serial.println("*** TONE STOPPED ***\n");

            toneActive = false;
        }
    }
}
