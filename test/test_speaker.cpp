#include <Arduino.h>
#include <unity.h>
#include "AD9833.h"
#include <Wire.h>
#include "PT2258.h"

// =====================================================================
// SPEAKER TEST - 500ms Tone Every 5 Seconds
// Tests audio output hardware at regular intervals
// =====================================================================

// --------------------- Pin Definitions ----------------------
#define FNC_PIN 2           // AD9833 SPI chip select
#define LED_PIN 8           // Status LED (indicates tone playing)

// --------------------- Test Tone Parameters ----------------------
#define TEST_TONE_FREQ 9500      // 9500 Hz pure tone
#define TEST_TONE_DURATION 500   // 500 ms tone duration
#define TEST_INTERVAL 5000       // 5000 ms (5 seconds) between tones

// Audio volume control
#define TEST_VOLUME_ATTENUATION 10  // PT2258 value (0=loudest, 79=muted)

// --------------------- Hardware Objects ----------------------
PT2258 pt2258(0x8C);              // Digital volume controller (I2C)
AD9833 waveGenerator(FNC_PIN);    // DDS waveform generator (SPI)

// --------------------- State Variables ----------------------
bool toneActive = false;
unsigned long toneStartTime = 0;
unsigned long lastToneTime = 0;
unsigned long toneCount = 0;

// =====================================================================
// SETUP FOR TESTS
// =====================================================================
void setUp(void) {
    // This runs before each test
}

void tearDown(void) {
    // This runs after each test
}

// =====================================================================
// TEST: Hardware Initialization
// =====================================================================
void test_hardware_init(void) {
    // Initialize GPIO pins
    pinMode(LED_PIN, OUTPUT);
    pinMode(FNC_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    // Initialize AD9833 DDS waveform generator
    waveGenerator.Begin();
    waveGenerator.EnableOutput(false);

    // Initialize PT2258 digital volume controller
    Wire.setClock(400000);  // I2C at 400 kHz

    int pt2258_result = pt2258.begin();
    TEST_ASSERT_EQUAL_INT(1, pt2258_result);  // 1 = success

    pt2258.attenuation(1, 79);  // Set max attenuation first
    pt2258.mute(true);          // Mute all channels

    Serial.println("[TEST] Hardware initialization complete");
}

// =====================================================================
// TEST: Periodic Tone Generation
// =====================================================================
void test_periodic_tone(void) {
    Serial.println("\n=============================================");
    Serial.println("=== SPEAKER TEST - 500ms Every 5 Seconds ===");
    Serial.println("=============================================");
    Serial.print("Frequency:  ");
    Serial.print(TEST_TONE_FREQ);
    Serial.println(" Hz");
    Serial.print("Duration:   ");
    Serial.print(TEST_TONE_DURATION);
    Serial.println(" ms");
    Serial.print("Interval:   ");
    Serial.print(TEST_INTERVAL);
    Serial.println(" ms");
    Serial.println("Press RESET to stop test\n");

    // Run for 30 seconds (6 tone cycles)
    unsigned long testStartTime = millis();
    unsigned long testDuration = 30000;  // 30 seconds

    while (millis() - testStartTime < testDuration) {
        unsigned long currentTime = millis();

        // ========== TRIGGER NEW TONE ==========
        if (!toneActive && (currentTime - lastToneTime >= TEST_INTERVAL)) {
            toneCount++;

            Serial.print("[");
            Serial.print(currentTime);
            Serial.print(" ms] Tone #");
            Serial.print(toneCount);
            Serial.print(" START (");
            Serial.print(TEST_TONE_FREQ);
            Serial.println(" Hz)");

            // Turn on LED indicator
            digitalWrite(LED_PIN, HIGH);

            // Configure and enable audio output
            pt2258.attenuation(1, TEST_VOLUME_ATTENUATION);  // Set volume
            pt2258.mute(false);                              // Unmute audio
            waveGenerator.ApplySignal(SINE_WAVE, REG0, TEST_TONE_FREQ);
            waveGenerator.EnableOutput(true);

            toneStartTime = currentTime;
            lastToneTime = currentTime;
            toneActive = true;
        }

        // ========== CHECK TONE DURATION ==========
        if (toneActive) {
            unsigned long elapsed = currentTime - toneStartTime;

            if (elapsed >= TEST_TONE_DURATION) {
                // Stop tone playback
                waveGenerator.EnableOutput(false);
                pt2258.mute(true);          // Mute audio
                pt2258.attenuation(1, 79);  // Set max attenuation
                digitalWrite(LED_PIN, LOW);

                Serial.print("[");
                Serial.print(currentTime);
                Serial.print(" ms] Tone #");
                Serial.print(toneCount);
                Serial.print(" END (duration: ");
                Serial.print(elapsed);
                Serial.println(" ms)\n");

                toneActive = false;
            }
        }

        // Small delay to prevent tight looping
        delay(1);
    }

    // Verify we played at least 5 tones in 30 seconds
    TEST_ASSERT_GREATER_OR_EQUAL(5, toneCount);

    Serial.println("\n=============================================");
    Serial.print("Test complete - ");
    Serial.print(toneCount);
    Serial.println(" tones played");
    Serial.println("=============================================\n");
}

// =====================================================================
// SETUP - Initialize Unity Test Framework
// =====================================================================
void setup() {
    delay(2000);  // Wait for board to stabilize

    Serial.begin(115200);

    UNITY_BEGIN();

    RUN_TEST(test_hardware_init);
    RUN_TEST(test_periodic_tone);

    UNITY_END();
}

// =====================================================================
// LOOP - Not used in Unity tests
// =====================================================================
void loop() {
    // Unity tests run in setup()
    // Infinite loop to prevent restart
    delay(1000);
}
