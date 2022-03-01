#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include "SPI.h"
#include "TFT_eSPI.h"
#include "FreeFonts.h"

class DisplayManager : public TFT_eSPI {
public:
  void Setup(void) {
    this->init();
    this->fillScreen(TFT_BLACK);
  }
  
};

#endif
