# Smart Ventilator 
<img width="235" height="311" alt="Image" src="https://github.com/user-attachments/assets/6b88ac00-15b8-44c1-8666-e66dcdd6522a" />
<img width="269" height="280" alt="Image" src="https://github.com/user-attachments/assets/eaac81d7-ef83-48a2-8072-0999e7fb8202" />

This project implements a basic ventilator control system using an Arduino, a MAX30100 pulse oximeter sensor, and a servo motor. It features **automatic** and **manual** modes to control the servo based on breathing parameters or oxygen saturation levels. Real-time vital signs and ventilator parameters are displayed on an OLED screen.

---

## Features

- **Pulse Oximeter integration:** Reads heart rate (BPM) and blood oxygen saturation (SpO₂) via MAX30100 sensor.
- **Servo motor control:** Controls servo angle based on SpO₂ levels or manual breathing parameters.
- **Manual mode:** User-adjustable ventilation parameters via analog sensors (air volume, exhalation time, air speed).
- **Automatic mode:** Servo moves smoothly between preset angles based on SpO₂ thresholds.
- **OLED Display:** Displays heart rate, SpO₂, and ventilator parameters in real time.
- **Button control:** Toggles between manual and automatic modes using a hardware button.
- **Interrupt-driven button reading** for responsive mode switching.

---

## Hardware Components

- Arduino (Uno, Mega, or compatible)
- MAX30100 Pulse Oximeter sensor
- Servo motor (e.g., SG90 or similar)
- OLED display (compatible with OakOLED library)
- Three analog sensors/potentiometers for:
  - Air volume (connected to A2)
  - Exhalation time (connected to A1)
  - Air speed (connected to A0)
- Push button connected to pin 2 (with internal pull-up resistor enabled)
- Connecting wires and power supply

---

## Pin Configuration

| Component             | Arduino Pin          |
|-----------------------|----------------------|
| Air Volume Sensor     | A2                   |
| Exhalation Time Sensor| A1                   |
| Air Speed Sensor      | A0                   |
| Servo Motor           | Digital 5            |
| Button                | Digital 2            |
| MAX30100 Sensor       | I2C (SDA, SCL) pins (A4, A5 on Uno) |
| OLED Display          | I2C (SDA, SCL) pins (A4, A5 on Uno) |

---

## Software Libraries Required

- [MAX30100_PulseOximeter](https://github.com/oxullo/Arduino-MAX30100)
- [OakOLED](https://github.com/OakOLED/OakOLED)
- [Servo](https://www.arduino.cc/en/Reference/Servo)
- [Wire](https://www.arduino.cc/en/Reference/Wire)
- [TimerOne](https://github.com/PaulStoffregen/TimerOne) (optional)

---

## How It Works

- On startup, the pulse oximeter sensor is initialized and OLED display shows status.
- The system reads heart rate and SpO₂ values from the sensor continuously.
- **Automatic Mode:**  
  The servo motor moves smoothly between two angles:  
  - `startAngle` (110°) and  
  - either `normalSpO2End` (70°) or `criticalSpO2End` (45°) depending on the latest SpO₂ reading.  
  The movement direction reverses at each endpoint for smooth oscillation.
- **Manual Mode:**  
  The servo is controlled based on analog sensor inputs representing air volume, exhalation time, and air speed. The OLED displays these parameters live.  
  The servo position and delay are adjusted dynamically according to sensor readings.
- The user toggles between manual and automatic modes by pressing the button connected to pin 2.

---

## Usage

1. Connect all hardware components as per the pin configuration.
2. Install the required Arduino libraries.
3. Upload the provided Arduino sketch to the board.
4. Open the Serial Monitor at 9600 baud to view live heart rate and SpO₂ data.
5. Use the button to switch between automatic and manual ventilation modes.
6. Adjust manual mode sensors to control the servo motor behavior.
7. Monitor real-time data on the OLED display.

---

## Notes

- The servo motion speed and angles can be adjusted by modifying the `moveSpeed`, `startAngle`, `normalSpO2End`, and `criticalSpO2End` constants in the code.
- Analog sensor values are scaled and rounded for calculation of breath volume and timing.
- The `_delay()` function is a custom non-blocking delay that updates the pulse oximeter during wait periods.
- The system assumes sensors connected to analog inputs provide voltage proportional to desired control parameters.

---

## License

This project is open-source and free to use for educational and non-commercial purposes.

---
