#include "U8g2lib.h"

// ==========================
// VARIABLE DEFINITIONS
// ==========================
#define SCK 13   // Serial Clock (SPI)
#define SDA 11   // MOSI (SPI)
#define CS 10    // Chip Select
#define DC 9     // Data/Command
#define Reset 8  // Reset Pin

const int potPinPlayers = A0;  // Potentiometer for players
const int potPinCards = A1;    // Potentiometer for cards per player
const int dirPin = 3;          // Stepper motor direction pin
const int stepPin = 4;         // Stepper motor step pin
const float stepAngle = 1.8;  // Stepper motor step angle in degrees
const int stepsPerRevolution = 360 / stepAngle; // Steps per full rotation
int moottori_1 = 6;           // Motor control pin 1
int moottori_2 = 5;           // Motor control pin 2
const float driveGearDiameter = 12.0;  // Stepper gear diameter in mm
const float drivenGearDiameter = 36.5; // Driven gear diameter in mm
const float gearRatio = drivenGearDiameter / driveGearDiameter; // Gear ratio


volatile bool interruptTriggered = false; // Flag to indicate interrupt trigger

// ==========================
// OLED SETUP
// ==========================
U8G2_SH1106_128X64_NONAME_1_4W_SW_SPI u8g2(U8G2_R0, SCK, SDA, CS, DC, Reset);

// ==========================
// FUNCTION DEFINITIONS
// ==========================


int calculate_players() {
    int potValue = analogRead(potPinPlayers);  // Read player potentiometer
    return map(potValue, 0, 1023, 1, 8);      // Map value to 1-8 players
}

int calculate_cards() {
    int potValue = analogRead(potPinCards);   // Read cards potentiometer
    int players = calculate_players();
    int maxCardsPerPlayer = 52 / players;     // Maximum cards per player
    return map(potValue, 0, 1023, 1, maxCardsPerPlayer); // Map to cards
}

void rotateStepperByPlayers() {
    Serial.println("Rotating");
    int players = calculate_players();
    float angle = 360.0 / players;            // Calculate rotation angle
    float adjustedAngle = angle * gearRatio;  // Adjust for gear ratio
    int steps = (adjustedAngle / stepAngle);  // Calculate number of steps
    digitalWrite(dirPin, HIGH);               // Set motor direction
   
    // Rotate motor by number of steps
    for (int i = 0; i < abs(steps); i++) {
        digitalWrite(stepPin, HIGH);          // Step high
        delayMicroseconds(5000);              // Wait for step
        digitalWrite(stepPin, LOW);           // Step low
        delayMicroseconds(500);               // Wait for step
    }
}

void fullRotation() {
    int players = calculate_players();
    float stepAnglePerPlayer = 360.0 / players; // Angle per player
    float rotatedAngle = 0;
    
    while (rotatedAngle < 360) {
        rotateStepperByPlayers();
        rotatedAngle += stepAnglePerPlayer;
      
        // Control motor with stepper driver
        for (int i = 0; i < calculate_players(); i++) {
            digitalWrite(moottori_1, LOW); 
            digitalWrite(moottori_2, 200);  
            delay(165);  
          
            digitalWrite(moottori_1, 180);
            digitalWrite(moottori_2, LOW); 
            delay(70);
          
            digitalWrite(moottori_1, LOW);
            digitalWrite(moottori_2, LOW);
            delay(1500);
        }
    }
}
// ==========================
// INTERRUPT FUNCTION
// ==========================
void triggerFullRotation() {
    interruptTriggered = true; // Set flag to true when interrupt occurs
}

// ==========================
// ARDUINO STANDARD FUNCTIONS
// ==========================

void setup() {
    Serial.begin(9600);   // Start serial communication
    u8g2.begin();         // Initialize OLED display

    pinMode(potPinPlayers, INPUT);  // Potentiometer for players
    pinMode(potPinCards, INPUT);    // Potentiometer for cards

    pinMode(dirPin, OUTPUT);        // Stepper direction pin
    pinMode(stepPin, OUTPUT);       // Stepper step pin
    pinMode(moottori_1, OUTPUT);    // Motor control pin 1
    pinMode(moottori_2, OUTPUT);    // Motor control pin 2

    // Set up interrupt on pin 2, triggering on falling edge
    pinMode(2, INPUT_PULLUP);   // Set pin 2 as input with pull-up resistor
    attachInterrupt(digitalPinToInterrupt(2), triggerFullRotation, FALLING);  // Trigger ISR on falling edge

    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(10, 30, "OLED Test");
    u8g2.sendBuffer();
    delay(2000);
}

void loop() {
    int players = calculate_players();
    int cards = calculate_cards();

    u8g2.firstPage();
    do {
        u8g2.setFont(u8g2_font_ncenB08_tr);
        u8g2.drawStr(10, 20, "Players:");
        u8g2.setCursor(80, 20);
        u8g2.print(players);
        u8g2.drawStr(10, 40, "Cards/Player:");
        u8g2.setCursor(100, 40);
        u8g2.print(cards);

    } while (u8g2.nextPage());

    // Check if interrupt has triggered the full rotation function
    if (interruptTriggered) {
        fullRotation();
        interruptTriggered = false;  // Reset flag after the full rotation
    }
  
    delay(1000);
}

