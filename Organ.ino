// pins 15~17 to GND, I2C bus address is 0x20
#include "Wire.h"

#define EVENT_NOTE_ON 0x09
#define EVENT_NOTE_OFF 0x08

#define NUM_KEYS 37

byte last[NUM_KEYS];
byte current[NUM_KEYS];
byte counter[NUM_KEYS];

const int DOWN = 0b10;
const int UP = 0b01;
const int HALFWAY = 0b11;

const int IO_SCAN = 0x20;
// Solo keyboard and pedals
const int IO_RETURN1 = 0x21;
const int IODIRA = 0x00;
const int IODIRB = 0x01;
const int IOVALA = 0x12;
const int IOVALB = 0x13;

const byte NOTE_MAPPING[13] = {59, 57, 55, 53, 51, 49, 48, 50, 52, 54, 56, 58, 60};
const byte VELOCITY_MAPPING[26] = {127, 111, 97, 86, 73, 64, 58, 53, 48, 44, 40, 36, 33, 30, 27, 24, 22, 20, 18, 16, 15, 14, 13, 12, 11, 10};
/*
N  ID MIDI
Cl 6  48
C# 5  49
D  7  50
D# 4  51
E  8  52
F  3  53
F# 9  54
G  2  55
G# 10  56
A  1  57
A# 11 58
B  0  59
C  12 60
*/

void sendMidi(byte event, byte m1, byte m2, byte m3) {
  MIDISerial.write(event);
  MIDISerial.write(m1);
  MIDISerial.write(m2);
  MIDISerial.write(m3);
  MIDISerial.flush();
}

void wireWrite(int io, int addr, int val) {
  Wire.beginTransmission(io);
  Wire.write(addr);
  Wire.write(val); // outputs
  Wire.endTransmission();
}

int wireRead(int io, int addr) {
  Wire.beginTransmission(io);
  Wire.write(addr);
  Wire.endTransmission();
  Wire.requestFrom(io, 1);
  return Wire.read();
}

void setup()
{
  for(int i = 0; i < NUM_KEYS; i++) {
    last[i] = UP;
  }
  Wire.begin(); // wake up I2C bus
  // Increase the speed - http://electronics.stackexchange.com/questions/29457/how-to-make-arduino-do-high-speed-i2c
  TWBR=2;
  
  // IO_SCAN - all outputs
  wireWrite(IO_SCAN, IODIRA, 0x00);
  wireWrite(IO_SCAN, IODIRB, 0x00);
  
  // IO_RETURN1 - all inputs
  wireWrite(IO_RETURN1, IODIRA, 0xFF);
  wireWrite(IO_RETURN1, IODIRB, 0xFF);
}

void loop()
{
  for(int i = 0; i < 13; i++) {
    int a, b;
    if(i < 6) {
      a = 0xFF;
      b = 0xFF & ~(1 << i);
    } else {
      a = 0xFF & ~(1 << (i - 5));
      b = 0xFF;
    }
    
    wireWrite(IO_SCAN, IOVALA, a);
    wireWrite(IO_SCAN, IOVALB, b);
    
    int return1 = wireRead(IO_RETURN1, IOVALB);
    int octaves[3];
    octaves[0] = (return1 >> 2) & 0b11;
    octaves[1] = ((return1 >> 1) & 0b1) | (((return1 >> 4) & 0b1) << 1);
    octaves[2] = (return1 & 0b1) | (((return1 >> 5) & 0b1) << 1);

    for(int octave = 0; octave < 3; octave++) {
      int note = NOTE_MAPPING[i] - 48 + octave*12;
      current[note] = octaves[octave];
    }
  }
  
  for(int i = 0; i < NUM_KEYS; i++) {
    int value = current[i];
    
    if(value != HALFWAY && value != last[i]) {
      int velocity = 64;
      if(value == DOWN) {
        sendMidi(EVENT_NOTE_ON, 0x91, i + 36, velocity);
      } else if(value == UP) {
        sendMidi(EVENT_NOTE_OFF, 0x81, i + 36, velocity);
      }
      last[i] = value;
    }
  }
}

