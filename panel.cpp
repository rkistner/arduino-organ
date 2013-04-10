#include <Arduino.h>
#include <SPI.h>
#include "electone_config.h"
#include "panel.h"


const int PIN_C0 = 5;
const int PIN_C1 = 4;

const int PIN_SDI = MISO;
const int PIN_SDO = MOSI;
const int PIN_CLK = SCK;

const int PERIOD = 5;

int shiftOut(int val, uint8_t bits=8) {
  // Clock idle when high.
  // Propogate on rising edge (before the falling transision), and capture on falling edge.
  
  delayMicroseconds(PERIOD);
  uint8_t i;
  int out = 0;
  for (i = 0; i < bits; i++)  {
    uint8_t in = !!(digitalRead(PIN_SDI));
    out |= in << i;
    digitalWrite(PIN_SDO, !!(val & (1 << i)));
    digitalWrite(PIN_CLK, LOW);
    delayMicroseconds(PERIOD);
    digitalWrite(PIN_CLK, HIGH);
  }
  digitalWrite(PIN_SDO, LOW);
  delayMicroseconds(PERIOD);
  return out;
}

void pulse() {
  delayMicroseconds(PERIOD);
  digitalWrite(PIN_CLK, LOW);
  delayMicroseconds(PERIOD);
  digitalWrite(PIN_CLK, HIGH);
  delayMicroseconds(PERIOD);
}

uint8_t push(int data) {
  digitalWrite(PIN_C0, HIGH);
  int r = shiftOut(data, 8);
  digitalWrite(PIN_C0, LOW);
  pulse();
  return r;
}

uint8_t pushHalf(int data) {
  digitalWrite(PIN_C0, HIGH);
  int r = shiftOut(data, 4);
  digitalWrite(PIN_C0, LOW);
  pulse();
  return r;
}

void p(int c0, int c1) {
  digitalWrite(PIN_C0, c0);
  digitalWrite(PIN_C1, c1);
  pulse();
}




uint8_t panel_recv[40];
uint8_t panel_last[40];
uint8_t panel_data[40];

void panel_setup() {
  
  pinMode(PIN_C0, OUTPUT);
  pinMode(PIN_C1, OUTPUT);
  pinMode(PIN_CLK, OUTPUT);
  pinMode(PIN_SDO, OUTPUT);
  pinMode(PIN_SDI, INPUT);
  
  digitalWrite(PIN_C0, LOW);
  digitalWrite(PIN_C1, LOW);
  digitalWrite(PIN_CLK, HIGH);
  digitalWrite(PIN_SDO, LOW);
  
  
  for(int i = 0; i < 40; i++) {
    panel_data[i] = 0x00;
  }
  
  // Panel LED's
  panel_data[16] = LED_R;
  panel_data[15] = LED_A;
  panel_data[14] = LED_L;
  panel_data[13] = LED_F;
  
}

void panel_read_write() {
  
  for(int i = 0; i < 40; i++) {
    panel_last[i] = panel_recv[i];
  }
  
  p(LOW, HIGH);
  
  digitalWrite(PIN_C1, LOW);
  for(int i = 0; i < 40; i++) {
    int r;
    if(i == 39) {
      r = pushHalf(0b1100);
    } else {
      r = push(0b00001100);
    }
    panel_recv[i] = r;
  }
  for(int i = 0; i < 40; i++) {
    if(i == 39) {
      pushHalf(panel_data[i]);
    } else {
      push(panel_data[i]);
    }
  }
  
  
  p(LOW, LOW);
  
  p(HIGH, HIGH);
  p(LOW, LOW);
  
  delay(10);
}

uint8_t getbit(uint8_t* array, int index) {
  int byte_index = (index - 56) >> 3;
  int bit_index = index & 0b111;
  return !!(array[byte_index] & (1 << bit_index));
}

void setbit(uint8_t* array, int index, uint8_t value) {
  int byte_index = (index - 56) >> 3;
  int bit_index = index & 0b111;
  uint8_t v = array[byte_index];
  uint8_t b = 1 << bit_index;
  if(value) {
    array[byte_index] = v | b;
  } else {
    array[byte_index] = v & ~b;
  }
}


void panelSendMidi(byte event, byte m1, byte m2, byte m3) {
  MIDIEvent e = {event, m1, m2, m3};
  MIDIUSB.write(e);
}


void send_button_event(int button, uint8_t on) {
  // least 7 bits is the control, other bits make up the channel
  /*uint8_t channel = (button >> 7) + 8;
  uint8_t var = button & 0x7f;
  panelSendMidi(0x0B, 0xB0 | channel, var, on ? 127 : 0);*/
  
  // Use NRPN messages
  
  uint8_t coarse = button >> 7;
  uint8_t fine = button & 0x7f;
  
  // Controller number
  panelSendMidi(0x0B, 0xB0, 99, coarse);
  panelSendMidi(0x0B, 0xB0, 98, fine);
  // Controller value (coarse)
  panelSendMidi(0x0B, 0xB0, 6, on ? 127 : 0);
  
  // For fine value:
  // panelSendMidi(0x0B, 0xB0, 38, fine_value);  
}

void panel_loop() {
  // offset = 56
  // 
  panel_read_write();
  
  int ci = 0;
  while(panel_config[ci].type != PANEL_END) {
    PanelConfig config = panel_config[ci];
    PanelConfig next = panel_config[ci+1];
    int len = next.bit_start - config.bit_start;
    
    if(config.type == PANEL_BUTTON) {
      for(int i = config.bit_start; i < next.bit_start; i++) {  
        uint8_t value = getbit(panel_recv, i);
        uint8_t prev = getbit(panel_last, i);
        if(value && !prev) {
          uint8_t state = getbit(panel_data, i);
          setbit(panel_data, i, !state);
          send_button_event(i, !state);
        }
      }
    }
    
    ci++;
  }
  
  MIDIUSB.flush();
}

