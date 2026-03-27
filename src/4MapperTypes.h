#ifndef FOURMAPPER_TYPES_H
#define FOURMAPPER_TYPES_H

#include <Arduino.h>

struct Scene {
  int id;
  char name[21];
  bool isLive;
};

enum ControllerState {
  DISCONNECTED,
  WAITING,
  CONNECTED,
  ERROR_STATE
};

#endif
