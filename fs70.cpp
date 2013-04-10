#include "electone_config.h"


PanelConfig fs70[60] = {
  // Auto Bass Chord
  {PANEL_BUTTON, 56},
  {PANEL_IGNORE, 66},
  
  {PANEL_SLIDER, 68},  // Sustain Pedal
  {PANEL_SLIDER, 72},  // Sustain Lower
  {PANEL_SLIDER, 76},  // Sustain Upper
  
  // Sustain
  {PANEL_BUTTON, 80},
  
  // Pedals - custom voices
  {PANEL_SLIDER, 84},  // Brilliance
  {PANEL_SLIDER, 88},  // Volume
  
  
  {PANEL_BUTTON, 92},
  
  // Pedals - combination
  {PANEL_SLIDER, 100},  // Volume
  
  {PANEL_BUTTON, 104},
  
  // Ensemble
  {PANEL_BUTTON, 108},
  
  
  // LOWER KEYBOARD
  
  // Special presets
  {PANEL_SLIDER, 116},  // Volume
  
  
  {PANEL_BUTTON, 120},
  {PANEL_IGNORE, 130},
  
  // Orchestra
  {PANEL_SLIDER, 132},  // Volume
  
  {PANEL_BUTTON, 136},
  {PANEL_IGNORE, 146},
  
  
  // Combination
  {PANEL_SLIDER, 148},  // Volume
  
  
  {PANEL_BUTTON, 152},
  
  // Auto Rhythm
  {PANEL_DIGIT, 160}, // 4 7-segment+dot	g-e-f-d-a-c-b-.
  {PANEL_DIGIT, 168}, // 3 7-segment	g-e-f-d-a-c-b
  {PANEL_DIGIT, 176}, // 2 7-segment	g-e-f-d-a-c-b
  {PANEL_DIGIT, 184}, // 1 7-segment	g-e-f-d-a-c-b, Bar/Beat LED
  
  {PANEL_IGNORE, 188},
  
  // 192	192	IC3	Tempo dial direction	Encoder	
  // 193..199	193..199	Tempo dial counter	Encoder	MSb first

  {PANEL_SLIDER, 200},  // Volume
  {PANEL_SLIDER, 204},  // Balance
  
  {PANEL_BUTTON, 208},
  
  // Rhythmic Chord 2
  {PANEL_SLIDER, 216},  // Volume
  
  {PANEL_BUTTON, 220},
  
  // Rhythmic Chord 1
  {PANEL_SLIDER, 224},  // Volume
  
  {PANEL_BUTTON, 228},
  
  // Auto Arpeggio
  {PANEL_SLIDER, 232},  // Volume
  
  {PANEL_BUTTON, 236},
  
  // Upper/lower custom voices
  {PANEL_SLIDER, 248},  // Volume
  
  {PANEL_BUTTON, 252},
  {PANEL_IGNORE, 266},
  
  // Upper Keyboard - Special presets
  {PANEL_SLIDER, 268},  // Volume
  {PANEL_BUTTON, 272},
  
  // Upper Keyboard - Orchestra
  {PANEL_SLIDER, 284},  // Volume
  {PANEL_BUTTON, 288},
  
  // Upper Keyboard - Combination
  {PANEL_SLIDER, 300},  // Volume
  {PANEL_BUTTON, 304},
  
  // Auto Rhytm
  {PANEL_BUTTON, 312},
  
  // SOLO KEYBOARD
  {PANEL_SLIDER, 332},  // Slide Control
  {PANEL_SLIDER, 336},  // Volume
  {PANEL_SLIDER, 340},  // Brilliance
  
  // Transposision, rest
  {PANEL_BUTTON, 344},
  {PANEL_IGNORE, 364},
  
  // Solo to Upper
  {PANEL_BUTTON, 366},
  {PANEL_IGNORE, 367},

  // Detune slider  
  {PANEL_SLIDER, 368},

  // End
  {PANEL_END, 372},
};



PanelConfig *panel_config = fs70;


