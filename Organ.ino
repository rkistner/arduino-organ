/*
 Example 41.2 - Microchip MCP23017 with Arduino
 http://tronixstuff.wordpress.com/tutorials > chapter 41
 John Boxall | CC by-sa-nc
 */
// pins 15~17 to GND, I2C bus address is 0x20
#include "Wire.h"

byte last[8];

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
    Wire.write(0xFF); // set all of bank B to inputs
    Wire.endTransmission();
    
//    Wire.endTransmission(0x20);
//    Wire.write(0x0D); // GPPUB register
//    Wire.write(0xFF); // enable all pull-up resistors on bank B
//    Wire.endTransmission();

}
void loop()
{
//    

//    
//    delay(1);
//    
//    
    
//    delay(1);
//    
    for(int i = 0; i < 8; i++) {
        Wire.beginTransmission(0x20);
        Wire.write(0x12); // address bank A
        int v = 0xFF & ~(1 << i);
        Wire.write(v); // value to send
        Wire.endTransmission();
        //delay(1);

        Wire.beginTransmission(0x20);
        Wire.write(0x13); // set MCP23017 memory pointer to GPIOB address
        Wire.endTransmission();
        Wire.requestFrom(0x20, 1); // request one byte of data from MCP20317
        byte inputs=Wire.read(); // store the incoming byte into "inputs"
        byte pressed = inputs & 0b11;
        if(pressed != last[i]) {  // || (pressed == 0b11 && i >= 5)
          Serial.print(i);
          Serial.print(' ');
          Serial.println(pressed, BIN); // display the contents of the GPIOB register in binary
        }
        last[i] = pressed;
        
        
        //delay(1); // for debounce
    }
}

