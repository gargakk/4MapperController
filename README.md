# 4MapperController

> Arduino library for building custom hardware controllers for [4mapper](https://4mapper.com) — the browser-based projection mapping software.

Connect physical controls (buttons, encoders, touchscreens...) to 4mapper via USB and switch scenes in real time, with no complex configuration required.

> **⚠️ Requirements:** This library requires a **4mapper Pro** account.
> [Register here](https://4mapper.com/login.php?mode=register) and then [upgrade to Pro](https://4mapper.com/upgrade.php) to enable USB controller support.
---

## ✨ Features

- **Trigger scenes** in 4mapper by pressing a button or turning an encoder
- **Real-time sync**: always knows which scene is active and whether the projector is live
- **Ultra-low latency USB communication** (< 5ms) via native serial
- **Callback-based event system**: react to events only when they happen
- **OLED display support** to show the current scene name on your controller

---

## 🛠️ Supported Hardware

Only ESP32 boards with **native USB CDC** are supported:

| Board | Support |
|---|---|
| ESP32-C3 | ✅ Recommended |
| ESP32-S2 | ✅ |
| ESP32-S3 | ✅ |
| Classic ESP32 (WROOM) | ❌ Not supported |

> **⚠️ Important:** In Arduino IDE, set `Tools → USB CDC On Boot → Enabled` before uploading your sketch.

---

## 📦 Installation

### Via Arduino Library Manager (recommended)
1. Open Arduino IDE
2. Go to **Sketch → Include Library → Manage Libraries**
3. Search for `4MapperController`
4. Click **Install**

### Manual Installation
1. Download the ZIP from the latest release
2. Extract to `~/Arduino/libraries/4MapperController/`
3. Restart Arduino IDE

### Dependency
This library requires **ArduinoJson >= 6.0.0**, also available from the Library Manager.

---

## 🚀 Quick Start

```cpp
#include <4MapperController.h>

FourMapperController controller;

void setup() {
  controller.begin(115200);

  controller.onScenesReceived([](Scene* scenes, int count) {
    Serial.print("Scenes received: ");
    Serial.println(count);
  });
}

void loop() {
  controller.update(); // always call this in the loop!
}
```

---

## 📋 Included Examples

### 1. `ButtonMatrix` — Button Grid
Connect up to 8 physical buttons (pins 2–9), each mapped to a scene in order. Pressing button N activates scene N.

```
Pin 2 → Scene 1
Pin 3 → Scene 2
...
Pin 9 → Scene 8
```

### 2. `EncoderOLED` — Rotary Encoder + OLED Display
- **Rotary encoder** (CLK pin 2, DT pin 3, SW pin 4) to navigate scenes
- **128×64 OLED display** (I²C, SDA pin 6, SCL pin 7) showing scene name, LIVE/OFF status, and active scene indicator

---

## 🔌 Communication Protocol

| Direction | Message | Meaning |
|---|---|---|
| ESP32 → PC | `READY` | Controller is online and looking for 4mapper |
| PC → ESP32 | `SCENES:[...]` | JSON array with all available scenes |
| PC → ESP32 | `ACTIVE:42` | ID of the currently active scene |
| PC → ESP32 | `LIVE:1` / `LIVE:0` | Projector output state |
| ESP32 → PC | `SCENE:42` | Request to switch to a scene |

---

## 📖 API Reference

### Initialization
```cpp
FourMapperController controller(20); // max 20 scenes (default)
controller.begin(115200);
```

### In the loop
```cpp
controller.update(); // required — processes incoming serial data
```

### Actions
```cpp
controller.selectScene(int sceneId);
```

### Reading state
```cpp
controller.getSceneCount();
controller.getActiveSceneId();
controller.isProjectorLive();
controller.getState();               // DISCONNECTED | WAITING | CONNECTED | ERROR_STATE
controller.getSceneByIndex(int i);
controller.getSceneById(int id);
controller.getScenes();
```

### Callbacks
```cpp
controller.onScenesReceived([](Scene* scenes, int count) { ... });
controller.onActiveSceneChanged([](int sceneId) { ... });
controller.onProjectorStateChanged([](bool isLive) { ... });
controller.onError([](const char* error) { ... });
```

### `Scene` struct
```cpp
scene->id      // numeric scene ID
scene->name    // scene name (max 20 characters)
scene->isLive  // true if this scene is currently in output
```

---

## 💡 How It Works

On startup, the controller sends periodic `READY` messages over USB. When 4mapper detects it, it responds with the full scene list as JSON. From that point, both devices stay in sync — every scene change or projector state update is pushed to the controller in real time.

---

## 📁 Project Structure

```
4MapperController/
├── src/
│   ├── 4MapperController.h
│   ├── 4MapperController.cpp
│   └── 4MapperTypes.h
├── examples/
│   ├── ButtonMatrix/
│   │   └── ButtonMatrix.ino
│   └── EncoderOLED/
│       └── EncoderOLED.ino
├── keywords.txt
├── library.properties
└── README.md
```

---

## 🤝 Contributing

Built something cool? Pull requests are welcome!

---

## 📄 License

MIT License — free for personal and commercial use.
