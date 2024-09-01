#include "graphics.h"

void
graphics::text( graphics::Color fg, graphics::Color bg, uint8_t size )
{
		setTextColor( fg, bg ) ;
		setTextSize( size ) ;
}

///

lcd480::lcd480() : graphics( 320, 480), lcd( ILI9486, A3,A2,A1,A0,A4 )
{
}

lcd480:: ~ lcd480()
{ }

void	lcd480::init()
{
		lcd.Init_LCD() ;
}

void
lcd480::fillScreen( Color c )
{
    lcd.Fill_Screen( c ) ;
}

void
lcd480::drawPixel( int16_t x, int16_t y, Color c )
{
		lcd.Set_Draw_color( c ) ;
		lcd.Draw_Pixel( x, y ) ;
}

#define FONT_WIDTH  6
#define FONT_HEIGHT 8

size_t
lcd480::write( uint8_t c ) 
{
		if ( '\n' == c ) {
			cursor_x = 0 ;
			cursor_y += textsize_y * FONT_HEIGHT ;
			return ;
		}
		if ( '\r' == c ) { return ; }

		lcd.Draw_Char( cursor_x + margin_x, cursor_y + margin_y, c, textcolor, textbgcolor, textsize_x, false ) ;
		cursor_x += textsize_x * FONT_WIDTH ;
}

