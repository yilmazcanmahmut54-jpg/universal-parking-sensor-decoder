# Protokol Tersine Mühendislik Notları

Bu doküman test edilen 4 sensörlü park sensörü ECU'sunun ekran SIGNAL hattından yapılan gerçek ölçümleri içerir.

## Fiziksel katman ve zamanlama
Tek dijital SIGNAL hattı CHANGE interrupt ile gözlenmiştir.

- Çerçeve arası LOW: yaklaşık 33100–33300 us
- Header HIGH: yaklaşık 960 us
- 0 biti HIGH: yaklaşık 76–84 us
- 0 biti LOW: yaklaşık 240–252 us
- 1 biti HIGH: yaklaşık 212–220 us
- 1 biti LOW: yaklaşık 108–120 us

Decoder toleransları:
- Frame gap LOW: 25000–40000 us
- Header HIGH: 700–1200 us
- Bit 0 HIGH: 50–130 us
- Bit 1 HIGH: 160–270 us

## Çerçeve
Her frame 17 bittir. Aktif ölçüm:

```
1 0000 XXXX XXXX XXXX
```

Son 12 bit RAW alanıdır.

Boş/bağlı olmayan slotlarda test sırasında şu örnekler görüldü:

```
1 0001 0001 1111 1110
1 0001 0011 1111 1110
1 0001 0101 1111 1110
1 0001 0111 1111 1110
```

Bu nedenle decoder yalnızca aktif `1 0000` prefix'ini mesafe olarak işler.

## Kanal kodlaması
12-bit RAW alanında kanal tabanı 0x200 adımlarla ilerler:

- A = 0x000
- B = 0x200
- C = 0x400
- D = 0x600

```
channel = raw >> 9
distance_raw = raw & 0x1FF
distance_cm ~= distance_raw / 2
```

## Kontrollü A sensörü kalibrasyonu

| Fiziksel mesafe | RAW | Hesaplanan |
|---:|---:|---:|
| 30 cm | 64 | 32 cm |
| 40 cm | 78 | 39 cm |
| 50 cm | 100 | 50 cm |
| 60 cm | 116 | 58 cm |
| 70 cm | 142 | 71 cm |
| 80 cm | 160 | 80 cm |
| 90 cm | 182 | 91 cm |
| 100 cm | 200 | 100 cm |
| 110 cm | 216 | 108 cm |
| 120 cm | 234 | 117 cm |
| 130 cm | 256 | 128 cm |
| 140 cm | 274 | 137 cm |
| 150 cm | 300 | 150 cm |
| 160 cm | 316 | 158 cm |
| 170 cm | 330 | 165 cm |
| 180 cm | 356 | 178 cm |
| 190 cm | 374 | 187 cm |
| 200 cm | 396 | 198 cm |
| 210 cm | 412 | 206 cm |
| 220 cm | 438 | 219 cm |

Ultrasonik ölçüm ve hedef geometrisi nedeniyle birkaç cm sapma normaldir.

## B/C/D doğrulaması

B:
- ~50 cm: raw 0x268 = 616 → (616-512)/2 = 52 cm
- ~100 cm: raw 716 → (716-512)/2 = 102 cm
- ~200 cm: raw 916 → (916-512)/2 = 202 cm

C:
- ~50 cm: raw 1128 → (1128-1024)/2 = 52 cm
- ~100 cm: raw 1224 → (1224-1024)/2 = 100 cm

D:
- ~50 cm: raw 1636 → (1636-1536)/2 = 50 cm
- ~100 cm: raw 1736 → (1736-1536)/2 = 100 cm

D ölçümleri özellikle 0x600 kanal tabanını doğrulamıştır.

## Önemli sınırlama
Bu bilgiler tek bir ECU ailesinde deneysel olarak elde edilmiştir. Başka park sensörü kitlerinde timing, voltaj, frame biçimi veya mesafe ölçeği değişebilir. Önce SIGNAL voltajını ölçün ve ham frame debug ile protokolü doğrulayın.
