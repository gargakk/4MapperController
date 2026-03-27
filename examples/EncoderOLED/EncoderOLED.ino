#include <4MapperController.h>
#include <U8g2lib.h>
#include <Wire.h>

#define ENCODER_CLK 2
#define ENCODER_DT  3
#define ENCODER_SW  4
#define I2C_SDA     6
#define I2C_SCL     7

FourMapperController controller(20);
U8G2_SSD1306_128X64_NONAME_F_HW_I2C display(U8G2_R0, U8X8_PIN_NONE, I2C_SCL, I2C_SDA);

int selectedIndex = 0;
int lastCLK = HIGH;
unsigned long lastButtonPress = 0;

void setup() {
  Wire.begin(I2C_SDA, I2C_SCL);
  display.begin();
  display.setFont(u8g2_font_6x10_tr);

  pinMode(ENCODER_CLK, INPUT_PULLUP);
  pinMode(ENCODER_DT,  INPUT_PULLUP);
  pinMode(ENCODER_SW,  INPUT_PULLUP);

  showSplash();

  controller.begin(115200);
  controller.onScenesReceived(onScenesReceived);
  controller.onActiveSceneChanged(onActiveSceneChanged);
  controller.onProjectorStateChanged(onProjectorStateChanged);
  controller.onError(onError);

  updateDisplay();
}

void loop() {
  controller.update();

  if (controller.isProjectorLive() && controller.getSceneCount() > 0) {
    handleEncoder();
  }
}

void onScenesReceived(Scene* scenes, int count) {
  selectedIndex = 0;
  updateDisplay();
}

void onActiveSceneChanged(int sceneId) {
  updateDisplay();
}

void onProjectorStateChanged(bool isLive) {
  updateDisplay();
}

void onError(const char* error) {
  showError(error);
}

void handleEncoder() {
  int currentCLK = digitalRead(ENCODER_CLK);
  if (currentCLK != lastCLK && currentCLK == LOW) {
    if (digitalRead(ENCODER_DT) == HIGH) {
      selectedIndex++;
      if (selectedIndex >= controller.getSceneCount()) selectedIndex = 0;
    } else {
      selectedIndex--;
      if (selectedIndex < 0) selectedIndex = controller.getSceneCount() - 1;
    }
    updateDisplay();
  }
  lastCLK = currentCLK;

  if (digitalRead(ENCODER_SW) == LOW) {
    if (millis() - lastButtonPress > 300) {
      Scene* scene = controller.getSceneByIndex(selectedIndex);
      if (scene) controller.selectScene(scene->id);
      lastButtonPress = millis();
    }
  }
}

void showSplash() {
  display.clearBuffer();
  display.drawStr(20, 30, "4mapper USB");
  display.drawStr(15, 45, "Controller");
  display.sendBuffer();
  delay(1500);
}

void updateDisplay() {
  display.clearBuffer();

  if (controller.getSceneCount() == 0) {
    display.drawStr(10, 30, "Waiting...");
    display.sendBuffer();
    return;
  }

  display.setFont(u8g2_font_6x10_tr);
  display.drawStr(0, 10, "4mapper");

  if (controller.isProjectorLive()) {
    display.drawStr(50, 10, "[LIVE]");
  } else {
    display.drawStr(45, 10, "[OFF]");
  }

  char countStr[20];
  sprintf(countStr, "%d/%d", selectedIndex + 1, controller.getSceneCount());
  display.drawStr(90, 10, countStr);

  Scene* scene = controller.getSceneByIndex(selectedIndex);
  if (scene) {
    display.setFont(u8g2_font_9x15_tr);
    int w = display.getStrWidth(scene->name);
    int x = (128 - w) / 2;
    if (x < 0) x = 0;
    display.drawStr(x, 35, scene->name);

    display.setFont(u8g2_font_6x10_tr);
    if (scene->id == controller.getActiveSceneId()) {
      display.drawStr(40, 52, "[ACTIVE]");
    }
  }

  display.sendBuffer();
}

void showError(const char* error) {
  display.clearBuffer();
  display.setFont(u8g2_font_6x10_tr);
  display.drawStr(5, 20, "ERROR:");
  display.drawStr(5, 35, error);
  display.sendBuffer();
  delay(2000);
  updateDisplay();
}
