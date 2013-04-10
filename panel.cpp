#include <Arduino.h>
#include <SPI.h>
#include "electone_config.h"
#include "panel.h"


// tr br t b tl bl m

const int LED_A = 0b1110111;
const int LED_B = 0b0101111;
const int LED_C = 0b0011110;
const int LED_D = 0b1101011;
const int LED_E = 0b0011111;
const int LED_F = 0b0010111;

const int LED_L = 0b0001110;
const int LED_R = 0b0000011;
const int LED_T = 0b0001111;

const int LED_0 = 0b1111110;
const int LED_1 = 0b1100000;
const int LED_2 = 0b1011011;
const int LED_3 = 0b1111001;
const int LED_4 = 0b1100101;
const int LED_5 = 0b0111101;
const int LED_6 = 0b0111111;
const int LED_7 = 0b1110000;
const int LED_8 = 0b1111111;
const int LED_9 = 0b1110101;

const int LED_HEX[16] = {
  LED_0,
  LED_1,
  LED_2,
  LED_3,
  LED_4,
  LED_5,
  LED_6,
  LED_7,
  LED_8,
  LED_9,
  LED_A,
  LED_B,
  LED_C,
  LED_D,
  LED_E,
  LED_F,
};

const int PIN_C0 = 5;
const int PIN_C1 = 4;

const int PIN_SDI = MISO;
const int PIN_SDO = MOSI;
const int PIN_CLK = SCK;

const int PERIOD = 5;




uint8_t panel_recv[40];
uint8_t panel_last[40];
uint8_t panel_data[40];

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


void sethex(int p, uint8_t value) {
  int low = LED_HEX[value & 0x0f];
  int high = LED_HEX[value >> 4];
  
  panel_data[16-p*2] = high;
  panel_data[15-p*2] = low;
}


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
  // offset = 56
  
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

uint8_t get4bits(uint8_t* array, int index) {
  int byte_index = (index - 56) >> 3;
  int bit_index = index & 0b111;
  uint8_t val;
  if(bit_index == 0) {
    val = array[byte_index] & 0x0f;
  } else {
    val = array[byte_index] >> 4;
  }
  
  // LSB to MSB
  return ((val & 0b1000) >> 3) | ((val & 0b0100) >> 1) | ((val & 0b0010) << 1) | ((val & 0b0001) << 3);
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

void set_nrpn_control(int index) {
  
  uint8_t coarse = index >> 7;
  uint8_t fine = index & 0x7f;
  
  // Controller number
  panelSendMidi(0x0B, 0xB0, 99, coarse);
  panelSendMidi(0x0B, 0xB0, 98, fine);
}

void send_button_event(int button, uint8_t on) {
  // Use NRPN messages
  set_nrpn_control(button);
  
  // Controller value (coarse)
  panelSendMidi(0x0B, 0xB0, 6, on ? 127 : 0);
  
  // For fine value:
  // panelSendMidi(0x0B, 0xB0, 38, fine_value);  
}

void send_slider_event(int slider, uint8_t value) {
  set_nrpn_control(slider);
  
  // Controller value (coarse). Input value is 0-15, we ajust to 0-120
  panelSendMidi(0x0B, 0xB0, 6, value << 3);
}

int control_high = 0;
int control_low = 0;

void midi_read() {
  
  while(MIDIUSB.available() > 0) {
    MIDIEvent e = MIDIUSB.read();

    if(e.type == 0x0B && e.m1 == 0xB0) {
      if(e.m2 == 99) {
        control_high = e.m3;
      } else if(e.m2 == 98) {
        control_low = e.m3;
      } else if(e.m2 == 6) {
        uint8_t value = e.m3;
        int control = (control_high << 7) | control_low;
        setbit(panel_data, control, value);
      }
    }
  }
}

uint8_t loopcount = 0;

void panel_loop() {
  
  midi_read();
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
    } else if(config.type == PANEL_SLIDER) {
      uint8_t value = get4bits(panel_recv, config.bit_start);
      uint8_t prev = get4bits(panel_last, config.bit_start);
      if(value != prev) {
        send_slider_event(config.bit_start, value);
      }
    }
    
    ci++;
  }
  
  MIDIUSB.flush();
}

