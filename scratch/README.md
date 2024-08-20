# Project Overview

This repository contains project files developed using the Arduino platform, specifically compiled for the ATMEGA328P microcontroller.

## Development Environment

- **Arduino Software (IDE):**  
  [Download and install here](https://www.arduino.cc/en/software)

- **Microcontroller:** ATMEGA328P  
  [Product Page](https://www.microchip.com/en-us/product/atmega328p) | [Datasheet](https://ww1.microchip.com/downloads/aemDocuments/documents/MCU08/ProductDocuments/DataSheets/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf)

## Dependencies

### Card Reader
This project utilizes the MFRC522 card reader module.  
- [Datasheet](https://www.nxp.com/docs/en/data-sheet/MFRC522.pdf)

### Required Libraries

1. **Display Driver:**
   - Library: [SPFD5408](https://github.com/JoaoLopesF/SPFD5408)

2. **Card Reader (SPI Communication):**
   - [SPI Reference](https://www.arduino.cc/reference/en/language/functions/communication/spi/) (built-in)
   - [MFRC522 Library](https://www.arduino.cc/reference/en/libraries/mfrc522/) (built-in)


## Keyboard Code

The keyboard code was originally described in the calculator example provided in the [`ScreenCode.ino`](./ScreenCode.ino) file

## Hardware Links

Below are the links to the hardware components used in this project:

- **Board (ATMEGA328P Arduino-Compatible Board):**  
  [Amazon Link](https://www.amazon.com/ATmega328P-Arduino-Compatible-Arduino-Voltage-Compatible/dp/B0D83J2TJJ/ref=sr_1_13?dib=eyJ2IjoiMSJ9.MazmhFfn-DF8W5oyX_S-tDFAqLRDaMJSkroaZhdQMdiuSGWTSdREaZBK7N09J3Pn-7Gx8Bv8kdZ0R-IWQJTIbU3fOqL8StkQPvvHVC-EztfFw0M_a0Gos1nAkRrCEL1Doz_4eClZD2FOVgdsJdvqis-XZ3ayC-20DwdhPZuYO6E2X7A9QsPIbYq2KSE9navnPAp1PtwpqjNinNj_8yyg3q0S4voM1GJX-LGdbqkq4y4.M0oPV4V1oBta4Hi0FMzCsmVU66aXMA35b5lKLoBOGyM&dib_tag=se&keywords=arduino%2Bboard&qid=1724138978&sr=8-13&th=1)

- **LCD Hat (2.4" TFT Display with Touch Support):**  
  [Amazon Link](https://www.amazon.com/Module-Screen-Board-Support-Touch/dp/B0CJM97NY8)

- **RFID Card Reader (MFRC522):**  
  [Amazon Link](https://www.amazon.com/HiLetgo-RFID-Kit-Arduino-Raspberry/dp/B01CSTW0IA/ref=sr_1_1_sspa?crid=3H00THYLTA1DC&dib=eyJ2IjoiMSJ9.8XPpZTgxO2fTkJM_KJ_NEjF6wAJALEtzcvtOO6tlwnE8lSEVOV4yj6gIXrqLKPaeqBsT9FMuu55BtulknNLd3PE-ibZxjA5xmXij9TZT7Fpb8_5XF2a9fr6R0fDezEbpWNA7BYdjmkk8_u8wNpEMhNLZD3P9Brx_Vu2CgviNwLYfOFTv9jTyBiQO4rxtPmSlYUrE-qr7QL_p3NtMlQMU7CeDfzc9XhK2dIbv8FlZgoY.2EWhyoQuVyWjIRjBc-cMA26xzoP8gGCXcTbetXUvOfc&dib_tag=se&keywords=rc522&qid=1724139088&sprefix=rc522%2Caps%2C113&sr=8-1-spons&sp_csd=d2lkZ2V0TmFtZT1zcF9hdGY&psc=1)
