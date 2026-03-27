#include <4MapperController.h>

#define BTN1 2
#define BTN2 3
#define BTN3 4
#define BTN4 5
#define BTN5 6
#define BTN6 7
#define BTN7 8
#define BTN8 9

FourMapperController controller(20);

const int buttons[] = {BTN1, BTN2, BTN3, BTN4, BTN5, BTN6, BTN7, BTN8};
const int buttonCount = 8;

void setup() {
  for (int i = 0; i < buttonCount; i++) {
    pinMode(buttons[i], INPUT_PULLUP);
  }

  controller.begin(115200);
  controller.onScenesReceived(onScenesReceived);
}

void loop() {
  controller.update();

  if (controller.isProjectorLive()) {
    for (int i = 0; i < buttonCount && i < controller.getSceneCount(); i++) {
      if (digitalRead(buttons[i]) == LOW) {
        Scene* scene = controller.getSceneByIndex(i);
        if (scene) {
          controller.selectScene(scene->id);
          delay(300);
        }
      }
    }
  }
}

void onScenesReceived(Scene* scenes, int count) {
  Serial.print("Loaded ");
  Serial.print(count);
  Serial.println(" scenes");
}
