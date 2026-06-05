#include <stdio.h>
#include <stdbool.h>
#include "TrafficLight.h"

// Globale Variable für die Bedingung [requestGreen] aus dem Automaten
bool requestGreen = false;

// Interne Status-Variablen für die Ampel-Visualisierung
bool RED    = false;
bool YELLOW = false;
bool GREEN  = false;

// Hilfsfunktion: Ampelzustand schön anzeigen
void show(void) {
    // Bildschirm löschen (funktioniert im VS-Code-Terminal unter WSL)
    printf("\033[2J\033[H");

    printf("Ampel-Automat gestartet. Eingaben:\n");
    printf(" t = Tick-Event senden\n");
    printf(" r = requestGreen umschalten (true/false)\n");
    printf(" q = Programm beenden\n\n");

    printf("--- Aktueller Ampelzustand ---\n");
    printf("  %s🔴 ROT%s\n",   RED    ? "\033[31m" : "\033[90m", "\033[0m");
    printf("  %s🟡 GELB%s\n", YELLOW ? "\033[33m" : "\033[90m", "\033[0m");
    printf("  %s🟢 GRÜN%s\n\n", GREEN  ? "\033[32m" : "\033[90m", "\033[0m");
}

// Die folgenden Funktionen werden von TrafficLight.c aufgerufen.
// Sie steuern nur die Visualisierung (kein echter GPIO etc.).

void turnOnRedLigth(void)   { RED = true;  show(); }
void turnOffRedLigth(void)  { RED = false; show(); }

void turnOnRedLight(void)   { RED = true;  show(); }
void turnOffRedLight(void)  { RED = false; show(); }

void turnOnYellowLight(void)  { YELLOW = true;  show(); }
void turnOffYellowLight(void) { YELLOW = false; show(); }

void turnOnGreenLight(void)   { GREEN = true;  show(); }
void turnOffGreenLight(void)  { GREEN = false; show(); }

int main(void) {
    TrafficLight sm;

    // Konstruktor der Zustandsmaschine aufrufen
    TrafficLight_ctor(&sm);

    // Startet die Zustandsmaschine (Initialzustand wird gesetzt)
    TrafficLight_start(&sm);

    // Anfangszustand anzeigen
    show();

    char input;
    while (1) {
        printf("Eingabe? ");
        scanf(" %c", &input);

        // Programm beenden
        if (input == 'q')
            break;

        // Tick-Event senden
        if (input == 't') {
            TrafficLight_dispatch_event(&sm, TrafficLight_EventId_TICK);
            printf("Aktueller Zustand (intern): %s\n",
                   TrafficLight_state_id_to_string(sm.state_id));
        }

        // Guard requestGreen toggeln
        if (input == 'r') {
            requestGreen = !requestGreen;
            printf("requestGreen = %d\n\n", requestGreen);
        }
    }

    return 0;
}
