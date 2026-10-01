# Universal Parking Sensor Decoder

[Türkçe README](README.md)

Open-source Arduino decoder for the single-wire display SIGNAL output used by a tested four-channel ultrasonic parking sensor ECU.

The protocol was empirically reverse-engineered from real timing captures and controlled distance measurements. Similar-looking parking sensor kits are **not guaranteed** to use the same protocol.

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

## Compatibility
“Universal” means the project is designed to be adapted to similar parking ECUs. It does not claim compatibility with every manufacturer or parking sensor kit.

## License
MIT License.
