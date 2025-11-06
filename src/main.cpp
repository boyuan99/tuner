#include <Arduino.h>
#include "AD9833.h"
#include <pt.h>
#include <Wire.h>
#include "PT2258.h"

// --------------------- Pin Definitions ----------------------
#define FNC_PIN 2
#define LED_PIN 8
#define BUTTON_PIN 9

// --------------------- Device Objects ------------------------
PT2258 pt2258(0x8C); // PT2258 Object
AD9833 waveGenerator(FNC_PIN);

// -------------------- Global Variables -----------------------
int frequencyList[] = {3000, 10000, 30000};
int volumeList[] = {20,12,10}; // Default volume / Starting Volume
int blinkCount = 0;
int frequencyIndex = 0;
bool isRunning = false;

// Protothread control blocks
static struct pt ptBlinkLED, ptGenerateTone;

// Protothread for blinking LED
static int blinkLED(struct pt *pt) {
    static unsigned long lastTime = 0;
    PT_BEGIN(pt);
    while (1) {
        lastTime = millis();
        PT_WAIT_UNTIL(pt, millis() - lastTime > 40);
        digitalWrite(LED_PIN, HIGH);
        lastTime = millis();
        PT_WAIT_UNTIL(pt, millis() - lastTime > 10);
        digitalWrite(LED_PIN, LOW);
        blinkCount++;
        if (blinkCount > 79) {
            lastTime = millis();
            PT_WAIT_UNTIL(pt, millis() - lastTime > 5000);
            blinkCount = 0;
        }
    }
    PT_END(pt);
}

// -----------------------------------------------------------------------------
// Protothread: Tone Generator
// -----------------------------------------------------------------------------
static int generateTone(struct pt *pt) {
    static unsigned long lastTime = 0;
    PT_BEGIN(pt);

    while (1) {
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3]+50,79));
        // pt2258.attenuation(1, 80);
        waveGenerator.ApplySignal(SINE_WAVE, REG0, frequencyList[frequencyIndex % 3]);
        PT_WAIT_UNTIL(pt, blinkCount > 29);
        waveGenerator.EnableOutput(true);
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > 500);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3]+40,79));
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > 500);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3]+30,79));
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > 500);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3]+20,79));
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > 500);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3]+16,79));
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > 500);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3]+15,79));
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > 500);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3]+14,79));
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > 500);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3]+13,79));
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > 500);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3]+12,79));
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > 500);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3]+11,79));
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > 500);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3]+10,79));
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > 500);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3]+9,79));
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > 500);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3]+8,79));
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > 500);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3]+7,79));
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > 500);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3]+6,79));
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > 500);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3]+5,79));
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > 500);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3]+4,79));
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > 500);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3]+3,79));
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > 500);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3]+2,79));
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > 500);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3]+1,79));
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > 500);
        pt2258.attenuation(1, volumeList[frequencyIndex % 3]);


        PT_WAIT_UNTIL(pt, blinkCount > 49);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3]+1,79));
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > 500);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3]+2,79));
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > 500);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3]+3,79));
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > 500);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3]+4,79));
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > 500);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3]+5,79));
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > 500);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3]+6,79));
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > 500);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3]+7,79));
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > 500);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3]+8,79));
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > 500);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3]+9,79));
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > 500);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3]+10,79));
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > 500);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3]+11,79));
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > 500);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3]+12,79));
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > 500);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3]+13,79));
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > 500);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3]+14,79));
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > 500);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3]+15,79));
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > 500);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3]+16,79));
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > 500);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3]+20,79));
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > 500);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3]+30,79));
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > 500);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3]+40,79));
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > 500);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3]+50,79));
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > 500);
        pt2258.attenuation(1, 79);
        waveGenerator.EnableOutput(false);

        PT_WAIT_UNTIL(pt, blinkCount > 79);
        lastTime = millis();
        PT_WAIT_UNTIL(pt, millis() - lastTime > 5000);
        if (blinkCount == 80) {
            blinkCount = 0;
        }
        frequencyIndex++;
    }
    PT_END(pt);
}

void setup() {
    waveGenerator.Begin();
    Serial.begin(9600);
    pinMode(LED_PIN, OUTPUT);
    pinMode(FNC_PIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    waveGenerator.EnableOutput(false);
    digitalWrite(LED_PIN, LOW);
    PT_INIT(&ptBlinkLED);
    PT_INIT(&ptGenerateTone);
    Wire.setClock(400000); // setting the I2C clock to 100KHz
    /* checking if the MCU can talk with the PT or not*/
    if (!pt2258.begin())
      Serial.println("PT2258 Successfully Initiated");
    else
      Serial.println("Failed to Initiate PT2258");
    /* Initiating PT with default volume and Pin*/
    pt2258.mute(false);
    pt2258.attenuation(1, 79);
}

void loop() {
    int currentButtonState = digitalRead(BUTTON_PIN);
    static int previousButtonState = HIGH;
    Serial.println(blinkCount);
    if (frequencyIndex == 30) {
        waveGenerator.EnableOutput(false);
        digitalWrite(LED_PIN, LOW);
        previousButtonState = HIGH;
        isRunning = !isRunning;
        blinkCount = 0;
        frequencyIndex = 0;
        return;
    }

    if (currentButtonState == LOW && previousButtonState == HIGH) {
        delay(50); // Debounce delay
        if (digitalRead(BUTTON_PIN) == LOW) {
            delay(3000);
            isRunning = !isRunning;
        }
    }

    if (isRunning) {
        // waveGenerator.EnableOutput(true);
        blinkLED(&ptBlinkLED);
        generateTone(&ptGenerateTone);
    }

    previousButtonState = currentButtonState;
}
