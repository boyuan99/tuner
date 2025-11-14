#include <Arduino.h>
#include "AD9833.h"
#include <Wire.h>
#include "PT2258.h"

// =====================================================================
// TDT-Controlled Pure Tone Generator
// Simple trigger-based audio stimulus for trace conditioning
// =====================================================================
// SYSTEM ARCHITECTURE:
// - TDT controls full experiment timeline (CS, trace, US, ITI)
// - Arduino acts as triggered tone generator only
// - TTL pulse on Pin 3 → Play tone for fixed duration
// =====================================================================

// --------------------- Pin Definitions ----------------------
#define FNC_PIN 2           // AD9833 SPI chip select
#define TRIGGER_PIN 3       // TTL trigger input from TDT
#define LED_PIN 8           // Status LED (indicates tone playing)

// --------------------- Tone Parameters ----------------------
#define TONE_FREQ 9500      // 9500 Hz pure tone (match eLife 2021)
#define TONE_DURATION 350   // 350 ms tone duration

// Audio volume control (adjust to achieve 78-84 dB SPL)
#define VOLUME_ATTENUATION 20  // PT2258 value (0=loudest, 79=muted)

// --------------------- Hardware Objects ----------------------
PT2258 pt2258(0x8C);              // Digital volume controller (I2C)
AD9833 waveGenerator(FNC_PIN);    // DDS waveform generator (SPI)

// --------------------- State Variables ----------------------
volatile bool triggerReceived = false;  // ISR flag
bool toneActive = false;                // Tone playing state
unsigned long toneStartTime = 0;        // Tone start timestamp
unsigned long toneCount = 0;            // Diagnostic counter

// =====================================================================
// INTERRUPT SERVICE ROUTINE
// =====================================================================
// Triggered by rising edge TTL pulse from TDT system
void triggerISR() {
    if (!toneActive) {  // Prevent re-triggering during playback
        triggerReceived = true;
    }
}

// =====================================================================
// SETUP - Initialize Hardware
// =====================================================================
void setup() {
    Serial.begin(115200);

    // Print system header
    Serial.println("\n\n");
    Serial.println("==============================================");
    Serial.println("===   TDT-CONTROLLED TONE GENERATOR       ===");
    Serial.println("===   Pure Tone: 9500 Hz, 350 ms          ===");
    Serial.println("==============================================");
    Serial.println("Mode: External trigger (Pin 3)");

    // Initialize GPIO pins
    pinMode(LED_PIN, OUTPUT);
    pinMode(FNC_PIN, OUTPUT);
    pinMode(TRIGGER_PIN, INPUT); 
    digitalWrite(LED_PIN, LOW);

    // Setup external trigger interrupt (rising edge)
    attachInterrupt(digitalPinToInterrupt(TRIGGER_PIN), triggerISR, RISING);
    Serial.println("[INIT] Trigger interrupt configured on Pin 3");

    // Initialize AD9833 DDS waveform generator
    waveGenerator.Begin();
    waveGenerator.EnableOutput(false);
    Serial.println("[INIT] AD9833 waveform generator initialized");

    // Initialize PT2258 digital volume controller
    Wire.setClock(400000);  // I2C at 400 kHz

    if (pt2258.begin()) {
        Serial.println("[INIT] PT2258 volume controller initialized");
    } else {
        Serial.println("[ERROR] PT2258 initialization FAILED!");
        Serial.println("       Check I2C wiring (SDA=A4, SCL=A5)");
    }

    pt2258.attenuation(1, 79);  // Set max attenuation first
    pt2258.mute(true);          // Then mute all channels (true = muted)

    // Display configuration
    Serial.println("\n--- TONE PARAMETERS ---");
    Serial.print("Frequency:        ");
    Serial.print(TONE_FREQ);
    Serial.println(" Hz");

    Serial.print("Duration:         ");
    Serial.print(TONE_DURATION);
    Serial.println(" ms");

    Serial.print("Volume (atten):   ");
    Serial.print(VOLUME_ATTENUATION);
    Serial.println(" dB");

    Serial.println("\n--- HARDWARE CONNECTIONS ---");
    Serial.println("Pin 3:  TTL trigger input (from TDT)");
    Serial.println("Pin 8:  Status LED (ON during tone)");
    Serial.println("Audio:  Connect to amplifier/speaker");

    Serial.println("\n==============================================");
    Serial.println("[READY] Waiting for TDT triggers...");
    Serial.println("==============================================\n");
}

// =====================================================================
// MAIN LOOP - Handle Trigger and Tone Timing
// =====================================================================
void loop() {
    // ========== CHECK FOR NEW TRIGGER ==========
    if (triggerReceived) {
        triggerReceived = false;
        toneCount++;

        // Start tone playback
        Serial.print("[");
        Serial.print(millis());
        Serial.print(" ms] Tone #");
        Serial.print(toneCount);
        Serial.print(" START (");
        Serial.print(TONE_FREQ);
        Serial.println(" Hz)");

        digitalWrite(LED_PIN, HIGH);  // Visual indicator

        // Configure and enable audio output
        pt2258.attenuation(1, VOLUME_ATTENUATION);  // Set volume
        pt2258.mute(false);                         // Unmute audio
        waveGenerator.ApplySignal(SINE_WAVE, REG0, TONE_FREQ);
        waveGenerator.EnableOutput(true);

        toneStartTime = millis();
        toneActive = true;
    }

    // ========== CHECK TONE DURATION ==========
    if (toneActive) {
        unsigned long elapsed = millis() - toneStartTime;

        if (elapsed >= TONE_DURATION) {
            // Stop tone playback
            waveGenerator.EnableOutput(false);
            pt2258.mute(true);          // Mute audio
            pt2258.attenuation(1, 79);  // Set max attenuation
            digitalWrite(LED_PIN, LOW);

            Serial.print("[");
            Serial.print(millis());
            Serial.print(" ms] Tone #");
            Serial.print(toneCount);
            Serial.print(" END (duration: ");
            Serial.print(elapsed);
            Serial.println(" ms)\n");

            toneActive = false;
        }
    }

    // No delay - keep loop responsive for precise timing
}
