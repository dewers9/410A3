#include <BigCrystal.h>
#include <LiquidCrystal.h>

// Set up according to your LCD pins
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);
BigCrystal bigCrystal(&lcd);

int h = 1;
int m = 0;
int s = 0;

char h_str[3], m_str[3], s_str[3]; // Buffers to hold the string representations

void setup() {
  Serial.begin(9600); // Start serial communication
  bigCrystal.begin(16, 2); // Initialize BigCrystal with the size of your LCD
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
  delay(980); // Adjust this delay to keep accurate time
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

  // Convert integer time to strings
  itoa(h, h_str, 10);
  itoa(m, m_str, 10);
  itoa(s, s_str, 10);

  // Clear the LCD
  clearBig();

  // Display time in big font
  displayTimeBig(h_str, 0); // Display hours at column 0
  bigCrystal.printBig(":",6,0);
  displayTimeBig(m_str, 7); // Display minutes at column 5 (adjust as needed)
  bigCrystal.setCursor(14, 0);
  bigCrystal. print(s_str); // Display seconds at column 10 (adjust as needed)
}

void displayTimeBig(char *time_str, int startCol) {
  if (time_str[1] == '\0') { // Single digit number
    bigCrystal.writeBig('0', startCol, 0); // Place a leading zero
    bigCrystal.writeBig(time_str[0], startCol + 3, 0); // Next digit
  } else {
    bigCrystal.writeBig(time_str[0], startCol, 0); // First digit
    bigCrystal.writeBig(time_str[1], startCol + 3, 0); // Second digit
  }
}

void clearBig() {
  for (int row = 0; row < 2; row++) {
    for (int col = 0; col < 16; col += 1) {
      bigCrystal.setCursor(col, row);
      bigCrystal.write(' '); // Clear each big character slot
    }
  }
}
