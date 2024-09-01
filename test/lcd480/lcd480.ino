#include <SPI.h>                      // Communication with SPI devices
#include <MFRC522.h>                  // RFID library

#include <LCDWIKI_GUI.h> //Core graphics library
#include <LCDWIKI_KBV.h> //Hardware-specific library

LCDWIKI_KBV lcd(ILI9486,A3,A2,A1,A0,A4) ;


#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

void setup()
{
  // init tft
	lcd.Init_LCD() ;

  SPI.begin() ;
}

void loop()  
{
		static int loop= 1 ;

		lcd.Fill_Screen( BLACK ) ;
		delay( 800 ) ;

		lcd.Fill_Screen( RED ) ;
		lcd.Set_Text_Back_colour( BLACK ) ;
		lcd.Set_Text_colour( WHITE ) ;
		lcd.Set_Text_Size( 1 ) ;

		lcd.Print_String( "Test", 10, 10 ) ;
		lcd.Print_Number_Int( loop ++, 10, 60 ) ;

		delay( 2000 ) ;
}
