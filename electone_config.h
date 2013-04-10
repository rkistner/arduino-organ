#include <inttypes.h>

const uint8_t PANEL_END = 0;
const uint8_t PANEL_BUTTON = 1;
const uint8_t PANEL_SLIDER = 2;
const uint8_t PANEL_DIGIT = 3;
const uint8_t PANEL_IGNORE = 4;

typedef struct {
    uint8_t type;
    uint16_t bit_start;
} PanelConfig;

extern PanelConfig *panel_config;

