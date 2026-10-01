# Universal Parking Sensor Decoder

[English README](README_EN.md)

Arduino Uno/Mega kullanarak, harici ekranlı 4 sensörlü ultrasonik park sensörü ECU'larının **tek hatlı SIGNAL çıkışını** çözmek için geliştirilmiş açık kaynak proje.

> Bu protokol gerçek bir park sensörü ECU'su üzerinde osiloskop/zaman ölçümleri ve kontrollü mesafe testleriyle tersine mühendislik yöntemiyle çıkarılmıştır. Benzer görünümlü tüm kitlerin aynı protokolü kullandığı garanti edilmez.

## Test edilen donanım
- Arduino Uno
- Arduino Mega 2560
- 4 sensörlü park sensörü ECU
- ECU ekranına giden SIGNAL hattı
- Ortak GND

MYCAR entegrasyonunda Arduino Mega üzerinde 4 sensör aynı anda başarıyla okunmuştur.

## Bağlantı
| Park sensörü ECU | Arduino |
|---|---|
| SIGNAL | D2 (INT0) |
| GND | GND |

ECU kendi normal beslemesiyle çalıştırılır.

**Önemli:** SIGNAL hattını Arduino'ya bağlamadan önce voltajını ölçün. Bu projede test edilen ECU doğrudan Uno/Mega ile çalışmıştır; farklı ECU'larda seviye dönüştürücü veya giriş koruması gerekebilir.

## Protokol özeti
Bir veri çerçevesi 17 bittir:

```
1 0000 XXXX XXXX XXXX
```

Aktif ölçüm çerçevesinde ilk 5 bit `1 0000`, sonraki 12 bit sensör kanalı ve mesafe bilgisidir.

Yaklaşık zamanlamalar:
- Frame gap LOW: 33.1–33.3 ms
- Header HIGH: ~960 us
- Bit 0: HIGH ~76–84 us, LOW ~240–252 us
- Bit 1: HIGH ~212–220 us, LOW ~108–120 us

Kanal blokları:
| Sensör | RAW aralığı | Taban |
|---|---:|---:|
| A | 0x000–0x1FF | 0x000 |
| B | 0x200–0x3FF | 0x200 |
| C | 0x400–0x5FF | 0x400 |
| D | 0x600–0x7FF | 0x600 |

Mesafe yaklaşık olarak:

```
mesafe_cm = (raw - kanal_tabani) / 2
```

Örnek: A sensörü 100 cm civarında `raw=200` verir → 200/2 = 100 cm.

Detaylı tersine mühendislik verileri: [docs/PROTOKOL_TR.md](docs/PROTOKOL_TR.md)

## Kullanım
`arduino/universal_parking_sensor_decoder/universal_parking_sensor_decoder.ino` dosyasını Arduino IDE ile açın, kartınızı seçin ve yükleyin. Serial Monitor'ü **115200 baud** açın.

Çıktı örneği:

```
A=50.0 cm | B=101.0 cm | C=100.0 cm | D=50.0 cm
```

## Not
Bu proje belirli bir ECU üzerinde deneysel olarak doğrulanmıştır. “Universal” adı, kodun farklı benzer kitlere uyarlanabilmesi amacıyla kullanılmıştır; bütün üreticilerle otomatik uyumluluk iddiası değildir.

## Lisans
MIT License.
