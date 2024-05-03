#include <LiquidCrystal.h>
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

int h = 0;
int m = 0;
int s = 0;

void setup() {
  Serial.begin(9600); // Start serial communication
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Time:");
}

void loop() {  
  // Check if there is data to read
  if (Serial.available() > 0) {
    // Expecting time in format "hh:mm:ss"
    h = Serial.parseInt();
    while (Serial.read() != ':') {} // wait for the first colon
    m = Serial.parseInt();
    while (Serial.read() != ':') {} // wait for the second colon
    s = Serial.parseInt();
  }
  
  // Time update logic
  s += 1;
  delay(990); // Adjust this delay to keep accurate time
  if (s >= 60) {
    s = 0;
    m += 1;
    if (m >= 60) {
      m = 0;
      h += 1;
      if (h >= 24) {
        h = 0;
      }
    }
  }

  // Update LCD display
  lcd.setCursor(6, 0);
  if (h < 10) lcd.print('0'); // Add leading zero for hours
  lcd.print(h);
  lcd.print(':');
  if (m < 10) lcd.print('0'); // Add leading zero for minutes
  lcd.print(m);
  lcd.print(':');
  if (s < 10) lcd.print('0'); // Add leading zero for seconds
  lcd.print(s);
}
