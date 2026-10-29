# Evidence review and pending material

## Received

- One Proteus schematic screenshot.
- Five LCD operating-point photographs and one complete prototype photograph.

## Still needed

- Exact original display library versions and confirmation of the final hardware firmware version.
- Required custom Proteus libraries/models, if absent from the installed environment.
- Confirmation of actual wiring, board clock, PWM pin and timer configuration.
- Exact installed diode types, capacitor ratings, inductor current rating, and driver supply.

## Interpretation limits

- The documented design distinguishes a 12-50 V simulation range from a 12-20 V hardware range.
- The documented simulation and hardware use different bootstrap capacitor values.
- The attached schematic labels the driver supply +10 V; the hardware description specifies a separate 20 V driver supply. Resolve this discrepancy from the actual hardware and component documentation before reconstruction.
- Reference percentage is not evidence of actual PWM duty.
- The documented feedback measures voltage, not motor RPM.
- Claims of soft start, current suppression, efficiency, ripple, and line regulation require independent measurements; the source alone does not verify these performance claims.
- The 1/11 divider calculation does not by itself establish comprehensive overvoltage protection.
- The stated switching frequency is 31.25 kHz; confirm it by measurement.
- Original sketch and editable Proteus project were subsequently recovered alongside the supplied project. An existing HEX build is included; source/build equivalence is unverified. No independent measurement logs or license are available.

This review records the supplied project evidence.

## Recovered source findings

- The Proteus project embeds ROOT.DSN and ROOT.CDB, and references `sketch_may16a/build/arduino.avr.uno/sketch_may16a.ino.hex`.
- The matching sketch folder and referenced HEX were found beside the supplied Proteus project. Originals were copied without modification.
- Sketch pins are PWM D9, reference A0, Vin A1, Vout A2. The screenshot uses the opposite A1/A2 sensing assignment.
- Source includes LCD 0x27 and SSD1306 OLED 0x3C, matching the two displays visible in prototype photographs.
- Displayed Vout is replaced by the target if absolute error is below 0.8 V; zero target also forces displayed zero. Published accuracy claims need independent raw-voltage measurements.
- No compile, hardware upload, or Proteus execution was performed. Existing HEX provenance relative to the recovered sketch is unverified.
