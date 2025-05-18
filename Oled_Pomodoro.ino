#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// I2C pins for ESP8266 (NodeMCU)
#define SDA_PIN 4  // D2
#define SCL_PIN 5  // D1
#define BUTTON_PIN 12  // D6 on NodeMCU

const int WORK_DURATION = 2;   // 25 minutes (for testing, use 2s)
const int BREAK_DURATION = 2;  // 5 minutes

enum State { WORK, BREAK };
State currentState = WORK;
int completedPomodoros = 0;

int timeRemaining = WORK_DURATION;
unsigned long previousMillis = 0;
bool isPaused = false;

unsigned long lastButtonPress = 0;
const unsigned long debounceDelay = 200;

void setup() {
  Wire.begin(SDA_PIN, SCL_PIN);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); 
  display.clearDisplay();
  display.display(); 

  pinMode(BUTTON_PIN, INPUT_PULLUP);  // internal pull-up
}

void loop() {
  handleButton();  // check pause/resume

  if (!isPaused && millis() - previousMillis >= 1000) {
    previousMillis = millis();
    timeRemaining--;

    if (timeRemaining <= 0) {
      if (currentState == WORK) {
        completedPomodoros++;
        currentState = BREAK;
        timeRemaining = BREAK_DURATION;
      } else {
        currentState = WORK;
        timeRemaining = WORK_DURATION;
      }
    }

    drawTime();
  }
}

void handleButton() {
  if (millis() - lastButtonPress > debounceDelay) {
    if (digitalRead(BUTTON_PIN) == LOW) {
      isPaused = !isPaused;
      lastButtonPress = millis();
      drawTime();  // refresh screen immediately
    }
  }
}

void drawTime() {
  display.clearDisplay();

  if (completedPomodoros >= 8) {
    display.setTextSize(2);  // Big text
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(10, 25);  // Centered visually
    display.print("Well Done!");
    display.display();
    return;  // Skip drawing timer/dots
  }

  int minutes = timeRemaining / 60;
  int seconds = timeRemaining % 60;

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print(currentState == WORK ? "Working" : "Rest Time!");

  if (isPaused) {
    display.setCursor(90, 0);
    display.print("Paused");
  }

  display.setTextSize(3);
  display.setCursor(20, 20);

  char buffer[6];
  sprintf(buffer, "%02d:%02d", minutes, seconds);
  display.print(buffer);

  drawPomodoroDots();
  display.display();
}


void drawPomodoroDots() {
  const int dotSize = 4;
  const int spacing = 6;
  const int groupGap = 10;
  int x = 20;
  int y = 58;

  for (int i = 0; i < completedPomodoros && i < 8; i++) {
    display.fillRect(x, y, dotSize, dotSize, SSD1306_WHITE);
    x += dotSize + spacing;
    if (i == 3) x += groupGap + 3;
  }
}
