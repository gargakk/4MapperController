#include "4MapperController.h"

FourMapperController::FourMapperController(int maxScenes) {
  _maxScenes = maxScenes;
  _scenes = new Scene[maxScenes];
  _sceneCount = 0;
  _activeSceneId = -1;
  _projectorLive = false;
  _state = DISCONNECTED;
  _lastReadyTime = 0;

  _onScenesReceived = nullptr;
  _onActiveSceneChanged = nullptr;
  _onProjectorStateChanged = nullptr;
  _onError = nullptr;
}

void FourMapperController::begin(unsigned long baudRate) {
  Serial.begin(baudRate);
  while (!Serial && millis() < 3000);
  delay(500);

  Serial.println("=== 4MAPPER CONTROLLER ===");
  _state = WAITING;

  for (int i = 0; i < 5; i++) {
    sendReady();
    delay(200);
  }

  Serial.println("Controller initialized");
}

void FourMapperController::update() {
  checkSerial();

  if (_sceneCount == 0 && millis() - _lastReadyTime > 3000) {
    sendReady();
    _lastReadyTime = millis();
  }
}

void FourMapperController::checkSerial() {
  while (Serial.available() > 0) {
    char c = Serial.read();

    if (c == '\n') {
      _inputBuffer.trim();

      if (_inputBuffer.length() > 0) {
        if (_inputBuffer.startsWith("SCENES:") && !_inputBuffer.endsWith("]")) {
          Serial.println("Incomplete SCENES, ignoring");
          _inputBuffer = "";
          return;
        }
        processCommand(_inputBuffer);
      }

      _inputBuffer = "";
    } else {
      _inputBuffer += c;
      if (_inputBuffer.length() > 2048) {
        Serial.println("Buffer overflow");
        _inputBuffer = "";
      }
    }
  }
}

void FourMapperController::processCommand(String cmd) {
  if (cmd.startsWith("SCENES:")) {
    parseScenes(cmd.substring(7));
  } else if (cmd.startsWith("ACTIVE:")) {
    _activeSceneId = cmd.substring(7).toInt();
    if (_onActiveSceneChanged) _onActiveSceneChanged(_activeSceneId);
  } else if (cmd.startsWith("LIVE:")) {
    _projectorLive = (cmd.substring(5).toInt() == 1);
    if (_onProjectorStateChanged) _onProjectorStateChanged(_projectorLive);
  } else if (cmd.startsWith("ERROR:")) {
    String error = cmd.substring(6);
    if (_onError) _onError(error.c_str());
  }
}

void FourMapperController::parseScenes(String json) {
  if (json.length() < 5) return;

  StaticJsonDocument<4096> doc;
  DeserializationError error = deserializeJson(doc, json);

  if (error) {
    Serial.print("JSON error: ");
    Serial.println(error.c_str());
    return;
  }

  _sceneCount = 0;
  JsonArray array = doc.as<JsonArray>();

  if (array.isNull() || array.size() == 0) return;

  for (JsonObject obj : array) {
    if (_sceneCount >= _maxScenes) break;
    _scenes[_sceneCount].id = obj["id"] | 0;
    const char* name = obj["name"] | "Scene";
    strncpy(_scenes[_sceneCount].name, name, 20);
    _scenes[_sceneCount].name[20] = '\0';
    _scenes[_sceneCount].isLive = obj["live"] | false;
    _sceneCount++;
  }

  _state = CONNECTED;
  if (_onScenesReceived) _onScenesReceived(_scenes, _sceneCount);
}

void FourMapperController::selectScene(int sceneId) {
  if (!_projectorLive) {
    if (_onError) _onError("Projector offline");
    return;
  }
  Serial.print("SCENE:");
  Serial.println(sceneId);
}

void FourMapperController::sendReady() {
  Serial.println("READY");
  _lastReadyTime = millis();
}

void FourMapperController::onScenesReceived(void (*callback)(Scene* scenes, int count)) { _onScenesReceived = callback; }
void FourMapperController::onActiveSceneChanged(void (*callback)(int sceneId)) { _onActiveSceneChanged = callback; }
void FourMapperController::onProjectorStateChanged(void (*callback)(bool isLive)) { _onProjectorStateChanged = callback; }
void FourMapperController::onError(void (*callback)(const char* error)) { _onError = callback; }

int FourMapperController::getSceneCount() { return _sceneCount; }
Scene* FourMapperController::getScenes() { return _scenes; }
int FourMapperController::getActiveSceneId() { return _activeSceneId; }
bool FourMapperController::isProjectorLive() { return _projectorLive; }
ControllerState FourMapperController::getState() { return _state; }

Scene* FourMapperController::getSceneById(int id) {
  for (int i = 0; i < _sceneCount; i++) {
    if (_scenes[i].id == id) return &_scenes[i];
  }
  return nullptr;
}

Scene* FourMapperController::getSceneByIndex(int index) {
  if (index >= 0 && index < _sceneCount) return &_scenes[index];
  return nullptr;
}
