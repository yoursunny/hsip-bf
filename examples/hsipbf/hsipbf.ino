#include <hsip-bf.hpp>

// Each hash function needs a 8-byte key, but they live in PROGMEM and don't consume RAM.
const uint8_t keys[] PROGMEM = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                                 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
                                 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
                                 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
                                 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
                                 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
                                 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77 };

// 4096-bit Bloom filter with 8 hash functions
HsipBf<4096, 8> bf(keys);

void setup()
{
  Serial.begin(115200);
  Serial.println();

  // add 16-bit binary numbers
  for (uint16_t i = 0; i < 128; ++i) {
    bf.add(&i, sizeof(i));
  }
  // add strings
  for (int i = 0; i < 128; ++i) {
    bf.add(String(i));
  }
  Serial.println(F("256 items added to Bloom filter"));

  // check 16-bit binary numbers
  for (uint16_t i = 0; i < 128; ++i) {
    if (!bf.contains(&i, sizeof(i))) {
      Serial.println(F("False negative detected."));
    }
  }
  // check strings
  for (int i = 0; i < 128; ++i) {
    if (!bf.contains(String(i))) {
      Serial.println(F("False negative detected."));
    }
  }

  Serial.println(F("Starting false positive test."));
  // expected FP%=0.000574496 https://hur.st/bloomfilter/?n=256&p=&m=4096&k=8
}

void loop()
{
  static unsigned long i = 0;
  static unsigned long nFound = 0;

  nFound += bf.contains(&i, sizeof(i));
  if (++i % 65536 == 0) {
    Serial.print("FP%=");
    Serial.println(double(nFound) / i, 9);
  }
}
