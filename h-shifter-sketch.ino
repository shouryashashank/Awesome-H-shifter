// 8 speed H shifter using 4 switches (plus neutral), by Akaki Kuumeri - Updated with 7-Segment Display
#include <Joystick.h>

// --- INPUT PINS ---
// 2: X left
// 3: X right
// 4: X right extreme
// 5: X up
// 6: Y down
// The 8th position (DOWN + RIGHT + RIGHTEXTREME) is now used for Reverse.

#define LEFT 0
#define RIGHT 1
#define RIGHTEXTREME 2
#define UP 3
#define DOWN 4
// Dedicated REVERSE_PIN and REVERSE_INPUT are no longer required.

// --- DISPLAY PINS (Common Anode assumed: LOW turns ON) ---
#define SEG_A 7
#define SEG_B 8
#define SEG_C 9
#define SEG_D 10
#define SEG_E 16 // A2
#define SEG_F 14 // A0
#define SEG_G 18 // A4

const int displayPins[] = {SEG_A, SEG_B, SEG_C, SEG_D, SEG_E, SEG_F, SEG_G};

// Joystick Setup: 8 buttons for 7 Forward Gears + 1 Reverse Gear (0-7 indices)
Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID,JOYSTICK_TYPE_GAMEPAD,
  8, 0,                      // Button Count, Hat Switch Count
  false, false, false,       // No X, Y, Z Axis
  false, false, false,       // No Rx, Ry, or Rz
  false, false,              // No rudder or throttle
  false, false, false);      // No accelerator, brake, or steering

// Last state of the buttons (8 buttons: Gear 1-7, Reverse)
int lastButtonState[8] = {0,0,0,0,0,0,0,0};
int currentButtonState[8] = {0,0,0,0,0,0,0,0};
// input array has 5 elements (0-4) for pins 2 through 6
int input[5] = {0,0,0,0,0}; 
int gear = 0; // 0 is neutral. 1-7 are forward gears. 8 is reverse.

void setup() {
  // Initialize Input Pins and enable internal PULLUP resistors for pins 2-6
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
  // Dedicated REVERSE_PIN (19) is no longer used

  // Initialize Display Pins as OUTPUT
  for (int i = 0; i < 7; i++) {
    pinMode(displayPins[i], OUTPUT);
    digitalWrite(displayPins[i], HIGH); // Start with all segments OFF (Common Anode)
  }

  // Initialize Joystick Library
  Joystick.begin();
}

/**
 * Turns segments of the 7-segment display ON or OFF based on the desired character.
 * We are using an array of 7 values, one for each segment (A to G).
 * 0 = ON (LOW), 1 = OFF (HIGH) for Common Anode.
 */
void updateDisplay(int currentGear) {
  // Digit patterns: {A, B, C, D, E, F, G} 
  // 1 = OFF (HIGH), 0 = ON (LOW)

  // Default to all OFF
  int segments[7] = {1, 1, 1, 1, 1, 1, 1}; 

  switch (currentGear) {
    case 1: // Display '1'
      // Segments B, C ON
      segments[1]=0; segments[2]=0;
      break;
    case 2: // Display '2'
      // Segments A, B, D, E, G ON
      segments[0]=0; segments[1]=0; segments[3]=0; segments[4]=0; segments[6]=0;
      break;
    case 3: // Display '3'
      // Segments A, B, C, D, G ON
      segments[0]=0; segments[1]=0; segments[2]=0; segments[3]=0; segments[6]=0;
      break;
    case 4: // Display '4'
      // Segments B, C, F, G ON
      segments[1]=0; segments[2]=0; segments[5]=0; segments[6]=0;
      break;
    case 5: // Display '5'
      // Segments A, C, D, F, G ON
      segments[0]=0; segments[2]=0; segments[3]=0; segments[5]=0; segments[6]=0;
      break;
    case 6: // Display '6'
      // Segments A, C, D, E, F, G ON
      segments[0]=0; segments[2]=0; segments[3]=0; segments[4]=0; segments[5]=0; segments[6]=0;
      break;
    case 7: // Display '7'
      // Segments A, B, C ON
      segments[0]=0; segments[1]=0; segments[2]=0;
      break;
    case 8: // Display 'R' (Reverse) - This is the former '8' position
      // Segments E and G ON (a common 'r' representation)
      segments[4]=0; segments[6]=0; 
      break;
    case 0: // Display '-' (Neutral / Middle Segment)
    default:
      // Only Segment G (Middle) ON as requested for "no input" / Neutral
      segments[6]=0;
      break;
  }

  // Write the state to the physical pins
  for (int i = 0; i < 7; i++) {
    digitalWrite(displayPins[i], segments[i]);
  }
}

void loop() {
  // Read the 5 shifter inputs (pins 2-6)
  for (int index = 0; index < 5; index++) {
    // Reads input pins 2, 3, 4, 5, 6 into array indices 0, 1, 2, 3, 4
    input[index] = !digitalRead(index + 2);
  }
  
  // --- GEAR LOGIC ---
  gear = 0; // Default to neutral

  if (input[UP]) { //if stick is up
    if (input[LEFT]) { //left
      gear = 1;
    } else if (input[RIGHT]) {
      if (input[RIGHTEXTREME]) { //if also right extreme
        gear = 7;
      } else {
        gear = 5;
      }
    } else {
      gear = 3; //stick is neither left nor right, but points up
    }
  } else if (input[DOWN]) {
    if (input[LEFT]) {
      gear = 2;
    } else if (input[RIGHT]) {
      if (input[RIGHTEXTREME]) { //if also right extreme
        gear = 8; // This position (gear 8) is now REVERSE (R)
      } else {
        gear = 6;
      }
    } else {
      gear = 4; //stick is neither left nor right, but points down
    }
  } //otherwise 0, neutral is kept

  // --- JOYSTICK BUTTONS UPDATE ---
  
  // Reset all button states to 0 (Released)
  for (int i = 0; i < 8; i++) {
    currentButtonState[i] = 0;
  }
  
  // Set the button corresponding to the current gear (1-7 Forward, 8 Reverse) to 1 (Pressed)
  if (gear >= 1 && gear <= 8) {
    currentButtonState[gear-1] = 1; // Gear 8 (Reverse) activates button 7 (index 7).
  }
  
  // Send out button commands only if the state has changed
  for (int index = 0; index < 8; index++) {
    if (currentButtonState[index] != lastButtonState[index]) {
      Joystick.setButton(index, currentButtonState[index]);
      lastButtonState[index] = currentButtonState[index];
    }
  }

  // --- DISPLAY UPDATE ---
  updateDisplay(gear);

  delay(10);
}