/*
 Example 41.2 - Microchip MCP23017 with Arduino
 http://tronixstuff.wordpress.com/tutorials > chapter 41
 John Boxall | CC by-sa-nc
 */
// pins 15~17 to GND, I2C bus address is 0x20
#include "Wire.h"

byte last[12];
byte counter[12];

void setup()
{
    Serial.begin(9600);
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
            counter[i] += 1;
        } else {
            if(pressed != last[i]) {  // || (pressed == 0b11 && i >= 5)
              Serial.print(i);
              Serial.print(' ');
              if(pressed == 0b01) {
                 Serial.print("DOWN");
              } else {
                  Serial.print("UP");
              }
              Serial.print(' ');
              Serial.print(counter[i]);
              Serial.println();
            }
            counter[i] = 0;
            last[i] = pressed;
        }
        
    }
}

