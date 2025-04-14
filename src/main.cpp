#include <Arduino.h>
#include <Keypad.h>
#include <LiquidCrystal.h>

const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

//Connect button right legs to columns, left legs to rows
byte rowPins[ROWS] = {2, 3, 4, 5}; 
byte colPins[COLS] = {6, 7, 8, 9}; 

//Create 4x4 Keypad object 
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

LiquidCrystal lcd(A0, A1, A2, A3, A4, A5);

char password[] = "1234";
char input[5];
byte inputIndex = 0;
byte attempts = 0;


void setup() {
  pinMode(10, OUTPUT); //Green LED
  pinMode(11, OUTPUT); //Red LED
  pinMode(13, INPUT_PULLUP); //Reset Button

  Serial.begin(9600); 
  lcd.begin(16, 2);
  lcd.print("Enter Password:");
  lcd.setCursor(0,1);
}

void loop() {
  char key = keypad.getKey();

  if(attempts >= 3){
    lcd.clear();
    lcd.print("No more attempts");
    lcd.setCursor(0,1);
    lcd.print("Press reset!");

    for(int i=0; i<3; i++){
      digitalWrite(11, HIGH);
      delay(300);
      digitalWrite(11, LOW);
      delay(300);
    }

    digitalWrite(11, HIGH);

    //Lock system till reset
    while(true){
      if(digitalRead(13) == LOW){
        attempts = 0;
        lcd.clear();
        lcd.print("Resetting!");
        digitalWrite(10, LOW);
        digitalWrite(11, LOW);
        delay(2000);
        lcd.clear();
        lcd.print("Enter Password:");
        break;
      }
    }
  }

  if (key) {
    //Logic for checking password entered
    if(inputIndex < 4){
      input[inputIndex] = key;
      inputIndex++;

      lcd.setCursor(inputIndex - 1, 1);
      lcd.print("*");
    }
    if(inputIndex == 4){
      input[4] = '\0';

      delay(500);
      lcd.setCursor(0,1);
      lcd.print("Checking...     ");
      delay(1000);

      if(strcmp(input, password) == 0){
        lcd.clear();
        lcd.print("Access Granted");
        digitalWrite(10, HIGH);
        digitalWrite(11, LOW);
      } else {
        attempts++;
        lcd.clear();
        lcd.print("Access Denied");
        digitalWrite(10, LOW);
        digitalWrite(11, HIGH);
      }
      delay(2000);
      lcd.clear();
      lcd.print("Enter Password:");
      lcd.setCursor(0,1);
      lcd.print("Attempts: ");
      lcd.print(attempts);
      delay(1500);

      //reset
      memset(input, 0, sizeof(input));
      inputIndex = 0;
      
      digitalWrite(10, LOW);
      digitalWrite(11, LOW);
      lcd.clear();
      lcd.print("Enter Password:");
    }
  }
}
