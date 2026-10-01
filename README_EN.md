# Universal Parking Sensor Decoder

[Türkçe README](README.md)

Open-source Arduino decoder for the single-wire display SIGNAL output used by a tested four-channel ultrasonic parking sensor ECU.

The protocol was empirically reverse-engineered from real timing captures and controlled distance measurements. Similar-looking parking sensor kits are **not guaranteed** to use the same protocol.

## Compatible parking sensor type

This project is intended for **parking sensor kits with an external display**. On the tested system, the cable from the parking sensor ECU to the display has **3 wires**:

- **+5V** — display power
- **GND / negative (-)** — common ground
- **SIGNAL** — carries the sensor distance data. On this type of parking sensor kit, the **SIGNAL wire is commonly yellow**.

For Arduino data reading, the **+5V display wire is not connected to an Arduino input**. Connect ECU **GND to Arduino GND** and **SIGNAL to Arduino D2**.

Although the SIGNAL wire is commonly yellow, wire colors can vary by manufacturer. Do not rely on color alone; verify +5V, GND and SIGNAL with measurements before connecting.

## Wiring

| Parking ECU | Arduino |
|---|---|
| SIGNAL | D2 (INT0) |
| GND | GND |

Power the ECU normally and share ground with the Arduino. Measure the SIGNAL voltage before connection; other ECU variants may require level shifting or input protection.

## Frame format

Active measurement frames contain 17 bits:

```
1 0000 XXXX XXXX XXXX
```

The final 12-bit field contains the sensor channel block and distance value.

Measured timing:
- Inter-frame LOW: ~33.1–33.3 ms
- Header HIGH: ~960 us
- Bit 0 HIGH: ~76–84 us
- Bit 1 HIGH: ~212–220 us

Channel bases:
- A: 0x000
- B: 0x200
- C: 0x400
- D: 0x600

Distance:

```
distance_cm ~= (raw - channel_base) / 2
```

The Arduino implementation uses `raw >> 9` for the channel and `raw & 0x1FF` for the distance field.

## Arduino
Open:

`arduino/universal_parking_sensor_decoder/universal_parking_sensor_decoder.ino`

Upload to an Arduino and open Serial Monitor at 115200 baud.

Example:

```
A=50.0 cm | B=101.0 cm | C=100.0 cm | D=50.0 cm
```

For measured calibration data and reverse-engineering details see [docs/PROTOKOL_TR.md](docs/PROTOKOL_TR.md).

## Raw frame test

To check whether a new or different ECU uses the same protocol, upload [raw_frame_test.ino](examples/raw_frame_test/raw_frame_test.ino).

The test sketch displays the **17-bit raw frame**, detected **sensor channel**, **RAW value**, and calculated **distance** in Serial Monitor at 115200 baud.

Example:

```
FRAME: 1 0000 0000 1100 1000 | SENSOR: A | RAW: 200 | DISTANCE: 100.0 cm
```

## Compatibility
“Universal” means the project is designed to be adapted to similar parking ECUs. It does not claim compatibility with every manufacturer or parking sensor kit.

## Inspiration / Reference
This work was started **inspired by** [morcibacsi/esp32_rmt_chinese_parking_aid](https://github.com/morcibacsi/esp32_rmt_chinese_parking_aid). That project demonstrated that data on the single SIGNAL wire of a parking sensor display system can be decoded with a microcontroller.

The protocol decoding, timing values, 17-bit frame structure, A/B/C/D channel mapping, distance formula, and Arduino code in this repository were obtained from **our own measurements and tests** on a different parking sensor ECU. The reference project's ECU does not use the same protocol/timing as the ECU tested here.

## License
MIT License.
