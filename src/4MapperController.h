#ifndef FOURMAPPER_CONTROLLER_H
#define FOURMAPPER_CONTROLLER_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include "4MapperTypes.h"

class FourMapperController {
public:
  FourMapperController(int maxScenes = 20);

  void begin(unsigned long baudRate = 115200);
  void update();

  void onScenesReceived(void (*callback)(Scene* scenes, int count));
  void onActiveSceneChanged(void (*callback)(int sceneId));
  void onProjectorStateChanged(void (*callback)(bool isLive));
  void onError(void (*callback)(const char* error));

  void selectScene(int sceneId);
  int getSceneCount();
  Scene* getScenes();
  Scene* getSceneById(int id);
  Scene* getSceneByIndex(int index);
  int getActiveSceneId();
  bool isProjectorLive();
  ControllerState getState();

  void sendReady();

private:
  Scene* _scenes;
  int _maxScenes;
  int _sceneCount;
  int _activeSceneId;
  bool _projectorLive;
  ControllerState _state;

  String _inputBuffer;
  unsigned long _lastReadyTime;

  void (*_onScenesReceived)(Scene* scenes, int count);
  void (*_onActiveSceneChanged)(int sceneId);
  void (*_onProjectorStateChanged)(bool isLive);
  void (*_onError)(const char* error);

  void checkSerial();
  void processCommand(String cmd);
  void parseScenes(String json);
};

#endif
