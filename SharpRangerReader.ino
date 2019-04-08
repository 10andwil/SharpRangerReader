/*
Testing the Sharp GP2Y0E02B sensor
Author: Marcus Ahlberg (marcus[at]aahlborg.se)
*/

#include <Wire.h>

#define ADDRESS         (0x80 >> 1) // 7 highest bits

#define SHIFT_ADDR      0x35
#define DISTANCE_ADDR   0x5E
#define RIGHT_EDGE_ADDR 0xF8 // C
#define LEFT_EDGE_ADDR  0xF9 // A
#define PEAK_EDGE_ADDR  0xFA // B

float distance_raw[2] = {0};
uint8_t shift = 0;
float distance_cm = 0;
char buf[100];

void setup()
{
  Wire.begin();
  Serial.begin(9600);

  delay(2000);

  Serial.println("Initializing");

  Wire.beginTransmission(ADDRESS);
  Wire.write(byte(SHIFT_ADDR));
  Wire.endTransmission();

  Wire.requestFrom(ADDRESS, 1);
  if (1 <= Wire.available())
  {
    shift = Wire.read();
  }

  Serial.print("Read shift bit: ");
  Serial.println(shift, HEX);
}

void loop()
{
  // Read basic measurement
  Wire.beginTransmission(ADDRESS);
  Wire.write(byte(DISTANCE_ADDR));
  Wire.endTransmission();

  Wire.requestFrom(ADDRESS, 2);

  if (2 <= Wire.available())
  {
    distance_raw[0] = Wire.read();
    distance_raw[1] = Wire.read();

    // Print distance in cm
    distance_cm = (distance_raw[0] * 16 + distance_raw[1]) / 16 / (int)pow(2, shift);
    dtostrf(distance_cm, 10, 2, buf);
    Serial.print("Distance: "); Serial.print(buf); Serial.println(" cm");
  }
  else
  {
    Serial.println("Read error");
  }

  // Read A-B-C data
  Wire.beginTransmission(ADDRESS);
  Wire.write(byte(RIGHT_EDGE_ADDR));
  Wire.endTransmission();

  Wire.requestFrom(ADDRESS, 3);

  if (3 <= Wire.available())
  {
    uint8_t c = Wire.read();
    uint8_t a = Wire.read();
    uint8_t b = Wire.read();
    uint8_t spotSize = c - a;
    uint8_t spotSym = abs((c - a) - 2 * b);

    // Print info
    sprintf(buf, "A: %u, B: %u, C: %u", a, b, c);
    Serial.println(buf);
    sprintf(buf, "Size: %u, Symmetry: %u", spotSize, spotSym);
    Serial.println(buf);
  }
  else
  {
    Serial.println("Read error");
  }

  delay(250);
}

