// 8 speed H shifter using 4 switches (plus neutral) - Updated with 7-Segment Display and 9th Ignition Button
#include <Joystick.h>

// --- INPUT PINS ---
// 2: X left
// 3: X right
// 4: X right extreme
// 5: X up
// 6: Y down
#define IGNITION_PIN 21 // New: Ignition Button on Pin 21 (A3)

#define LEFT 0
#define RIGHT 1
#define RIGHTEXTREME 3  // mistakenly soldered in wrong pin
#define UP 2
#define DOWN 4
#define IGNITION_INPUT_INDEX 5 // Index 5 in the input array

// The 9th button index on the joystick will be used for ignition
#define IGNITION_JOYSTICK_BUTTON 8 

// --- DISPLAY PINS (Common Anode assumed: LOW turns ON) ---
#define SEG_A 7
#define SEG_B 8
#define SEG_C 9
#define SEG_D 10
#define SEG_E 16 // A2
#define SEG_F 14 // A0
#define SEG_G 18 // A4

const int displayPins[] = {SEG_A, SEG_B, SEG_C, SEG_D, SEG_E, SEG_F, SEG_G};

// --- DISPLAY CONFIGURATION ---
// Set this constant to 'false' for Common Anode (LOW=ON).
// Set this constant to 'true' for Common Cathode (HIGH=ON).
const bool IS_COMMON_CATHODE = false; 

// Joystick Setup: INCREASED to 9 buttons (8 forward/reverse + 1 ignition)
Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID,JOYSTICK_TYPE_GAMEPAD,
  9, 0,                      // Button Count is now 9
  false, false, false,       // No X, Y, Z Axis
  false, false, false,       // No Rx, Ry, or Rz
  false, false,              // No rudder or throttle
  false, false, false);      // No accelerator, brake, or steering

// Last state of the buttons (9 buttons: Gear 1-7, Reverse, Ignition)
int lastButtonState[9] = {0,0,0,0,0,0,0,0,0};
int currentButtonState[9] = {0,0,0,0,0,0,0,0,0};
// input array has 6 elements (0-4 shifter, 5 ignition)
int input[6] = {0,0,0,0,0,0}; 
int gear = 0; // 0 is neutral. 1-7 are forward gears. 8 is reverse.

void setup() {
  // Initialize Shifter Input Pins (2-6)
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
  // Initialize Ignition Pin
  pinMode(IGNITION_PIN, INPUT_PULLUP);

  // Initialize Display Pins as OUTPUT
  for (int i = 0; i < 7; i++) {
    pinMode(displayPins[i], OUTPUT);
    // All segments OFF initially
    digitalWrite(displayPins[i], IS_COMMON_CATHODE ? LOW : HIGH); 
  }

  // Initialize Joystick Library
  Joystick.begin();
}

/**
 * Turns segments of the 7-segment display ON or OFF based on the desired character.
 * 0 = logically ON, 1 = logically OFF
 */
void updateDisplay(int currentGear) {
  // Digit patterns: {A, B, C, D, E, F, G} 
  // 1 = OFF, 0 = ON (LOGICALLY)

  int segments[7] = {1, 1, 1, 1, 1, 1, 1}; // Default to all OFF

  switch (currentGear) {
    case 1: segments[1]=0; segments[2]=0; break;
    case 2: segments[0]=0; segments[1]=0; segments[3]=0; segments[4]=0; segments[6]=0; break;
    case 3: segments[0]=0; segments[1]=0; segments[2]=0; segments[3]=0; segments[6]=0; break;
    case 4: segments[1]=0; segments[2]=0; segments[5]=0; segments[6]=0; break;
    case 5: segments[0]=0; segments[2]=0; segments[3]=0; segments[5]=0; segments[6]=0; break;
    case 6: segments[0]=0; segments[2]=0; segments[3]=0; segments[4]=0; segments[5]=0; segments[6]=0; break;
    case 7: segments[0]=0; segments[1]=0; segments[2]=0; break;
    case 8: // Display 'R' (Reverse)
      segments[4]=0; segments[6]=0; 
      break;
    case 0: // Display '-' (Neutral / Middle Segment)
    default:
      segments[6]=0; // Middle Segment G ON
      break;
  }

  // Write the state to the physical pins
  for (int i = 0; i < 7; i++) {
    // Apply display type logic
    if (IS_COMMON_CATHODE) {
        digitalWrite(displayPins[i], !segments[i]); // CC: 1=ON
    } else {
        digitalWrite(displayPins[i], segments[i]); // CA: 0=ON
    }
  }
}

/**
 * Creates a "loading" or spinning animation by sequentially lighting segments.
 */
void animateLoading() {
  // Static variables maintain their value between calls
  static const long interval = 100; // Time per segment (100ms)
  static long previousMillis = 0;
  static int segmentIndex = 0;
  
  // Segments to light up sequentially (A, B, C, D, E, F)
  // This creates a clockwise spinning effect
  const int loadingSegments[] = {SEG_A, SEG_B, SEG_C, SEG_D, SEG_E, SEG_F};
  const int loadingLength = 6;

  if (millis() - previousMillis >= interval) {
    previousMillis = millis();

    // 1. Turn off all segments first
    for (int i = 0; i < 7; i++) {
        // OFF logic: HIGH for CA (false), LOW for CC (true)
        digitalWrite(displayPins[i], IS_COMMON_CATHODE ? LOW : HIGH);
    }
    
    // 2. Turn on the current segment
    int pinToWrite = loadingSegments[segmentIndex];
    // ON logic: LOW for CA (false), HIGH for CC (true)
    digitalWrite(pinToWrite, IS_COMMON_CATHODE ? HIGH : LOW);

    // 3. Move to the next segment
    segmentIndex = (segmentIndex + 1) % loadingLength;
  }
}

void loop() {
  // Read the 5 shifter inputs (pins 2-6, indices 0-4)
  for (int index = 0; index < 5; index++) {
    input[index] = !digitalRead(index + 2);
  }
  // Read the Ignition Input (pin 19, index 5)
  input[IGNITION_INPUT_INDEX] = !digitalRead(IGNITION_PIN);
  bool ignitionPressed = input[IGNITION_INPUT_INDEX];
  
  // --- GEAR LOGIC ---
  // Only detect gear if ignition is OFF (optional, but prevents phantom gear changes during startup)
  if (!ignitionPressed) {
    gear = 0; // Default to neutral

    if (input[UP]) { //if stick is up
      if (input[LEFT]) { gear = 1; } 
      else if (input[RIGHT]) {
        if (input[RIGHTEXTREME]) { gear = 7; } 
        else { gear = 5; }
      } else { gear = 3; }
    } else if (input[DOWN]) {
      if (input[LEFT]) { gear = 2; } 
      else if (input[RIGHT]) {
        if (input[RIGHTEXTREME]) { gear = 8; } // Gear 8 is Reverse (R)
        else { gear = 6; }
      } else { gear = 4; }
    } 
  } else {
    // If ignition is pressed, keep gear at 0 (or whatever state is desired while starting)
    gear = 0;
  }
  
  // --- JOYSTICK BUTTONS UPDATE ---
  
  // 1. Reset all gear buttons (0-7)
  for (int i = 0; i < 8; i++) {
    currentButtonState[i] = 0;
  }
  
  // 2. Set the current gear button (0-7) if a gear is engaged
  if (gear >= 1 && gear <= 8) {
    currentButtonState[gear-1] = 1; 
  }

  // 3. Set the ignition button (index 8)
  currentButtonState[IGNITION_JOYSTICK_BUTTON] = ignitionPressed;
  
  // Send out button commands for all 9 buttons (0-8)
  for (int index = 0; index < 9; index++) {
    if (currentButtonState[index] != lastButtonState[index]) {
      Joystick.setButton(index, currentButtonState[index]);
      lastButtonState[index] = currentButtonState[index];
    }
  }

  // --- DISPLAY UPDATE ---
  if (ignitionPressed) {
    animateLoading(); // Run animation if ignition button is pressed
  } else {
    updateDisplay(gear); // Display current gear/neutral
  }

  delay(10);
}
