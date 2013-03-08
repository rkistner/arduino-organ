/*
 Example 41.2 - Microchip MCP23017 with Arduino
 http://tronixstuff.wordpress.com/tutorials > chapter 41
 John Boxall | CC by-sa-nc
 */
// pins 15~17 to GND, I2C bus address is 0x20
#include "Wire.h"
#define MIDI_AUTO_INSTANCIATE    0
#include <MIDI.h>

MIDI_CREATE_INSTANCE(HardwareSerial, Serial, MyMIDI)

byte last[12];
byte counter[12];

const int DOWN = 0b01;
const int UP = 0b10;

const byte NOTE_MAPPING[12] = {59, 57, 55, 53, 51, 49, 52, 50, 54, 56, 58, 60};
const byte VELOCITY_MAPPING[26] = {127, 111, 97, 86, 73, 64, 58, 53, 48, 44, 40, 36, 33, 30, 27, 24, 22, 20, 18, 16, 15, 14, 13, 12, 11, 10};
/*
Note ID MIDI
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

void setup()
{
    Wire.begin(); // wake up I2C bus
    TWBR=2;  // Increase the speed - http://electronics.stackexchange.com/questions/29457/how-to-make-arduino-do-high-speed-i2c
    
    Wire.beginTransmission(0x20);
    Wire.write(0x00); // IODIRA register
    Wire.write(0x00); // set all of bank A to outputs
    Wire.endTransmission();
    
    
    Wire.beginTransmission(0x20);
    Wire.write(0x01); // IODIRB register
    Wire.write(0b00000011); // set all of bank B to inputs
    Wire.endTransmission();
    
    MyMIDI.begin(4);          // Launch MIDI and listen to channel 4
    Serial.begin(115200);
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
        byte pressed = inputs & 0b11;

        if(pressed == 0b11) {
            if(counter[i] < 50) {
                counter[i] += 1;
            }
        } else {
            if(pressed != last[i]) {
               byte note = NOTE_MAPPING[i];
               byte count = counter[i];
               byte velocity;
               if(count < 26) {
                   velocity = VELOCITY_MAPPING[count];
               } else {
                   velocity = 10;
               }
               if(pressed == DOWN) {
                   MyMIDI.sendNoteOn(note, velocity, 1);  // Send a Note (pitch 42, velo 127 on channel 1)
               } else {
                   MyMIDI.sendNoteOff(note, velocity, 1);
               }
//              Serial.print(i);
//              Serial.print(' ');
//              if(pressed == 0b01) {
//                 Serial.print("DOWN");
//              } else {
//                  Serial.print("UP");
//              }
//              Serial.print(' ');
//              Serial.print(counter[i]);
//              Serial.println();
            }
            counter[i] = 0;
            last[i] = pressed;
        }
        
    }
}

