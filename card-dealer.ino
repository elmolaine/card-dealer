#include "U8g2lib.h"

// ==========================
// VARIABLE DEFINITIONS
// ==========================
#define SCK 13   // Serial Clock
#define SDA 11   // MOSI
#define CS 10    // Chip Select
#define DC 9     // Data/Command
#define Reset 8  // Reset Pin

const int potPinPlayers = A0;
const int potPinCards = A1;

const int dirPin = 2;     // Direction pin
const int stepPin = 3;    // Step pin
const float stepAngle = 1.8; // Stepper motor step angle in degrees
const int stepsPerRevolution = 360 / stepAngle; // Total steps per full rotation

// ==========================
// OLED SETUP
// ==========================
U8G2_SH1106_128X64_NONAME_1_4W_SW_SPI u8g2(U8G2_R0, SCK, SDA, CS, DC, Reset); // Fill in the SCK, SDA, CS, DC and Reset pins

// ==========================
// FUNCTION DEFINITIONS
// ==========================


// Function to calculate the number of players based on potentiometer reading
int calculate_players() {
    int potValue = analogRead(potPinPlayers); // Read the potentiometer value (0-1023)
    return map(potValue, 0, 1023, 1, 8); // Map to a range of 1-8
}

// Function to calculate the number of cards based on another potentiometer reading
int calculate_cards() {
    int potValue = analogRead(potPinCards); // Read the potentiometer value (0-1023)
    int players = calculate_players(); // Get the number of players
    int maxCardsPerPlayer = 52 / players; // Calculate the maximum cards each player can get
    
    // Map the potentiometer value to a range from 1 to maxCardsPerPlayer
    return map(potValue, 0, 1023, 1, maxCardsPerPlayer); 
}

// Function to rotate the stepper motor based on the number of players
void rotateStepperByPlayers() {
    int players = calculate_players();
    float angle = 360.0 / players; // Calculate the angle for each player
    int steps = (angle / stepAngle); // Convert angle to steps
    digitalWrite(dirPin, angle > 0 ? HIGH : LOW); // Set direction
    for (int i = 0; i < abs(steps); i++) {
        digitalWrite(stepPin, HIGH);
        delayMicroseconds(500); // Adjust speed if necessary
        digitalWrite(stepPin, LOW);
        delayMicroseconds(500);
    }
}

// Function to complete a full 360-degree rotation using rotateStepperByPlayers
void fullRotation() {
    int players = calculate_players();
    float stepAnglePerPlayer = 360.0 / players;
    float rotatedAngle = 0;
    
    while (rotatedAngle < 360) {
        rotateStepperByPlayers();
        rotatedAngle += stepAnglePerPlayer;
        delay(1000); // Wait before next movement
    }
}

// ==========================
// ARDUINO STANDARD FUNCTIONS
// ==========================

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  u8g2.begin();

  pinMode(potPinPlayers, INPUT);
  pinMode(potPinCards, INPUT);

  pinMode(dirPin, OUTPUT);
  pinMode(stepPin, OUTPUT);
  Serial.println("OLED initialized!");
    
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(10, 30, "OLED Test");
  u8g2.sendBuffer();
  delay(2000); // Show message for 2 seconds
}

  //fullRotation(); // Perform full 360-degree rotation based on players
  //delay(5000)
void loop() {

// Keep the OLED constaly updating
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
    delay(1000);
}
