/*

  The MIT License (MIT)

  Copyright (c) 2017 HOLGER IMBERY, CONTACT@CONNECTEDOBJECTS.CLOUD

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/
#include <Homie.h>

#define PIN_RELAY 12
#define PIN_LED 13
#define PIN_BUTTON 0
#define INTERVAL 60
//#define CLEAN_UP

unsigned long lastSent = 0;
int relayState = LOW;
bool stateChange = false;

int buttonState;
int lastButtonState = LOW;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

HomieNode switchNode("switch", "switch");

bool switchHandler(HomieRange range, String value) {
  if (value == "true") {
    digitalWrite(PIN_RELAY, HIGH);
    switchNode.setProperty("on").send("true");
    Serial.println("Switch is on");
  } else if (value == "false") {
    digitalWrite(PIN_RELAY, LOW);
    switchNode.setProperty("on").send("false");
    Serial.println("Switch is off");
  } else {
    return false;
  }
  return true;
}

void setupHandler() {

}

void loopHandler() {

}

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println();
  pinMode(PIN_RELAY, OUTPUT);
  digitalWrite(PIN_RELAY, LOW);

  Homie_setFirmware("SonoffS20", "1.1.0");
  Homie.setLedPin(PIN_LED, LOW).setResetTrigger(PIN_BUTTON, LOW, 5000);

  Homie.setSetupFunction(setupHandler);
  Homie.setLoopFunction(loopHandler);

  switchNode.advertise("on").settable(switchHandler);
  #ifdef CLEAN_UP
    Homie.reset();
  #endif
  Homie.setup();
}

void loop() {
  int reading = digitalRead(PIN_BUTTON);
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;
       if (buttonState == HIGH) {
        stateChange = true;
        relayState = !relayState;
      }
    }
  }
  lastButtonState = reading;
  if (stateChange) {
    digitalWrite(PIN_RELAY, relayState);
    digitalWrite(PIN_LED, !relayState);
    switchNode.setProperty("on").send((relayState == HIGH)? "true" : "false");
    stateChange = false;
  }
  Homie.loop();
}
