// pins 15~17 to GND, I2C bus address is 0x20
#include "Wire.h"

#include "panel.h"

#define EVENT_NOTE_ON 0x09
#define EVENT_NOTE_OFF 0x08

#define NUM_KEYS 148

byte last[NUM_KEYS];
byte current[NUM_KEYS];
byte counter[NUM_KEYS];

const int DOWN = 0b10;
const int UP = 0b01;
const int HALFWAY = 0b11;

const int IO_SCAN = 0x20;
// Solo keyboard and pedals
const int IO_RETURN1 = 0x21;
// Upper and lower manuals
const int IO_RETURN2 = 0x22;

const int IODIRA = 0x00;
const int IODIRB = 0x01;
const int IOVALA = 0x12;
const int IOVALB = 0x13;

const int CHANNEL_PEDALS = 0;
const int CHANNEL_LOWER = 1;
const int CHANNEL_UPPER = 2;
const int CHANNEL_SOLO = 3;

// Mapping from return line number to note sequence number
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

const byte NOTE_MAPPING[13] = {11, 9, 7, 5, 3, 1, 0, 2, 4, 6, 8, 10, 12};

// Mapping from tick count to velocity (not used currently)
const byte VELOCITY_MAPPING[26] = {127, 111, 97, 86, 73, 64, 58, 53, 48, 44, 40, 36, 33, 30, 27, 24, 22, 20, 18, 16, 15, 14, 13, 12, 11, 10};


void sendMidi(byte event, byte m1, byte m2, byte m3) {
  MIDIEvent e = {event, m1, m2, m3};
  MIDIUSB.write(e);
}

void wireWrite(int io, int addr, int val) {
  Wire.beginTransmission(io);
  Wire.write(addr);
  Wire.write(val);
  Wire.endTransmission();
}

int wireRead(int io, int addr) {
  Wire.beginTransmission(io);
  Wire.write(addr);
  Wire.endTransmission();
  Wire.requestFrom(io, 1);
  return Wire.read();
}

// Given: return value v, positions a and b
// Returns: the two bits at positions a (low bit) and b (high bit)
int getKey(int v, int a, int b) {
  return ((v >> a) & 0b1) | (((v >> b) & 0b1) << 1);
}

void keyboard_setup()
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
  
  // IO_RETURN2 - all inputs
  wireWrite(IO_RETURN2, IODIRA, 0xFF);
  wireWrite(IO_RETURN2, IODIRB, 0xFF);
}

void setup() {
  panel_setup();
  keyboard_setup();
}

void sendAllMidi(int channel, int offset, int length, int midiOffset) {
  for(int i = 0; i < length; i++) {
    int value = current[offset+i];
    
    if(value != HALFWAY && value != last[offset+i]) {
      int velocity = 64;
      if(value == DOWN) {
        sendMidi(EVENT_NOTE_ON, 0x90 | channel, i + midiOffset, velocity);
      } else if(value == UP) {
        sendMidi(EVENT_NOTE_OFF, 0x80 | channel, i + midiOffset, velocity);
      }
      last[offset+i] = value;
    }
  }
  MIDIUSB.flush();
}

void keyboard_loop() {
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
    int return2b = wireRead(IO_RETURN2, IOVALB);
    int return2a = wireRead(IO_RETURN2, IOVALA);
        
    int note = NOTE_MAPPING[i];
    
    // Pedals - only 1 switch for down, no halfway switch
    if(return1 & (1 << 6)) {
      current[0 + note] = UP;
    } else {
      current[0 + note] = DOWN;
    }
    
    
    // Lower manual
    current[13 + note] = getKey(return2a, 4, 3);
    current[25 + note] = getKey(return2a, 5, 2);
    current[37 + note] = getKey(return2a, 6, 1);
    current[49 + note] = getKey(return2a, 7, 0);
    
    // Upper manual
    current[62 + note] = getKey(return2b, 3, 4);
    current[74 + note] = getKey(return2b, 2, 5);
    current[86 + note] = getKey(return2b, 1, 6);
    current[98 + note] = getKey(return2b, 0, 7);
    
    // Solo
    current[111 + note] = getKey(return1, 2, 3);
    current[123 + note] = getKey(return1, 1, 4);
    current[135 + note] = getKey(return1, 0, 5);
  }
  
  for(int channel = 0; channel < 4; channel++) {
    if(channel == CHANNEL_PEDALS) {
      sendAllMidi(channel, 0, 13, 48);
    } else if(channel == CHANNEL_LOWER) {
      sendAllMidi(channel, 13, 49, 36);
    } else if(channel == CHANNEL_UPPER) {
      sendAllMidi(channel, 62, 49, 36);
    } else if(channel == CHANNEL_SOLO) {
      sendAllMidi(channel, 111, 37, 36);
    }
  }
}


void loop() {
  keyboard_loop();
  panel_loop();
}

