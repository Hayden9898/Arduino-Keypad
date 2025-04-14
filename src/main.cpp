#include <Arduino.h>
#include <Keypad.h>
#include <LiquidCrystal.h>

// --- CONSTANTS ---
const byte ROWS = 4;
const byte COLS = 4;
const char password[] = "1234";
const byte resetButtonPin = 13;
const byte greenLed = 10;
const byte redLed = 11;

// --- KEYPAD SETUP ---
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {2, 3, 4, 5};
byte colPins[COLS] = {6, 7, 8, 9};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// --- LCD SETUP ---
LiquidCrystal lcd(A0, A1, A2, A3, A4, A5);

// --- VARIABLES ---
char input[5];
byte inputIndex = 0;
byte attempts = 0;

//
// --- FUNCTION DECLARATIONS ---
//
void grantAccess();
void denyAccess();
void resetSystem();
void handleLockout();
void handleInput(char key);


void setup() {
  pinMode(greenLed, OUTPUT);
  pinMode(redLed, OUTPUT);
  pinMode(resetButtonPin, INPUT_PULLUP);

  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.print("Enter Password:");
  lcd.setCursor(0, 1);
}

void loop() {
  if (attempts >= 3) {
    handleLockout();
    return;
  }

  char key = keypad.getKey();
  if (key) {
    handleInput(key);
  }
}

//
// --- FUNCTION DEFINITIONS ---
//

void handleInput(char key) {
  if (inputIndex < 4) {
    input[inputIndex++] = key;
    lcd.setCursor(inputIndex - 1, 1);
    lcd.print("*");
  }

  if (inputIndex == 4) {
    input[4] = '\0';

    delay(500);
    lcd.setCursor(0, 1);
    lcd.print("Checking...     ");
    delay(1000);

    if (strcmp(input, password) == 0) {
      grantAccess();
    } else {
      denyAccess();
    }

    delay(2000);
    lcd.clear();
    lcd.print("Enter Password:");
    lcd.setCursor(0, 1);
    lcd.print("Attempts: ");
    lcd.print(attempts);
    delay(1500);

    memset(input, 0, sizeof(input));
    inputIndex = 0;
    digitalWrite(greenLed, LOW);
    digitalWrite(redLed, LOW);
    lcd.clear();
    lcd.print("Enter Password:");
  }
}

void grantAccess() {
  lcd.clear();
  lcd.print("Access Granted");
  digitalWrite(greenLed, HIGH);
  digitalWrite(redLed, LOW);
}

void denyAccess() {
  attempts++;
  lcd.clear();
  lcd.print("Access Denied");
  digitalWrite(greenLed, LOW);
  digitalWrite(redLed, HIGH);
}

void resetSystem() {
  attempts = 0;
  lcd.clear();
  lcd.print("Resetting...");
  digitalWrite(greenLed, LOW);
  digitalWrite(redLed, LOW);
  delay(2000);
  lcd.clear();
  lcd.print("Enter Password:");
  lcd.setCursor(0, 1);
}

void handleLockout() {
  lcd.clear();
  lcd.print("No more attempts");
  lcd.setCursor(0, 1);
  lcd.print("Press reset!");

  for (int i = 0; i < 3; i++) {
    digitalWrite(redLed, HIGH);
    delay(300);
    digitalWrite(redLed, LOW);
    delay(300);
  }
  digitalWrite(redLed, HIGH);

  while (true) {
    if (digitalRead(resetButtonPin) == LOW) {
      resetSystem();
      break;
    }
  }
}