#include <inttypes.h>

const uint8_t PANEL_END = 0;
const uint8_t PANEL_BUTTON = 1;
const uint8_t PANEL_SLIDER = 2;
const uint8_t PANEL_DIGIT = 3;
const uint8_t PANEL_IGNORE = 4;

// dot tr br t b tl bl m

const int LED_A = 0b01110111;
const int LED_F = 0b00010111;
const int LED_L = 0b00001110;
const int LED_R = 0b00000011;
const int LED_T = 0b00001111;
const int LED_7 = 0b01110000;
const int LED_8 = 0b01111111;

typedef struct {
    uint8_t type;
    uint16_t bit_start;
} PanelConfig;

extern PanelConfig *panel_config;

