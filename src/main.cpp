#include <Arduino.h>

// Pin Definitions
#define DIP_1 PIN_PB0
#define DIP_2 PIN_PC0
#define DIP_3 PIN_PC1
#define DIP_4 PIN_PC2
#define DIP_5 PIN_PA1
#define DIP_6 PIN_PA2
#define DIP_7 PIN_PA3
#define DIP_8 PIN_PB1

#define RELAY_1 PIN_PA4
#define RELAY_2 PIN_PA5
#define RELAY_3 PIN_PA6
#define RELAY_4 PIN_PA7
#define RELAY_5 PIN_PB5
#define RELAY_6 PIN_PB4
#define RELAY_7 PIN_PC3

const int dipPins[]   = {PIN_PB0, PIN_PC0, PIN_PC1, PIN_PC2, PIN_PA1, PIN_PA2, PIN_PA3, PIN_PB1};
const int relayPins[] = {PIN_PA4, PIN_PA5, PIN_PA6, PIN_PA7, PIN_PB5, PIN_PB4, PIN_PC3};

const unsigned long RELAY_INTERVAL = 2000; 
const unsigned long REPORT_INTERVAL = 500;  

unsigned long lastActionTime = 0;
unsigned long lastReportTime = 0;
int currentRelayIndex = -1; 
bool sequenceActive = true;

void setup() {
    Serial.begin(115200);
    for (int i = 0; i < 7; i++) {
        pinMode(relayPins[i], OUTPUT);
        digitalWrite(relayPins[i], LOW);
    }
    for (int i = 0; i < 8; i++) {
        pinMode(dipPins[i], INPUT); 
    }
}

void loop() {
    unsigned long currentMillis = millis();

    if (sequenceActive) {
        if (currentMillis - lastActionTime >= RELAY_INTERVAL) {
            if (currentRelayIndex >= 0) {
                digitalWrite(relayPins[currentRelayIndex], LOW);
            }
            
            currentRelayIndex++;
            
            if (currentRelayIndex < 7) {
                digitalWrite(relayPins[currentRelayIndex], HIGH);
                lastActionTime = currentMillis;
                Serial.print("PROGRESS:");
                Serial.println(currentRelayIndex + 1); 
            } else {
                sequenceActive = false;
                currentRelayIndex = -1;
                Serial.println("PROGRESS:DONE");
            }
        }
    }

    if (currentMillis - lastReportTime >= REPORT_INTERVAL) {
        Serial.print("DIP:");
        for (int i = 0; i < 8; i++) {
            int state = (digitalRead(dipPins[i]) == LOW) ? 1 : 0;
            Serial.print(state);
            if (i < 7) Serial.print(",");
        }
        Serial.println();
        lastReportTime = currentMillis;
    }

    if (Serial.available() > 0) {
        String command = Serial.readStringUntil('\n');
        command.trim();
        if (command == "START_TEST") {
            sequenceActive = true;
            currentRelayIndex = -1;
            lastActionTime = 0;
        } else if (command.startsWith("R")) {
            int relayNum = command.substring(1, 2).toInt();
            int state = command.substring(3, 4).toInt();
            if (relayNum >= 1 && relayNum <= 7) {
                digitalWrite(relayPins[relayNum - 1], state);
            }
        }
    }
}