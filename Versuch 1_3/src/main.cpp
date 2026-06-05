#include <Arduino.h>

/* ================================================================
   BLOC 1 : VERSUCH 1 - Simple Blink (Einfaches Blinken)
   ================================================================ */

/*const int LED_PIN = 2; // Pin für die LED

void setup() {
    pinMode(LED_PIN, OUTPUT); // Pin als Ausgang konfigurieren
}

void loop() {
    digitalWrite(LED_PIN, HIGH); // LED einschalten (an)
    delay(1000);                 // Eine Sekunde warten
    digitalWrite(LED_PIN, LOW);  // LED ausschalten (aus)
    delay(1000);                 // Eine Sekunde warten
}*/





/* ================================================================
   BLOC 2 : VERSUCH 2 - Mehrere LEDs unterschiedlich leuchten lassen
   ================================================================ */

int ledPins[] = {13, 12, 14, 27, 26, 25, 33, 32};
int numLeds = 8;

void setup() {
    for (int i = 0; i < numLeds; i++) {
        pinMode(ledPins[i], OUTPUT);
    }
}

void loop() {
    // 1) Alle LEDs werden ein- und nach einer Sekunde wieder ausgeschaltet.
    for (int i = 0; i < numLeds; i++) {
        digitalWrite(ledPins[i], HIGH);
    }
    delay(1000);
    for (int i = 0; i < numLeds; i++) {
        digitalWrite(ledPins[i], LOW);
    }
    delay(1000);

    // 2) LEDs nacheinander von links nach rechts ein,
    //    dann nach einer Sekunde von rechts nach links wieder aus
    for (int i = 0; i < numLeds; i++) {
        digitalWrite(ledPins[i], HIGH);
        delay(250);
    }
    delay(1000);
    for (int i = numLeds - 1; i >= 0; i--) {
        digitalWrite(ledPins[i], LOW);
        delay(250);
    }

    // 3) Lauflicht links -> rechts -> links, 500ms zwischen zwei LEDs
    // Hinweg
    for (int i = 0; i < numLeds; i++) {
        digitalWrite(ledPins[i], HIGH);
        delay(500);
        digitalWrite(ledPins[i], LOW);
    }
    // Rückweg (ohne doppelte End-LED)
    for (int i = numLeds - 2; i > 0; i--) {
        digitalWrite(ledPins[i], HIGH);
        delay(500);
        digitalWrite(ledPins[i], LOW);
    }
}





/* ================================================================ 
   BLOC 3 : VERSUCH 3 - Bouton et Toggle (Tastersteuerung)
   ================================================================ */

/*const int BUTTON_PIN = 4; // Pin für den Taster
const int GREEN_LED = 2;  // Pin für die grüne LED

bool ledState = false;
int lastButtonState = HIGH;  // HIGH = bouton relâché (normal avec pull-up externe)

void setup() {
    pinMode(GREEN_LED, OUTPUT);
    pinMode(BUTTON_PIN, INPUT); 
}

void loop() {
    int currentButtonState = digitalRead(BUTTON_PIN);

    // Détection de la transition HAUT → BAS (bouton pressé)
    if (currentButtonState == LOW && lastButtonState == HIGH) {

        ledState = !ledState; 
        digitalWrite(GREEN_LED, ledState);

        delay(50); // Anti-rebond
    }

    lastButtonState = currentButtonState;
}


// à utiliser seule dans loop() si tu veux tester ce comportement.
//j'appui le Boutton l'ampoule verte s'allume. quand je relache elle s'etteint.

void loop() {
    int button = digitalRead(BUTTON_PIN);  // HIGH = relâché, LOW = appuyé

    if (button == LOW) {           // bouton appuyé
        digitalWrite(GREEN_LED, HIGH);
    } else {                       // bouton relâché
        digitalWrite(GREEN_LED, LOW);
    }
}*/


