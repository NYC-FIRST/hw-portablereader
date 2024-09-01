#include <SPI.h>                      // Communication with SPI devices
#include <MFRC522.h>                  // RFID library

#include "graphics.h"

lcd480	lcd ;

void setup()
{
  // init tft
	lcd.init() ;

  SPI.begin() ;
}

void loop()  
{
		static int loop= 1 ;

		lcd.fillScreen( BLACK ) ;
		delay( 800 ) ;

		lcd.fillScreen( RED ) ;
		lcd.text( WHITE, BLACK, 1 ) ;

		lcd.setMargin( 10, 10 ) ;
		lcd.setCursor( 0, 0 ) ;
		lcd.println( "Test\n" ) ;
		lcd.println( loop ++ , BIN ) ;

		delay( 2000 ) ;
}

