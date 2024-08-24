#include <SPFD5408_Adafruit_GFX.h>    // Core graphics library
#include <SPFD5408_Adafruit_TFTLCD.h> // Hardware-specific library
#include <SPFD5408_TouchScreen.h>
#include <SPI.h>                      // Communication with SPI devices
#include <MFRC522.h>                  // RFID library

// Define LCD pins (assigned default values)
#define YP A1  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 7   // can be a digital pin
#define XP 6   // can be a digital pin

#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
#define LCD_RESET A4

// Assign names to colors and pressure
#define WHITE   TFTLCD_WHITE 
#define YELLOW  TFTLCD_YELLOW 
#define CYAN    TFTLCD_CYAN 
#define PINK    TFTLCD_PINK 
#define RED     TFTLCD_RED 
#define GREEN   TFTLCD_GREEN 
#define BLUE    TFTLCD_BLUE 
#define BLACK   TFTLCD_BLACK 

#define DARKGREEN  TFTLCD_GREEN & 0x03F0
#define MINPRESSURE 10
#define MAXPRESSURE 1000

// Calibrate TFT LCD
#define TS_MINX 210
#define TS_MINY 100
#define TS_MAXX 875
#define TS_MAXY 800

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300); // 300 is the sensitivity
Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET); // Start communication with LCD

#define GRID_SIZE 4

String symbol[GRID_SIZE][GRID_SIZE] = {
  { "7", "8", "9", "Add" },
  { "4", "5", "6", "Rem" },
  { "1", "2", "3", "Wrt" },  // "Wrt" added to write the number to the card
  { "X", "0", ".", "Rst" }   // "Rst" added to clear the screen
};

uint16_t colors[][4] = { 
  { TFTLCD_RED, BLACK, BLACK, GREEN },
  { BLACK, TFTLCD_RED, BLACK, DARKGREEN },
  { BLACK, TFTLCD_RED, TFTLCD_RED, DARKGREEN },
  { BLUE, PINK, BLACK, DARKGREEN },
};

#define BASE_NUM_X 0
#define BASE_NUM_Y 80
#define SIZE_NUM_X 60
#define SIZE_NUM_Y 60

long Num1, Num2;
String currentNumber = "";  // Treat the number as a string while typing

// RFID Pins
#define RST_PIN   A4   
#define SS_PIN    A5    

MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance
MFRC522::MIFARE_Key key; // Create a MIFARE_Key structure to hold the key

// Block number and buffer settings
int blockNum = 2; // Block number to read/write from
byte bufferLen = 18; // Buffer size for reading block data
byte readBlockData[18]; // Buffer to hold read block data
byte writeBlockData[16]; // Buffer to hold data to be written

// Mode flag: true = write mode, false = read mode
bool isWriteMode = false;

void setup() {
  Serial.begin(9600); // Initialize serial monitor for RFID output
  
  // Initialize LCD
  tft.reset(); // Always reset at start
  tft.begin(0x9341); // My LCD uses LIL9341 Interface driver IC
  tft.setRotation(0); // Rotate so that the power jack faces up - optional
  tft.fillScreen(WHITE);

  draw_BoxNButtons(); 
  
  // Initialize RFID
  while (!Serial);    // Do nothing if no serial port is opened (for ATMEGA32U4-based Arduinos)
  SPI.begin();        // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522
  ShowReaderDetails(); // Show details of PCD - MFRC522 Card Reader details
  Serial.println(F("Scan PICC to read or write data to block 2..."));
}

void loop() {
  // Handle touchscreen input without blocking
  char p = checkTouch();
  static int action = 0;

  if (p != '\0') { // Process touch if a valid button is pressed
    if (p == 'W') { // 'Wrt' button pressed, switch to write mode
      isWriteMode = true; // Switch to write mode
      DisplayResult(); // Update display with pink background
      Serial.println(F("Write mode activated. Scan the card to write."));
      return;
    }
    if (p == 'R') { // "Rst" button pressed, clear the screen and reset the number
      currentNumber = "";  // Reset the current number
      isWriteMode = false; // Return to read mode after reset
      DisplayResult();  // Clear the display
      return;
    }

    switch (p) {
      case '0': case '1': case '2': case '3': case '4':
      case '5': case '6': case '7': case '8': case '9':
        if (currentNumber.length() < 4) {  // Only allow up to 4 digits
          currentNumber += p;  // Append the digit to the current number
        }
        break;

      case '/': case '*': case '-': case '+':
        action = p; Num1 = currentNumber.toInt(); currentNumber = ""; // Convert string to int for calculation
        tft.setCursor(200, 20);
        tft.setTextColor(RED);
        tft.println(p);
        delay(300);
        break;

      case 'C':
        currentNumber = ""; Num1 = Num2 = 0;
        break;

      case '=':
        Num2 = currentNumber.toInt();
        CalculateResult(action);
        currentNumber = String(Num1);  // Update the current number to the result
        break;

      default:
        break;
    }

    DisplayResult();
    delay(300);
  }

  // RFID handling based on the mode
  if (isWriteMode) {
    writeNumberToCard();  // Write to the card if in write mode
  } else {
    readCardData();  // Otherwise, read from the card
  }
}

char checkTouch() {
  TSPoint p = ts.getPoint();
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  if ((p.z < MINPRESSURE) || (p.z > MAXPRESSURE)) {
    return '\0'; // No valid touch detected
  }

  // Map touchscreen coordinates to grid position
  p.x = map(p.x, TS_MINX, TS_MAXX, 240, 0);
  p.y = map(p.y, TS_MINY, TS_MAXY, 320, 0);

  int gridx = (p.x - BASE_NUM_X + 1) / SIZE_NUM_X;
  int gridy = (p.y - BASE_NUM_Y + 1) / SIZE_NUM_Y;

  if (gridx < 0 || gridy < 0 || gridx >= GRID_SIZE || gridy >= GRID_SIZE) {
    return '\0';
  }

  return symbol[gridy][gridx][0];
}

void CalculateResult(int action) {
  switch (action) {
    case '+': Num1 = Num1 + Num2; break;
    case '-': Num1 = Num1 - Num2; break;
    case '*': Num1 = Num1 * Num2; break;
    case '/': if (Num2 != 0) Num1 = Num1 / Num2; break;
  }
}

void DisplayResult() {
  // Change background color based on mode: cyan in read mode, pink in write mode
  uint16_t backgroundColor = isWriteMode ? PINK : CYAN;

  tft.fillRect(0, 0, 240, 80, backgroundColor); // Clear result box with the correct color
  tft.setCursor(10, 20);
  tft.setTextSize(4);
  tft.setTextColor(BLACK);
  tft.println(currentNumber); // Update new value
}

void DisplayBlockData(byte blockData[]) {
  tft.fillRect(0, 0, 240, 80, CYAN); // Clear the area for displaying block data
  tft.setCursor(10, 20);
  tft.setTextSize(3);
  tft.setTextColor(RED);
  for (int i = 0; i < 16; i++) {
    tft.write(blockData[i]); // Display the block data on the screen
  }
}

void writeNumberToCard() {
  // Ensure the number is exactly 4 digits by padding with leading zeroes if needed
  while (currentNumber.length() < 4) {
    currentNumber = "0" + currentNumber;
  }

  // Convert the number to a byte array
  for (int i = 0; i < 16; i++) {
    if (i < currentNumber.length()) {
      writeBlockData[i] = currentNumber[i];
    } else {
      writeBlockData[i] = 0; // Pad with null characters if the string is shorter than 16 bytes
    }
  }

  // Handle RFID input
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    // Prepare the key for authentication
    for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF; // Default key is 0xFF for all bytes

    // Authenticate with the card
    MFRC522::StatusCode status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockNum, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
      Serial.print(F("Authentication failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
      mfrc522.PICC_HaltA();
      mfrc522.PCD_StopCrypto1(); // Stop encryption
      return;
    }

    // Write data to the block
    status = mfrc522.MIFARE_Write(blockNum, writeBlockData, 16);
    if (status != MFRC522::STATUS_OK) {
      Serial.print(F("Writing failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
      mfrc522.PICC_HaltA();
      mfrc522.PCD_StopCrypto1(); // Stop encryption
      return;
    }

    Serial.println(F("Data written to the card successfully!"));
    isWriteMode = false; // Switch back to read mode after writing
    DisplayResult(); // Update display to cyan background

    // Halt communication with the card and stop encryption
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1(); // Stop encryption
  }
}

void readCardData() {
  // Handle RFID input only in read mode
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    // Prepare the key for authentication
    for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF; // Default key is 0xFF for all bytes

    // Authenticate with the card
    MFRC522::StatusCode status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockNum, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
      Serial.print(F("Authentication failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
      mfrc522.PICC_HaltA();
      mfrc522.PCD_StopCrypto1(); // Stop encryption
      return;
    }

    // Read data from the block
    status = mfrc522.MIFARE_Read(blockNum, readBlockData, &bufferLen);
    if (status != MFRC522::STATUS_OK) {
      Serial.print(F("Reading failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
      mfrc522.PICC_HaltA();
      mfrc522.PCD_StopCrypto1(); // Stop encryption
      return;
    }

    // Display the read data on the TFT screen
    DisplayBlockData(readBlockData);

    // Serial output for debugging
    Serial.print("Block Data: ");
    for (int i = 0; i < 16; i++) {
      Serial.write(readBlockData[i]);
    }
    Serial.println();

    // Halt communication with the card and stop encryption
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1(); // Stop encryption

    // Ready for the next card
  }
}

void draw_BoxNButtons() {
  tft.fillRect(0, 0, 240, 80, CYAN); // Draw the Result Box

  for (int i = GRID_SIZE, y = BASE_NUM_Y; i; i--, y += SIZE_NUM_Y) {
    for (int j = GRID_SIZE, x = BASE_NUM_X; j; j--, x += SIZE_NUM_X) {
      tft.fillRect(x, y, SIZE_NUM_X, SIZE_NUM_Y, colors[GRID_SIZE - i][GRID_SIZE - j]);
    }
  }

  // Draw Horizontal and Vertical Lines
  for (int h = BASE_NUM_Y; h <= 320; h += SIZE_NUM_Y) {
    tft.drawFastHLine(BASE_NUM_X, h, GRID_SIZE * SIZE_NUM_X, WHITE);
  }

  for (int v = BASE_NUM_X; v <= 240; v += SIZE_NUM_X) {
    tft.drawFastVLine(v, BASE_NUM_Y, GRID_SIZE * SIZE_NUM_Y, WHITE);
  }

  // Display keypad labels
  for (int j = 0; j < GRID_SIZE; j++) {
    for (int i = 0; i < GRID_SIZE; i++) {
      String s = symbol[j][i];
      int w = 16 * s.length();
      tft.setCursor((BASE_NUM_X + (SIZE_NUM_X - w) / 2) + (SIZE_NUM_X * i), (BASE_NUM_Y + (SIZE_NUM_Y / 2) - 10) + (SIZE_NUM_Y * j));
      tft.setTextSize(3);
      tft.setTextColor(WHITE);
      tft.println(symbol[j][i]);
    }
  }
}

void ShowReaderDetails() {
  byte v = mfrc522.PCD_ReadRegister(mfrc522.VersionReg);
  Serial.print(F("MFRC522 Software Version: 0x"));
  Serial.print(v, HEX);
  if (v == 0x91)
    Serial.print(F(" = v1.0"));
  else if (v == 0x92)
    Serial.print(F(" = v2.0"));
  else
    Serial.print(F(" (unknown)"));
  Serial.println("");

  if (v == 0x00 || v == 0xFF) {
    Serial.println(F("WARNING: Communication failure, is the MFRC522 properly connected?"));
  }
}
