
// graphics libraries for lcd240 and lcd480

#include <Adafruit_GFX.h>

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF


#include <Adafruit_TFTLCD.h> // Hardware-specific library

class graphics : public Adafruit_GFX
{
		public:
			using Adafruit_GFX::Adafruit_GFX ;

			typedef uint16_t	Color ;

			void	setMargin( int16_t x, int16_t y ) { margin_x= x, margin_y = y ; }
			void	text( Color fg, Color bg = BLACK, uint8_t size = 1 ) ;

		protected:
			int		margin_x = 0, margin_y = 0 ;

} ;

class lcd240 : public graphics
{
} ;


#include <LCDWIKI_GUI.h> //Core graphics library
#include <LCDWIKI_KBV.h> //Hardware-specific library

class lcd480 : public graphics
{
		public:
			lcd480( ) ;
		  ~ lcd480( ) ;

			void	init() ;

			void  fillScreen( Color ) override ;

			void	drawPixel( int16_t x, int16_t y, Color color ) override ;

		protected:
			LCDWIKI_KBV  lcd ;

			size_t 	write(uint8_t) override ;
} ;

