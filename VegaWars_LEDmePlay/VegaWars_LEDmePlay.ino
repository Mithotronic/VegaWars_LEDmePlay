// Vega Wars
// for the LEDmePlay
//
// www.mithotronic.de
//
// Version: 1.0.1
// Author: Michael Rosskopf (2020)
//
// Thanks to Thomas Laubach!
// 
// Release Notes:
// V1.0.1: Support of LEDmePI (2021)
// V1.0.0: First release

// Include libraries for adafruit matrix
#include <Adafruit_GFX.h>   // Core graphics library
#include <RGBmatrixPanel.h> // Hardware-specific library
#include <avr/pgmspace.h> // Necessary to maintain data in program memory

// Setup adafruit matrix
#define CLK 50
#define OE  51
#define LAT 10
#define A   A0
#define B   A1
#define C   A2
#define D   A3
RGBmatrixPanel matrix(A, B, C, D, CLK, LAT, OE, false);

// Audio out
int audio = 2;

// Define colors
#define TP 0 // transparent
#define BL 1 // black
#define R1 2
#define R2 3
#define R3 4
#define G1 5
#define G2 6
#define G3 7
#define B1 8
#define B2 9
#define B3 10
#define Y1 11
#define Y2 12
#define Y3 13
#define P1 14
#define P2 15
#define P3 16
#define T1 17
#define T2 18
#define T3 19
#define W1 20
#define W2 21
#define W3 22
#define RO 23 // Rose
#define EX 24 // Used for exhaust of ships (flickers)

// Define notes
#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978

// Joystick 1
int buttonU1 = 30;
int buttonD1 = 32;
int buttonL1 = 34;
int buttonR1 = 36;
int buttonFire1 = 38;
int analogX1 = 8;
int analogY1 = 9;

// Joystick 2
int buttonU2 = 31;
int buttonD2 = 33;
int buttonL2 = 35;
int buttonR2 = 37;
int buttonFire2 = 39;
int analogX2 = 10;
int analogY2 = 11;

// Sensitivity of analog thumb joysticks (of the LEDmePlay Joypad) in case of "digital usage" (detects movement if deviates from center position value of 512 by sensitivity value)
const int sensitivity = 192;

// Sensitivity of analog thumb joysticks (minimal difference from center position)
const int sensitivityAnalog = 64;

// Returns the value of maximal excursion already at 0 + maxValueDistance or 1024 - maxValueDistance
const int maxValueDistanceAnalog = 64;

// Helper
int denominatorAnalog = 512 - sensitivityAnalog - maxValueDistanceAnalog;

// Buffer for analog joystick values
double joy1X;
double joy1Y;
double joy2X;
double joy2Y;

// Other buttons
int buttonReset = 42;
int buttonPause = 43;

// Flight control of player´s ship
float xVector;          // X direction in which the player´s ship flies (x position in the middle of the screen)
float yVector;          // Y direction in which the player´s ship flies (y position in the middle of the screen)
float xVectorNew;       // Updated x direction in which the player´s ship flies
float yVectorNew;       // Updated y direction in which the player´s ship flies

float upperLeftX;       // X position of the upper left corner of the screen (between 0 and 127): xVector - 15.5
float upperLeftY;       // Y position of the upper left corner of the screen (between 0 and 127): yVector - 10.5
float upperLeftXNew;    // Updated x position of the upper left corner of the screen (between 0 and 127)
float upperLeftYNew;    // Updated y position of the upper left corner of the screen (between 0 and 127)

float objectXDistance;  // X Distance of other object (ship, shot) to movement vector of the player
float objectYDistance;  // Y Distance of other object (ship, shot) to movement vector of the player
float objectXDirection; // Used to calculate the effect of player movement to the screen position of objects
float objectYDirection; // Used to calculate the effect of player movement to the screen position of objects
float objectNormFactor; // Used to calculate the effect of player movement to the screen position of objects
float objectOverallDistance; // Overall distance of other object (ship, shot) to movement vector of the player to calculate relative z-speed (straight line, 64 is max value which is a simplification)

// Other function of player´s ship
byte shieldEnergy;    // Shield energy: Player´s ship explodes if 0
byte boostEnergy;     // Extra speed available as long as boostEnergy is above 0
byte weaponEnergy;    // Required to fire plasma guns
byte missiles;        // Number of remaining guided missiles
boolean boostOn;      // True if boost in use
float extraThrust;    // Current extra thrust value (if boost is used, the thrust builds up
boolean gotHit;       // True if hit by a shot or a missile
boolean missileAlert; // True if there is an incoming missile
byte newWaveWarning;  // >0 plays the warning sound that a new wave of enemy ships has appeared

// Variables for player´s guided missiles (target locking and firing missile <- one at a time)
// Note: For enemy missile the structure Shot is used)
long lockTimer;         // Counts the time required to lock a target
byte target;            // Ship index of currently locked target
boolean locking;        // True if currently locking a target
boolean targetLocked;   // True if a targt is locked
boolean missileArmed;   // True if missile is reday to fire
boolean missileActive;  // True if missile has been fired
float missileXPosition; // X position of missile
float missileYPosition; // Y position of missile
float missileZPosition; // Z position of missile
float missileXVector;   // X direction of missile
float missileYVector;   // Y direction of missile
float missileZVector;   // Z direction of missile
  
// Starfield effect in the background (of course these ones shall not be real stars but something to show that you are flying through space)
struct Star
{
  byte counter;                // Connts the lifespan of a start
  float x;                     // X position of star
  float y;                     // Y position of star
  float xDirection;            // X direction of star
  float yDirection;            // Y direction of star
};
const byte numberOfStars = 16; // Number of stars that could appear simultaneously on the screen
Star star[numberOfStars];      // Array with all stars
byte nextStar = 0;             // Index of the next star to be used
float originOffsetX;           // Initial x position of stars
float originOffsetY;           // Initial y position of stars

// Vega and planet
byte vegaX = 31;   // Initial x position of vega
byte vegaY = 63;   // Initial y position of vega
byte planetX = 95; // Initial x position of planet
byte planetY = 63; // Initial y position of planet

// Ships
struct Ship
{
  byte type;               // Ship type
  float xPosition;         // X position
  float yPosition;         // Y position
  float zPosition;         // Z position (0 = Player´s ship, 127 = max)
  float xSpeed;            // X direction of ship
  float ySpeed;            // Y direction of ship
  float zSpeed;            // Z direction of ship
  float sideSpeed;         // Random movement in x/y direction (after calculation, xSpeed and ySpeed are updated)
  
  int shield;              // Current shield power
  
  boolean frontVisible;    // true = ship flies towards player´s ship, false = the other way round
  boolean active;          // true = ship is active, false = not active

  byte fireRateFront;      // Likelihood to fire with front guns
  byte fireRateRear;       // Likelihood to fire with rear guns
  byte fireDistance;       // General range of guns
  byte fireAccuracy;       // The higher the number, the higher is the spreading of shots
  byte missileRateFront;   // Likelihood to fire front missile
  byte missileRateRear;    // Likelihood to fire front missile
  
  byte bitmapOffset;       // Helper to store the offset in progmem array for currently drawn bitmap of the ship
  byte screenWidth;        // x-dimension of drawn bitmap
  byte screenHeight;       // y-dimension of drawn bitmap
  byte hasBeenHit;         // >0: player has hit ship (ship flashs up)
  byte fireCounter;        // Set >0 if ship has fired. Value is decremented cyclically. If >0 it is highlighted on the radar.
  byte flightType;         // Stores the flight type
  byte flightMode;         // Stores current flight mode (see above)
  float flightModeCounter; // In some situations, a ship stays in a specific flight mode until the counter reaches a specific value
  float xEvasion;          // Additional movement vector in x-direction if ship avoids collision
  float yEvasion;          // Additional movement vector in y-direction if ship avoids collision
};
const byte numberOfShips = 8; // Number of ships that could show up simultaneously in a battle
Ship ship[numberOfShips];     // Array with all ships
byte nextShip;                // Index of the next ship to be used
byte bitmap;                  // Helper with the bitmap number which is used to show the ship on screen (depends e.g. on distance)

// Ship flight types (general behavior)
#define EVASION_FLIGHT_TYPE    1 // Ship evades to prevent collision
#define ACCELERATE_FLIGHT_TYPE 2 // Ship accelerates to prevent collision

// Ship flight modes (current flight phase)
#define REGULAR_FLIGHT_MODE    1 // Normal flying
#define EVASION_FLIGHT_MODE    2 // Phase of evading
#define ACCELERATE_FLIGHT_MODE 3 // Phase of accelerating
#define ATTACK_FLIGHT_MODE     4 // Attack phase in which ship tries to come closer to player´s ship

// Helper for quicker redrawing of ships
float xPos;
float yPos;

// Shots and enemy missiles
struct Shot
{
  byte type;       // Player shot, enemy shot or enemy missile
  boolean active;  // True if visible
  float xPosition; // X position
  float yPosition; // Y position
  float zPosition; // Z position
  float xVector;   // X direction
  float yVector;   // Y direction
  float zVector;   // Z direction
  float xOffset;   // Helper to store x offset if a shot is aimed not to the player´s front (vector + offset defined the hit zone)
  float yOffset;   // Helper to store Y offset if a shot is aimed not to the player´s front (vector + offset defined the hit zone)
};
const byte numberOfShots = 16; // Number of shots that could be visualized simultaneously
Shot shot[numberOfShots];      // Array with shots
byte nextShot;                 // Index of the next shot to be used
boolean fired;                 // True if fired (helper variable)
byte shotDelay;                // Counter to ensure a time period between shots
boolean firedLeft;             // True if player has fired the last shot with the left gun (the next one will be fired with the right gun)

#define PLAYER_SHOT   1 // Shot fired by the player (missiles fired by the player have their own data strcuture)
#define ENEMY_SHOT    2 // Regular shot fired by an enemy ship
#define ENEMY_MISSILE 3 // Missile fired by an enemy ship

// Explosions
struct Explosion
{
  boolean active;  // True if visible
  byte phase;      // Counter to show the right phase of the explosion
  float xPosition; // Y position
  float yPosition; // Y position
  float zPosition; // Z position
};
const byte numberOfExplosions = 4;       // Number of possibly simultaneously explosions
Explosion explosion[numberOfExplosions]; // Array with explosions
byte nextExplosion;                      // Index of the next explosion to be used

// Ships and corresponding bitmaps
#define SCOUT        1
#define FIGHTER      2
#define HEAVYFIGHTER 3
#define BOMBER       4
#define SATELLITE    5
#define HAUNEBU      6
#define CORVETTE     7
#define DESTROYER    8

const uint8_t scout[] PROGMEM = {
  8, 11, 15, 23, 43, 46, 50, 58,  

  1, 1,
  Y1,

  2, 1,
  Y1, Y1,

  3, 2,
  Y2, Y1, Y2,
  Y2, Y1, Y2,

  6, 3,
  Y2, TP, Y1, Y1, TP, Y2,
  Y1, Y2, B1, B1, Y2, Y1,
  Y2, TP, Y2, Y2, TP, Y2,

  1, 1,
  Y1,

  2, 1,
  Y1, Y1,

  3, 2,
  Y2, Y1, Y2,
  Y2, EX, Y2,

  6, 3,
  Y2, TP, Y1, Y1, TP, Y2,
  Y1, Y2, EX, EX, Y2, Y1,
  Y2, TP, Y2, Y2, TP, Y2
};

const uint8_t fighter[] PROGMEM = {
  8, 11, 15, 25, 59, 62, 66, 76,

  1, 1,
  G1,

  2, 1,
  G1, G1,

  4, 2,
  TP, G2, G2, TP,
  G1, TP, TP, G1,

  8, 4,
  TP, TP, TP, G1, G1, TP, TP, TP,
  TP, G2, G2, B1, B1, G2, G2, TP,
  G1, G1, TP, G1, G1, TP, G1, G1,
  G1, TP, TP, TP, TP, TP, TP, G1,

  1, 1,
  G1,

  2, 1,
  G1, G1,

  4, 2,
  TP, EX, EX, TP,
  G1, TP, TP, G1,

  8, 4,
  TP, TP, TP, G1, G1, TP, TP, TP,
  TP, G2, EX, G2, G2, EX, G2, TP,
  G1, G1, TP, G1, G1, TP, G1, G1,
  G1, TP, TP, TP, TP, TP, TP, G1
};

const uint8_t heavyfighter[] PROGMEM = {
  8, 11, 15, 25, 59, 62, 66, 76,

  1, 1,
  R1,

  2, 1,
  R1, R1,

  4, 2,
  R1, R2, R2, R1,
  R1, TP, TP, R1,

  8, 4,
  R1, TP, TP, R1, R1, TP, TP, R1,
  TP, R1, TP, B1, B1, TP, R1, TP,
  TP, R1, R2, R1, R1, R2, R1, TP,
  R1, TP, TP, TP, TP, TP, TP, R1,

  1, 1,
  R1,

  2, 1,
  R1, R1,

  4, 2,
  R1, EX, EX, R1,
  R1, TP, TP, R1,

  8, 4,
  R1, TP, TP, R1, R1, TP, TP, R1,
  TP, R1, TP, EX, EX, TP, R1, TP,
  TP, R1, R2, R1, R1, R2, R1, TP,
  R1, TP, TP, TP, TP, TP, TP, R1
};

const uint8_t bomber[] PROGMEM = {
  8, 11, 15, 29, 71, 74, 78, 92,  

  1, 1,
  B1,

  2, 1,
  B1, B1,

  4, 3,
  TP, B1, B1, TP,
  B1, B2, B2, B1,
  TP, B1, B1, TP,

  8, 5,
  TP, TP, B2, TP, TP, B2, TP, TP,
  B2, TP, B1, B1, B1, B1, TP, B2,
  TP, B1, B1, T2, T2, B1, B1, TP,
  B2, TP, B2, B1, B1, B2, TP, B2,
  TP, TP, TP, B2, B2, TP, TP, TP,

  1, 1,
  B1,

  2, 1,
  B1, B1,

  4, 3,
  TP, B1, B1, TP,
  B1, EX, EX, R1,
  TP, B1, B1, TP,

  8, 5,
  TP, TP, B2, TP, TP, B2, TP, TP,
  B2, TP, EX, B2, B2, EX, TP, B2,
  TP, B1, B1, B1, B1, B1, B1, TP,
  B2, TP, EX, B1, B1, EX, TP, B2,
  TP, TP, TP, B2, B2, TP, TP, TP
};

const uint8_t satellite[] PROGMEM = {
  8, 11, 15, 25, 59, 62, 66, 76,

  1, 1,
  T1,

  1, 2,
  T1, 
  T1,

  2, 4,
  T1, T1,
  T2, T2,
  T2, T2,
  T1, T1,

  4, 8,
  Y1, TP, TP, Y1,
  TP, Y1, Y1, TP,
  TP, T1, T1, TP,
  T1, T2, T2, T1,
  T1, B1, B1, T1,
  TP, T2, T2, TP,
  T2, T1, T1, T2,
  T1, TP, TP, T1,

  1, 1,
  T1,

  1, 2,
  T1, 
  T1,

  2, 4,
  T1, T1,
  T2, T2,
  T2, T2,
  T1, T1,

  4, 8,
  Y1, TP, TP, Y1,
  TP, Y1, Y1, TP,
  TP, T1, T1, TP,
  T1, T2, T2, T1,
  T1, B1, B1, T1,
  TP, T2, T2, TP,
  T2, T1, T1, T2,
  T1, TP, TP, T1
};

const uint8_t haunebu[] PROGMEM = {
  8, 11, 15, 32, 89, 92, 96, 113,  

  1, 1,
  G1,

  2, 1,
  G1, G1,

  5, 3,
  TP, TP, G1, TP, TP,
  TP, G1, G2, G1, TP,
  G2, G2, G3, G2, G2,

  11, 5,
  TP, TP, TP, TP, G1, G1, G1, TP, TP, TP, TP,
  TP, TP, TP, TP, G2, W1, G2, TP, TP, TP, TP,
  TP, G1, G1, G2, W1, W2, W1, G2, G1, G1, TP,
  G1, G1, G2, G2, G2, W2, G2, G2, G2, G1, G1,
  TP, TP, TP, G1, G2, G2, G2, G1, TP, TP, TP,

  1, 1,
  G1,

  2, 1,
  G1, G1,

  5, 3,
  TP, TP, G1, TP, TP,
  TP, G1, G2, G1, TP,
  G2, G2, G3, G2, G2,

  11, 5,
  TP, TP, TP, TP, G1, G1, G1, TP, TP, TP, TP,
  TP, TP, TP, TP, G2, G2, G2, TP, TP, TP, TP,
  TP, G1, G1, G1, EX, G2, EX, G1, G1, G1, TP,
  G1, G1, G2, G2, EX, G2, EX, G2, G2, G1, G1,
  TP, TP, TP, G1, G2, G2, G2, G1, TP, TP, TP
};

const uint8_t corvette[] PROGMEM = {
  8, 12, 17, 37, 99, 103, 108, 128, 

  2, 1,
  P1, P1,

  3, 1,
  P1, P2, P1,

  6, 2,
  TP, TP, P1, P1, TP, TP,
  P1, P1, P2, P2, P1, P1,
  P1, TP, P1, P1, TP, P1,

  12, 5,
  TP, TP, TP, TP, P1, TP, TP, P1, TP, TP, TP, TP,
  TP, TP, TP, TP, P1, P2, P2, P1, TP, TP, TP, TP,
  P2, P2, P1, P1, B1, B1, B1, B1, P1, P1, P2, P2,
  W1, W1, P1, P1, P2, P1, P1, P2, TP, TP, W1, W1,
  P1, P1, TP, TP, TP, P1, P1, TP, TP, TP, P1, P1,

  2, 1,
  P1, P1,

  3, 1,
  P1, P2, P1,

  6, 2,
  TP, TP, P1, P1, TP, TP,
  P1, P1, P2, P2, P1, P1,
  P1, TP, P1, P1, TP, P1,

  12, 5,
  TP, TP, TP, TP, P1, TP, TP, P1, TP, TP, TP, TP,
  TP, TP, TP, TP, P1, P2, P2, P1, TP, TP, TP, TP,
  EX, EX, P1, W1, P1, P1, P1, P1, W1, P1, EX, EX,
  EX, EX, P1, W1, P2, P1, P1, P2, W1, TP, EX, EX,
  P1, P1, TP, TP, TP, W1, W1, TP, TP, TP, P1, P1
};

const uint8_t destroyer[] PROGMEM = {
  8, 12, 20, 40, 114, 118, 126, 146,

  2, 1,
  R1, R1,

  3, 2,
  TP, R1, TP,
  R1, R2, R1,

  6, 3,
  TP, TP, R1, R1, TP, TP,
  R2, TP, R2, R2, TP, R2,
  R2, R1, R1, R1, R1, R2,

  12, 6,
  TP, TP, TP, TP, R1, TP, TP, R1, TP, TP, TP, TP,
  TP, R1, TP, TP, R1, TP, TP, R1, TP, TP, R1, TP,
  R1, R2, R1, TP, R1, R2, R2, R1, TP, R1, R2, R1,
  R1, W1, W1, R1, B1, B1, B1, B1, R1, W1, W1, R1,
  R1, R2, R1, R1, R1, R2, R2, R1, R1, R1, R2, R1,
  TP, R1, TP, TP, R1, TP, TP, R1, TP, TP, R1, TP, 

  2, 1,
  R1, R1,

  3, 2,
  TP, R1, TP,
  R1, R2, R1,

  6, 3,
  TP, TP, R1, R1, TP, TP,
  EX, TP, R2, R2, TP, EX,
  R2, R1, R1, R1, R1, R2,

  12, 6,
  TP, TP, TP, TP, R1, TP, TP, R1, TP, TP, TP, TP,
  TP, R1, TP, TP, R1, TP, TP, R1, TP, TP, R1, TP,
  R1, R2, R1, TP, W1, R2, R2, W1, TP, R1, R2, R1,
  R1, EX, EX, R1, W1, R1, R1, W1, R1, EX, EX, R1,
  R1, EX, EX, R1, W1, R2, R2, W1, R1, EX, EX, R1,
  TP, R1, TP, TP, R1, TP, TP, R1, TP, TP, R1, TP
};

// Game mode
#define STORY 1
#define MELEE 2

byte gameMode;     // Game mode: Story or melee
byte level;        // Current level: 1 - 3 (from planet 2 you can chose a planet --> level and planet are not identical)
byte planetLevel2; // Planet selected for level 2
byte planetLevel3; // Planet selected for level 3

// Helper for planets
byte planetNumber; // Number of current planet
byte planetOffset; // Index at which the bitmap of the plant starts
byte planetWidth;  // Width of planet bitmap
byte planetHeight; // Height of planet bitmap

// Planet bitmaps
byte numberOfPlanets = 6;
const uint8_t planets[] PROGMEM = {
  6, 44, 62, 80, 131, 158,

  6, 6,
  TP, TP, TP, TP, TP, Y1,
  TP, TP, T2, T1, Y1, TP,
  TP, T2, T1, Y1, T1, TP,
  TP, T2, Y1, T1, T1, TP,
  TP, Y1, T2, T1, TP, TP,
  Y1, TP, TP, TP, TP, TP,

  4, 4,
  TP, R2, R1, TP,
  R2, R1, R1, R1,
  R2, R1, R1, R1,
  TP, R2, R1, TP,  

  4, 4,
  TP, B2, B1, TP,
  B2, B1, B1, B1,
  B2, B1, B1, B1,
  TP, B2, B1, TP,  

  7, 7,
  TP, TP, W2, W1, W1, TP, TP,
  TP, W1, W1, W1, W1, W1, TP,
  RO, RO, RO, RO, RO, RO, RO,
  W2, W1, W1, W1, W1, W1, W1,
  RO, RO, RO, RO, RO, RO, RO,
  TP, W2, W1, RO, RO, W1, TP,
  TP, TP, W2, W1, W1, TP, TP,

  5, 5,
  TP, P2, P1, P1, TP,
  B2, B1, B1, B1, B1,
  P2, P1, P1, P1, P1,
  B2, B1, B1, B1, B1,
  TP, P2, P1, P1, TP,
  
  7, 7,
  TP, TP, R2, R1, R1, TP, TP,
  TP, R1, R1, R1, R1, R1, TP,
  RO, RO, RO, RO, RO, RO, RO,
  R2, R1, R1, R1, R1, R1, R1,
  RO, RO, RO, RO, RO, RO, RO,
  TP, R2, R1, R1, R1, R1, TP,
  TP, TP, R2, R1, R1, TP, TP
};

// Levels and attack waves
byte planetEnemiesOffset; // Position in planetEnemies array where encoding of enemies of current planet starts
byte wave;                // Current wave
byte waveOffset;          // Offset on planetEnemiesOffset where encoding of current wave starts
byte numberOfWaves;       // Number of attack waves for current planet
byte shipsInWave;         // Number of ships in current wave
byte killsInWave;         // Shot down ships in current wave

const uint8_t planetEnemies[] PROGMEM = {
  6, 22, 41, 61, 83, 105,
  
  3,
  4, 7, 11,
  2, FIGHTER, FIGHTER,
  3, SCOUT, SCOUT, SCOUT,
  4, SCOUT, SCOUT, FIGHTER, FIGHTER,

  3,
  4, 9, 13,
  4, SATELLITE, SATELLITE, SATELLITE, SATELLITE,
  3, FIGHTER, FIGHTER, FIGHTER,
  5, SCOUT, SCOUT, SCOUT, HAUNEBU, HAUNEBU,

  3,
  4, 8, 14,
  3, BOMBER, BOMBER, BOMBER,
  5, SCOUT, SCOUT, FIGHTER, FIGHTER, FIGHTER,
  5, SCOUT, SCOUT, BOMBER, BOMBER, CORVETTE,

  3,
  4, 9, 15,
  4, FIGHTER, FIGHTER, BOMBER, BOMBER,
  5, SATELLITE, SATELLITE, SATELLITE, HAUNEBU, HAUNEBU,
  6, SCOUT, SCOUT, BOMBER, BOMBER, CORVETTE, CORVETTE,

  3,
  4, 9, 15,
  4, HEAVYFIGHTER, HEAVYFIGHTER, HEAVYFIGHTER, HEAVYFIGHTER,
  5, FIGHTER, FIGHTER, FIGHTER, HAUNEBU, HAUNEBU,
  6, FIGHTER, FIGHTER, HEAVYFIGHTER, HEAVYFIGHTER, CORVETTE, DESTROYER,

  3,
  4, 13, 20,
  8, SATELLITE, SATELLITE, SATELLITE, SATELLITE, SATELLITE, SATELLITE, SATELLITE, SATELLITE,
  6, FIGHTER, FIGHTER, FIGHTER, HEAVYFIGHTER, HEAVYFIGHTER, HEAVYFIGHTER, 
  6, BOMBER, BOMBER, HEAVYFIGHTER, HEAVYFIGHTER, DESTROYER, DESTROYER
};

// Helper for the melee menu to select the enemy ships
byte numberOfScoutsInMelee;
byte numberOfFightersInMelee;
byte numberOfBombersInMelee;
byte numberOfHeavyFightersInMelee;

boolean playfield[32][24]; // Playfield which is used to visualize space, ships, and shots (height of just 24 since the lower part of the overall screen is used for displays)

byte counter = 0; // Helper

boolean reset;           // If true, go back to title screen
boolean destroyed;       // True if ship destroyed
boolean planetOrbitDone; // True if level completed

// Bitmap for the ship in the title picture
const uint8_t titlepicture[] PROGMEM = {
  W1, W1, W1, W1, W2,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  R1, R2, R2, R2,  0,  0,  0,  0,  0, R1, B1, B2,  0,  0,  0,  0,
   0, R1, R1, R1, R1, R1, R1, R1, R1, R1, R1, B2, B2,  0,  0,  0,
   0,  0, R1, R2, R2, W1, W1, R2, W1, W1, R2, R1, R1, R1, R1,  0,
   0,  0, R1, R1, R1, R1, R1, R2, R2, R2, R2, R2, R2, R2, R2, R2,
   0, R1, R2, R2, R1, R2, R2, R1, R1, R1, R1, R1, R1, W1, W1,  0,
   0, R1, R2, R1, R2, R2,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  R2, R2, R2, R2, R2, W1, W1, W2,  0,  0,  0,  0,  0,  0,  0,  0
};

// Game synchronization
unsigned long engineLoopStartPoint; // Contains millisconds since start of LEDmePlay at begin of each engine loop


// Function declarations
void setup();
boolean joy1Up();
boolean joy1Down();
boolean joy1Left();
boolean joy1Right();
float joy1XValue();
float joy1YValue();
boolean joy1Fire();
boolean joy1FireA();
boolean joy1FireB();
boolean joy1IsLEDmePlayJoypad();
boolean joy2Up();
boolean joy2Down();
boolean joy2Left();
boolean joy2Right();
float joy2XValue();
float joy2YValue();
boolean joy2Fire();
boolean joy2FireA();
boolean joy2FireB();
boolean joy2IsLEDmePlayJoypad();
void drawM(int x, int y);
void drawT(int x, int y);
void mithotronic();
void setLEDMePlayColor(int i);
void ledMePlay();
boolean buttonFire1Clicked(int delayValue);
void clickSound();
void title();
void selectGameMode();
void selectNextPlanet();
void drawShip(byte shipType, byte x, byte y);
void meleeMenu();
void startupSequence();
void playersShipDestroyed();
void planetOrbitDoneScreen();
void drawFighter(byte k);
void storyDoneScreen(byte planetLevel3);
void meleeWonScreen();
boolean isVisible(byte x, byte y);
void drawPixel(byte _x, byte _y, byte _color);
void drawLineIfVisible(int16_t x0, int16_t y0, int16_t x1, int16_t y1, byte r, byte g, byte b);
void setLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, byte r, byte g, byte b);
void drawCircleWhereVisible(int16_t x0, int16_t y0, int16_t radius, byte r, byte g, byte b);
void drawCockpitFrame(byte r, byte g, byte b);
void drawCockpit();
void updateShieldEnergyDisplay();
void updateBoostEnergyDisplay();
void updateWeaponEnergyDisplay();
void updateMissileDisplay();
void blinkMissile();
void drawCrosshairs();
void updateStarfield();
void drawPlanet(byte number, byte x, byte y, byte draw);
void updateVega();
void updatePlanet();
void initializeShip(byte type);
void initializeShip(byte type, float xPosition, float yPosition);
boolean shipVisible(byte i, float x, float y);
void updateShips();
void initializeShot();
void initializeShot(float x, float y, float z, byte type, byte fireAccuracy);
void updateShots();
void updateMissile();
void initializeExplosion(float x, float y, float z);
void updateExplosions();
void checkTargets();
boolean checkForHit(float x, float y, float z, byte damage);
void clearPlayfield();
void initializeGame();
void initializePlanet();
void initializeWave();
void initializeMelee();
void loop();


void setup() 
{
  // Initialize serial connection
  Serial.begin(9600);

  // Initialize random number generator
  randomSeed(analogRead(40));

  // Initialize joysticks and  buttons
  pinMode(buttonL1, INPUT);     
  pinMode(buttonR1, INPUT);     
  pinMode(buttonU1, INPUT);     
  pinMode(buttonD1, INPUT);     
  pinMode(buttonFire1, INPUT);     
  pinMode(buttonL2, INPUT);     
  pinMode(buttonR2, INPUT);     
  pinMode(buttonU2, INPUT);     
  pinMode(buttonD2, INPUT);     
  pinMode(buttonFire2, INPUT);     
  pinMode(buttonReset, INPUT);     
  pinMode(buttonPause, INPUT);    

  // Activate internal pull-up resistors 
  digitalWrite(buttonL1, HIGH);
  digitalWrite(buttonR1, HIGH);
  digitalWrite(buttonU1, HIGH);
  digitalWrite(buttonD1, HIGH);
  digitalWrite(buttonFire1, HIGH);
  digitalWrite(buttonL2, HIGH);
  digitalWrite(buttonR2, HIGH);
  digitalWrite(buttonU2, HIGH);
  digitalWrite(buttonD2, HIGH);
  digitalWrite(buttonFire2, HIGH);
  digitalWrite(buttonReset, HIGH);
  digitalWrite(buttonPause, HIGH);

  // Initialize matrix and define text mode
  matrix.begin();
  matrix.setTextSize(1);
  matrix.setTextWrap(false);

  // Logos
  mithotronic();
  ledMePlay();
  
  // Enter the game loop
  loop();
}

// Joystick inquiry (allows to use a classic joystick or a LEDmePlay Joypad without any configuration)
boolean joy1Up()
{
  if((digitalRead(buttonU1) == LOW && digitalRead(buttonD1) != LOW) || (digitalRead(buttonL1) == LOW && digitalRead(buttonR1) == LOW && analogRead(analogY1) > (512 + sensitivity))){ return true; }
  return false;
}
boolean joy1Down()
{
  if((digitalRead(buttonD1) == LOW && digitalRead(buttonU1) != LOW) || (digitalRead(buttonL1) == LOW && digitalRead(buttonR1) == LOW && analogRead(analogY1) < (512 - sensitivity))){ return true; }
  return false;
}
boolean joy1Left()
{
  if((digitalRead(buttonL1) == LOW && digitalRead(buttonR1) != LOW) || (digitalRead(buttonL1) == LOW && digitalRead(buttonR1) == LOW && analogRead(analogX1) > (512 + sensitivity))){ return true; }
  return false;
}
boolean joy1Right()
{
  if((digitalRead(buttonR1) == LOW && digitalRead(buttonL1) != LOW) || (digitalRead(buttonL1) == LOW && digitalRead(buttonR1) == LOW && analogRead(analogX1) < (512 - sensitivity))){ return true; }
  return false;
}
float joy1XValue()
{
  joy1X = analogRead(analogX1);
  if((joy1X > (512 - sensitivityAnalog)) && (joy1X < (512 + sensitivityAnalog))){ return 0.0; }
  else if(joy1X > (1024 - maxValueDistanceAnalog)){ return 1.0; }
  else if(joy1X < maxValueDistanceAnalog){ return -1.0; }
  else if(joy1X > 512){ return (1.0 * (joy1X - sensitivityAnalog - 512) / denominatorAnalog); }
  else{ return (-1.0 * (512 - joy1X - sensitivityAnalog) / denominatorAnalog); }
}
float joy1YValue()
{
  joy1Y = analogRead(analogY1);
  if((joy1Y > (512 - sensitivityAnalog)) && (joy1Y < (512 + sensitivityAnalog))){ return 0.0; }
  else if(joy1Y > (1024 - maxValueDistanceAnalog)){ return 1.0; }
  else if(joy1Y < maxValueDistanceAnalog){ return -1.0; }
  else if(joy1Y > 512){ return (1.0 * (joy1Y - sensitivityAnalog - 512) / denominatorAnalog); }
  else{ return (-1.0 * (512 - joy1Y - sensitivityAnalog) / denominatorAnalog); }
}
boolean joy1Fire()
{
  if(digitalRead(buttonFire1) == LOW || (digitalRead(buttonU1) == LOW && digitalRead(buttonD1) == LOW)){ return true; }
  return false;
}
boolean joy1FireA()
{
  if(digitalRead(buttonFire1) == LOW){ return true; }
  return false;
}
boolean joy1FireB()
{
  if(digitalRead(buttonU1) == LOW && digitalRead(buttonD1) == LOW){ return true; }
  return false;
}
boolean joy1IsLEDmePlayJoypad()
{
  if(digitalRead(buttonL1) == LOW && digitalRead(buttonR1) == LOW){ return true; }
  return false;
}
boolean joy2Up()
{
  if((digitalRead(buttonU2) == LOW && digitalRead(buttonD2) != LOW) || (digitalRead(buttonL2) == LOW && digitalRead(buttonR2) == LOW && analogRead(analogY2) > (512 + sensitivity))){ return true; }
  return false;
}
boolean joy2Down()
{
  if((digitalRead(buttonD2) == LOW && digitalRead(buttonU2) != LOW) || (digitalRead(buttonL2) == LOW && digitalRead(buttonR2) == LOW && analogRead(analogY2) < (512 - sensitivity))){ return true; }
  return false;
}
boolean joy2Left()
{
  if((digitalRead(buttonL2) == LOW && digitalRead(buttonR2) != LOW) || (digitalRead(buttonL2) == LOW && digitalRead(buttonR2) == LOW && analogRead(analogX2) > (512 + sensitivity))){ return true; }
  return false;
}
boolean joy2Right()
{
  if((digitalRead(buttonR2) == LOW && digitalRead(buttonL2) != LOW) || (digitalRead(buttonL2) == LOW && digitalRead(buttonR2) == LOW && analogRead(analogX2) < (512 - sensitivity))){ return true; }
  return false;
}
float joy2XValue()
{
  joy2X = analogRead(analogX2);
  if((joy2X > (512 - sensitivityAnalog)) && (joy2X < (512 + sensitivityAnalog))){ return 0.0; }
  else if(joy2X > (1024 - maxValueDistanceAnalog)){ return 1.0; }
  else if(joy2X < maxValueDistanceAnalog){ return -1.0; }
  else if(joy2X > 512){ return (1.0 * (joy2X - sensitivityAnalog - 512) / denominatorAnalog); }
  else{ return (-1.0 * (512 - joy2X - sensitivityAnalog) / denominatorAnalog); }
}
float joy2YValue()
{
  joy2Y = analogRead(analogY2);
  if((joy2Y > (512 - sensitivityAnalog)) && (joy2Y < (512 + sensitivityAnalog))){ return 0.0; }
  else if(joy2Y > (1024 - maxValueDistanceAnalog)){ return 1.0; }
  else if(joy2Y < maxValueDistanceAnalog){ return -1.0; }
  else if(joy2Y > 512){ return (1.0 * (joy2Y - sensitivityAnalog - 512) / denominatorAnalog); }
  else{ return (-1.0 * (512 - joy2Y - sensitivityAnalog) / denominatorAnalog); }
}
boolean joy2Fire()
{
  if(digitalRead(buttonFire2) == LOW || (digitalRead(buttonU2) == LOW && digitalRead(buttonD2) == LOW)){ return true; }
  return false;
}
boolean joy2FireA()
{
  if(digitalRead(buttonFire2)){ return true; }
  return false;
}
boolean joy2FireB()
{
  if(digitalRead(buttonU2) == LOW && digitalRead(buttonD2) == LOW){ return true; }
  return false;
}
boolean joy2IsLEDmePlayJoypad()
{
  if(digitalRead(buttonL2) == LOW && digitalRead(buttonR2) == LOW){ return true; }
  return false;
}

// Draw the M of the Mithotronic logo
void drawM(int x, int y)
{
  matrix.fillRect(x + 2, y + 2, 6, 1, matrix.Color333(0, 0, 0));
  matrix.fillRect(x, y + 3, 10, 1, matrix.Color333(0, 0, 0));

  matrix.fillRect(x, y + 4, 2, 6, matrix.Color333(3, 3, 3));
  matrix.fillRect(x + 2, y + 3, 2, 2, matrix.Color333(3, 3, 3));
  matrix.fillRect(x + 4, y + 4, 2, 6, matrix.Color333(3, 3, 3));
  matrix.fillRect(x + 6, y + 3, 2, 2, matrix.Color333(3, 3, 3));
  matrix.fillRect(x + 8, y + 4, 2, 6, matrix.Color333(3, 3, 3));
}

// Draw the T of the Mithotronic logo
void drawT(int x, int y)
{
  matrix.fillRect(x, y + 5, 6, 1, matrix.Color333(0, 0, 0));
  matrix.fillRect(x + 2, y + 10, 3, 1, matrix.Color333(0, 0, 0));

  matrix.fillRect(x, y + 3, 6, 2, matrix.Color333(0, 0, 7));
  matrix.fillRect(x + 2, y, 2, 10, matrix.Color333(0, 0, 7));
  matrix.fillRect(x + 4, y + 8, 1, 2, matrix.Color333(0, 0, 7));
}

// Draw the animated Mithotronic logo and play jingle
void mithotronic()
{
  int i = -10;
  do
  {
    drawM(7, i);
    drawT(19, 22 - i);
    i++;
    delay(50);
  }
  while(i <= 11);

  // Jingle on start screen
  tone(audio,NOTE_C4,200);
  delay(400+20);
  tone(audio,NOTE_C4,90);
  delay(200-20);
  tone(audio,NOTE_G4,140);
  delay(400+20);
  tone(audio,NOTE_G4,140);
  delay(200-20);
  tone(audio,NOTE_C5,450);
  delay(600);
  tone(audio,NOTE_AS4,140);
  delay(200-20);
  tone(audio,NOTE_A4,130);
  delay(200-10);
  tone(audio,NOTE_F4,120);
  delay(200);
  tone(audio,NOTE_G4,1000);
  delay(3000);
}

// Sets color for the next character to show the LEDmePLay logo
void setLEDMePlayColor(int i)
{
  switch(i % 9)
  {
    case 0:
    matrix.setTextColor(matrix.Color333(5,0,0));
    break;
    case 1:
    matrix.setTextColor(matrix.Color333(5,2,0));
    break;
    case 2:
    matrix.setTextColor(matrix.Color333(2,5,0));
    break;
    case 3:
    matrix.setTextColor(matrix.Color333(0,5,0));
    break;
    case 4:
    matrix.setTextColor(matrix.Color333(0,5,2));
    break;
    case 5:
    matrix.setTextColor(matrix.Color333(0,2,5));
    break;
    case 6:
    matrix.setTextColor(matrix.Color333(0,0,5));
    break;
    case 7:
    matrix.setTextColor(matrix.Color333(2,0,5));
    break;
    case 8:
    matrix.setTextColor(matrix.Color333(5,0,2));
    break;
  }
}

// Draw the LEDmePlay logo
void ledMePlay()
{
  // Clear screen
  matrix.fillScreen(matrix.Color333(0, 0, 0));

  int i = 0;
  do
  {
    // Write 'LEDmePlay'
    setLEDMePlayColor(i++);
    matrix.setCursor(7, 5);
    matrix.println("L");
    setLEDMePlayColor(i++);
    matrix.setCursor(13, 5);
    matrix.println("E");
    setLEDMePlayColor(i++);
    matrix.setCursor(19, 5);
    matrix.println("D");

    setLEDMePlayColor(i++);
    matrix.setCursor(10, 11);
    matrix.println("m");
    setLEDMePlayColor(i++);
    matrix.setCursor(16, 11);
    matrix.println("e");

    setLEDMePlayColor(i++);
    matrix.setCursor(4, 19);
    matrix.println("P");
    setLEDMePlayColor(i++);
    matrix.setCursor(10, 19);
    matrix.println("l");
    setLEDMePlayColor(i++);
    matrix.setCursor(16, 19);
    matrix.println("a");
    setLEDMePlayColor(i++);
    matrix.setCursor(22, 19);
    matrix.println("y");
    
    i++;
    if(i > 81)
    {
      i = 0;
    }

    int j = 0;
    do
    {
      j++;
      delay(1);
    }
    while(j < 250 && digitalRead(buttonFire1) != LOW && digitalRead(buttonFire2) != LOW);
  }
  while(digitalRead(buttonFire1) != LOW && digitalRead(buttonFire2) != LOW);
  tone(audio,1024,20);
  delay(200);
  matrix.fillRect(0, 0, 32, 32, matrix.Color333(0,0,0));
}

// buttonFire1Clicked: Used for menu selection (triggers a sound effect)
boolean buttonFire1Clicked(int delayValue)
{
  if(!joy1Fire())
  {
    return false;
  }
  else
  {
    do
    {
      delay(1);
    }
    while(joy1Fire());
    clickSound();
    delay(delayValue);  
    return true;
  }
}

// clickSound: Sound effect
void clickSound()
{
  // SOUND: Menu button clicked
  for(byte i = 0; i < 25; i++)
  {
    tone(audio, 100 + 50 * i, 2);
    delay(1);   
  }
}



// Menus and information screens

// title: Title screen of the game
void title()
{
  // Clear screen
  matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));

  // Clear playfield (required for title screen)
  clearPlayfield();
  
  boolean titleStarActive[32];
  float titleStarX[32];
  byte titleStarY[32];
  float titleStarSpeed[32];
  byte nextTitleStar = 0;

  // Draw fighter
  drawFighter(8);

  for(byte i = 0; i < 32; i++)
  {
    titleStarActive[i] = false;
    titleStarX[i] = 0.0;
    titleStarY[i] = 0;
    titleStarSpeed[i] = 0.0;
  }
    
  do
  {
    // Star effect
    if(random(100) < 20)
    {
      if(!titleStarActive[nextTitleStar])
      {
        titleStarActive[nextTitleStar] = true;
        titleStarX[nextTitleStar] = 31.0;
        titleStarY[nextTitleStar] = random(32);
        titleStarSpeed[nextTitleStar] = (float(random(5)) / 10.0) + 0.6;
        nextTitleStar++;
        if(nextTitleStar == 32){ nextTitleStar = 0; }        
      }
    }
    
    // Draw stars
    for(byte i = 0; i < 32; i++)
    {
      // Remove star from old position
      if(titleStarActive[i])
      {
        if(byte(titleStarX[i]) < 8 || byte(titleStarX[i]) > 23 || titleStarY[i] < 12 || titleStarY[i] > 19)
        {
          matrix.drawPixel(byte(titleStarX[i]), titleStarY[i], matrix.Color333(0, 0, 0));
        }
        else
        {
          if(byte(pgm_read_byte_near(titlepicture + (titleStarY[i] - 12) * 16 + byte(titleStarX[i]) - 8)) == 0)
          {
            matrix.drawPixel(byte(titleStarX[i]), titleStarY[i], matrix.Color333(0, 0, 0));            
          }
        }
      }

      // Move star
      titleStarX[i] = titleStarX[i] - titleStarSpeed[i];
      if(titleStarX[i] < 0.0){ titleStarActive[i] = false; }

      // Draw star at new position
      if(titleStarActive[i])
      {
        if(byte(titleStarX[i]) < 8 || byte(titleStarX[i]) > 23 || titleStarY[i] < 12 || titleStarY[i] > 19)
        {
          matrix.drawPixel(byte(titleStarX[i]), titleStarY[i], matrix.Color333(1, 1, 3));
        }
        else
        {
          if(byte(pgm_read_byte_near(titlepicture + (titleStarY[i] - 12) * 16 + byte(titleStarX[i]) - 8)) == 0)
          {
            matrix.drawPixel(byte(titleStarX[i]), titleStarY[i], matrix.Color333(1, 1, 3));            
          }
        }
      }      
    }

    // Exhaust
    if(random(128) < 64){ matrix.drawPixel(9, 15, matrix.Color333(random(3) + 3, random(3) + 3, 2)); } else { matrix.drawPixel(9, 15, matrix.Color333(0, 0, 0)); }
    if(random(128) < 64){ matrix.drawPixel(9, 16, matrix.Color333(random(3) + 3, random(3) + 3, 2)); } else { matrix.drawPixel(9, 16, matrix.Color333(0, 0, 0)); }
    if(random(128) < 32){ matrix.drawPixel(8, 15, matrix.Color333(random(3) + 3, random(3) + 2, 1)); } else { matrix.drawPixel(8, 15, matrix.Color333(0, 0, 0)); }
    if(random(128) < 32){ matrix.drawPixel(8, 16, matrix.Color333(random(3) + 3, random(3) + 2, 1)); } else { matrix.drawPixel(8, 16, matrix.Color333(0, 0, 0)); }
    if(random(128) < 16){ matrix.drawPixel(7, 15, matrix.Color333(random(3) + 2, random(3) + 1, 0)); } else { matrix.drawPixel(7, 15, matrix.Color333(0, 0, 0)); }
    if(random(128) < 16){ matrix.drawPixel(7, 16, matrix.Color333(random(3) + 2, random(3) + 1, 0)); } else { matrix.drawPixel(7, 16, matrix.Color333(0, 0, 0)); }
    if(random(128) < 8){  matrix.drawPixel(6, 15, matrix.Color333(random(3) + 1, random(3), 0)); }     else { matrix.drawPixel(6, 15, matrix.Color333(0, 0, 0)); }
    if(random(128) < 8){  matrix.drawPixel(6, 16, matrix.Color333(random(3) + 1, random(3), 0)); }     else { matrix.drawPixel(6, 16, matrix.Color333(0, 0, 0)); }

    // Text
    matrix.setTextColor(matrix.Color333(3, 1, 1));
    matrix.setCursor(4, 1);
    matrix.println("Vega");
    matrix.setCursor(4, 24);
    matrix.println("Wars");  
  }
  while(!buttonFire1Clicked(100));
}

// selectGameMode: Select "Story" or "Melee" mode
void selectGameMode()
{
  // Clear screen
  matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));

  matrix.setTextColor(matrix.Color333(3, 1, 1));
  matrix.setCursor(1, 6);
  matrix.print("Story");    
  matrix.setCursor(1, 18);
  matrix.print("Melee");    

  matrix.drawLine(0, 4, 31, 4, matrix.Color333(1, 1, 3));
  matrix.drawLine(0, 14, 31, 14, matrix.Color333(1, 1, 3));
  matrix.drawLine(0, 16, 31, 16, matrix.Color333(0, 0, 0));
  matrix.drawLine(0, 26, 31, 26, matrix.Color333(0, 0, 0));

  gameMode = STORY;

  // Wait for fire button to be pressed
  do
  {
    delay(1);
    if(joy1Up())
    {
      gameMode = STORY;
      matrix.drawLine(0, 4, 31, 4, matrix.Color333(1, 1, 3));
      matrix.drawLine(0, 14, 31, 14, matrix.Color333(1, 1, 3));
      matrix.drawLine(0, 16, 31, 16, matrix.Color333(0, 0, 0));
      matrix.drawLine(0, 26, 31, 26, matrix.Color333(0, 0, 0));
      delay(200);
    }
    if(joy1Down())
    {
      gameMode = MELEE;
      matrix.drawLine(0, 4, 31, 4, matrix.Color333(0, 0, 0));
      matrix.drawLine(0, 14, 31, 14, matrix.Color333(0, 0, 0));
      matrix.drawLine(0, 16, 31, 16, matrix.Color333(1, 1, 3));
      matrix.drawLine(0, 26, 31, 26, matrix.Color333(1, 1, 3));
      delay(200);
    }
  }
  while(!joy1Fire());

  do
  {
    delay(1);
  }
  while(joy1Fire());
  clickSound();
  delay(100);    
}

// selectNextPlanet: Select the next planet (which allows different pathes to finish the game with different levels of difficulty). Only used for "Story" mode.
void selectNextPlanet()
{
  // Clear screen
  matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));

  // Planets
  drawPlanet(1, 13, 3, true);
  drawPlanet(2, 8, 13, true);
  drawPlanet(3, 20, 13, true);
  drawPlanet(4, 2, 22, true);
  drawPlanet(5, 14, 24, true);
  drawPlanet(6, 23, 22, true);

  // Update planet number (pre-selection)
  if(level == 1){ planetNumber = 1; }
  if(level == 2)
  { 
     matrix.drawLine(14, 9, 12, 12, matrix.Color333(1, 1, 3)); 
     planetNumber = 2;
  }
  if(level == 3)
  {
    if(planetLevel2 == 2)
    { 
      matrix.drawLine(14, 9, 12, 12, matrix.Color333(1, 1, 3)); 
      matrix.drawLine(7, 17, 6, 20, matrix.Color333(1, 1, 3)); 
      planetNumber = 4;
    }
    else if(planetLevel2 == 3)
    { 
      matrix.drawLine(17, 8, 19, 12, matrix.Color333(1, 1, 3)); 
      matrix.drawLine(19, 17, 16, 22, matrix.Color333(1, 1, 3)); 
      planetNumber = 5;
    }
  }

  delay(1000);

  boolean toggle = true;

  do
  { 
    if(planetNumber == 1){ drawPlanet(1, 13, 3, toggle); }
    if(planetNumber == 2){ drawPlanet(2, 8, 13, toggle); }
    if(planetNumber == 3){ drawPlanet(3, 20, 13, toggle); }
    if(planetNumber == 4){ drawPlanet(4, 2, 22, toggle); }
    if(planetNumber == 5){ drawPlanet(5, 14, 24, toggle); }
    if(planetNumber == 6){ drawPlanet(6, 23, 22, toggle); }

    // Switch planet
    if(toggle) // This guarantees that the last selected planet remains visible
    {
      if(joy1Left())
      {
        if(level == 2)
        {
          matrix.drawLine(14, 9, 12, 12, matrix.Color333(1, 1, 3)); 
          matrix.drawLine(17, 8, 19, 12, matrix.Color333(0, 0, 0)); 
          planetNumber = 2;
        }
        else if(level == 3)
        {
          if(planetLevel2 == 2)
          {
            matrix.drawLine(7, 17, 6, 20, matrix.Color333(1, 1, 3)); 
            matrix.drawLine(12, 17, 15, 22, matrix.Color333(0, 0, 0)); 
            planetNumber = 4;            
          }
          else if(planetLevel2 == 3)
          {
            matrix.drawLine(19, 17, 16, 22, matrix.Color333(1, 1, 3)); 
            matrix.drawLine(24, 17, 25, 20, matrix.Color333(0, 0, 0)); 
            planetNumber = 5;                        
          }
        }                
      }
      else if(joy1Right())
      {
        if(level == 2)
        {
          matrix.drawLine(14, 9, 12, 12, matrix.Color333(0, 0, 0)); 
          matrix.drawLine(17, 8, 19, 12, matrix.Color333(1, 1, 3)); 
          planetNumber = 3;
        }
        else if(level == 3)
        { 
          if(planetLevel2 == 2)
          {
            matrix.drawLine(7, 17, 6, 20, matrix.Color333(0, 0, 0)); 
            matrix.drawLine(12, 17, 15, 22, matrix.Color333(1, 1, 3)); 
            planetNumber = 5;                        
          }
          else if(planetLevel2 == 3)
          {
            matrix.drawLine(19, 17, 16, 22, matrix.Color333(0, 0, 0)); 
            matrix.drawLine(24, 17, 25, 20, matrix.Color333(1, 1, 3)); 
            planetNumber = 6;                                    
          }
        }                
      }
    }

    if(toggle){ toggle = false; }
    else{ toggle = true; }
    delay(100);
  }
  while(!buttonFire1Clicked(100));

  if(level == 2)
  {
    planetLevel2 = planetNumber;
    if(planetLevel2 == 2)
    {
      matrix.setTextColor(matrix.Color333(1, 1, 3));
      matrix.setCursor(4, 8);
      matrix.println("Easy");
      matrix.setCursor(4, 16);
      matrix.println("path");       
    }
    else if(planetLevel2 == 3)
    {
      matrix.setTextColor(matrix.Color333(1, 1, 3));
      matrix.setCursor(4, 8);
      matrix.println("Hard");
      matrix.setCursor(4, 16);
      matrix.println("path");            
    }
    delay(2000);
  }
  if(level == 3)
  {
    planetLevel3 = planetNumber;
    if(planetLevel3 == 4)
    {
      matrix.setTextColor(matrix.Color333(1, 1, 3));
      matrix.setCursor(4, 8);
      matrix.println("Easy");
      matrix.setCursor(4, 16);
      matrix.println("path");       
    }
    else if(planetLevel3 == 5)
    {
      matrix.setTextColor(matrix.Color333(1, 1, 3));
      matrix.setCursor(6, 8);
      matrix.println("Med");
      matrix.setCursor(4, 16);
      matrix.println("path");       
    }  
    else if(planetLevel3 == 6)
    {
      matrix.setTextColor(matrix.Color333(1, 1, 3));
      matrix.setCursor(4, 8);
      matrix.println("Hard");
      matrix.setCursor(4, 16);
      matrix.println("path");       
    }  
    delay(2000);
  }

  delay(100);   
}

// drawShip: Draws a ship bitmap to the screen. Function is only used for the melee menu in "Melee" mode.
void drawShip(byte shipType, byte x, byte y)
{
  byte width = 0;
  byte height = 0;
  byte bitmapOffset = 0;
  
  switch(shipType)
  {
    case SCOUT:
      width = 6;
      height = 3;
      bitmapOffset = 23;
      break;
    case FIGHTER:
      width = 8;
      height = 4;
      bitmapOffset = 25;
      break;
    case HEAVYFIGHTER:
      width = 8;
      height = 4;
      bitmapOffset = 25;
      break;
    case BOMBER:
      width = 8;
      height = 5;
      bitmapOffset = 29;
      break;
    default:
      break;                      
  }

  for(byte j = 0; j < height; j++)
  {
    for(byte k = 0; k < width; k++)
    {
      switch(shipType)
      {
        case SCOUT:
          drawPixel(x + k, y + j, byte(pgm_read_byte_near(scout + 2 + bitmapOffset + (j * width) + k)));
          break;
        case FIGHTER:
          drawPixel(x + k, y + j, byte(pgm_read_byte_near(fighter + 2 + bitmapOffset + (j * width) + k)));
          break;
        case HEAVYFIGHTER:
          drawPixel(x + k, y + j, byte(pgm_read_byte_near(heavyfighter + 2 + bitmapOffset + (j * width) + k)));
          break;
        case BOMBER:
          drawPixel(x + k, y + j, byte(pgm_read_byte_near(bomber + 2 + bitmapOffset + (j * width) + k)));
          break;
        default:
          break;                      
      }
    }
  } 
}

// meleeMenu: Used to select the enemy ships for a "Melee" mode game
void meleeMenu()
{
  numberOfScoutsInMelee = 0;
  numberOfFightersInMelee = 0;
  numberOfBombersInMelee = 0;
  numberOfHeavyFightersInMelee = 0;

  boolean updateView = true;
  boolean startMelee = false;
  byte cursorPosition = 5;
  
  // Clear screen
  matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));

  // Draw enemy ships
  drawShip(SCOUT, 1, 4);
  drawShip(FIGHTER, 16, 3);
  drawShip(BOMBER, 0, 13);
  drawShip(HEAVYFIGHTER, 16, 14);
  
  matrix.setTextColor(matrix.Color333(3, 1, 1));  
  matrix.setCursor(1, 22);
  matrix.print("Start");    

  // Wait for fire button to be pressed
  do
  {
    delay(1);

    if(updateView)
    {
      matrix.drawLine(0, 0, 31, 0, matrix.Color333(0, 0, 0));
      matrix.drawLine(0, 10, 31, 10, matrix.Color333(0, 0, 0));
      matrix.drawLine(0, 20, 31, 20, matrix.Color333(0, 0, 0));
      matrix.drawLine(0, 30, 31, 30, matrix.Color333(0, 0, 0));
      if(cursorPosition == 1) // Scout
      {
        matrix.drawLine(0, 0, 15, 0, matrix.Color333(1, 1, 3));
        matrix.drawLine(0, 10, 15, 10, matrix.Color333(1, 1, 3));
      }
      else if(cursorPosition == 2) // Fighter
      {
        matrix.drawLine(16, 0, 31, 0, matrix.Color333(1, 1, 3));
        matrix.drawLine(16, 10, 31, 10, matrix.Color333(1, 1, 3));
      }
      else if(cursorPosition == 3) // Bomber
      {
        matrix.drawLine(0, 10, 15, 10, matrix.Color333(1, 1, 3));
        matrix.drawLine(0, 20, 15, 20, matrix.Color333(1, 1, 3));
      }
      else if(cursorPosition == 4) // Heavy Fighter
      {
        matrix.drawLine(16, 10, 31, 10, matrix.Color333(1, 1, 3));
        matrix.drawLine(16, 20, 31, 20, matrix.Color333(1, 1, 3));
      }
      else // Start
      {
        matrix.drawLine(0, 20, 31, 20, matrix.Color333(1, 1, 3));
        matrix.drawLine(0, 30, 31, 30, matrix.Color333(1, 1, 3));
      }
      matrix.fillRect(9, 2, 5, 7, matrix.Color333(0, 0, 0));
      matrix.setCursor(9, 2);
      matrix.print(numberOfScoutsInMelee);    
      matrix.fillRect(25, 2, 5, 7, matrix.Color333(0, 0, 0));
      matrix.setCursor(25, 2);
      matrix.print(numberOfFightersInMelee);    
      matrix.fillRect(9, 12, 5, 7, matrix.Color333(0, 0, 0));
      matrix.setCursor(9, 12);
      matrix.print(numberOfBombersInMelee);    
      matrix.fillRect(25, 12, 5, 7, matrix.Color333(0, 0, 0));
      matrix.setCursor(25, 12);
      matrix.print(numberOfHeavyFightersInMelee);          
      updateView = false;
      delay(200);      
    }
    
    if(joy1Up())
    {
      if(cursorPosition == 1)     { cursorPosition = 1; } // Scout
      else if(cursorPosition == 2){ cursorPosition = 2; } // Fighter
      else if(cursorPosition == 3){ cursorPosition = 1; } // Bomber
      else if(cursorPosition == 4){ cursorPosition = 2; } // Heavy Fighter
      else if(cursorPosition == 5){ cursorPosition = 3; } // Start
      else if(cursorPosition == 6){ cursorPosition = 4; } // Start
      updateView = true;
    }
    if(joy1Down())
    {
      if(cursorPosition == 1)     { cursorPosition = 3; } // Scout
      else if(cursorPosition == 2){ cursorPosition = 4; } // Fighter
      else if(cursorPosition == 3){ cursorPosition = 5; } // Bomber
      else if(cursorPosition == 4){ cursorPosition = 6; } // Heavy Fighter
      updateView = true;
    }
    if(joy1Left())
    {
      if(cursorPosition == 1)     { cursorPosition = 1; } // Scout
      else if(cursorPosition == 2){ cursorPosition = 1; } // Fighter
      else if(cursorPosition == 3){ cursorPosition = 3; } // Bomber
      else if(cursorPosition == 4){ cursorPosition = 3; } // Heavy Fighter
      updateView = true;
    }
    if(joy1Right())
    {
      if(cursorPosition == 1)     { cursorPosition = 2; } // Scout
      else if(cursorPosition == 2){ cursorPosition = 2; } // Fighter
      else if(cursorPosition == 3){ cursorPosition = 4; } // Bomber
      else if(cursorPosition == 4){ cursorPosition = 4; } // Heavy Fighter
      updateView = true;
    }
    if(joy1Fire())
    {
      if(cursorPosition == 1)     { numberOfScoutsInMelee++; clickSound(); } // Scout
      else if(cursorPosition == 2){ numberOfFightersInMelee++; clickSound();  } // Fighter
      else if(cursorPosition == 3){ numberOfBombersInMelee++; clickSound();  } // Bomber
      else if(cursorPosition == 4){ numberOfHeavyFightersInMelee++; clickSound();  } // Heavy Fighter
      else if(cursorPosition == 5 || cursorPosition == 6) // Start
      { 
        if((numberOfScoutsInMelee + numberOfFightersInMelee + numberOfBombersInMelee + numberOfHeavyFightersInMelee) > 0)
        {
          startMelee = true;
          clickSound();
        }
        else
        {
          // SOUND: No enemy ships selected
          tone(audio, 100, 50);
          delay(100);
        }
      }
      if((numberOfScoutsInMelee + numberOfFightersInMelee + numberOfBombersInMelee + numberOfHeavyFightersInMelee) > 8)
      {
        numberOfScoutsInMelee = 0;
        numberOfFightersInMelee = 0;
        numberOfBombersInMelee = 0;        
        numberOfHeavyFightersInMelee = 0;
      }
      updateView = true;      
    }
  }
  while(!startMelee);
  clickSound();
  delay(100);  
}

// startupSequence: Start up player´s ship and launch from the docking bay
void startupSequence()
{
  // Draw tunnel
  drawLineIfVisible(12, 9, 19, 9, 1, 1, 1);
  drawLineIfVisible(19, 9, 19, 12, 1, 1, 1);
  drawLineIfVisible(19, 12, 12, 12, 1, 1, 1);
  drawLineIfVisible(12, 12, 12, 9, 1, 1, 1);
  for(byte i = 0; i < 12; i++)
  {
    if(isVisible(11 - i, 8 - (i / 2))){ matrix.drawPixel(11 - i, 8 - (i / 2), matrix.Color333(1, 1, 1)); }
    if(isVisible(20 + i, 8 - (i / 2))){ matrix.drawPixel(20 + i, 8 - (i / 2), matrix.Color333(1, 1, 1)); }
    if(isVisible(20 + i, 13 + (i / 2))){ matrix.drawPixel(20 + i, 13 + (i / 2), matrix.Color333(1, 1, 1)); }
    if(isVisible(11 - i, 13 + (i / 2))){ matrix.drawPixel(11 - i, 13 + (i / 2), matrix.Color333(1, 1, 1)); }
  }

  // Draw gate
  matrix.fillRect(13, 10, 2, 2, matrix.Color333(0, 1, 1));
  matrix.fillRect(15, 10, 2, 2, matrix.Color333(0, 3, 3));
  matrix.fillRect(17, 10, 2, 2, matrix.Color333(0, 1, 1));
  
  // Start ship systems
  shieldEnergy = 0;
  boostEnergy = 0;
  weaponEnergy = 0;
  missiles = 0;

  delay(1000);
  for(int i = 0; i < 240; i++)
  {
    if(i % 10 == 0)
    {
      if(shieldEnergy < 9){ shieldEnergy++; }
      if(i > 50 && boostEnergy < 90){ boostEnergy = boostEnergy + 10; }
      if(i > 100 && weaponEnergy < 9){ weaponEnergy++; }
      if(i > 150 && missiles < 5){ missiles++; }
    }
    
    updateShieldEnergyDisplay();
    updateBoostEnergyDisplay();
    updateWeaponEnergyDisplay();
    updateMissileDisplay();

    // SOUND: Ship startup
    tone(audio, 50 + (i * 2), 10);
    delay(10);   
  }

  // Open gate
  delay(500);
  matrix.fillRect(13, 10, 1, 2, matrix.Color333(0, 1, 1));
  matrix.fillRect(14, 10, 1, 2, matrix.Color333(0, 3, 3));
  matrix.fillRect(15, 10, 1, 2, matrix.Color333(0, 0, 0));
  matrix.fillRect(16, 10, 1, 2, matrix.Color333(0, 0, 0));
  matrix.fillRect(17, 10, 1, 2, matrix.Color333(0, 3, 3));
  matrix.fillRect(18, 10, 1, 2, matrix.Color333(0, 1, 1));
  delay(500);
  matrix.fillRect(13, 10, 1, 2, matrix.Color333(0, 3, 3));
  matrix.fillRect(14, 10, 1, 2, matrix.Color333(0, 0, 0));
  matrix.fillRect(17, 10, 1, 2, matrix.Color333(0, 0, 0));
  matrix.fillRect(18, 10, 1, 2, matrix.Color333(0, 3, 3));
  delay(500);
  matrix.fillRect(13, 10, 1, 2, matrix.Color333(0, 0, 0));
  matrix.fillRect(18, 10, 1, 2, matrix.Color333(0, 0, 0));

  // SOUND: Startup engine
  for(byte i = 0; i < 100; i++)
  {
    tone(audio, 50 + i + random(50), 10);
    delay(10);
  }

  // Fly out of base ship
  int i = 0;
  int j = 0;
  int k = (256 - (i * 16)) / 10;
  do
  {
    j++;

    if(j == k)
    {
      j = 0;
      k = (320 - (i * 16)) / 10;

      // Remove tunnel from old position
      if(i == 0)
      {
        matrix.drawRect(12, 9, 8, 4, matrix.Color333(0, 0, 0));
      }
      else
      {
        drawLineIfVisible(11 - i, 8 - (i / 2), 20 + i, 8 - (i / 2), 0, 0, 0);
        drawLineIfVisible(20 + i, 8 - (i / 2), 20 + i, 13 + (i / 2), 0, 0, 0);
        drawLineIfVisible(20 + i, 13 + (i / 2), 11 - i, 13 + (i / 2), 0, 0, 0);
        drawLineIfVisible(11 - i, 13 + (i / 2), 11 - i, 8 - (i / 2), 0, 0, 0);
      }
      
      // Update i
      i++;

      // Draw tunnel at new position
      drawLineIfVisible(11 - i, 8 - (i / 2), 20 + i, 8 - (i / 2), 1, 1, 1);
      drawLineIfVisible(20 + i, 8 - (i / 2), 20 + i, 13 + (i / 2), 1, 1, 1);
      drawLineIfVisible(20 + i, 13 + (i / 2), 11 - i, 13 + (i / 2), 1, 1, 1);
      drawLineIfVisible(11 - i, 13 + (i / 2), 11 - i, 8 - (i / 2), 1, 1, 1);
    }
    
    // SOUND: Fly out of base ship
    tone(audio,150 + random(50), 10);
    delay(10);
  }
  while(i < 20);

  // SOUND: Startup engine
  for(byte i = 50; i > 0; i--)
  {
    tone(audio, 50 + (2 * i) + random(50), 10);
    delay(10);
  }
}

// playerShipDestroyed: Game over animation and menu to retry level (only in "Story" mode)
void playersShipDestroyed()
{
  for(byte i = 7; i != 255; i--)
  {
    for(byte j = 0; j < 16; j++)
    {
      matrix.drawLine(15 - j, 15 - j, 16 + j, 15 - j, matrix.Color333(i, i, i));
      matrix.drawLine(16 + j, 15 - j, 16 + j, 16 + j, matrix.Color333(i, i, i));
      matrix.drawLine(16 + j, 16 + j, 15 - j, 16 + j, matrix.Color333(i, i, i));
      matrix.drawLine(15 - j, 16 + j, 15 - j, 15 - j, matrix.Color333(i, i, i));
    }    
    // SOUND: Players ship destroyed
    tone(audio, 50 + 10 * pow(2, i + 1), 40);
  }
  delay(2000);

  for(int i = 0; i < 100; i++)
  {
    matrix.setTextColor(matrix.Color333(0, 0, 0));
    matrix.setCursor(4, 8);
    matrix.println("Game");
    matrix.setCursor(4, 16);
    matrix.println("Over");
    tone(audio, 100, 5);
    delay(10);
    matrix.setTextColor(matrix.Color333(3, 1, 1));
    matrix.setCursor(4, 8);
    matrix.println("Game");
    matrix.setCursor(4, 16);
    matrix.println("Over");
    tone(audio, 1100 - i * 10, 5);
    delay(10);
  }
  delay(2000);

  if(gameMode == STORY)
  {
    // Ask for retry
    boolean retry = true;
  
    matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));
  
    matrix.setTextColor(matrix.Color333(3, 1, 1));
    matrix.setCursor(1, 6);
    matrix.print("Start");    
    matrix.setCursor(1, 18);
    matrix.print("Retry");    
  
    matrix.drawLine(0, 4, 31, 4, matrix.Color333(0, 0, 0));
    matrix.drawLine(0, 14, 31, 14, matrix.Color333(0, 0, 0));
    matrix.drawLine(0, 16, 31, 16, matrix.Color333(1, 1, 3));
    matrix.drawLine(0, 26, 31, 26, matrix.Color333(1, 1, 3));
  
    // Wait for fire button to be pressed
    do
    {
      delay(1);
      if(joy1Up())
      {
        retry = false;
        matrix.drawLine(0, 4, 31, 4, matrix.Color333(1, 1, 3));
        matrix.drawLine(0, 14, 31, 14, matrix.Color333(1, 1, 3));
        matrix.drawLine(0, 16, 31, 16, matrix.Color333(0, 0, 0));
        matrix.drawLine(0, 26, 31, 26, matrix.Color333(0, 0, 0));
        delay(200);
      }
      if(joy1Down())
      {
        retry = true;
        matrix.drawLine(0, 4, 31, 4, matrix.Color333(0, 0, 0));
        matrix.drawLine(0, 14, 31, 14, matrix.Color333(0, 0, 0));
        matrix.drawLine(0, 16, 31, 16, matrix.Color333(1, 1, 3));
        matrix.drawLine(0, 26, 31, 26, matrix.Color333(1, 1, 3));
        delay(200);
      }
    }
    while(!joy1Fire());
  
    if(retry){ destroyed = false; }
    
    do
    {
      delay(1);
    }
    while(joy1Fire());
    clickSound();
    delay(100);
  } 
}

// planetOrbitDoneScreen: Shows that a level has been finished
void planetOrbitDoneScreen()
{
  matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));

  for(int i = 0; i < 100; i++)
  {
    matrix.setTextColor(matrix.Color333(0, 0, 0));
    matrix.setCursor(1, 8);
    matrix.println("Orbit");
    matrix.setCursor(1, 16);
    matrix.println("done!");
    tone(audio, 100, 5);
    delay(10);
    matrix.setTextColor(matrix.Color333(1, 1, 3));
    matrix.setCursor(1, 8);
    matrix.println("Orbit");
    matrix.setCursor(1, 16);
    matrix.println("done!");
    tone(audio, 100 + i * 10, 5);
    delay(10);
  }
  delay(2000);
}

// drawFighter: Draws the fighter bitmap from the "Story done" animation
void drawFighter(byte k)
{
  // Draw fighter
  for(byte j = 0; j < 8; j++)
  {
    for(byte i = 0; i < 16; i++)
    {
      drawPixel(i + k, j + 12, byte(pgm_read_byte_near(titlepicture + j * 16 + i)));    
    }    
  }
  drawPixel(k - 1, 12, BL);
  drawPixel(k - 1, 13, BL);
  drawPixel(k, 14, BL);
  drawPixel(k + 1, 15, BL);
  drawPixel(k + 1, 16, BL);
  drawPixel(k, 17, BL);
  drawPixel(k, 18, BL);
  drawPixel(k - 1, 19, BL);
}

// storyDoneScreen: Animation after "Story" mode has been finished
void storyDoneScreen(byte planetLevel3)
{
  boolean titleStarActive[32];
  float titleStarX[32];
  byte titleStarY[32];
  float titleStarSpeed[32];
  byte nextTitleStar = 0;
  byte fighterXOffset = 8;

  // Clear screen
  matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));

  // Clear playfield (required for story done sequence)
  clearPlayfield();

  // drawFighter
  drawFighter(fighterXOffset);

  for(byte i = 0; i < 32; i++)
  {
    titleStarActive[i] = false;
    titleStarX[i] = 0.0;
    titleStarY[i] = 0;
    titleStarSpeed[i] = 0.0;
  }
    
  for(int j = 0; j < 1000; j++)
  {
    // Star effect
    if(random(100) < 20)
    {
      if(!titleStarActive[nextTitleStar])
      {
        titleStarActive[nextTitleStar] = true;
        titleStarX[nextTitleStar] = 31.0;
        titleStarY[nextTitleStar] = random(32);
        if(j <= 100)
        {
          titleStarSpeed[nextTitleStar] = (float(random(5)) / 10.0) + 0.6;
        }
        else
        {
          titleStarSpeed[nextTitleStar] = ((float(random(5)) / 10.0) + 0.6) * (1.0 + float(j - 100) / 100.0);
        }        
        nextTitleStar++;
        if(nextTitleStar == 32){ nextTitleStar = 0; }        
      }
    }
    
    // Draw stars
    for(byte i = 0; i < 32; i++)
    {
      // Remove stars from old position
      if(titleStarActive[i])
      {
        if(byte(titleStarX[i]) < 8 || byte(titleStarX[i]) > 23 || titleStarY[i] < 12 || titleStarY[i] > 19)
        {
          matrix.drawPixel(byte(titleStarX[i]), titleStarY[i], matrix.Color333(0, 0, 0));
        }
        else
        {
          if(byte(pgm_read_byte_near(titlepicture + (titleStarY[i] - 12) * 16 + byte(titleStarX[i]) - 8)) == 0)
          {
            matrix.drawPixel(byte(titleStarX[i]), titleStarY[i], matrix.Color333(0, 0, 0));            
          }
        }
      }

      // Move star
      titleStarX[i] = titleStarX[i] - titleStarSpeed[i];
      if(titleStarX[i] < 0.0){ titleStarActive[i] = false; }

      // Draw star at new position
      if(titleStarActive[i])
      {
        if(byte(titleStarX[i]) < 8 || byte(titleStarX[i]) > 23 || titleStarY[i] < 12 || titleStarY[i] > 19)
        {
          matrix.drawPixel(byte(titleStarX[i]), titleStarY[i], matrix.Color333(1, 1, 3));
        }
        else
        {
          if(byte(pgm_read_byte_near(titlepicture + (titleStarY[i] - 12) * 16 + byte(titleStarX[i]) - 8)) == 0)
          {
            matrix.drawPixel(byte(titleStarX[i]), titleStarY[i], matrix.Color333(1, 1, 3));            
          }
        }
      }      
    }

    // Update fighter position
    if(j > 850 && j % 5 == 0)
    {
      fighterXOffset++;
      drawFighter(fighterXOffset);
    }

    // Exhaust
    if(random(128) < 64 + (j / 100)){ matrix.drawPixel(fighterXOffset + 1, 15, matrix.Color333(random(3) + 3, random(3) + 3, 2)); } else { matrix.drawPixel(9, 15, matrix.Color333(0, 0, 0)); }
    if(random(128) < 64 + (j / 100)){ matrix.drawPixel(fighterXOffset + 1, 16, matrix.Color333(random(3) + 3, random(3) + 3, 2)); } else { matrix.drawPixel(9, 16, matrix.Color333(0, 0, 0)); }
    if(random(128) < 32 + (j / 100)){ matrix.drawPixel(fighterXOffset, 15, matrix.Color333(random(3) + 3, random(3) + 2, 1)); } else { matrix.drawPixel(8, 15, matrix.Color333(0, 0, 0)); }
    if(random(128) < 32 + (j / 100)){ matrix.drawPixel(fighterXOffset, 16, matrix.Color333(random(3) + 3, random(3) + 2, 1)); } else { matrix.drawPixel(8, 16, matrix.Color333(0, 0, 0)); }
    if(random(128) < 16 + (j / 100)){ matrix.drawPixel(fighterXOffset - 1, 15, matrix.Color333(random(3) + 2, random(3) + 1, 0)); } else { matrix.drawPixel(7, 15, matrix.Color333(0, 0, 0)); }
    if(random(128) < 16 + (j / 100)){ matrix.drawPixel(fighterXOffset - 1, 16, matrix.Color333(random(3) + 2, random(3) + 1, 0)); } else { matrix.drawPixel(7, 16, matrix.Color333(0, 0, 0)); }
    if(random(128) < 8 + (j / 100)){  matrix.drawPixel(fighterXOffset - 2, 15, matrix.Color333(random(3) + 1, random(3), 0)); }     else { matrix.drawPixel(6, 15, matrix.Color333(0, 0, 0)); }
    if(random(128) < 8 + (j / 100)){  matrix.drawPixel(fighterXOffset - 2, 16, matrix.Color333(random(3) + 1, random(3), 0)); }     else { matrix.drawPixel(6, 16, matrix.Color333(0, 0, 0)); }
    matrix.drawPixel(fighterXOffset - 3, 15, matrix.Color333(0, 0, 0));
    matrix.drawPixel(fighterXOffset - 3, 16, matrix.Color333(0, 0, 0));

    // SOUND: Engine sound
    tone(audio, 50 + (j / 10) + random(50), 10);
    delay(10);
  }

  // Text
  matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));
  
  matrix.setTextColor(matrix.Color333(3, 1, 1));
  if(planetLevel3 ==4)
  {
    matrix.setTextColor(matrix.Color333(1, 1, 3));
    matrix.setCursor(4, 0);
    matrix.println("Easy");
    matrix.setCursor(1, 8);
    matrix.println("done!");
    matrix.setTextColor(matrix.Color333(3, 1, 1));
    matrix.setCursor(1, 16);
    matrix.println(" Try");
    matrix.setCursor(4, 24);
    matrix.println("med!");
  }
  else if(planetLevel3 ==5)
  {
    matrix.setTextColor(matrix.Color333(1, 1, 3));
    matrix.setCursor(1, 0);
    matrix.println(" Med");
    matrix.setCursor(1, 8);
    matrix.println("done!");
    matrix.setTextColor(matrix.Color333(3, 1, 1));
    matrix.setCursor(1, 16);
    matrix.println(" Try");
    matrix.setCursor(1, 24);
    matrix.println("hard!");
  }
  else if(planetLevel3 ==6)
  {
    matrix.setTextColor(matrix.Color333(1, 1, 3));
    matrix.setCursor(4, 0);
    matrix.println("Hard");
    matrix.setCursor(1, 8);
    matrix.println("done!");
    matrix.setTextColor(matrix.Color333(3, 1, 1));
    matrix.setCursor(1, 16);
    matrix.println(" All");
    matrix.setCursor(1, 24);
    matrix.println("done!");
  }
  delay(200);
  do
  {
  }
  while(!buttonFire1Clicked(100));
  delay(200);
}  

// meleeWonScreen: Shows that melee has been won
void meleeWonScreen()
{
  matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));
  matrix.setTextColor(matrix.Color333(1, 1, 3));
  matrix.setCursor(1, 0);
  matrix.println("Melee");
  matrix.setCursor(4, 8);
  matrix.println("won!");
  matrix.setTextColor(matrix.Color333(3, 1, 1));
  matrix.setCursor(1, 16);
  matrix.println(" Try");
  matrix.setCursor(1, 24);
  matrix.println("Story");

  delay(200);
  do
  {
  }
  while(!buttonFire1Clicked(100));
  delay(200);
}



// Low level drawing functions

// isVisible: Returns true if a pixel is visible behind the cockpit frame
boolean isVisible(byte x, byte y)
{
  if(x > 31 || y > 23){ return false; }
  return !playfield[x][y];
}

// Draws a pixel in _color at screen position (_x, _y) (used to draw bitmaps)
void drawPixel(byte _x, byte _y, byte _color)
{
  if(_color == BL){ matrix.drawPixel(_x, _y, matrix.Color333(0, 0, 0)); }
  else if(_color == R1){ matrix.drawPixel(_x, _y, matrix.Color333(1, 0, 0)); }
  else if(_color == R2){ matrix.drawPixel(_x, _y, matrix.Color333(3, 0, 0)); }
  else if(_color == R3){ matrix.drawPixel(_x, _y, matrix.Color333(5, 0, 0)); }
  else if(_color == G1){ matrix.drawPixel(_x, _y, matrix.Color333(0, 1, 0)); }
  else if(_color == G2){ matrix.drawPixel(_x, _y, matrix.Color333(0, 3, 0)); }
  else if(_color == G3){ matrix.drawPixel(_x, _y, matrix.Color333(0, 5, 0)); }
  else if(_color == B1){ matrix.drawPixel(_x, _y, matrix.Color333(0, 0, 1)); }
  else if(_color == B2){ matrix.drawPixel(_x, _y, matrix.Color333(0, 0, 3)); }
  else if(_color == B3){ matrix.drawPixel(_x, _y, matrix.Color333(0, 0, 5)); }
  else if(_color == Y1){ matrix.drawPixel(_x, _y, matrix.Color333(1, 1, 0)); }
  else if(_color == Y2){ matrix.drawPixel(_x, _y, matrix.Color333(3, 3, 0)); }
  else if(_color == Y3){ matrix.drawPixel(_x, _y, matrix.Color333(5, 5, 0)); }
  else if(_color == P1){ matrix.drawPixel(_x, _y, matrix.Color333(1, 0, 1)); }
  else if(_color == P2){ matrix.drawPixel(_x, _y, matrix.Color333(3, 0, 3)); }
  else if(_color == P3){ matrix.drawPixel(_x, _y, matrix.Color333(5, 0, 5)); }
  else if(_color == T1){ matrix.drawPixel(_x, _y, matrix.Color333(0, 1, 1)); }
  else if(_color == T2){ matrix.drawPixel(_x, _y, matrix.Color333(0, 3, 3)); }
  else if(_color == T3){ matrix.drawPixel(_x, _y, matrix.Color333(0, 5, 5)); }
  else if(_color == W1){ matrix.drawPixel(_x, _y, matrix.Color333(1, 1, 1)); }
  else if(_color == W2){ matrix.drawPixel(_x, _y, matrix.Color333(3, 3, 3)); }
  else if(_color == W3){ matrix.drawPixel(_x, _y, matrix.Color333(5, 5, 5)); }
  else if(_color == RO){ matrix.drawPixel(_x, _y, matrix.Color333(3, 1, 1)); }
  else if(_color == EX){ matrix.drawPixel(_x, _y, matrix.Color333(random(5) + 2, random(5) + 2, 0)); }
}

// drawLineIfVisible: Draws a line in a defined color behind cockpit frame
void drawLineIfVisible(int16_t x0, int16_t y0, int16_t x1, int16_t y1, byte r, byte g, byte b)
{
  int16_t swapBuffer;
  int16_t steep = abs(y1 - y0) > abs(x1 - x0);
  if(steep)
  {
    swapBuffer = x0; x0 = y0; y0 = swapBuffer;
    swapBuffer = x1; x1 = y1; y1 = swapBuffer;
  }

  if(x0 > x1)
  {
    swapBuffer = x0; x0 = x1; x1 = swapBuffer;
    swapBuffer = y0; y0 = y1; y1 = swapBuffer;
  }

  int16_t dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);

  int16_t err = dx / 2;
  int16_t ystep;

  if (y0 < y1)
  {
    ystep = 1;
  }
  else
  {
    ystep = -1;
  }

  for(; x0<=x1; x0++)
  {
    if(steep)
    {
      if(isVisible(y0, x0)){ matrix.drawPixel(y0, x0, matrix.Color333(r, g, b)); }
    } 
    else
    {
      if(isVisible(x0, y0)){ matrix.drawPixel(x0, y0, matrix.Color333(r, g, b)); }
    }
    err -= dy;
    if (err < 0)
    {
      y0 += ystep;
      err += dx;
    }
  }
}

// setLine: Draws a line in a defined color
void setLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, byte r, byte g, byte b)
{
  int16_t swapBuffer;
  int16_t steep = abs(y1 - y0) > abs(x1 - x0);
  if(steep)
  {
    swapBuffer = x0; x0 = y0; y0 = swapBuffer;
    swapBuffer = x1; x1 = y1; y1 = swapBuffer;
  }

  if(x0 > x1)
  {
    swapBuffer = x0; x0 = x1; x1 = swapBuffer;
    swapBuffer = y0; y0 = y1; y1 = swapBuffer;
  }

  int16_t dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);

  int16_t err = dx / 2;
  int16_t ystep;

  if (y0 < y1)
  {
    ystep = 1;
  }
  else
  {
    ystep = -1;
  }

  for(; x0<=x1; x0++)
  {
    if(steep)
    {
      matrix.drawPixel(y0, x0, matrix.Color333(r, g, b));
      playfield[y0][x0] = 1;
    } 
    else
    {
      matrix.drawPixel(x0, y0, matrix.Color333(r, g, b));
      playfield[x0][y0] = 1;
    }
    err -= dy;
    if (err < 0)
    {
      y0 += ystep;
      err += dx;
    }
  }
}

// drawCircleWhereVisible: Draws a circle in a defined color
void drawCircleWhereVisible(int16_t x0, int16_t y0, int16_t radius, byte r, byte g, byte b)
{
  int16_t f = 1 - radius;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * radius;
  int16_t x = 0;
  int16_t y = radius;

  if(isVisible(x0, y0 + radius)){ matrix.drawPixel(x0, y0 + radius, matrix.Color333(r, g, b)); }
  if(isVisible(x0, y0 - radius)){ matrix.drawPixel(x0, y0 - radius, matrix.Color333(r, g, b)); }
  if(isVisible(x0 + radius, y0)){ matrix.drawPixel(x0 + radius, y0, matrix.Color333(r, g, b)); }
  if(isVisible(x0 - radius, y0)){ matrix.drawPixel(x0 - radius, y0, matrix.Color333(r, g, b)); }

  while(x<y)
  {
    if(f >= 0)
    {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    if(isVisible(x0 + x, y0 + y)){ matrix.drawPixel(x0 + x, y0 + y, matrix.Color333(r, g, b)); }
    if(isVisible(x0 - x, y0 + y)){ matrix.drawPixel(x0 - x, y0 + y, matrix.Color333(r, g, b)); }
    if(isVisible(x0 + x, y0 - y)){ matrix.drawPixel(x0 + x, y0 - y, matrix.Color333(r, g, b)); }
    if(isVisible(x0 - x, y0 - y)){ matrix.drawPixel(x0 - x, y0 - y, matrix.Color333(r, g, b)); }
    if(isVisible(x0 + y, y0 + x)){ matrix.drawPixel(x0 + y, y0 + x, matrix.Color333(r, g, b)); }
    if(isVisible(x0 - y, y0 + x)){ matrix.drawPixel(x0 - y, y0 + x, matrix.Color333(r, g, b)); }
    if(isVisible(x0 + y, y0 - x)){ matrix.drawPixel(x0 + y, y0 - x, matrix.Color333(r, g, b)); }
    if(isVisible(x0 - y, y0 - x)){ matrix.drawPixel(x0 - y, y0 - x, matrix.Color333(r, g, b)); }
  }
}



// Cockpit of player´s ship

// drawCockpitFrame: Draws the cockpit frame in a defined color (e.g. used to flash the cockpit frame in white after a hit)
void drawCockpitFrame(byte r, byte g, byte b)
{
  matrix.drawLine(0, 0, 31, 0, matrix.Color333(r, g, b));
  matrix.drawLine(2, 1, 6, 21, matrix.Color333(r, g, b));
  matrix.drawLine(29, 1, 25, 21, matrix.Color333(r, g, b));
  matrix.drawLine(0, 23, 3, 23, matrix.Color333(r, g, b));
  matrix.drawLine(4, 22, 9, 22, matrix.Color333(r, g, b));
  matrix.drawLine(10, 21, 21, 21, matrix.Color333(r, g, b));
  matrix.drawLine(22, 22, 27, 22, matrix.Color333(r, g, b));
  matrix.drawLine(28, 23, 31, 23, matrix.Color333(r, g, b));  
}

// drawCockpit: Draw the full cockpit
void drawCockpit()
{
  // Cockpit frame
  setLine(0, 0, 31, 0, 0, 0, 1);
  setLine(2, 1, 6, 21, 0, 0, 1);
  setLine(29, 1, 25, 21, 0, 0, 1);
  setLine(0, 23, 3, 23, 0, 0, 1);
  setLine(4, 22, 9, 22, 0, 0, 1);
  setLine(10, 21, 21, 21, 0, 0, 1);
  setLine(22, 22, 27, 22, 0, 0, 1);
  setLine(28, 23, 31, 23, 0, 0, 1);

  // Set additional pixel on playfield to "used" to ensure that ships, stars, and shots do not overwrite the cockpit
  for(byte i = 10; i < 22; i++){ playfield[i][22] = 1; }
  for(byte i = 4; i < 28; i++){ playfield[i][23] = 1; }

  // Controls

  // Radar
  matrix.drawLine(12, 22, 19, 22, matrix.Color333(1, 1, 1));
  matrix.drawLine(11, 23, 11, 30, matrix.Color333(1, 1, 1));
  matrix.drawLine(20, 23, 20, 30, matrix.Color333(1, 1, 1));
  matrix.drawLine(12, 31, 19, 31, matrix.Color333(1, 1, 1));
}

// updateShieldEnergyDisplay: Updates the shield energy bar on the screen
void updateShieldEnergyDisplay()
{
  for(byte i = 0; i < 9; i++)
  {
    if(i < shieldEnergy)
    {
      matrix.drawPixel(1 + i, 25, matrix.Color333(3, 0, 0));
      matrix.drawPixel(1 + i, 26, matrix.Color333(1, 0, 0));
    }
    else
    {
      matrix.drawPixel(1 + i, 25, matrix.Color333(0, 0, 0));
      matrix.drawPixel(1 + i, 26, matrix.Color333(0, 0, 0));
    }
  }
}

// updateBoostEnergyDisplay: Updates the boost energy bar on the screen
void updateBoostEnergyDisplay()
{
  for(byte i = 0; i < 9; i++)
  {
    if(i < (boostEnergy / 10))
    {
      matrix.drawPixel(1 + i, 29, matrix.Color333(0, 0, 3));
      matrix.drawPixel(1 + i, 30, matrix.Color333(0, 0, 1));
    }
    else
    {
      matrix.drawPixel(1 + i, 29, matrix.Color333(0, 0, 0));
      matrix.drawPixel(1 + i, 30, matrix.Color333(0, 0, 0));
    }    
  }  
}

// updateWeaponEnergyDisplay: Updates the weapon energy bar on the screen
void updateWeaponEnergyDisplay()
{
  for(byte i = 0; i < 9; i++)
  {
    if(i < weaponEnergy)
    {
      matrix.drawPixel(22 + i, 25, matrix.Color333(3, 3, 0));
      matrix.drawPixel(22 + i, 26, matrix.Color333(1, 1, 0));
    }
    else
    {
      matrix.drawPixel(22 + i, 25, matrix.Color333(0, 0, 0));
      matrix.drawPixel(22 + i, 26, matrix.Color333(0, 0, 0));
    }    
  }  
}

// updateMissileDisplay: Updates the number of remaining missiles on the screen
void updateMissileDisplay()
{
  for(byte i = 0; i < 5; i++)
  {
    if(i < missiles)
    {
      matrix.drawPixel(22 + (i * 2), 29, matrix.Color333(3, 3, 3));
      matrix.drawPixel(22 + (i * 2), 30, matrix.Color333(1, 1, 1));      
    }
    else
    {
      matrix.drawPixel(22 + (i * 2), 29, matrix.Color333(0, 0, 0));
      matrix.drawPixel(22 + (i * 2), 30, matrix.Color333(0, 0, 0));      
    }
  }  
}

// blinkMissile: Let the missile display blink if missile is ready to fire
void blinkMissile()
{
  if(counter % 8 < 4)
  {
    matrix.drawPixel(22 + ((missiles - 1) * 2), 29, matrix.Color333(5, 5, 5));
    matrix.drawPixel(22 + ((missiles - 1) * 2), 30, matrix.Color333(3, 3, 3));
  }
  else
  {
    matrix.drawPixel(22 + ((missiles - 1) * 2), 29, matrix.Color333(0, 0, 0));
    matrix.drawPixel(22 + ((missiles - 1) * 2), 30, matrix.Color333(0, 0, 0));              
  }
  if(counter % 16 == 0)
  {    
    // SOUND: Target locked
    tone(audio, 400, 50);          
  }
}

// drawCrosshairs: Draws the crosshair. Color depends on status: Normal (blue), locking (white), locked (red)
void drawCrosshairs()
{
  if(locking)
  {
    matrix.drawPixel(15, 8, matrix.Color333(1, 1, 1));
    matrix.drawPixel(16, 8, matrix.Color333(1, 1, 1));
    matrix.drawPixel(13, 10, matrix.Color333(1, 1, 1));
    matrix.drawPixel(18, 10, matrix.Color333(1, 1, 1));
    matrix.drawPixel(15, 12, matrix.Color333(1, 1, 1));
    matrix.drawPixel(16, 12, matrix.Color333(1, 1, 1));
  }
  else if(targetLocked)
  {
    matrix.drawPixel(15, 8, matrix.Color333(1, 0, 0));
    matrix.drawPixel(16, 8, matrix.Color333(1, 0, 0));
    matrix.drawPixel(13, 10, matrix.Color333(1, 0, 0));
    matrix.drawPixel(18, 10, matrix.Color333(1, 0, 0));
    matrix.drawPixel(15, 12, matrix.Color333(1, 0, 0));
    matrix.drawPixel(16, 12, matrix.Color333(1, 0, 0));    
  }
  else
  {
    matrix.drawPixel(15, 8, matrix.Color333(0, 0, 1));
    matrix.drawPixel(16, 8, matrix.Color333(0, 0, 1));
    matrix.drawPixel(13, 10, matrix.Color333(0, 0, 1));
    matrix.drawPixel(18, 10, matrix.Color333(0, 0, 1));
    matrix.drawPixel(15, 12, matrix.Color333(0, 0, 1));
    matrix.drawPixel(16, 12, matrix.Color333(0, 0, 1));    
  }
}



// Stars and planets

// updateStarfield: Updates the starfield effect by moving and redrawing the stars
void updateStarfield()
{
  // Start new star
  if(random(100) < 15 && star[nextStar].counter == 0)
  {
    star[nextStar].counter = 1;
    star[nextStar].x = 16.0 + originOffsetX;
    star[nextStar].y = 12.0 + originOffsetY;
    int angle = random(360);
    star[nextStar].xDirection = sin(angle);
    star[nextStar].yDirection = cos(angle);
    nextStar++;
    if(nextStar == numberOfStars){ nextStar = 0; }
  }
  for(byte i = 0; i < numberOfStars; i++)
  {
    if(star[i].counter > 0)
    {
      // Remove star from old position
      if(isVisible(byte(star[i].x), byte(star[i].y))){ matrix.drawPixel(byte(star[i].x), byte(star[i].y), matrix.Color333(0, 0, 0)); }

      // Move star
      star[i].x = star[i].x + (star[i].xDirection / 2.0);
      star[i].y = star[i].y + (star[i].yDirection / 2.0);
      star[i].xDirection = star[i].xDirection * (1.025 + (extraThrust * 3));
      star[i].yDirection = star[i].yDirection * (1.025 + (extraThrust * 3));
      star[i].counter++;

      // Check whether star is still active
      if(star[i].x < 0 || star[i].x > 32 || star[i].y < 0 || star[i].y > 23){ star[i].counter = 0; }
    }

    // Draw star at new position if still active
    if(star[i].counter > 5 && isVisible(byte(star[i].x), byte(star[i].y))){ matrix.drawPixel(byte(star[i].x), byte(star[i].y), matrix.Color333(1, 1, 3)); }
  }
}

// drawPlanet: Draw a planet of type 'number' at a defined position. If draw == false, the planet is removed.
void drawPlanet(byte number, byte x, byte y, byte draw)
{

  planetOffset = byte(pgm_read_byte_near(planets + number - 1));
  planetWidth = byte(pgm_read_byte_near(planets + planetOffset));
  planetHeight = byte(pgm_read_byte_near(planets + planetOffset + 1));
  planetOffset = planetOffset + 2;
  
  for(byte i = 0; i < planetHeight; i++)
  {
    for(byte j = 0; j < planetWidth; j++)
    {
      if(draw)
      {
        drawPixel(x + j, y + i, byte(pgm_read_byte_near(planets + planetOffset + (i * planetWidth) + j)));
      }
      else
      {
        matrix.drawPixel(x + j, y + i, matrix.Color333(0, 0, 0));        
      }
    }
  }  
}

// updateVega: Update central star Vega depending on player´s ship position
void updateVega()
{
  // Remove Vega from old position
  if(vegaX > (byte(upperLeftX) - 2) && vegaX < (byte(upperLeftX) + 32) && vegaY > (byte(upperLeftY) - 2) && vegaY < (byte(upperLeftY) + 23))
  {
    if(isVisible(vegaX - byte(upperLeftX), vegaY - byte(upperLeftY))){ matrix.drawPixel(vegaX - byte(upperLeftX), vegaY - byte(upperLeftY), matrix.Color333(0, 0, 0)); }
    if(isVisible(vegaX - byte(upperLeftX) + 1, vegaY - byte(upperLeftY))){ matrix.drawPixel(vegaX - byte(upperLeftX) + 1, vegaY - byte(upperLeftY), matrix.Color333(0, 0, 0)); }
    if(isVisible(vegaX - byte(upperLeftX), vegaY - byte(upperLeftY) + 1)){ matrix.drawPixel(vegaX - byte(upperLeftX), vegaY - byte(upperLeftY) + 1, matrix.Color333(0, 0, 0)); }
    if(isVisible(vegaX - byte(upperLeftX) + 1, vegaY - byte(upperLeftY) + 1)){ matrix.drawPixel(vegaX - byte(upperLeftX) + 1, vegaY - byte(upperLeftY) + 1, matrix.Color333(0, 0, 0)); }
  }
  // Remove Vega from radar
  matrix.drawPixel(12 + (4 + byte((vegaX - upperLeftX - 31.5) / 16.0)) % 8, 23 + (4 + byte((vegaY - upperLeftY - 26.5) / 16.0)) % 8, matrix.Color333(0, 0, 0));      
  
  // Draw Vega to new position
  if(vegaX > (byte(upperLeftXNew) - 2) && vegaX < (byte(upperLeftXNew) + 32) && vegaY > (byte(upperLeftYNew) - 2) && vegaY < (byte(upperLeftYNew) + 23))
  {
    if(isVisible(vegaX - byte(upperLeftXNew), vegaY - byte(upperLeftYNew))){ matrix.drawPixel(vegaX - byte(upperLeftXNew), vegaY - byte(upperLeftYNew), matrix.Color333(1 + random(3), 1 + random(3), 1 + random(3))); }
    if(isVisible(vegaX - byte(upperLeftXNew) + 1, vegaY - byte(upperLeftYNew))){ matrix.drawPixel(vegaX - byte(upperLeftXNew) + 1, vegaY - byte(upperLeftYNew), matrix.Color333(1 + random(3), 1 + random(3), 1 + random(3))); }
    if(isVisible(vegaX - byte(upperLeftXNew), vegaY - byte(upperLeftYNew) + 1)){ matrix.drawPixel(vegaX - byte(upperLeftXNew), vegaY - byte(upperLeftYNew) + 1, matrix.Color333(1 + random(3), 1 + random(3), 1 + random(3))); }
    if(isVisible(vegaX - byte(upperLeftXNew) + 1, vegaY - byte(upperLeftYNew) + 1)){ matrix.drawPixel(vegaX - byte(upperLeftXNew) + 1, vegaY - byte(upperLeftYNew) + 1, matrix.Color333(1 + random(3), 1 + random(3), 1 + random(3))); }
  }
  // Draw Vega on radar
  matrix.drawPixel(12 + (4 + byte((vegaX - upperLeftXNew - 31.5) / 16.0)) % 8, 23 + (4 + byte((vegaY - upperLeftYNew - 26.5) / 16.0)) % 8, matrix.Color333(1, 1, 0));      
}

// updatePlanet: Update current planet depending on player´s ship position
void updatePlanet()
{
  // Remove planet from old position
  if(planetX > (byte(upperLeftX) - planetWidth) && planetX < (byte(upperLeftX) + 32) && planetY > (byte(upperLeftY) - planetHeight) && planetY < (byte(upperLeftY) + 24))
  {
    for(byte i = 0; i < planetHeight; i++)
    {
      for(byte j = 0; j < planetWidth; j++)
      {
        if(isVisible(byte(planetX - upperLeftX + j), byte(planetY - upperLeftY + i)))
        {
          drawPixel(byte(planetX - upperLeftX + j), byte(planetY - upperLeftY + i), BL);
        }
      }
    }
  }
  // Remove planet from radar
  matrix.drawPixel(12 + (4 + byte((planetX - upperLeftX - 31.5) / 16.0)) % 8, 23 + (4 + byte((planetY - upperLeftY - 26.5) / 16.0)) % 8, matrix.Color333(0, 0, 0));      
    
  // Draw planet to new position
  if(planetX > (byte(upperLeftXNew) - planetWidth) && planetX < (byte(upperLeftXNew) + 32) && planetY > (byte(upperLeftYNew) - planetHeight) && planetY < (byte(upperLeftYNew) + 24))
  {
    for(byte i = 0; i < planetHeight; i++)
    {
      for(byte j = 0; j < planetWidth; j++)
      {
        if(isVisible(byte(planetX - upperLeftXNew + j), byte(planetY - upperLeftYNew + i)))
        {
          drawPixel(byte(planetX - upperLeftXNew + j), byte(planetY - upperLeftYNew + i), byte(pgm_read_byte_near(planets + planetOffset + (i * planetWidth) + j)));
        }
      }
    }
  }
  // Draw planet on radar
  matrix.drawPixel(12 + (4 + byte((planetX - upperLeftXNew - 31.5) / 16.0)) % 8, 23 + (4 + byte((planetY - upperLeftYNew - 26.5) / 16.0)) % 8, matrix.Color333(0, 0, 1));      
}



// Enemy ships

// initializeShip: Initializes a new enemy ship with a specific type at a random position
void initializeShip(byte type)
{
  initializeShip(type, random(128), random(128));
}

// initializeShip: Initializes a new enemy ship with a specific type at a defined x/y-position
void initializeShip(byte type, float xPosition, float yPosition)
{
  if(!ship[nextShip].active)
  {
    switch(type)
    {
      case SCOUT:
        ship[nextShip].shield = 1;
        ship[nextShip].fireRateFront = 3;
        ship[nextShip].fireRateRear = 0;
        ship[nextShip].fireDistance = 16;
        ship[nextShip].fireAccuracy = 4;
        ship[nextShip].missileRateFront = 0;
        ship[nextShip].missileRateRear = 0;
        ship[nextShip].zSpeed = 0.4;
        ship[nextShip].sideSpeed = 0.25;
        ship[nextShip].flightType = EVASION_FLIGHT_TYPE;
        break;
      case FIGHTER:
        ship[nextShip].shield = 2;
        ship[nextShip].fireRateFront = 4;
        ship[nextShip].fireRateRear = 0;
        ship[nextShip].fireDistance = 24;
        ship[nextShip].fireAccuracy = 4;
        ship[nextShip].missileRateFront = 0;
        ship[nextShip].missileRateRear = 0;
        ship[nextShip].zSpeed = 0.3;
        ship[nextShip].sideSpeed = 0.2;
        ship[nextShip].flightType = EVASION_FLIGHT_TYPE;
        break;
      case HEAVYFIGHTER:
        ship[nextShip].shield = 3;
        ship[nextShip].fireRateFront = 5;
        ship[nextShip].fireRateRear = 0;
        ship[nextShip].fireDistance = 24;
        ship[nextShip].fireAccuracy = 4;
        ship[nextShip].missileRateFront = 0;
        ship[nextShip].missileRateRear = 0;
        ship[nextShip].zSpeed = 0.25;
        ship[nextShip].sideSpeed = 0.15;
        ship[nextShip].flightType = EVASION_FLIGHT_TYPE;
        break;
      case BOMBER:
        ship[nextShip].shield = 4;
        ship[nextShip].fireRateFront = 0;
        ship[nextShip].fireRateRear = 3;
        ship[nextShip].fireDistance = 16;
        ship[nextShip].fireAccuracy = 6;
        ship[nextShip].missileRateFront = 2;
        ship[nextShip].missileRateRear = 0;
        ship[nextShip].zSpeed = 0.2;
        ship[nextShip].sideSpeed = 0.1;
        ship[nextShip].flightType = EVASION_FLIGHT_TYPE;
        break;
      case SATELLITE:
        ship[nextShip].shield = 3;
        ship[nextShip].fireRateFront = 4;
        ship[nextShip].fireRateRear = 4;
        ship[nextShip].fireDistance = 32;
        ship[nextShip].fireAccuracy = 6;
        ship[nextShip].missileRateFront = 0;
        ship[nextShip].missileRateRear = 0;
        ship[nextShip].zSpeed = 0.0;
        ship[nextShip].sideSpeed = 0.0;
        ship[nextShip].flightType = EVASION_FLIGHT_TYPE;
        break;
      case HAUNEBU:
        ship[nextShip].shield = 5;
        ship[nextShip].fireRateFront = 5;
        ship[nextShip].fireRateRear = 5;
        ship[nextShip].fireDistance = 32;
        ship[nextShip].fireAccuracy = 6;
        ship[nextShip].missileRateFront = 0;
        ship[nextShip].missileRateRear = 0;
        ship[nextShip].zSpeed = 0.2;
        ship[nextShip].sideSpeed = 0.1;
        ship[nextShip].flightType = EVASION_FLIGHT_TYPE;
        break;
      case CORVETTE:
        ship[nextShip].shield = 6;
        ship[nextShip].fireRateFront = 6;
        ship[nextShip].fireRateRear = 6;
        ship[nextShip].fireDistance = 32;
        ship[nextShip].fireAccuracy = 8;
        ship[nextShip].missileRateFront = 2;
        ship[nextShip].missileRateRear = 2;
        ship[nextShip].zSpeed = 0.15;
        ship[nextShip].sideSpeed = 0.05;
        ship[nextShip].flightType = ACCELERATE_FLIGHT_TYPE;
        break;
      case DESTROYER:
        ship[nextShip].shield = 8;
        ship[nextShip].fireRateFront = 7;
        ship[nextShip].fireRateRear = 7;
        ship[nextShip].fireDistance = 32;
        ship[nextShip].fireAccuracy = 8;
        ship[nextShip].missileRateFront = 3;
        ship[nextShip].missileRateRear = 3;
        ship[nextShip].zSpeed = 0.15;
        ship[nextShip].sideSpeed = 0.05;
        ship[nextShip].flightType = ACCELERATE_FLIGHT_TYPE;
        break;
      default:
        break;                      
    }
    ship[nextShip].type = type;
    ship[nextShip].xPosition = xPosition;
    ship[nextShip].yPosition = yPosition;
    ship[nextShip].zPosition = 120.0 + float(random(8));
    ship[nextShip].xSpeed = 0.0;
    ship[nextShip].ySpeed = 0.0;
    ship[nextShip].frontVisible = true;
    ship[nextShip].active = true;
    ship[nextShip].hasBeenHit = 0;
    ship[nextShip].fireCounter = 0;    
    ship[nextShip].flightMode = REGULAR_FLIGHT_MODE;
    ship[nextShip].flightModeCounter = 0.0;
    ship[nextShip].xEvasion = 0.0;
    ship[nextShip].yEvasion = 0.0;
  }
  nextShip++;
  if(nextShip == numberOfShips){ nextShip = 0; }
}

// shipVisible: Returns true if ship with index i is visible through the cockpit window
boolean shipVisible(byte i, float x, float y)
{
  if(x < 111 && y < 111)
  {
    if(!(ship[i].xPosition > (byte(x) - (ship[i].screenWidth / 2)) && ship[i].xPosition < (byte(x) + 32))){ return false; }
    if(!(ship[i].yPosition > (byte(y) - (ship[i].screenHeight / 2)) && ship[i].yPosition < (byte(y) + 24))){ return false; }
    return true;
  }
  else
  {
    if(! ((ship[i].xPosition > (byte(x) - (ship[i].screenWidth / 2)) && ship[i].xPosition < (byte(x) + 32)) || ((ship[i].xPosition + 128.0) > (byte(x) - (ship[i].screenWidth / 2)) && ship[i].xPosition < (byte(x) + 32)) )){ return false; }
    if(! ((ship[i].yPosition > (byte(y) - (ship[i].screenHeight / 2)) && ship[i].yPosition < (byte(y) + 24)) || ((ship[i].yPosition + 128.0) > (byte(y) - (ship[i].screenHeight / 2)) && ship[i].yPosition < (byte(y) + 24)) )){ return false; }
    return true;
  }
}

// updateShips: Update all enemy ships
void updateShips()
{
  for(byte i = 0; i < numberOfShips; i++)
  {
    if(ship[i].active)
    {    
      xPos = ship[i].xPosition - (ship[i].screenWidth / 2) - upperLeftX;
      yPos = ship[i].yPosition - (ship[i].screenHeight / 2) - upperLeftY;

      // Remove ship from old position
      if(ship[i].zPosition < 64.0 && shipVisible(i, upperLeftX, upperLeftY))
      {
        for(byte j = 0; j < ship[i].screenHeight; j++)
        {
          for(byte k = 0; k < ship[i].screenWidth; k++)
          {
            if(isVisible(byte(xPos + k) % 128, byte(yPos + j) % 128))
            {
              drawPixel(byte(xPos + k) % 128, byte(yPos + j) % 128, BL);
            }
          }
        }
      }

      // Remove ship from radar
      matrix.drawPixel(12 + (4 + byte((ship[i].xPosition - upperLeftX - 31.5) / 16.0)) % 8, 23 + (4 + byte((ship[i].yPosition - upperLeftY - 26.5) / 16.0)) % 8, matrix.Color333(0, 0, 0));      

      // Effects of players movement on ship position
      objectXDirection = ship[i].xPosition - xVector;
      objectXDistance = abs(objectXDirection);
      if(abs(ship[i].xPosition - xVector - 128.0) < objectXDistance)
      { 
        objectXDistance = abs(ship[i].xPosition - xVector - 128.0);
        objectXDirection = objectXDirection - 128.0;
      }
      objectYDirection = ship[i].yPosition - yVector;
      objectYDistance = abs(objectYDirection);
      if(abs(ship[i].yPosition - yVector - 128.0) < objectYDistance)
      { 
        objectYDistance = abs(ship[i].yPosition - yVector - 128.0);
        objectYDirection = objectYDirection - 128.0;
      }
      objectOverallDistance = objectXDistance + objectYDistance;
      if(objectOverallDistance > 64.0){ objectOverallDistance = 64.0; }

      objectNormFactor = sqrt(sq(objectXDirection) + sq(objectYDirection));

      if(objectNormFactor > 0)
      {
        ship[i].xPosition = ship[i].xPosition + (objectXDirection / objectNormFactor) * (sin(objectXDistance / 20.3718327158) / (ship[i].zPosition + 1.0) * 5.0);
        ship[i].yPosition = ship[i].yPosition + (objectYDirection / objectNormFactor) * (sin(objectYDistance / 20.3718327158) / (ship[i].zPosition + 1.0) * 5.0);      
      }
      ship[i].zPosition = ship[i].zPosition - (cos(objectOverallDistance / 20.3718327158) * (1.0 + extraThrust * 30.0)) * 0.5;
      
      // Ship fires
      if(ship[i].frontVisible)
      {
        if(random(100) < ship[i].fireRateFront && ship[i].zPosition > 6.0 && ship[i].zPosition < ship[i].fireDistance)
        {
          initializeShot(ship[i].xPosition, ship[i].yPosition, ship[i].zPosition, ENEMY_SHOT, ship[i].fireAccuracy);
          ship[i].fireCounter = 3;
        }
        if(random(100) < ship[i].missileRateFront && ship[i].zPosition > 12.0 && ship[i].zPosition < 32.0) // Missiles have a maximum range of 32 and the accuracy parameter has no impact
        {
          initializeShot(ship[i].xPosition, ship[i].yPosition, ship[i].zPosition, ENEMY_MISSILE, ship[i].fireAccuracy);
        }
      }
      else
      {
        if(random(100) < ship[i].fireRateRear && ship[i].zPosition > 6.0 && ship[i].zPosition < ship[i].fireDistance)
        {
          initializeShot(ship[i].xPosition, ship[i].yPosition, ship[i].zPosition, ENEMY_SHOT, ship[i].fireAccuracy);
          ship[i].fireCounter = 3;
        }
        if(random(100) < ship[i].missileRateRear && ship[i].zPosition > 12.0 && ship[i].zPosition < 32.0) // Missiles have a maximum range of 32 and the accuracy parameter has no impact
        {
          initializeShot(ship[i].xPosition, ship[i].yPosition, ship[i].zPosition, ENEMY_MISSILE, ship[i].fireAccuracy);
        }        
      }

      // Ship movement

      // Ship evades to avoid collision (Evasion Flight Type)
      if(ship[i].flightType == EVASION_FLIGHT_TYPE && ship[i].flightMode == REGULAR_FLIGHT_MODE && ship[i].zPosition < 4.0 && shipVisible(i, upperLeftX, upperLeftY))
      {
        // Ship switches to evasion flight mode to get it out of player´s view (to make it look more realistic since the ship cannot be viewed larger if distance is closer)
        ship[i].flightMode = EVASION_FLIGHT_MODE;
        ship[i].flightModeCounter = 0.0;
        int angle = random(360);
        ship[i].xEvasion = sin(angle) / 2.0;
        ship[i].yEvasion = cos(angle) / 2.0;        
      }
      if(ship[i].flightType == EVASION_FLIGHT_TYPE && ship[i].flightMode == EVASION_FLIGHT_MODE)
      {
        // Evade
        ship[i].xPosition = ship[i].xPosition + ship[i].xEvasion;
        ship[i].yPosition = ship[i].yPosition + ship[i].yEvasion;
        ship[i].xEvasion = ship[i].xEvasion * 1.05;
        ship[i].yEvasion = ship[i].yEvasion * 1.05;
        ship[i].flightModeCounter = ship[i].flightModeCounter + sqrt(sq(ship[i].xEvasion) + sq(ship[i].yEvasion));
        if(ship[i].zPosition < 2.0){ ship[i].zPosition = 2.0; }
        if(ship[i].flightModeCounter > 63.0)
        {
          // Switch back since it is guaranteed that the player cannot see the ship
          ship[i].flightMode = REGULAR_FLIGHT_MODE;
          if(ship[i].frontVisible){ ship[i].frontVisible = false; }
          else{ ship[i].frontVisible = true; }
        }
      }
      
      // Ship accelerates to avoid collision (Acceleration Flight Type)
      if(ship[i].flightType == ACCELERATE_FLIGHT_TYPE && ship[i].flightMode == REGULAR_FLIGHT_MODE && ship[i].zPosition < 4.0 && ship[i].frontVisible)
      {
        ship[i].flightMode = ACCELERATE_FLIGHT_MODE;
        ship[i].zPosition = 4.0;
        ship[i].frontVisible = false;
        ship[i].zSpeed = ship[i].zSpeed + 0.25;
      }
      else if(ship[i].flightType == ACCELERATE_FLIGHT_TYPE && ship[i].flightMode == ACCELERATE_FLIGHT_MODE && ship[i].zPosition > 127.0 && !ship[i].frontVisible)
      {
        ship[i].flightMode = REGULAR_FLIGHT_MODE;
        ship[i].zPosition = 127.0;
        ship[i].frontVisible = true;        
        ship[i].zSpeed = ship[i].zSpeed - 0.25;
      }

      // Attack (If ship is out of sight it accelerates to reduce distance to player´s ship. Note: If it is in sight, the distane is anyway reduced.)
      if(ship[i].flightMode == REGULAR_FLIGHT_MODE)
      {
        if(random(100) < 5 && !shipVisible(i, upperLeftX, upperLeftY))
        {
          ship[i].flightMode = ATTACK_FLIGHT_MODE;
        }
      }
      if(ship[i].flightMode == ATTACK_FLIGHT_MODE && shipVisible(i, upperLeftX, upperLeftY))
      {
        ship[i].flightMode = REGULAR_FLIGHT_MODE;  
      }
      
      // Ship movement according to speed vector
      if(ship[i].flightMode == REGULAR_FLIGHT_MODE || ship[i].flightMode == ACCELERATE_FLIGHT_MODE) // If ship is in attack mode or has been locked as target, it increased x/y speed
      {
        ship[i].xPosition = ship[i].xPosition + ship[i].xSpeed;
        ship[i].yPosition = ship[i].yPosition + ship[i].ySpeed;
      }
      else if(ship[i].flightMode == ATTACK_FLIGHT_MODE || (targetLocked && target == i))
      {
        ship[i].xPosition = ship[i].xPosition + (ship[i].xSpeed * 3.0);
        ship[i].yPosition = ship[i].yPosition + (ship[i].ySpeed * 3.0);        
      }

      // Update x/y-movement
      if(random(100) < 5)
      {
        int angle = random(360);
        ship[i].xSpeed = sin(angle) * ship[i].sideSpeed;
        ship[i].ySpeed = cos(angle) * ship[i].sideSpeed;  
      }

      // Update z-movement (0 = closest distance)
      if(ship[i].flightMode == REGULAR_FLIGHT_MODE || ship[i].flightMode == ACCELERATE_FLIGHT_MODE)
      {
        if(ship[i].frontVisible){ ship[i].zPosition = ship[i].zPosition - ship[i].zSpeed; }
        else{                     ship[i].zPosition = ship[i].zPosition + ship[i].zSpeed; }
      }
      else if(ship[i].flightMode == ATTACK_FLIGHT_MODE)
      {
        if(ship[i].frontVisible){ ship[i].zPosition = ship[i].zPosition - (ship[i].zSpeed * 2.5); }
        else{                     ship[i].zPosition = ship[i].zPosition + (ship[i].zSpeed * 2.5); }        
      }
      
      // Ship turns (if minimal or maximal z-position has been reached)
      if(ship[i].zPosition < 0.0)
      {
        ship[i].flightMode = REGULAR_FLIGHT_MODE;
        ship[i].zPosition = 0.0;
        if(ship[i].frontVisible){ ship[i].frontVisible = false; }
        else{ ship[i].frontVisible = true; }
        if(random(100) > 50){ ship[i].xPosition = ship[i].xPosition + 64.0; }
        else{ ship[i].yPosition = ship[i].yPosition + 64.0; }                   
      }
      else if(ship[i].zPosition > 127.0 && ship[i].flightMode != ACCELERATE_FLIGHT_MODE)
      {
        ship[i].zPosition = 127.0;
        ship[i].frontVisible = true;
      }

      // Make sure that ship stays inside the 128*128 playfield
      if(ship[i].xPosition < 0.0){ ship[i].xPosition = ship[i].xPosition + 128.0; }
      else if(ship[i].xPosition >= 128.0){ ship[i].xPosition = ship[i].xPosition - 128.0; }
      if(ship[i].yPosition < 0.0){ ship[i].yPosition = ship[i].yPosition + 128.0; }
      else if(ship[i].yPosition >= 128.0){ ship[i].yPosition = ship[i].yPosition - 128.0; }

      // Calculate bitmap to be shown
      if(ship[i].frontVisible)
      {
        if(ship[i].zPosition >= 32.0 && ship[i].zPosition < 64.0){ bitmap = 0; }
        else if(ship[i].zPosition >= 16.0)                       { bitmap = 1; }
        else if(ship[i].zPosition >= 8.0)                        { bitmap = 2; }
        else if(ship[i].zPosition < 8.0)                         { bitmap = 3; }
      }
      else
      {
        if(ship[i].zPosition >= 32.0 && ship[i].zPosition < 64.0){ bitmap = 4; }
        else if(ship[i].zPosition >= 16.0)                       { bitmap = 5; }
        else if(ship[i].zPosition >= 8.0)                        { bitmap = 6; }
        else if(ship[i].zPosition < 8.0)                         { bitmap = 7; }
      }      
      switch(ship[i].type)
      {
        case SCOUT:
          ship[i].bitmapOffset = byte(pgm_read_byte_near(scout + bitmap));
          ship[i].screenWidth = byte(pgm_read_byte_near(scout + ship[i].bitmapOffset));
          ship[i].screenHeight = byte(pgm_read_byte_near(scout + ship[i].bitmapOffset + 1));
          ship[i].bitmapOffset = ship[i].bitmapOffset + 2;
          break;
        case FIGHTER:
          ship[i].bitmapOffset = byte(pgm_read_byte_near(fighter + bitmap));
          ship[i].screenWidth = byte(pgm_read_byte_near(fighter + ship[i].bitmapOffset));
          ship[i].screenHeight = byte(pgm_read_byte_near(fighter + ship[i].bitmapOffset + 1));
          ship[i].bitmapOffset = ship[i].bitmapOffset + 2;
          break;
        case HEAVYFIGHTER:
          ship[i].bitmapOffset = byte(pgm_read_byte_near(heavyfighter + bitmap));
          ship[i].screenWidth = byte(pgm_read_byte_near(heavyfighter + ship[i].bitmapOffset));
          ship[i].screenHeight = byte(pgm_read_byte_near(heavyfighter + ship[i].bitmapOffset + 1));
          ship[i].bitmapOffset = ship[i].bitmapOffset + 2;
          break;
        case BOMBER:
          ship[i].bitmapOffset = byte(pgm_read_byte_near(bomber + bitmap));
          ship[i].screenWidth = byte(pgm_read_byte_near(bomber + ship[i].bitmapOffset));
          ship[i].screenHeight = byte(pgm_read_byte_near(bomber + ship[i].bitmapOffset + 1));
          ship[i].bitmapOffset = ship[i].bitmapOffset + 2;
          break;
        case SATELLITE:
          ship[i].bitmapOffset = byte(pgm_read_byte_near(satellite + bitmap));
          ship[i].screenWidth = byte(pgm_read_byte_near(satellite + ship[i].bitmapOffset));
          ship[i].screenHeight = byte(pgm_read_byte_near(satellite + ship[i].bitmapOffset + 1));
          ship[i].bitmapOffset = ship[i].bitmapOffset + 2;
          break;
        case HAUNEBU:
          ship[i].bitmapOffset = byte(pgm_read_byte_near(haunebu + bitmap));
          ship[i].screenWidth = byte(pgm_read_byte_near(haunebu + ship[i].bitmapOffset));
          ship[i].screenHeight = byte(pgm_read_byte_near(haunebu + ship[i].bitmapOffset + 1));
          ship[i].bitmapOffset = ship[i].bitmapOffset + 2;
          break;
        case CORVETTE:
          ship[i].bitmapOffset = byte(pgm_read_byte_near(corvette + bitmap));
          ship[i].screenWidth = byte(pgm_read_byte_near(corvette + ship[i].bitmapOffset));
          ship[i].screenHeight = byte(pgm_read_byte_near(corvette + ship[i].bitmapOffset + 1));
          ship[i].bitmapOffset = ship[i].bitmapOffset + 2;
          break;
        case DESTROYER:
          ship[i].bitmapOffset = byte(pgm_read_byte_near(destroyer + bitmap));
          ship[i].screenWidth = byte(pgm_read_byte_near(destroyer + ship[i].bitmapOffset));
          ship[i].screenHeight = byte(pgm_read_byte_near(destroyer + ship[i].bitmapOffset + 1));
          ship[i].bitmapOffset = ship[i].bitmapOffset + 2;
          break;
        default:
          break;                      
      }

      // Check if ship has been destroyed
      if(ship[i].shield <= 0)
      { 
        initializeExplosion(ship[i].xPosition, ship[i].yPosition, ship[i].zPosition); 
        ship[i].active = false;
        targetLocked = false; // Important: Release target since it might have been already destroyed
        killsInWave++;
      }
  
      // Draw ship to new position
      if(ship[i].active)
      {
        xPos = ship[i].xPosition - (ship[i].screenWidth / 2) - upperLeftXNew;
        yPos = ship[i].yPosition - (ship[i].screenHeight / 2) - upperLeftYNew;

        if(ship[i].zPosition < 64.0 && shipVisible(i, upperLeftXNew, upperLeftYNew))
        {
          if(ship[i].hasBeenHit == 0)
          {
            for(byte j = 0; j < ship[i].screenHeight; j++)
            {
              for(byte k = 0; k < ship[i].screenWidth; k++)
              {
                if(isVisible(byte(xPos + k) % 128, byte(yPos + j) % 128))
                {
                  switch(ship[i].type)
                  {
                    case SCOUT:
                      drawPixel(byte(xPos + k) % 128, byte(yPos + j) % 128, byte(pgm_read_byte_near(scout + ship[i].bitmapOffset + (j * ship[i].screenWidth) + k)));
                      break;
                    case FIGHTER:
                      drawPixel(byte(xPos + k) % 128, byte(yPos + j) % 128, byte(pgm_read_byte_near(fighter + ship[i].bitmapOffset + (j * ship[i].screenWidth) + k)));
                      break;
                    case HEAVYFIGHTER:
                      drawPixel(byte(xPos + k) % 128, byte(yPos + j) % 128, byte(pgm_read_byte_near(heavyfighter + ship[i].bitmapOffset + (j * ship[i].screenWidth) + k)));
                      break;
                    case BOMBER:
                      drawPixel(byte(xPos + k) % 128, byte(yPos + j) % 128, byte(pgm_read_byte_near(bomber + ship[i].bitmapOffset + (j * ship[i].screenWidth) + k)));
                      break;
                    case SATELLITE:
                      drawPixel(byte(xPos + k) % 128, byte(yPos + j) % 128, byte(pgm_read_byte_near(satellite + ship[i].bitmapOffset + (j * ship[i].screenWidth) + k)));
                      break;
                    case HAUNEBU:
                      drawPixel(byte(xPos + k) % 128, byte(yPos + j) % 128, byte(pgm_read_byte_near(haunebu + ship[i].bitmapOffset + (j * ship[i].screenWidth) + k)));
                      break;
                    case CORVETTE:
                      drawPixel(byte(xPos + k) % 128, byte(yPos + j) % 128, byte(pgm_read_byte_near(corvette + ship[i].bitmapOffset + (j * ship[i].screenWidth) + k)));
                      break;
                    case DESTROYER:
                      drawPixel(byte(xPos + k) % 128, byte(ship[i].yPosition - (ship[i].screenHeight / 2) - upperLeftYNew + j) % 128, byte(pgm_read_byte_near(destroyer + ship[i].bitmapOffset + (j * ship[i].screenWidth) + k)));
                      break;
                    default:
                      break;                      
                  }
                }
              }
            }
          }
          else
          {
            for(byte j = 0; j < ship[i].screenHeight; j++)
            {
              for(byte k = 0; k < ship[i].screenWidth; k++)
              {
                if(isVisible(byte(xPos + k) % 128, byte(yPos + j) % 128))
                {
                  switch(ship[i].type)
                  {
                    case SCOUT:
                      if(byte(pgm_read_byte_near(scout + ship[i].bitmapOffset + (j * ship[i].screenWidth) + k)) != TP)
                      {
                        drawPixel(byte(xPos + k) % 128, byte(yPos + j) % 128, W3);
                      }
                      break;
                    case FIGHTER:
                      if(byte(pgm_read_byte_near(fighter + ship[i].bitmapOffset + (j * ship[i].screenWidth) + k)) != TP)
                      {
                        drawPixel(byte(xPos + k) % 128, byte(yPos + j) % 128, W3);
                      }
                      break;
                    case HEAVYFIGHTER:
                      if(byte(pgm_read_byte_near(heavyfighter + ship[i].bitmapOffset + (j * ship[i].screenWidth) + k)) != TP)
                      {
                        drawPixel(byte(xPos + k) % 128, byte(yPos + j) % 128, W3);
                      }
                      break;
                    case BOMBER:
                      if(byte(pgm_read_byte_near(bomber + ship[i].bitmapOffset + (j * ship[i].screenWidth) + k)) != TP)
                      {
                        drawPixel(byte(xPos + k) % 128, byte(yPos + j) % 128, W3);
                      }
                      break;
                    case SATELLITE:
                      if(byte(pgm_read_byte_near(satellite + ship[i].bitmapOffset + (j * ship[i].screenWidth) + k)) != TP)
                      {
                        drawPixel(byte(xPos + k) % 128, byte(yPos + j) % 128, W3);
                      }
                      break;
                    case HAUNEBU:
                      if(byte(pgm_read_byte_near(haunebu + ship[i].bitmapOffset + (j * ship[i].screenWidth) + k)) != TP)
                      {
                        drawPixel(byte(xPos + k) % 128, byte(yPos + j) % 128, W3);
                      }
                      break;
                    case CORVETTE:
                      if(byte(pgm_read_byte_near(corvette + ship[i].bitmapOffset + (j * ship[i].screenWidth) + k)) != TP)
                      {
                        drawPixel(byte(xPos + k) % 128, byte(yPos + j) % 128, W3);
                      }
                      break;
                    case DESTROYER:
                      if(byte(pgm_read_byte_near(destroyer + ship[i].bitmapOffset + (j * ship[i].screenWidth) + k)) != TP)
                      {
                        drawPixel(byte(xPos + k) % 128, byte(yPos + j) % 128, W3);
                      }
                      break;
                    default:
                      break;                      
                  }
                }
              }
            }
            if(ship[i].hasBeenHit > 0){ ship[i].hasBeenHit = ship[i].hasBeenHit - 1; }
          }
        }
        // Draw ship on radar
        if(ship[i].fireCounter == 0)
        {
          matrix.drawPixel(12 + (4 + byte((ship[i].xPosition - upperLeftXNew - 31.5) / 16.0)) % 8, 23 + (4 + byte((ship[i].yPosition - upperLeftYNew - 26.5) / 16.0)) % 8, matrix.Color333(1, 1, 1));      
        }
        else
        {
          matrix.drawPixel(12 + (4 + byte((ship[i].xPosition - upperLeftXNew - 31.5) / 16.0)) % 8, 23 + (4 + byte((ship[i].yPosition - upperLeftYNew - 26.5) / 16.0)) % 8, matrix.Color333(7, 0, 0));                
        }
        if(ship[i].fireCounter > 0){ ship[i].fireCounter--; }
      }
    }   
  }
}



// Shots, missiles, and explosions

// initializeShot: Initialize player´s shot
void initializeShot()
{
  if(!shot[nextShot].active)
  {
    shot[nextShot].type = PLAYER_SHOT;
    shot[nextShot].active = true;
    shot[nextShot].zPosition = 0.0;
    shot[nextShot].zVector = 1.8;
    if(firedLeft)
    {
      shot[nextShot].xPosition = xVector - 18.5;
      shot[nextShot].yPosition = yVector + 9.25;
      shot[nextShot].xVector = 4.0;      
      shot[nextShot].yVector = -2.0;      
    }
    else
    {
      shot[nextShot].xPosition = xVector + 18.5;
      shot[nextShot].yPosition = yVector + 9.25;      
      shot[nextShot].xVector = -4.0;      
      shot[nextShot].yVector = -2.0;      
    }
    
    nextShot++;
    if(nextShot == numberOfShots){ nextShot = 0; }
  }
}

// initializeShot: Initialize a shot by an enemy ship
void initializeShot(float x, float y, float z, byte type, byte fireAccuracy)
{
  float xTarget;
  float yTarget;
  float xDistance = 255.0;
  float yDistance = 255.0;

  fireAccuracy = fireAccuracy + byte(z / 8);
  
  if(!shot[nextShot].active)
  {
    if(type == ENEMY_SHOT)
    {
      shot[nextShot].type = type;
      shot[nextShot].active = true;
      shot[nextShot].xPosition = x;
      shot[nextShot].yPosition = y;
      shot[nextShot].zPosition = z;
      shot[nextShot].zVector = -0.5;
      for(byte i = 0; i < 4; i++)
      {
        if(xDistance > abs(x - (byte(xVector + (i * 32.0)) % 128))){ xDistance = abs(x - (byte(xVector + (i * 32.0)) % 128)); xTarget = byte(xVector + (i * 32.0)) % 128; }
        if(yDistance > abs(y - (byte(yVector + (i * 32.0)) % 128))){ yDistance = abs(y - (byte(yVector + (i * 32.0)) % 128)); yTarget = byte(yVector + (i * 32.0)) % 128; }
      }
      shot[nextShot].xVector = (xTarget + random(fireAccuracy) - (fireAccuracy / 2) - shot[nextShot].xPosition) / (shot[nextShot].zPosition / shot[nextShot].zVector * -1.0);
      shot[nextShot].yVector = (yTarget + random(fireAccuracy) - (fireAccuracy / 2) - shot[nextShot].yPosition) / (shot[nextShot].zPosition / shot[nextShot].zVector * -1.0);   
      shot[nextShot].xOffset = xTarget - xVector;
      shot[nextShot].yOffset = yTarget - yVector;
    }
    else if(type == ENEMY_MISSILE)
    {
      shot[nextShot].type = type;
      shot[nextShot].active = true;
      shot[nextShot].xPosition = x;
      shot[nextShot].yPosition = y;
      shot[nextShot].zPosition = z;
      shot[nextShot].zVector = -0.3;
      for(byte i = 0; i < 4; i++)
      {
        if(xDistance > abs(x - (byte(xVector + (i * 32.0)) % 128))){ xDistance = abs(x - (byte(xVector + (i * 32.0)) % 128)); xTarget = byte(xVector + (i * 32.0)) % 128; }
        if(yDistance > abs(y - (byte(yVector + (i * 32.0)) % 128))){ yDistance = abs(y - (byte(yVector + (i * 32.0)) % 128)); yTarget = byte(yVector + (i * 32.0)) % 128; }
      }
      shot[nextShot].xVector = (xTarget - shot[nextShot].xPosition) / (shot[nextShot].zPosition / shot[nextShot].zVector * -1.0);      
      shot[nextShot].yVector = (yTarget - shot[nextShot].yPosition) / (shot[nextShot].zPosition / shot[nextShot].zVector * -1.0);     
      shot[nextShot].xOffset = xTarget - xVector;
      shot[nextShot].yOffset = yTarget - yVector;      
    }
    
    nextShot++;
    if(nextShot == numberOfShots){ nextShot = 0; }

    // SOUND: Enemy fires shot
    tone(audio, 100, 20);
  }
}

// updateShots: Update all shots (player´s shots, enemy shots, and enemy missiles <- for player´s missiles a different function is used)
void updateShots()
{
  for(byte i = 0; i < numberOfShots; i++)
  {
    if(shot[i].active)
    {
      // Remove shot from old position
      if(shot[i].zPosition >= 8.0 && shot[i].zPosition < 32.0)
      { 
        if(isVisible(byte(shot[i].xPosition - upperLeftX) % 128, byte(shot[i].yPosition - upperLeftY) % 128)){ drawPixel(byte(shot[i].xPosition - upperLeftX) % 128, byte(shot[i].yPosition - upperLeftY) % 128, BL); }
      }
      else if(shot[i].zPosition >= 4.0)
      { 
        if(isVisible(byte(shot[i].xPosition - upperLeftX) % 128, byte(shot[i].yPosition - upperLeftY) % 128)){ drawPixel(byte(shot[i].xPosition - upperLeftX) % 128, byte(shot[i].yPosition - upperLeftY) % 128, BL); }
        if(isVisible(byte(shot[i].xPosition - upperLeftX + 1) % 128, byte(shot[i].yPosition - upperLeftY) % 128)){ drawPixel(byte(shot[i].xPosition - upperLeftX + 1) % 128, byte(shot[i].yPosition - upperLeftY) % 128, BL); }
        if(isVisible(byte(shot[i].xPosition - upperLeftX) % 128, byte(shot[i].yPosition - upperLeftY + 1) % 128)){ drawPixel(byte(shot[i].xPosition - upperLeftX) % 128, byte(shot[i].yPosition - upperLeftY + 1) % 128, BL); }
        if(isVisible(byte(shot[i].xPosition - upperLeftX + 1) % 128, byte(shot[i].yPosition - upperLeftY + 1) % 128)){ drawPixel(byte(shot[i].xPosition - upperLeftX + 1) % 128, byte(shot[i].yPosition - upperLeftY + 1) % 128, BL); }
      }
      else if(shot[i].zPosition < 4.0)
      { 
        if(isVisible(byte(shot[i].xPosition - upperLeftX) % 128, byte(shot[i].yPosition - upperLeftY - 1) % 128)){ drawPixel(byte(shot[i].xPosition - upperLeftX) % 128, byte(shot[i].yPosition - upperLeftY - 1) % 128, BL); }
        if(isVisible(byte(shot[i].xPosition - upperLeftX + 1) % 128, byte(shot[i].yPosition - upperLeftY - 1) % 128)){ drawPixel(byte(shot[i].xPosition - upperLeftX + 1) % 128, byte(shot[i].yPosition - upperLeftY - 1) % 128, BL); }
        if(isVisible(byte(shot[i].xPosition - upperLeftX - 1) % 128, byte(shot[i].yPosition - upperLeftY) % 128)){ drawPixel(byte(shot[i].xPosition - upperLeftX - 1) % 128, byte(shot[i].yPosition - upperLeftY) % 128, BL); }
        if(isVisible(byte(shot[i].xPosition - upperLeftX + 2) % 128, byte(shot[i].yPosition - upperLeftY) % 128)){ drawPixel(byte(shot[i].xPosition - upperLeftX + 2) % 128, byte(shot[i].yPosition - upperLeftY) % 128, BL); }
        if(isVisible(byte(shot[i].xPosition - upperLeftX - 1) % 128, byte(shot[i].yPosition - upperLeftY + 1) % 128)){ drawPixel(byte(shot[i].xPosition - upperLeftX - 1) % 128, byte(shot[i].yPosition - upperLeftY + 1) % 128, BL); }
        if(isVisible(byte(shot[i].xPosition - upperLeftX + 2) % 128, byte(shot[i].yPosition - upperLeftY + 1) % 128)){ drawPixel(byte(shot[i].xPosition - upperLeftX + 2) % 128, byte(shot[i].yPosition - upperLeftY + 1) % 128, BL); }
        if(isVisible(byte(shot[i].xPosition - upperLeftX) % 128, byte(shot[i].yPosition - upperLeftY + 2) % 128)){ drawPixel(byte(shot[i].xPosition - upperLeftX) % 128, byte(shot[i].yPosition - upperLeftY + 2) % 128, BL); }
        if(isVisible(byte(shot[i].xPosition - upperLeftX + 1) % 128, byte(shot[i].yPosition - upperLeftY + 2) % 128)){ drawPixel(byte(shot[i].xPosition - upperLeftX + 1) % 128, byte(shot[i].yPosition - upperLeftY + 2) % 128, BL); }
      }
      if(shot[i].type == ENEMY_MISSILE)
      {
        // Draw missile to radar
        matrix.drawPixel(12 + (4 + byte((shot[i].xPosition - upperLeftX - 31.5) / 16.0)) % 8, 23 + (4 + byte((shot[i].yPosition - upperLeftY - 26.5) / 16.0)) % 8, matrix.Color333(0, 0, 0));                    
      }
      
      // Move shot
      if(shot[i].type == PLAYER_SHOT)
      {
        shot[i].zPosition = shot[i].zPosition + shot[i].zVector;
        if(shot[i].zPosition >= 24.0){ shot[i].active = false; }
        shot[i].xPosition = shot[i].xPosition + shot[i].xVector;
        shot[i].yPosition = shot[i].yPosition + shot[i].yVector;
        if(shot[i].xVector > 0.0)
        {
          shot[i].xVector = shot[i].xVector - 0.5;
        }
        else if(shot[i].xVector < 0.0)
        {
          shot[i].xVector = shot[i].xVector + 0.5;        
        }
        if(shot[i].yVector > 0.0)
        {
          shot[i].yVector = shot[i].yVector - 0.25;
        }
        else if(shot[i].yVector < 0.0)
        {
          shot[i].yVector = shot[i].yVector + 0.25;        
        }
        
        // SOUND: Player shot
        if(shot[i].zPosition < 1.0){       tone(audio, 1600, 10); }
        else if(shot[i].zPosition < 2.0){ tone(audio, 800, 10); }
        else if(shot[i].zPosition < 3.0){ tone(audio, 400, 10); }
        else if(shot[i].zPosition < 4.0){ tone(audio, 200, 10); }
        else if(shot[i].zPosition < 5.0){ tone(audio, 100, 10); }
        else if(shot[i].zPosition < 6.0){ tone(audio, 50, 10); }
        
        // Check for hit
        if(shot[i].zPosition >= 4.0)
        {
          if(checkForHit(shot[i].xPosition, shot[i].yPosition, shot[i].zPosition, 1))
          { 
            shot[i].active = false;
          } 
        }
      }
      else if(shot[i].type == ENEMY_SHOT)
      {
        shot[i].xPosition = shot[i].xPosition + shot[i].xVector;
        shot[i].yPosition = shot[i].yPosition + shot[i].yVector;
        shot[i].zPosition = shot[i].zPosition + shot[i].zVector;
        if(shot[i].zPosition < 1.0)
        { 
          shot[i].active = false;
          
          if(((abs(shot[i].xPosition - xVector) + abs(shot[i].yPosition - yVector)) < 4.0) || ((abs(shot[i].xPosition - (xVector + shot[i].xOffset)) + abs(shot[i].yPosition - (yVector + shot[i].yOffset))) < 4.0))
          {
            gotHit = true;
            
            // SOUND: Got Hit
            tone(audio, 800, 10);
            drawCockpitFrame(7, 7, 7);
            
            if(shieldEnergy > 0)
            { 
              shieldEnergy--;
              updateShieldEnergyDisplay();
            }
          }
        }
      }
      else if(shot[i].type == ENEMY_MISSILE)
      {
        missileAlert = true;
        
        shot[i].xPosition = shot[i].xPosition + shot[i].xVector;
        shot[i].yPosition = shot[i].yPosition + shot[i].yVector;
        shot[i].zPosition = shot[i].zPosition + shot[i].zVector;

        // Guide the missile
        if((abs(shot[i].xPosition - xVector) + abs(shot[i].yPosition - yVector)) < (abs(shot[i].xPosition - (xVector + shot[i].xOffset)) + abs(shot[i].yPosition - (yVector + shot[i].yOffset))))
        {
          if(xVector < shot[i].xPosition){ shot[i].xPosition = shot[i].xPosition - 0.25; }
          else{ shot[i].xPosition = shot[i].xPosition + 0.25; }
          if(yVector < shot[i].yPosition){ shot[i].yPosition = shot[i].yPosition - 0.25; }
          else{ shot[i].yPosition = shot[i].yPosition + 0.25; }
        }
        else
        {
          if((xVector + shot[i].xOffset) < shot[i].xPosition){ shot[i].xPosition = shot[i].xPosition - 0.25; }
          else{ shot[i].xPosition = shot[i].xPosition + 0.25; }
          if((yVector + shot[i].yOffset) < shot[i].yPosition){ shot[i].yPosition = shot[i].yPosition - 0.25; }
          else{ shot[i].yPosition = shot[i].yPosition + 0.25; }          
        }

        if(shot[i].zPosition < 1.0)
        { 
          shot[i].active = false;
          if(((abs(shot[i].xPosition - xVector) + abs(shot[i].yPosition - yVector)) < 4.0) || ((abs(shot[i].xPosition - (xVector + shot[i].xOffset)) + abs(shot[i].yPosition - (yVector + shot[i].yOffset))) < 4.0))
          {
            gotHit = true;
            
            // SOUND: Got Hit
            tone(audio, 800, 10);
            drawCockpitFrame(7, 7, 7);
            
            if(shieldEnergy > 0)
            { 
              shieldEnergy--;
              if(shieldEnergy > 0)
              {
                shieldEnergy--;
              }
              updateShieldEnergyDisplay();
            }
          }
        }        
      }
      
      // Draw shot to new position
      if(shot[i].active)
      {
        if(shot[i].type == PLAYER_SHOT)
        {
          if(shot[i].zPosition >= 8.0 && shot[i].zPosition < 32.0)
          { 
            if(isVisible(byte(shot[i].xPosition - upperLeftXNew) % 128, byte(shot[i].yPosition - upperLeftYNew) % 128)){ drawPixel(byte(shot[i].xPosition - upperLeftXNew) % 128, byte(shot[i].yPosition - upperLeftYNew) % 128, P1); }
          }
          else if(shot[i].zPosition >= 4.0)
          { 
            if(isVisible(byte(shot[i].xPosition - upperLeftXNew) % 128, byte(shot[i].yPosition - upperLeftYNew) % 128)){ drawPixel(byte(shot[i].xPosition - upperLeftXNew) % 128, byte(shot[i].yPosition - upperLeftYNew) % 128, P2); }
            if(isVisible(byte(shot[i].xPosition - upperLeftXNew + 1) % 128, byte(shot[i].yPosition - upperLeftYNew) % 128)){ drawPixel(byte(shot[i].xPosition - upperLeftXNew + 1) % 128, byte(shot[i].yPosition - upperLeftYNew) % 128, P2); }
            if(isVisible(byte(shot[i].xPosition - upperLeftXNew) % 128, byte(shot[i].yPosition - upperLeftYNew + 1) % 128)){ drawPixel(byte(shot[i].xPosition - upperLeftXNew) % 128, byte(shot[i].yPosition - upperLeftYNew + 1) % 128, P2); }
            if(isVisible(byte(shot[i].xPosition - upperLeftXNew + 1) % 128, byte(shot[i].yPosition - upperLeftYNew + 1) % 128)){ drawPixel(byte(shot[i].xPosition - upperLeftXNew + 1) % 128, byte(shot[i].yPosition - upperLeftYNew + 1) % 128, P2); }
          }
          else if(shot[i].zPosition < 4.0)
          { 
            if(isVisible(byte(shot[i].xPosition - upperLeftXNew) % 128, byte(shot[i].yPosition - upperLeftYNew - 1) % 128)){ drawPixel(byte(shot[i].xPosition - upperLeftXNew) % 128, byte(shot[i].yPosition - upperLeftYNew - 1) % 128, P3); }
            if(isVisible(byte(shot[i].xPosition - upperLeftXNew + 1) % 128, byte(shot[i].yPosition - upperLeftYNew - 1) % 128)){ drawPixel(byte(shot[i].xPosition - upperLeftXNew + 1) % 128, byte(shot[i].yPosition - upperLeftYNew - 1) % 128, P3); }
            if(isVisible(byte(shot[i].xPosition - upperLeftXNew - 1) % 128, byte(shot[i].yPosition - upperLeftYNew) % 128)){ drawPixel(byte(shot[i].xPosition - upperLeftXNew - 1) % 128, byte(shot[i].yPosition - upperLeftYNew) % 128, P3); }
            if(isVisible(byte(shot[i].xPosition - upperLeftXNew + 2) % 128, byte(shot[i].yPosition - upperLeftYNew) % 128)){ drawPixel(byte(shot[i].xPosition - upperLeftXNew + 2) % 128, byte(shot[i].yPosition - upperLeftYNew) % 128, P3); }
            if(isVisible(byte(shot[i].xPosition - upperLeftXNew - 1) % 128, byte(shot[i].yPosition - upperLeftYNew + 1) % 128)){ drawPixel(byte(shot[i].xPosition - upperLeftXNew - 1) % 128, byte(shot[i].yPosition - upperLeftYNew + 1) % 128, P3); }
            if(isVisible(byte(shot[i].xPosition - upperLeftXNew + 2) % 128, byte(shot[i].yPosition - upperLeftYNew + 1) % 128)){ drawPixel(byte(shot[i].xPosition - upperLeftXNew + 2) % 128, byte(shot[i].yPosition - upperLeftYNew + 1) % 128, P3); }
            if(isVisible(byte(shot[i].xPosition - upperLeftXNew) % 128, byte(shot[i].yPosition - upperLeftYNew + 2) % 128)){ drawPixel(byte(shot[i].xPosition - upperLeftXNew) % 128, byte(shot[i].yPosition - upperLeftYNew + 2) % 128, P3); }
            if(isVisible(byte(shot[i].xPosition - upperLeftXNew + 1) % 128, byte(shot[i].yPosition - upperLeftYNew + 2) % 128)){ drawPixel(byte(shot[i].xPosition - upperLeftXNew + 1) % 128, byte(shot[i].yPosition - upperLeftYNew + 2) % 128, P3); }
          } 
        }
        else if(shot[i].type == ENEMY_SHOT)
        {
          if(shot[i].zPosition >= 8.0 && shot[i].zPosition < 32.0)
          { 
            if(isVisible(byte(shot[i].xPosition - upperLeftXNew) % 128, byte(shot[i].yPosition - upperLeftYNew) % 128)){ drawPixel(byte(shot[i].xPosition - upperLeftXNew) % 128, byte(shot[i].yPosition - upperLeftYNew) % 128, Y1); }
          }
          else if(shot[i].zPosition >= 4.0)
          { 
            if(isVisible(byte(shot[i].xPosition - upperLeftXNew) % 128, byte(shot[i].yPosition - upperLeftYNew) % 128)){ drawPixel(byte(shot[i].xPosition - upperLeftXNew) % 128, byte(shot[i].yPosition - upperLeftYNew) % 128, Y2); }
            if(isVisible(byte(shot[i].xPosition - upperLeftXNew + 1) % 128, byte(shot[i].yPosition - upperLeftYNew) % 128)){ drawPixel(byte(shot[i].xPosition - upperLeftXNew + 1) % 128, byte(shot[i].yPosition - upperLeftYNew) % 128, Y2); }
            if(isVisible(byte(shot[i].xPosition - upperLeftXNew) % 128, byte(shot[i].yPosition - upperLeftYNew + 1) % 128)){ drawPixel(byte(shot[i].xPosition - upperLeftXNew) % 128, byte(shot[i].yPosition - upperLeftYNew + 1) % 128, Y2); }
            if(isVisible(byte(shot[i].xPosition - upperLeftXNew + 1) % 128, byte(shot[i].yPosition - upperLeftYNew + 1) % 128)){ drawPixel(byte(shot[i].xPosition - upperLeftXNew + 1) % 128, byte(shot[i].yPosition - upperLeftYNew + 1) % 128, Y2); }
          }
          else if(shot[i].zPosition < 4.0)
          { 
            if(isVisible(byte(shot[i].xPosition - upperLeftXNew) % 128, byte(shot[i].yPosition - upperLeftYNew - 1) % 128)){ drawPixel(byte(shot[i].xPosition - upperLeftXNew) % 128, byte(shot[i].yPosition - upperLeftYNew - 1) % 128, Y3); }
            if(isVisible(byte(shot[i].xPosition - upperLeftXNew + 1) % 128, byte(shot[i].yPosition - upperLeftYNew - 1) % 128)){ drawPixel(byte(shot[i].xPosition - upperLeftXNew + 1) % 128, byte(shot[i].yPosition - upperLeftYNew - 1) % 128, Y3); }
            if(isVisible(byte(shot[i].xPosition - upperLeftXNew - 1) % 128, byte(shot[i].yPosition - upperLeftYNew) % 128)){ drawPixel(byte(shot[i].xPosition - upperLeftXNew - 1) % 128, byte(shot[i].yPosition - upperLeftYNew) % 128, Y3); }
            if(isVisible(byte(shot[i].xPosition - upperLeftXNew + 2) % 128, byte(shot[i].yPosition - upperLeftYNew) % 128)){ drawPixel(byte(shot[i].xPosition - upperLeftXNew + 2) % 128, byte(shot[i].yPosition - upperLeftYNew) % 128, Y3); }
            if(isVisible(byte(shot[i].xPosition - upperLeftXNew - 1) % 128, byte(shot[i].yPosition - upperLeftYNew + 1) % 128)){ drawPixel(byte(shot[i].xPosition - upperLeftXNew - 1) % 128, byte(shot[i].yPosition - upperLeftYNew + 1) % 128, Y3); }
            if(isVisible(byte(shot[i].xPosition - upperLeftXNew + 2) % 128, byte(shot[i].yPosition - upperLeftYNew + 1) % 128)){ drawPixel(byte(shot[i].xPosition - upperLeftXNew + 2) % 128, byte(shot[i].yPosition - upperLeftYNew + 1) % 128, Y3); }
            if(isVisible(byte(shot[i].xPosition - upperLeftXNew) % 128, byte(shot[i].yPosition - upperLeftYNew + 2) % 128)){ drawPixel(byte(shot[i].xPosition - upperLeftXNew) % 128, byte(shot[i].yPosition - upperLeftYNew + 2) % 128, Y3); }
            if(isVisible(byte(shot[i].xPosition - upperLeftXNew + 1) % 128, byte(shot[i].yPosition - upperLeftYNew + 2) % 128)){ drawPixel(byte(shot[i].xPosition - upperLeftXNew + 1) % 128, byte(shot[i].yPosition - upperLeftYNew + 2) % 128, Y3); }
          }             
        }
        else if(shot[i].type == ENEMY_MISSILE)
        {
          if(shot[i].zPosition >= 8.0 && shot[i].zPosition < 32.0)
          { 
            if(isVisible(byte(shot[i].xPosition - upperLeftXNew) % 128, byte(shot[i].yPosition - upperLeftYNew) % 128)){ drawPixel(byte(shot[i].xPosition - upperLeftXNew) % 128, byte(shot[i].yPosition - upperLeftYNew) % 128, G1); }
          }
          else if(shot[i].zPosition >= 4.0)
          { 
            if(isVisible(byte(shot[i].xPosition - upperLeftXNew) % 128, byte(shot[i].yPosition - upperLeftYNew) % 128)){ drawPixel(byte(shot[i].xPosition - upperLeftXNew) % 128, byte(shot[i].yPosition - upperLeftYNew) % 128, G2); }
            if(isVisible(byte(shot[i].xPosition - upperLeftXNew + 1) % 128, byte(shot[i].yPosition - upperLeftYNew) % 128)){ drawPixel(byte(shot[i].xPosition - upperLeftXNew + 1) % 128, byte(shot[i].yPosition - upperLeftYNew) % 128, G2); }
            if(isVisible(byte(shot[i].xPosition - upperLeftXNew) % 128, byte(shot[i].yPosition - upperLeftYNew + 1) % 128)){ drawPixel(byte(shot[i].xPosition - upperLeftXNew) % 128, byte(shot[i].yPosition - upperLeftYNew + 1) % 128, G2); }
            if(isVisible(byte(shot[i].xPosition - upperLeftXNew + 1) % 128, byte(shot[i].yPosition - upperLeftYNew + 1) % 128)){ drawPixel(byte(shot[i].xPosition - upperLeftXNew + 1) % 128, byte(shot[i].yPosition - upperLeftYNew + 1) % 128, G2); }
          }
          else if(shot[i].zPosition < 4.0)
          { 
            if(isVisible(byte(shot[i].xPosition - upperLeftXNew) % 128, byte(shot[i].yPosition - upperLeftYNew - 1) % 128)){ drawPixel(byte(shot[i].xPosition - upperLeftXNew) % 128, byte(shot[i].yPosition - upperLeftYNew - 1) % 128, G3); }
            if(isVisible(byte(shot[i].xPosition - upperLeftXNew + 1) % 128, byte(shot[i].yPosition - upperLeftYNew - 1) % 128)){ drawPixel(byte(shot[i].xPosition - upperLeftXNew + 1) % 128, byte(shot[i].yPosition - upperLeftYNew - 1) % 128, G3); }
            if(isVisible(byte(shot[i].xPosition - upperLeftXNew - 1) % 128, byte(shot[i].yPosition - upperLeftYNew) % 128)){ drawPixel(byte(shot[i].xPosition - upperLeftXNew - 1) % 128, byte(shot[i].yPosition - upperLeftYNew) % 128, G3); }
            if(isVisible(byte(shot[i].xPosition - upperLeftXNew + 2) % 128, byte(shot[i].yPosition - upperLeftYNew) % 128)){ drawPixel(byte(shot[i].xPosition - upperLeftXNew + 2) % 128, byte(shot[i].yPosition - upperLeftYNew) % 128, G3); }
            if(isVisible(byte(shot[i].xPosition - upperLeftXNew - 1) % 128, byte(shot[i].yPosition - upperLeftYNew + 1) % 128)){ drawPixel(byte(shot[i].xPosition - upperLeftXNew - 1) % 128, byte(shot[i].yPosition - upperLeftYNew + 1) % 128, G3); }
            if(isVisible(byte(shot[i].xPosition - upperLeftXNew + 2) % 128, byte(shot[i].yPosition - upperLeftYNew + 1) % 128)){ drawPixel(byte(shot[i].xPosition - upperLeftXNew + 2) % 128, byte(shot[i].yPosition - upperLeftYNew + 1) % 128, G3); }
            if(isVisible(byte(shot[i].xPosition - upperLeftXNew) % 128, byte(shot[i].yPosition - upperLeftYNew + 2) % 128)){ drawPixel(byte(shot[i].xPosition - upperLeftXNew) % 128, byte(shot[i].yPosition - upperLeftYNew + 2) % 128, G3); }
            if(isVisible(byte(shot[i].xPosition - upperLeftXNew + 1) % 128, byte(shot[i].yPosition - upperLeftYNew + 2) % 128)){ drawPixel(byte(shot[i].xPosition - upperLeftXNew + 1) % 128, byte(shot[i].yPosition - upperLeftYNew + 2) % 128, G3); }
          }
          // Draw missile to radar
          matrix.drawPixel(12 + (4 + byte((shot[i].xPosition - upperLeftXNew - 31.5) / 16.0)) % 8, 23 + (4 + byte((shot[i].yPosition - upperLeftYNew - 26.5) / 16.0)) % 8, matrix.Color333(0, 1, 1));                    
        }
      }
    }
  }
}

// updateMissile: Update player´s missile
void updateMissile()
{
  if(missileActive)
  {
    // Remove missile from old position
    if(missileZPosition >= 8.0 && missileZPosition < 64.0)
    { 
      if(isVisible(byte(missileXPosition - upperLeftX) % 128, byte(missileYPosition - upperLeftY) % 128)){ matrix.drawPixel(byte(missileXPosition - upperLeftX) % 128, byte(missileYPosition - upperLeftY) % 128, matrix.Color333(0, 0, 0)); }
    }
    else if(missileZPosition >= 4.0)
    { 
      if(isVisible(byte(missileXPosition - upperLeftX) % 128, byte(missileYPosition - upperLeftY) % 128)){ matrix.drawPixel(byte(missileXPosition - upperLeftX) % 128, byte(missileYPosition - upperLeftY) % 128, matrix.Color333(0, 0, 0)); }
      if(isVisible(byte(missileXPosition - upperLeftX + 1) % 128, byte(missileYPosition - upperLeftY) % 128)){ matrix.drawPixel(byte(missileXPosition - upperLeftX + 1) % 128, byte(missileYPosition - upperLeftY) % 128, matrix.Color333(0, 0, 0)); }
      if(isVisible(byte(missileXPosition - upperLeftX) % 128, byte(missileYPosition - upperLeftY + 1) % 128)){ matrix.drawPixel(byte(missileXPosition - upperLeftX) % 128, byte(missileYPosition - upperLeftY + 1) % 128, matrix.Color333(0, 0, 0)); }
      if(isVisible(byte(missileXPosition - upperLeftX + 1) % 128, byte(missileYPosition - upperLeftY + 1) % 128)){ matrix.drawPixel(byte(missileXPosition - upperLeftX + 1) % 128, byte(missileYPosition - upperLeftY + 1) % 128, matrix.Color333(0, 0, 0)); }
    }
    else if(missileZPosition < 4.0)
    { 
      if(isVisible(byte(missileXPosition - upperLeftX) % 128, byte(missileYPosition - upperLeftY - 1) % 128)){ matrix.drawPixel(byte(missileXPosition - upperLeftX) % 128, byte(missileYPosition - upperLeftY - 1) % 128, matrix.Color333(0, 0, 0)); }
      if(isVisible(byte(missileXPosition - upperLeftX + 1) % 128, byte(missileYPosition - upperLeftY - 1) % 128)){ matrix.drawPixel(byte(missileXPosition - upperLeftX + 1) % 128, byte(missileYPosition - upperLeftY - 1) % 128, matrix.Color333(0, 0, 0)); }
      if(isVisible(byte(missileXPosition - upperLeftX - 1) % 128, byte(missileYPosition - upperLeftY) % 128)){ matrix.drawPixel(byte(missileXPosition - upperLeftX - 1) % 128, byte(missileYPosition - upperLeftY) % 128, matrix.Color333(0, 0, 0)); }
      if(isVisible(byte(missileXPosition - upperLeftX + 2) % 128, byte(missileYPosition - upperLeftY) % 128)){ matrix.drawPixel(byte(missileXPosition - upperLeftX + 2) % 128, byte(missileYPosition - upperLeftY) % 128, matrix.Color333(0, 0, 0)); }
      if(isVisible(byte(missileXPosition - upperLeftX - 1) % 128, byte(missileYPosition - upperLeftY + 1) % 128)){ matrix.drawPixel(byte(missileXPosition - upperLeftX - 1) % 128, byte(missileYPosition - upperLeftY + 1) % 128, matrix.Color333(0, 0, 0)); }
      if(isVisible(byte(missileXPosition - upperLeftX + 2) % 128, byte(missileYPosition - upperLeftY + 1) % 128)){ matrix.drawPixel(byte(missileXPosition - upperLeftX + 2) % 128, byte(missileYPosition - upperLeftY + 1) % 128, matrix.Color333(0, 0, 0)); }
      if(isVisible(byte(missileXPosition - upperLeftX) % 128, byte(missileYPosition - upperLeftY + 2) % 128)){ matrix.drawPixel(byte(missileXPosition - upperLeftX) % 128, byte(missileYPosition - upperLeftY + 2) % 128, matrix.Color333(0, 0, 0)); }
      if(isVisible(byte(missileXPosition - upperLeftX + 1) % 128, byte(missileYPosition - upperLeftY + 2) % 128)){ matrix.drawPixel(byte(missileXPosition - upperLeftX + 1) % 128, byte(missileYPosition - upperLeftY + 2) % 128, matrix.Color333(0, 0, 0)); }
    }

    // Move missile
    missileZPosition = missileZPosition + missileZVector;
    if(missileZPosition >= 64.0){ missileActive = false; }
    missileXPosition = missileXPosition + missileXVector;
    missileYPosition = missileYPosition + missileYVector;
    if(missileYVector > 0.0)
    {
      missileYVector = missileYVector - 0.25;
    }
    else if(missileYVector < 0.0)
    {
      missileYVector = missileYVector + 0.25;        
    }

    if(ship[target].xPosition > missileXPosition && missileZPosition > 4.0)
    {
      missileXPosition = missileXPosition + 0.25;
    }
    else if(ship[target].xPosition < missileXPosition && missileZPosition > 4.0)
    {
      missileXPosition = missileXPosition - 0.25;
    }
    if(ship[target].yPosition > missileYPosition && missileZPosition > 4.0)
    {
      missileYPosition = missileYPosition + 0.25;
    }
    else if(ship[target].yPosition < missileYPosition && missileZPosition > 4.0)
    {
      missileYPosition = missileYPosition - 0.25;
    }

    // SOUND: Missile
    tone(audio,200 + random(200),10);
    
    // Check for hit
    if(missileZPosition >= 4.0)
    {
      if(checkForHit(missileXPosition, missileYPosition, missileZPosition, 4))
      { 
        missileActive = false;
      } 
    }
        
    // Draw missile to new position
    if(missileActive)
    {
      if(missileZPosition >= 8.0 && missileZPosition < 64.0)
      { 
        if(isVisible(byte(missileXPosition - upperLeftXNew) % 128, byte(missileYPosition - upperLeftYNew) % 128)){ matrix.drawPixel(byte(missileXPosition - upperLeftXNew) % 128, byte(missileYPosition - upperLeftYNew) % 128, matrix.Color333(1 + random(2), 1 + random(2), 0)); }
      }
      else if(missileZPosition >= 4.0)
      { 
        if(isVisible(byte(missileXPosition - upperLeftXNew) % 128, byte(missileYPosition - upperLeftYNew) % 128)){ matrix.drawPixel(byte(missileXPosition - upperLeftXNew) % 128, byte(missileYPosition - upperLeftYNew) % 128, matrix.Color333(1 + random(3), 1 + random(3), 0)); }
        if(isVisible(byte(missileXPosition - upperLeftXNew + 1) % 128, byte(missileYPosition - upperLeftYNew) % 128)){ matrix.drawPixel(byte(missileXPosition - upperLeftXNew + 1) % 128, byte(missileYPosition - upperLeftYNew) % 128, matrix.Color333(1 + random(3), 1 + random(3), 0)); }
        if(isVisible(byte(missileXPosition - upperLeftXNew) % 128, byte(missileYPosition - upperLeftYNew + 1) % 128)){ matrix.drawPixel(byte(missileXPosition - upperLeftXNew) % 128, byte(missileYPosition - upperLeftYNew + 1) % 128, matrix.Color333(1 + random(3), 1 + random(3), 0)); }
        if(isVisible(byte(missileXPosition - upperLeftXNew + 1) % 128, byte(missileYPosition - upperLeftYNew + 1) % 128)){ matrix.drawPixel(byte(missileXPosition - upperLeftXNew + 1) % 128, byte(missileYPosition - upperLeftYNew + 1) % 128, matrix.Color333(1 + random(3), 1 + random(3), 0)); }
      }
      else if(missileZPosition < 4.0)
      { 
        if(isVisible(byte(missileXPosition - upperLeftXNew) % 128, byte(missileYPosition - upperLeftYNew - 1) % 128)){ matrix.drawPixel(byte(missileXPosition - upperLeftXNew) % 128, byte(missileYPosition - upperLeftYNew - 1) % 128, matrix.Color333(1 + random(4), 1 + random(4), 0)); }
        if(isVisible(byte(missileXPosition - upperLeftXNew + 1) % 128, byte(missileYPosition - upperLeftYNew - 1) % 128)){ matrix.drawPixel(byte(missileXPosition - upperLeftXNew + 1) % 128, byte(missileYPosition - upperLeftYNew - 1) % 128, matrix.Color333(1 + random(4), 1 + random(4), 0)); }
        if(isVisible(byte(missileXPosition - upperLeftXNew - 1) % 128, byte(missileYPosition - upperLeftYNew) % 128)){ matrix.drawPixel(byte(missileXPosition - upperLeftXNew - 1) % 128, byte(missileYPosition - upperLeftYNew) % 128, matrix.Color333(1 + random(4), 1 + random(4), 0)); }
        if(isVisible(byte(missileXPosition - upperLeftXNew + 2) % 128, byte(missileYPosition - upperLeftYNew) % 128)){ matrix.drawPixel(byte(missileXPosition - upperLeftXNew + 2) % 128, byte(missileYPosition - upperLeftYNew) % 128, matrix.Color333(1 + random(4), 1 + random(4), 0)); }
        if(isVisible(byte(missileXPosition - upperLeftXNew - 1) % 128, byte(missileYPosition - upperLeftYNew + 1) % 128)){ matrix.drawPixel(byte(missileXPosition - upperLeftXNew - 1) % 128, byte(missileYPosition - upperLeftYNew + 1) % 128, matrix.Color333(1 + random(4), 1 + random(4), 0)); }
        if(isVisible(byte(missileXPosition - upperLeftXNew + 2) % 128, byte(missileYPosition - upperLeftYNew + 1) % 128)){ matrix.drawPixel(byte(missileXPosition - upperLeftXNew + 2) % 128, byte(missileYPosition - upperLeftYNew + 1) % 128, matrix.Color333(1 + random(4), 1 + random(4), 0)); }
        if(isVisible(byte(missileXPosition - upperLeftXNew) % 128, byte(missileYPosition - upperLeftYNew + 2) % 128)){ matrix.drawPixel(byte(missileXPosition - upperLeftXNew) % 128, byte(missileYPosition - upperLeftYNew + 2) % 128, matrix.Color333(1 + random(4), 1 + random(4), 0)); }
        if(isVisible(byte(missileXPosition - upperLeftXNew + 1) % 128, byte(missileYPosition - upperLeftYNew + 2) % 128)){ matrix.drawPixel(byte(missileXPosition - upperLeftXNew + 1) % 128, byte(missileYPosition - upperLeftYNew + 2) % 128, matrix.Color333(1 + random(4), 1 + random(4), 0)); }
      }      
    }
  }  
}

// initializeExplosion: Initialize an explosion at specific coordinates
void initializeExplosion(float x, float y, float z)
{
  if(!explosion[nextExplosion].active)
  {
    explosion[nextExplosion].active = true;
    explosion[nextExplosion].phase = 1;
    explosion[nextExplosion].xPosition = x;
    explosion[nextExplosion].yPosition = y;
    explosion[nextExplosion].zPosition = z;
    nextExplosion++;
    if(nextExplosion == numberOfExplosions){ nextExplosion = 0; }
  }
}

// updateExplosions: Update all explosions
void updateExplosions()
{
  byte explosionSize;
  for(byte i = 0; i < numberOfExplosions; i++)
  {
    if(explosion[i].active)
    {
      // Remove explosion at old position
      if(explosion[i].zPosition >= 32.0 && explosion[i].zPosition < 64.0){ explosionSize = 1; }
      else if(explosion[i].zPosition >= 16.0)                       { explosionSize = 2; }
      else if(explosion[i].zPosition >= 8.0)                        { explosionSize = 3; }
      else if(explosion[i].zPosition < 8.0)                         { explosionSize = 5; }

      if(explosion[i].phase < 11)
      {
        for(byte j = 0; j < (float(explosion[i].phase) / 10.0 * explosionSize); j++)
        {
          drawCircleWhereVisible(byte(explosion[i].xPosition - upperLeftX) % 128, byte(explosion[i].yPosition - upperLeftY) % 128, j, 0, 0, 0);
        }
      }
      else
      {
        for(byte j = 0; j < explosionSize; j++)
        {
          drawCircleWhereVisible(byte(explosion[i].xPosition - upperLeftX) % 128, byte(explosion[i].yPosition - upperLeftY) % 128, j, 0, 0, 0);
        }        
      }

      // Update explosion
      explosion[i].phase++;
      if(explosion[i].phase == 20){ explosion[i].active = false; }

      // Draw explosion at new position
      if(explosion[i].active)
      {
        if(explosion[i].zPosition >= 32.0 && explosion[i].zPosition < 64.0){ explosionSize = 1; }
        else if(explosion[i].zPosition >= 16.0)                       { explosionSize = 2; }
        else if(explosion[i].zPosition >= 8.0)                        { explosionSize = 3; }
        else if(explosion[i].zPosition < 8.0)                         { explosionSize = 5; }
        
        if(explosion[i].phase < 11)
        {
          for(byte j = 0; j < (float(explosion[i].phase) / 10.0 * explosionSize); j++)
          {
            drawCircleWhereVisible(byte(explosion[i].xPosition - upperLeftXNew) % 128, byte(explosion[i].yPosition - upperLeftYNew) % 128, j, 7 - (float(explosion[i].phase) / 3.0), 7 - (float(explosion[i].phase) / 3.0), 7 - (float(explosion[i].phase) / 2.0));
          }
        }
        else
        {
          for(byte j = (float(explosion[i].phase - 10) / 10.0 * explosionSize); j < explosionSize; j++)
          {
            drawCircleWhereVisible(byte(explosion[i].xPosition - upperLeftXNew) % 128, byte(explosion[i].yPosition - upperLeftYNew) % 128, j, 4 - (float(explosion[i].phase - 10) / 3.0), 4 - (float(explosion[i].phase - 10) / 3.0), 0);
          }        
        }

        // SOUND: Explosion
        tone(audio, 800 / pow(2, (explosion[i].phase / 2)) + random(21 - explosion[i].phase), 40);
      }
    }
  }
}



// Target locking and checking for hits

// checkTargets: Function is doing the locking of a target and checks whether is locked target remains locked
void checkTargets()
{
  if(!missileActive)
  {
    if(targetLocked)
    {
      if(((abs(ship[target].xPosition - xVector) + abs(ship[target].yPosition - yVector)) > 8.0) && ship[target].active && ship[target].zPosition < 64.0)
      {
        targetLocked = false;
        missileArmed = false;
        updateMissileDisplay();
      }
    }
    if(locking)
    {
      if(((abs(ship[target].xPosition - xVector) + abs(ship[target].yPosition - yVector)) < 4.0) && ship[target].active && ship[target].zPosition < 64.0)
      {
        lockTimer++;
        if(lockTimer > 8)
        {
          if(counter % 8 == 0)
          {
            // SOUND: Locking
            tone(audio, 200, 50);
          }
        }
        if(lockTimer == 40)
        {
          targetLocked = true;
          locking = false;
        }
      }
      else
      {
        locking = false;
      }
    }
    else if(!locking && !targetLocked && missiles > 0)
    {
      target = 0;
      for(byte i = 0; i < numberOfShips; i++)
      {
        if(ship[i].active)
        {    
          if(((abs(ship[i].xPosition - xVector) + abs(ship[i].yPosition - yVector)) < 4.0) && ship[i].active && ship[i].zPosition < 64.0)
          {
            if(!locking){ target = i; }
            locking = true;
            lockTimer = 0;
            if(ship[target].active && (ship[target].zPosition > ship[i].zPosition))
            {
              target = i;
            }
          }
        }
      }  
    }
  }
}

// checkForHit: Returns true if player has hit an enemy ship with a weapon with a specific damage value
boolean checkForHit(float x, float y, float z, byte damage)
{
  for(byte j = 0; j < numberOfShips; j++)
  {
    if(ship[j].active && (abs(z - ship[j].zPosition) < 2.0))
    {
      if(ship[j].zPosition >= 32.0 && ship[j].zPosition < 64.0)
      {
        if((abs(x - ship[j].xPosition) + abs(y - ship[j].yPosition)) < 1.0){ ship[j].shield = ship[j].shield - damage; ship[j].hasBeenHit = 5; return true; }
      }
      else if(ship[j].zPosition >= 16.0)
      {
        if((abs(x - ship[j].xPosition) + abs(y - ship[j].yPosition)) < 2.0){ ship[j].shield = ship[j].shield - damage; ship[j].hasBeenHit = 5; return true; }        
      }
      else if(ship[j].zPosition >= 8.0)
      {
        if((abs(x - ship[j].xPosition) + abs(y - ship[j].yPosition)) < 4.0){ ship[j].shield = ship[j].shield - damage; ship[j].hasBeenHit = 5; return true; }        
      }
      else if(ship[j].zPosition < 8.0)
      {
        if((abs(x - ship[j].xPosition) + abs(y - ship[j].yPosition)) < 8.0){ ship[j].shield = ship[j].shield - damage; ship[j].hasBeenHit = 5; return true; }               
      }
    }
  }
  return false;
}



// Initialize game

// clearPlayfield: Initialize playfield with 0 (nothing on the playfield)
void clearPlayfield()
{
  for(byte i = 0; i < 32; i++)
  {
    for(byte j = 0; j < 24; j++)
    {
      playfield[i][j] = 0;
    }    
  }
}

// initializeGame: Initialize a new game
void initializeGame()
{
  // Clear screen
  matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));

  // Clear playfield
  clearPlayfield();

  boostOn = false;
  extraThrust = 0.0;
  gotHit = false;
  missileAlert = false;

  lockTimer = 0;
  target = 0;
  locking = false;
  targetLocked = false;
  missileArmed = false;
  missileActive = false;
  missileXPosition = 0.0;
  missileYPosition = 0.0;
  missileZPosition = 0.0;
  missileXVector = 0.0;
  missileYVector = 0.0;
  missileZVector = 0.0;

  for(byte i = 0; i < numberOfShips; i++)
  {
    ship[i].type = 0;
    ship[i].xPosition = 0.0;
    ship[i].yPosition = 0.0;
    ship[i].zPosition = 0.0;
    ship[i].xSpeed = 0.0;
    ship[i].ySpeed = 0.0;
    ship[i].zSpeed = 0.0;
    ship[i].shield = 0;
    ship[i].frontVisible = true;
    ship[i].active = false;
    ship[i].bitmapOffset = 0;
    ship[i].screenWidth = 0;
    ship[i].screenHeight = 0;
    ship[i].hasBeenHit = 0;
    ship[i].fireCounter = 0;
    ship[i].flightMode = REGULAR_FLIGHT_MODE;
    ship[i].flightModeCounter = 0.0;
    ship[i].xEvasion = 0.0;
    ship[i].yEvasion = 0.0;
  }
  nextShip = 0;

  for(byte i = 0; i < numberOfShots; i++)
  {
    shot[i].type = 0;
    shot[i].active = false;
    shot[i].xPosition = 0.0;
    shot[i].yPosition = 0.0;
    shot[i].zPosition = 0.0;
    shot[i].xVector = 0.0;
    shot[i].yVector = 0.0;
    shot[i].zVector = 0.0;
    shot[i].xOffset = 0.0;
    shot[i].yOffset = 0.0;
  }
  nextShot = 0;
  fired = false;
  shotDelay = 0;

  for(byte i = 0; i < numberOfExplosions; i++)
  {
    explosion[i].active = false;
    explosion[i].phase = 0;
    explosion[i].xPosition = 0.0;
    explosion[i].yPosition = 0.0;
    explosion[i].zPosition = 0.0;
  }
  nextExplosion = 0;
  
  xVector = 63.0;
  yVector = 63.0;
  xVectorNew = xVector;
  yVectorNew = yVector;
  originOffsetX = 0.0;
  originOffsetY = 0.0;  

  counter = 0;

  reset = false;
  destroyed = false;
  planetOrbitDone = false;
}

// initializePlanet: Initialize a planet.
void initializePlanet()
{
  // Set planet variables (depending on planet number)
  planetOffset = byte(pgm_read_byte_near(planets + planetNumber - 1));
  planetWidth = byte(pgm_read_byte_near(planets + planetOffset));
  planetHeight = byte(pgm_read_byte_near(planets + planetOffset + 1));
  planetOffset = planetOffset + 2;

  // Encode enemies of current planet
  planetEnemiesOffset = byte(pgm_read_byte_near(planetEnemies + planetNumber - 1));
  numberOfWaves = byte(pgm_read_byte_near(planetEnemies + planetEnemiesOffset));
  wave = 0;

  shipsInWave = 0;
  killsInWave = 0;
}

// initializePlanet: Initialize an attack wave. In "Story" mode, there are different attack waves per planet.
void initializeWave()
{
  newWaveWarning = 36;
  killsInWave = 0;
  waveOffset = byte(pgm_read_byte_near(planetEnemies + planetEnemiesOffset + wave + 1));
  shipsInWave = byte(pgm_read_byte_near(planetEnemies + planetEnemiesOffset + waveOffset)); 

  byte wavePositionX = random(128);
  byte wavePositionY = random(128);

  for(byte i = 0; i < shipsInWave; i++)
  {
    initializeShip(byte(pgm_read_byte_near(planetEnemies + planetEnemiesOffset + waveOffset + 1 + i)), (wavePositionX - 2 + random(4)) % 128, (wavePositionY - 2 + random(4)) % 128);      
  }
}

// initializeMelee: Used to initialize the enemy ships in "Melee" mode.
void initializeMelee()
{
  // Set planet variables (it is always planet 1 in melee)
  planetOffset = byte(pgm_read_byte_near(planets + planetNumber - 1));
  planetWidth = byte(pgm_read_byte_near(planets + planetOffset));
  planetHeight = byte(pgm_read_byte_near(planets + planetOffset + 1));
  planetOffset = planetOffset + 2;

  wave = 0;
  killsInWave = 0;
  shipsInWave = numberOfScoutsInMelee + numberOfFightersInMelee + numberOfBombersInMelee + numberOfHeavyFightersInMelee;

  for(byte i = 0; i < shipsInWave; i++)
  {
    if(numberOfScoutsInMelee > 0)
    {
      initializeShip(SCOUT);
      numberOfScoutsInMelee--;
    }
    else if(numberOfFightersInMelee > 0)
    {
      initializeShip(FIGHTER);
      numberOfFightersInMelee--;
    }    
    else if(numberOfBombersInMelee > 0)
    {
      initializeShip(BOMBER);
      numberOfBombersInMelee--;
    }    
    else if(numberOfHeavyFightersInMelee > 0)
    {      
      initializeShip(HEAVYFIGHTER);
      numberOfHeavyFightersInMelee--;
    }    
  }
}



// Main loop

// loop: Main loop
void loop()
{
  // Show title screen
  title();

  // Select game mode: Story or Melee
  selectGameMode();
  
  level = 1;

  planetNumber = 1;
  planetLevel2 = 0;
  planetLevel3 = 0;

  int waveCompleteCounter = 50;

  do
  {
    // Graphical menu to select the next planet in case of story mode
    if(gameMode == STORY)
    { 
      selectNextPlanet();

      // Initialize game, the planet, and the first attack wave
      initializeGame();
      initializePlanet();
    }
    // Graphical menu to select the oponents in a melee
    if(gameMode == MELEE)
    { 
      meleeMenu();

      // Initialize game and melee planet
      initializeGame();
      initializeMelee();
    }
    
    // Show the startup sequence
    drawCockpit();
    startupSequence();

    do
    {
      // Get time of engine loop start point (important for a constant game speed)
      engineLoopStartPoint = millis();
  
      if(gotHit)
      {
        // SOUND: Got Hit
        tone(audio, 400, 10);
        
        gotHit = false;
        drawCockpitFrame(0, 0, 1);
        
        // SOUND: Got Hit
        tone(audio, 200, 10);
      }
      
      if(joy1IsLEDmePlayJoypad()) // LEDmePlay Joypad
      {
        // Left
        if(joy1XValue() > 0.0)
        {
          xVectorNew = xVector - joy1XValue() * 0.5;
          if(xVectorNew <= 0.0){ xVectorNew = xVectorNew + 128.0; }
          if(originOffsetX < 8.0){ originOffsetX = originOffsetX + 0.5; }
        }
        // Right
        else if(joy1XValue() < 0.0)
        {
          xVectorNew = xVector - joy1XValue() * 0.5;        
          if(xVectorNew >= 128.0){ xVectorNew = xVectorNew - 128.0; }
          if(originOffsetX > -8.0){ originOffsetX = originOffsetX - 0.5; }
        }
        else
        {
          if(originOffsetX < 0.0){ originOffsetX = originOffsetX + 0.5; }
          else if(originOffsetX > 0.0){ originOffsetX = originOffsetX - 0.5; }
        }
        // Up
        if(joy1YValue() > 0.0)
        {
          yVectorNew = yVector + joy1YValue() * 0.5;
          if(yVectorNew >= 128.0){ yVectorNew = yVectorNew - 128.0; }        
          if(originOffsetY > -8.0){ originOffsetY = originOffsetY - 0.5; }
        }
        // Down
        else if(joy1YValue() < 0.0)
        {
          yVectorNew = yVector + joy1YValue() * 0.5;        
          if(yVectorNew <= 0.0){ yVectorNew = yVectorNew + 128.0; }
          if(originOffsetY < 8.0){ originOffsetY = originOffsetY + 0.5; }
        }
        else
        {
          if(originOffsetY < 0.0){ originOffsetY = originOffsetY + 0.5; }
          else if(originOffsetY > 0.0){ originOffsetY = originOffsetY - 0.5; }
        }
        // Fire
        if(joy1FireA() && !fired && shotDelay == 0 && weaponEnergy > 0 && !missileArmed)
        {
          fired = true;
          shotDelay = 8;
          initializeShot();
          weaponEnergy--;
          updateWeaponEnergyDisplay();
          if(firedLeft){ firedLeft = false; }
          else{ firedLeft = true; }       
        }
        else if(!joy1Fire() && fired){ fired = false; }
        // Boost
        if(joy1FireB())
        {
          // Boost
          if(boostEnergy > 0 && !targetLocked && !boostOn)
          {
            boostEnergy--;
            boostOn = true;
            if(extraThrust < 0.025){ extraThrust = extraThrust + 0.001; }         
            updateBoostEnergyDisplay();
            // SOUND: Boost
            tone(audio,150 + random(50),10);
          }
          // Fire missile
          if(!boostOn && targetLocked)
          {
            missileArmed = true;
            if(joy1FireA())
            {
              targetLocked = false;
              missileArmed = false;
              missileActive = true;
              missileXPosition = xVector;
              missileYPosition = yVector + 9.25;
              missileZPosition = 0.0;
              missileXVector = 0.0;      
              missileYVector = -2.0;
              missileZVector = 0.5;  
              missiles--;
              updateMissileDisplay();
              fired = true;
              shotDelay = 10;
            }
          }
        }
        else
        {
          if(extraThrust > 0.0){ extraThrust = extraThrust - 0.001; }
  
          if(missileArmed)
          {
            targetLocked = false;
            missileArmed = false;          
          }
        }
      }
      else // Classic joystick(s) <- You need two
      {
        // Left
        if(joy1Left())
        {
          xVectorNew = xVector - 0.5;
          if(xVectorNew <= 0.0){ xVectorNew = xVectorNew + 128.0; }
          if(originOffsetX < 8.0){ originOffsetX = originOffsetX + 0.5; }
        }
        // Right
        else if(joy1Right())
        {
          xVectorNew = xVector + 0.5;        
          if(xVectorNew >= 128.0){ xVectorNew = xVectorNew - 128.0; }
          if(originOffsetX > -8.0){ originOffsetX = originOffsetX - 0.5; }
        }
        else
        {
          if(originOffsetX < 0.0){ originOffsetX = originOffsetX + 0.5; }
          else if(originOffsetX > 0.0){ originOffsetX = originOffsetX - 0.5; }
        }
        // Up
        if(joy1Up())
        {
          yVectorNew = yVector + 0.5;
          if(yVectorNew >= 128.0){ yVectorNew = yVectorNew - 128.0; }        
          if(originOffsetY > -8.0){ originOffsetY = originOffsetY - 0.5; }
        }
        // Down
        else if(joy1Down())
        {
          yVectorNew = yVector - 0.5;        
          if(yVectorNew <= 0.0){ yVectorNew = yVectorNew + 128.0; }
          if(originOffsetY < 8.0){ originOffsetY = originOffsetY + 0.5; }
        }
        else
        {
          if(originOffsetY < 0.0){ originOffsetY = originOffsetY + 0.5; }
          else if(originOffsetY > 0.0){ originOffsetY = originOffsetY - 0.5; }
        }
        // Fire
        if(joy1Fire() && !fired && shotDelay == 0 && weaponEnergy > 0 && !missileArmed)
        {
          fired = true;
          shotDelay = 10;
          initializeShot();
          weaponEnergy--;
          updateWeaponEnergyDisplay();
          if(firedLeft){ firedLeft = false; }
          else{ firedLeft = true; }       
        }
        else if(!joy1Fire() && fired){ fired = false; }
        // Boost
        if(joy2Up())
        {
          // Boost
          if(boostEnergy > 0 && !targetLocked && !boostOn)
          {
            boostEnergy--;
            boostOn = true;
            if(extraThrust < 0.025){ extraThrust = extraThrust + 0.001; }         
            updateBoostEnergyDisplay();
            // SOUND: Boost
            tone(audio,150 + random(50),10);
          }
        }
        else
        {
          if(extraThrust > 0.0){ extraThrust = extraThrust - 0.001; }          
        }
        // Fire missile
        if(!boostOn && targetLocked)
        {
          missileArmed = true;
          if(joy2Down())
          {
            targetLocked = false;
            missileArmed = false;
            missileActive = true;
            missileXPosition = xVector;
            missileYPosition = yVector + 9.25;
            missileZPosition = 0.0;
            missileXVector = 0.0;      
            missileYVector = -2.0;
            missileZVector = 0.5;  
            missiles--;
            updateMissileDisplay();
            fired = true;
            shotDelay = 10;
          }
        }
        else
        { 
          if(missileArmed)
          {
            targetLocked = false;
            missileArmed = false;          
          }
        }
      }
  
      upperLeftX = xVector - 15.5;
      upperLeftY = yVector - 10.5;
      upperLeftXNew = xVectorNew - 15.5;
      upperLeftYNew = yVectorNew - 10.5;
  
      if(upperLeftX < 0){ upperLeftX = upperLeftX + 128.0; }
      else if(upperLeftX >= 128){ upperLeftX = upperLeftX - 128.0; }
      if(upperLeftY < 0){ upperLeftY = upperLeftY + 128.0; }
      else if(upperLeftY >= 128){ upperLeftY = upperLeftY - 128.0; }
      if(upperLeftXNew < 0){ upperLeftXNew = upperLeftXNew + 128.0; }
      else if(upperLeftXNew >= 128){ upperLeftXNew = upperLeftXNew - 128.0; }
      if(upperLeftYNew < 0){ upperLeftYNew = upperLeftYNew + 128.0; }
      else if(upperLeftYNew >= 128){ upperLeftYNew = upperLeftYNew - 128.0; }
      
      updateVega();
      updatePlanet();
      updateStarfield();
      updateShips();
      updateExplosions();
      updateShots();
      updateMissile();
      checkTargets();
      drawCrosshairs(); 
  
      xVector = xVectorNew;
      yVector = yVectorNew;
    
      if(shotDelay > 0){ shotDelay--; }
      boostOn = false;
  
      if(counter % 8 == 0 && boostEnergy < 90)
      {
        boostEnergy++;
        updateBoostEnergyDisplay();
      }
      if(counter % 64 == 0 && weaponEnergy < 9)
      {
        weaponEnergy++;
        updateWeaponEnergyDisplay();
      }
      if(targetLocked)
      {
        blinkMissile();
      }
      if(missileAlert)
      {
        // SOUND: Missile alert
        tone(audio, 100 + 32 * (counter % 8), 50);  
      }
      missileAlert = false;
  
      // Player´s ship destoyed
      if(shieldEnergy == 0){ destroyed = true; }

      // New wave warning
      if(newWaveWarning > 0)
      {
        // SOUND: New wave warning
        if((newWaveWarning % 12) == 0)
        {
          tone(audio, 400, 100);
        }
        else if((newWaveWarning % 12) == 9)
        {
          tone(audio, 800, 200);
        }
        newWaveWarning--;
      }
  
      // Wave cleared
      if(killsInWave == shipsInWave)
      { 
        waveCompleteCounter--;
        if(waveCompleteCounter == 0) // Ensures that player can see the explosion of the last enemy before the next wave starts or the orbit has been cleared
        {
          if(gameMode == STORY)
          {
            if(wave == numberOfWaves)
            {
              planetOrbitDone = true;
            }
            else
            {
              initializeWave();
              wave++;
            }
          }
          else if(gameMode == MELEE)
          {
            planetOrbitDone = true;
          }
          waveCompleteCounter = 50;
        }
      }
  
      counter++;
      
      // Reset
      if(digitalRead(buttonReset) == LOW)
      {
        reset = true;
        break;
      }
  
      // Pause
      if(digitalRead(buttonPause) == LOW)
      {
        clickSound();
        delay(100);
        do
        {
        }
        while((!digitalRead(buttonPause) == LOW) && (!joy1Fire()) && (!joy2Fire()));
        clickSound();
        delay(100);
      }
  
      // Loop until 20 milliseconds has passed since begin of engine loop
      do
      {
        delay(10);
      }
      while(abs(millis() - engineLoopStartPoint) < 40);
    }
    while(!(reset || destroyed || planetOrbitDone));
    
    if(destroyed)
    {     
      playersShipDestroyed();
    }
    else if(planetOrbitDone)
    { 
      if(gameMode == STORY)
      {
        planetOrbitDoneScreen();
        level++;
        if(level == 4)
        { 
          storyDoneScreen(planetLevel3);
          level = 1;
          planetLevel2 = 0;
          planetLevel3 = 0;
          reset = true;
        }
      }
      else if(gameMode == MELEE)
      {
        meleeWonScreen();
        reset = true;
      }
    }

  }
  while(!(reset || destroyed));
}
