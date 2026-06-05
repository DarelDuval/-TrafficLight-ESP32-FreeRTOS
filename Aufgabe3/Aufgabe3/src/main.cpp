#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>

/* -------------------------------------------------
   Définition des Pins (adapter selon ton câblage)
   ------------------------------------------------- */
const int RED_LED    = 13;   // LED rouge (en haut à droite)
const int YELLOW_LED = 12;   // LED jaune
const int GREEN_LED  = 2;    // LED verte (breadboard gauche)
const int BUTTON_PIN = 4;    // Taster (Versuch 3)

/* -------------------------------------------------
   Etats du feu tricolore
   ------------------------------------------------- */
enum TrafficLightState {
    STATE_RED,
    STATE_RED_YELLOW,
    STATE_GREEN,
    STATE_YELLOW
};

TrafficLightState currentState;
unsigned long stateTimeMs = 0;

/* -------------------------------------------------
   Events + Queue
   ------------------------------------------------- */
enum AmpelEvent {
    EVENT_REQUEST_GREEN
};

QueueHandle_t xRequestQueue = nullptr;
int lastButtonState = HIGH;

/* -------------------------------------------------
   Prototypes LEDs
   ------------------------------------------------- */
void initRedLight();
void initYellowLight();
void initGreenLight();

void turnOnRedLight();
void turnOffRedLight();
void turnOnYellowLight();
void turnOffYellowLight();
void turnOnGreenLight();
void turnOffGreenLight();

/* -------------------------------------------------
   Prototypes états
   ------------------------------------------------- */
void enterStateRed();
void enterStateRedYellow();
void enterStateGreen();
void enterStateYellow();

/* -------------------------------------------------
   Prototypes Tasks
   ------------------------------------------------- */
void vTaskTrafficLight(void *pvParameters);
void vTaskButton(void *pvParameters);
void vTaskSerial(void *pvParameters);

/* =================================================
   SETUP
   ================================================= */
void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("Start Aufgabe 3");

    // Init LEDs
    initRedLight();
    initYellowLight();
    initGreenLight();

    // Bouton
    pinMode(BUTTON_PIN, INPUT);
    lastButtonState = digitalRead(BUTTON_PIN);

    // Etat initial
    enterStateRed();

    // Queue de 5 events
    xRequestQueue = xQueueCreate(5, sizeof(AmpelEvent));
    if (xRequestQueue == nullptr) {
        Serial.println("ERREUR : Impossible de créer la Queue !");
        while (1) delay(1000);
    }

    // Task 1 : Automate (Ampel)
    xTaskCreate(
        vTaskTrafficLight,
        "TrafficLightTask",
        2048,
        NULL,
        2,
        NULL
    );

    // Task 2 : Bouton
    xTaskCreate(
        vTaskButton,
        "ButtonTask",
        2048,
        NULL,
        1,
        NULL
    );

    // Task 3 : Serial Command Interpreter
    xTaskCreate(
        vTaskSerial,
        "SerialTask",
        4096,
        NULL,
        1,
        NULL
    );
}

void loop() {
    vTaskDelay(portMAX_DELAY);
}

/* =================================================
   LED FUNCTIONS
   ================================================= */
void initRedLight() {
    pinMode(RED_LED, OUTPUT);
    turnOffRedLight();
}

void initYellowLight() {
    pinMode(YELLOW_LED, OUTPUT);
    turnOffYellowLight();
}

void initGreenLight() {
    pinMode(GREEN_LED, OUTPUT);
    turnOffGreenLight();
}

void turnOnRedLight()    { digitalWrite(RED_LED, HIGH); }
void turnOffRedLight()   { digitalWrite(RED_LED, LOW); }

void turnOnYellowLight() { digitalWrite(YELLOW_LED, HIGH); }
void turnOffYellowLight(){ digitalWrite(YELLOW_LED, LOW); }

void turnOnGreenLight()  { digitalWrite(GREEN_LED, HIGH); }
void turnOffGreenLight() { digitalWrite(GREEN_LED, LOW); }

/* =================================================
   ETATS
   ================================================= */
void enterStateRed() {
    turnOnRedLight();
    turnOffYellowLight();
    turnOffGreenLight();
    stateTimeMs = 0;
    currentState = STATE_RED;
}

void enterStateRedYellow() {
    turnOnRedLight();
    turnOnYellowLight();
    turnOffGreenLight();
    stateTimeMs = 0;
    currentState = STATE_RED_YELLOW;
}

void enterStateGreen() {
    turnOffRedLight();
    turnOffYellowLight();
    turnOnGreenLight();
    stateTimeMs = 0;
    currentState = STATE_GREEN;
}

void enterStateYellow() {
    turnOffRedLight();
    turnOnYellowLight();
    turnOffGreenLight();
    stateTimeMs = 0;
    currentState = STATE_YELLOW;
}

/* =================================================
   TASK 1 : Traffic Light Automat
   ================================================= */
void vTaskTrafficLight(void *pvParameters) {
    (void) pvParameters;

    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(1)); // tick toutes les 1 ms
        stateTimeMs++;

        // traiter event venant de la queue
        AmpelEvent evt;
        if (xQueueReceive(xRequestQueue, &evt, 0) == pdPASS &&
            evt == EVENT_REQUEST_GREEN &&
            currentState == STATE_RED) {

            enterStateRedYellow();
        }

        // transitions temporelles
        switch (currentState) {
            case STATE_RED_YELLOW:
                if (stateTimeMs > 2000) enterStateGreen();
                break;

            case STATE_GREEN:
                if (stateTimeMs > 5000) enterStateYellow();
                break;

            case STATE_YELLOW:
                if (stateTimeMs > 3000) enterStateRed();
                break;

            default:
                break;
        }
    }
}

/* =================================================
   TASK 2 : Button
   ================================================= */
void vTaskButton(void *pvParameters) {
    (void) pvParameters;

    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(10)); // anti-rebond

        int current = digitalRead(BUTTON_PIN);

        if (current == LOW && lastButtonState == HIGH) {
            AmpelEvent evt = EVENT_REQUEST_GREEN;
            xQueueSend(xRequestQueue, &evt, 0);
        }

        lastButtonState = current;
    }
}

/* =================================================
   TASK 3 : Serial Menu
   ================================================= */
void vTaskSerial(void *pvParameters) {
    (void) pvParameters;

    for (;;) {
        // Menu
        Serial.println();
        Serial.println("Waehlen Sie einen Eintrag:");
        Serial.println("1. Auslesen des Zustands der Ampel");
        Serial.println("2. Setzen des Requests");
        Serial.print("Auswahl: ");

        int choice = 0;

        while (choice == 0) {
            if (Serial.available() > 0) {
                char c = Serial.read();
                if (c == '1' || c == '2') {
                    choice = c - '0';
                    Serial.println(choice);
                }
            }
            vTaskDelay(pdMS_TO_TICKS(10));
        }

        // Actions
        if (choice == 1) {
            Serial.print("Aktueller Zustand: ");
            switch (currentState) {
                case STATE_RED:        Serial.println("Rot"); break;
                case STATE_RED_YELLOW: Serial.println("Rot-Gelb"); break;
                case STATE_GREEN:      Serial.println("Gruen"); break;
                case STATE_YELLOW:     Serial.println("Gelb"); break;
                default:               Serial.println("Unbekannt"); break;
            }
        }
        else if (choice == 2) {
            AmpelEvent evt = EVENT_REQUEST_GREEN;
            xQueueSend(xRequestQueue, &evt, portMAX_DELAY);
            Serial.println("Request an Ampel gesendet.");
        }
    }
}
