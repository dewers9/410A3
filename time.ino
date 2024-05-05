#include <LiquidCrystal.h>
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

int h = 0;
int m = 0;
int s = 0;

void setup() {
  Serial.begin(9600); // Start serial communication
  lcd.begin(16, 2);
  
  // Receive initial time from serial input
  while (!Serial.available()) {}
  h = Serial.parseInt();
  while (Serial.read() != ':') {}
  m = Serial.parseInt();
  while (Serial.read() != ':') {}
  s = Serial.parseInt();
}

void loop() {
  s = s + 1;
  delay(990);
  if (s == 60) {
    s = 0;
    m = m + 1;
    if (m == 60) {
      m = 0;
      h = h + 1;
      if (h == 24) {
        h = 0;
      }
    }
  }

  lcd.setCursor(0, 0);
  lcd.print("Time:");
  lcd.setCursor(6, 0);
  if (h < 10) {
    lcd.print("0");
  }
  lcd.print(h);
  lcd.setCursor(8, 0);
  lcd.print(":");
  lcd.setCursor(9, 0);
  if (m < 10) {
    lcd.print("0");
  }
  lcd.print(m);
  lcd.setCursor(11, 0);
  lcd.print(":");
  lcd.setCursor(12, 0);
  if (s < 10) {
    lcd.print("0");
  }
  lcd.print(s);
}
