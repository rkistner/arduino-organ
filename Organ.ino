// pins 15~17 to GND, I2C bus address is 0x20
#include "Wire.h"

#define EVENT_NOTE_ON 0x09
#define EVENT_NOTE_OFF 0x08

byte last[24];
byte counter[24];

const int DOWN = 0b01;
const int UP = 0b10;

const byte NOTE_MAPPING[12] = {59, 57, 55, 53, 51, 49, 52, 50, 54, 56, 58, 60};
const byte VELOCITY_MAPPING[26] = {127, 111, 97, 86, 73, 64, 58, 53, 48, 44, 40, 36, 33, 30, 27, 24, 22, 20, 18, 16, 15, 14, 13, 12, 11, 10};
/*
No ID MIDI
C# 5  49
D  7  50
D# 4  51
E  6  52
F  3  53
F# 8  54
G  2  55
G# 9  56
A  1  57
A# 10 58
B  0  59
C  11 60
*/

void sendMidi(byte event, byte m1, byte m2, byte m3) {
  MIDISerial.write(event);
  MIDISerial.write(m1);
  MIDISerial.write(m2);
  MIDISerial.write(m3);
  MIDISerial.flush();
}

void setup()
{
    Wire.begin(); // wake up I2C bus
    // Increase the speed - http://electronics.stackexchange.com/questions/29457/how-to-make-arduino-do-high-speed-i2c
    TWBR=2;
    
    Wire.beginTransmission(0x20);
    Wire.write(0x00); // IODIRA register
    Wire.write(0x00); // set all of bank A to outputs
    Wire.endTransmission();
    
    
    Wire.beginTransmission(0x20);
    Wire.write(0x01); // IODIRB register
    Wire.write(0b00000011); // set all of bank B to inputs
    Wire.endTransmission();
}
void loop()
{
    for(int i = 0; i < 12; i++) {
        int a, b;
        if(i < 8) {
            a = 0xFF & ~(1 << i);
            b = 0xFF;
        } else {
            a = 0xFF;
            b = 0xFF & ~(1 << (i - 6));
        }
        Wire.beginTransmission(0x20);
        Wire.write(0x12); // address bank A
        Wire.write(a); // value to send
        Wire.endTransmission();
        
        Wire.beginTransmission(0x20);
        Wire.write(0x13); // address bank B
        Wire.write(b); // value to send
        Wire.endTransmission();

        Wire.beginTransmission(0x20);
        Wire.write(0x13); // set MCP23017 memory pointer to GPIOB address
        Wire.endTransmission();
        Wire.requestFrom(0x20, 1); // request one byte of data from MCP20317
        byte inputs=Wire.read(); // store the incoming byte into "inputs"
        byte pressed[2];
        pressed[0] = inputs & 0b11;
        pressed[1] = (inputs >> 6) & 0b11;
        
        for(int o = 0; o < 2; o++) {
            int j = o*12+i;
            if(pressed[o] == 0b11) {
                if(counter[j] < 50) {
                    counter[j] += 1;
                }
            } else {
                if(pressed[o] != last[j]) {
                   byte note = NOTE_MAPPING[i];
                   byte count = counter[j];
                   byte velocity;
                   if(count < 26) {
                       velocity = VELOCITY_MAPPING[count];
                   } else {
                       velocity = 10;
                   }
                   velocity = 64;
                   if(pressed[o] == DOWN) {
                     sendMidi(EVENT_NOTE_ON, 0x91, note+o*12, velocity);
                   } else {
                     sendMidi(EVENT_NOTE_OFF, 0x81, note+o*12, velocity);
                   }
                   /*
                   Serial.print(i);
                   Serial.print(' ');
                   if(pressed[o] == DOWN) {
                     Serial.print("DOWN");
                   } else {
                     Serial.print("UP");
                   }
                   Serial.print(' ');
                   Serial.print(counter[j]);
                   Serial.println();
                   */
                }
                counter[j] = 0;
                last[j] = pressed[o];
            }
        }
        
    }
}

