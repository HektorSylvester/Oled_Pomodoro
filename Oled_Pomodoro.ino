#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <EEPROM.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// I2C pins for ESP8266 (NodeMCU)
#define SDA_PIN 4  // D2
#define SCL_PIN 5  // D1
#define BUTTON_PIN 12  // D6 on NodeMCU

const int WORK_DURATION = 1500;   // 25 minutes = 1500 sec
const int BREAK_DURATION = 300;  // 5 minutes = 300 sec
const int EEPROM_ADDR = 0;  // Address to store pomodoro count
const unsigned long RESET_HOLD_TIME = 3000;  // 3 seconds hold to reset

enum State { WORK, BREAK };
State currentState = WORK;
int completedPomodoros = 0;
int timeRemaining = WORK_DURATION;
unsigned long previousMillis = 0;
bool isPaused = true;
unsigned long lastButtonPress = 0;
const unsigned long debounceDelay = 200;

// Button handling variables
unsigned long buttonPressStartTime = 0;
bool buttonIsPressed = false;

void setup() {
  Serial.begin(115200);  // For debugging
  
  // Initialize EEPROM
  EEPROM.begin(512);
  
  // Read saved pomodoro count from EEPROM
  completedPomodoros = EEPROM.read(EEPROM_ADDR);
  Serial.print("Loaded pomodoro count: ");
  Serial.println(completedPomodoros);
  
  // Initialize I2C and display
  Wire.begin(SDA_PIN, SCL_PIN);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();
  
  pinMode(BUTTON_PIN, INPUT_PULLUP);  // internal pull-up
  
  // Initial display update
  drawTime();
}

void loop() {
  handleButton();  // check pause/resume and reset functionality
  
  if (!isPaused && millis() - previousMillis >= 1000) {
    previousMillis = millis();
    timeRemaining--;
    
    if (timeRemaining <= 0) {
      if (currentState == WORK) {
        completedPomodoros++;
        savePomodoros();  // Save to EEPROM when completing a pomodoro
        
        currentState = BREAK;
        timeRemaining = BREAK_DURATION;
        // Continue not paused during break
        isPaused = false;
      } else {
        currentState = WORK;
        timeRemaining = WORK_DURATION;
        // Start next pomodoro paused
        isPaused = true;
      }
      drawTime(); // Update display immediately
    }
    drawTime();
  }
}

void handleButton() {
  int buttonState = digitalRead(BUTTON_PIN);
  unsigned long currentTime = millis();
  
  // Button pressed (LOW due to pull-up resistor)
  if (buttonState == LOW) {
    // If button just pressed, record the time
    if (!buttonIsPressed) {
      buttonIsPressed = true;
      buttonPressStartTime = currentTime;
    }
    
    // Check for long press (reset functionality)
    if (buttonIsPressed && (currentTime - buttonPressStartTime >= RESET_HOLD_TIME)) {
      // Reset pomodoro count
      completedPomodoros = 0;
      savePomodoros();
      
      // Visual feedback for reset
      displayResetAnimation();
      
      // Reset button state to prevent multiple triggers
      buttonIsPressed = false;
      
      // Update display
      drawTime();
    }
  } 
  // Button released
  else if (buttonState == HIGH && buttonIsPressed) {
    // Short press - toggle pause state
    if (currentTime - buttonPressStartTime < RESET_HOLD_TIME) {
      if (currentTime - lastButtonPress > debounceDelay) {
        isPaused = !isPaused;
        lastButtonPress = currentTime;
        drawTime();  // refresh screen immediately
      }
    }
    
    // Reset button state
    buttonIsPressed = false;
  }
}

void savePomodoros() {
  EEPROM.write(EEPROM_ADDR, completedPomodoros);
  EEPROM.commit();
  Serial.print("Saved pomodoro count: ");
  Serial.println(completedPomodoros);
}

void displayResetAnimation() {
  // Simple animation to indicate reset
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(25, 25);
  display.print("RESET!");
  display.display();
  delay(1000);  // Show reset message for 1 second
}

void drawTime() {
  display.clearDisplay();
  
  if (completedPomodoros >= 8) {
    display.setTextSize(2);  // Big text
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(10, 25);  // Centered visually
    display.print("Well Done");
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
