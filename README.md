# Awesome H-Shifter

A DIY 8-speed H-pattern shifter that converts a physical H-stick and switches into a USB gamepad for sim racing. The project uses a Arduino Pro Micro (ATmega32U4) and a single-digit 7-segment display to show the current gear, plus an ignition button that runs a loading animation.

<img width="200" alt="image" src="https://github.com/user-attachments/assets/dfe48835-6a08-469d-ab43-f14f92e1588e" />

---

## Features

- 8 forward positions (1–7) + Reverse (R) + Neutral
- Ignition push-button that triggers a loading animation and can disable gear outputs while pressed
- Exposes gears and ignition as joystick buttons via the Arduino Joystick library (native USB)
- Simple wiring based on a Arduino Pro Micro (or any compatible ATmega32U4 board)
- Single 7-segment display for gear/neutral/reverse feedback
- Eagle schematic included for reference (`awesome-H-shifter.sch`)

---

## Repository contents

- `awesome-H-shifter.sch` — Eagle schematic showing wiring of the Arduino Pro Micro, switches and 7-segment display
- `h-shifter-sketch.ino` — Arduino sketch for reading the shifter, updating the display and reporting joystick buttons
- `3d-print files` — contains the 3d print files for the shifter
- `README.md` — this file
- `LICENSE` — project license

---

## Hardware

Required parts (suggested):

- Arduino Pro Micro (ATmega32U4) or equivalent
- 5 momentary switches wired in an H pattern (left/right/extreme right/up/down)
- 1 ignition push button
- 1 single-digit 7-segment display (common-anode by default in the sketch)
- Wires, soldering tools, optional perfboard, enclosure and 3D-printed housing
- M3 nuts and bolts (20 mm and 30 mm or other similar lengths)
- 2 Rubber bands

See the included schematic for wiring and footprint details.

ps: DO NOT get Arduino uno or nano. we need a micro processor with HID capability. you can use any other micro processor that has HID support like esp32 or raspberry pie zero (will have to re write the code)

---

## Wiring / Pin mapping

The wiring in `h-shifter-sketch.ino` maps pins as follows (Pro Micro pin numbers):

- Shifter inputs (configured INPUT_PULLUP):
  - Pin 2 -> LEFT
  - Pin 3 -> RIGHT
  - Pin 4 -> RIGHTEXTREME
  - Pin 5 -> UP
  - Pin 6 -> DOWN
- Ignition button: Pin 21 (A3) — INPUT_PULLUP (active low)

7-segment segment pins (as defined in sketch):
- SEG_A: 7
- SEG_B: 8
- SEG_C: 9
- SEG_D: 10
- SEG_E: 16 (A2)
- SEG_F: 14 (A0)
- SEG_G: 18 (A4)

Joystick mapping (USB gamepad):
- Buttons 0..7 represent gears 1..8 (button index = gear-1)
- Button 8 (index 8) is ignition

---

## Software / Upload

1. Open `h-shifter-sketch.ino` in the Arduino IDE or PlatformIO.
2. Select the correct board (Pro Micro or other ATmega32U4 board) and the correct port.
3. Ensure a compatible Joystick library is available. The sketch uses the built-in Joystick support for 32U4 cores; if your core lacks it, install an appropriate Joystick library.
4. Compile and upload the sketch.
5. Test the device in your OS game controller settings or in a game — the board should enumerate as a USB gamepad with 9 buttons.

---

## Behavior summary

- With ignition NOT pressed: the shifter inputs determine the gear. Neutral is used when no direction is pressed.
- With ignition pressed: the sketch runs a spinning/loading animation on the 7-segment and gear outputs remain disabled (configurable).
- The sketch only sends changed button states to the host to minimize USB traffic.

---

## Customization

- To use a common-cathode display, set `IS_COMMON_CATHODE` in `h-shifter-sketch.ino` to `true` and adjust wiring if needed.
- Remap input or segment pins by editing the `#define` statements at the top of `h-shifter-sketch.ino`.
- Add debounce or filtering to the input reads if you experience noisy switches.
- Change joystick button count or behavior in the Joystick constructor if you add more inputs.

---

## Troubleshooting

- Device not enumerating: make sure you uploaded to an ATmega32U4-based board (native USB). Reflash bootloader if necessary.
- Segments appear inverted or incorrect: confirm whether the display is common-anode (default) or common-cathode and set `IS_COMMON_CATHODE` accordingly.
- Phantom gear changes: implement debouncing (software or hardware), add short delays after reads, or use capacitors / better switch contacts.
- Wrong pin numbers: double-check the Pro Micro pin labels (A0/A1/A2 etc. may be mapped to different digital numbers depending on core).

---

## Schematic and photos

- See `awesome-H-shifter.sch` for the full Eagle schematic.
- The repository includes photos of an example 3D-printed shifter housing and wiring for reference.

<img width="500" alt="image" src="https://github.com/user-attachments/assets/c16eb23b-fede-475f-8223-b3024f93efef" />
<img width="500" alt="image" src="https://github.com/user-attachments/assets/e48a7c2a-67cc-47dd-a45a-820e0e89f8f5" />
<img width="500" alt="image" src="https://github.com/user-attachments/assets/948c8e64-ab19-4ce8-b4d0-378abcff49b6" />
<img width="500" alt="image" src="https://github.com/user-attachments/assets/66df14f0-edce-41a8-b4ff-d16e591e92c2" />




---

## 3D printing files

The `3d-print files/` folder contains printable and editable models:

- `8shifter-body-new-clamp.3mf` — shifter body with integrated clamp
- `shift light.step` — STEP model of the shift display; edit to add more buttons if needed

Notes:
- Use either the body-with-clamp file or a plain body `.stl` (if you have one); there is no need to print both.
- The clamp version of the body is slightly more complex to print and may not be feasible on some bedslinger printers. The other parts are straightforward.
- Additional STEP models and the full original design are available from the creator's page: https://github.com/akakikuumeri/Akaki-H-shifter
- If you use the original body design, print clamps from: https://makerworld.com/en/models/504560-g-clamp-deeper?from=search#profileId-420208

---

## Credits

- Original 3D model and design by Akaki Kuumeri: https://github.com/akakikuumeri/Akaki-H-shifter
    - Akaki Kuumeri GitHub: https://github.com/akakikuumeri
- Original build and showcase video by Akaki Kuumeri: https://www.youtube.com/watch?v=fTknGo8n0Co
    - Akaki Kuumeri YouTube channel: https://www.youtube.com/@akakikuumeri
- Detailed assembly video by S60Team: https://www.youtube.com/watch?v=xyWhzIESEnE
  - S60Team YouTube channel: https://www.youtube.com/@S60Team
- Clamp designer on MakerWorld: https://makerworld.com/en/@sweb3791
  - G-Clamp deeper model: https://makerworld.com/en/models/504560-g-clamp-deeper?from=search#profileId-420208

---

## Contributing

Contributions, issues and pull requests are welcome. When submitting changes, prefer small focused commits and document any pin or wiring changes.

---

## License

See the `LICENSE` file in this repository.

---

Enjoy building and racing!
