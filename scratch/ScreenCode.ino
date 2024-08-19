/*______Import Libraries_______*/
#include <SPFD5408_Adafruit_GFX.h>    // Core graphics library
#include <SPFD5408_Adafruit_TFTLCD.h> // Hardware-specific library
#include <SPFD5408_TouchScreen.h>
#include <SPI.h>                      // Communication with SPI devices
#include <MFRC522.h>                  // 
/*______End of Libraries_______*/

/*______Define LCD pins (I have asigned the default values)_______*/
#define YP A1  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 7   // can be a digital pin
#define XP 6   // can be a digital pin

#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0

#define LCD_RESET A4

/*_______End of defanitions______*/

/*______Assign names to colors and pressure_______*/
#define WHITE   TFTLCD_WHITE //Black->White
#define YELLOW    TFTLCD_YELLOW //Blue->Yellow
#define CYAN     TFTLCD_CYAN //Red->Cyan
#define PINK   TFTLCD_PINK //Green-> Pink
#define RED    TFTLCD_RED //Cyan -> Red
#define GREEN TFTLCD_GREEN //Pink -> Green 
#define BLUE  TFTLCD_BLUE //Yellow->Blue
#define BLACK   TFTLCD_BLACK //White-> Black

#define DARKGREEN  TFTLCD_GREEN & 0x03F0
#define MINPRESSURE 10
#define MAXPRESSURE 1000

/*_______Assigned______*/


/*____Calibrate TFT LCD_____*/
#define TS_MINX 210
#define TS_MINY 100
#define TS_MAXX 875
#define TS_MAXY 800

/*______End of Calibration______*/

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300); //300 is the sensitivity
Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET); //Start communication with LCD

#define GRID_SIZE 4

String symbol[GRID_SIZE][GRID_SIZE] = {
  { "7", "8", "9", "Add" },
  { "4", "5", "6", "Rem" },
  { "1", "2", "3", "Ex" },
  { "X", "0", ".", "Rst" }
};
uint16_t colors[][4] = { 
  { TFTLCD_RED, BLACK, BLACK, GREEN, },
  { BLACK, TFTLCD_RED, BLACK, DARKGREEN, },
  { BLACK, TFTLCD_RED, TFTLCD_RED, DARKGREEN, },
  { BLUE, PINK, BLACK, DARKGREEN, },
} ;

#define BASE_NUM_X 0
#define BASE_NUM_Y 80
#define SIZE_NUM_X 60
#define SIZE_NUM_Y 40

 long Num1,Num2,Number;

void setup() {

  Serial.begin(9600); //Use serial monitor for debugging
  tft.reset(); //Always reset at start
  tft.begin(0x9341); // My LCD uses LIL9341 Interface driver IC
  tft.setRotation(0); // I just roated so that the power jack faces up - optional
  tft.fillScreen(WHITE);

  draw_BoxNButtons(); 
}

void loop() {
  char p = waitTouch() ;
  static int action = 0 ;

  switch ( p )
  {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      Number = (Number * 10) + ( p - '0' ) ;
      break ;
      
     case '/':
     case '*':
     case '-':
     case '+':
       action= p, Num1= Number, Number= 0 ;
        tft.setCursor(200, 20) ;
        tft.setTextColor(RED);
        tft.println( p ) ;
        delay(300);
       break ;
     case 'C':
      Number=Num1=Num2=0 ;
      break ;
     case '=':
      Num2=Number;
      CalculateResult(action);
      break ;
     default:
      return ;
  }
 
  DisplayResult();   
  delay(300);
}


char waitTouch() {
  TSPoint p;

  do {
    p = ts.getPoint(); 
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);

  } while((p.z < MINPRESSURE )|| (p.z > MAXPRESSURE));
    Serial.print("Raw X: ");
  Serial.print(p.x);
  Serial.print(", Raw Y: ");
  Serial.println(p.y);

  p.x = map(p.x, TS_MINX, TS_MAXX, 240, 0);
  p.y = map(p.y, TS_MINY, TS_MAXY, 320, 0);

  int gridx= (p.x - BASE_NUM_X +1) / SIZE_NUM_X ;
  int gridy= (p.y - BASE_NUM_Y +1) / SIZE_NUM_Y ;
  if (( gridx < 0 ) || (gridy < 0 ) || (gridx >= GRID_SIZE ) || (gridy >= GRID_SIZE ))
     return '\0' ;

  Serial.print("Pressed button ") ;
  Serial.print( symbol[gridy][gridx] ) ;
  return symbol[gridy][gridx][0] ;
}

void CalculateResult(int action)
{
  switch ( action )
  {
    case '+':  Number= Num1+Num2 ;  break ;
    case '-':  Number= Num1-Num2 ;  break ;
    case '*':  Number= Num1*Num2 ;  break ;
    case '/':  Number= Num1/Num2 ;  break ;
  }
}

void DisplayResult()

{
    tft.fillRect(0, 0, 240, 80, CYAN);  //clear result box
    tft.setCursor(10, 20);
    tft.setTextSize(4);
    tft.setTextColor(BLACK);
    tft.println(Number); //update new value
}

void draw_BoxNButtons()
{
  //Draw the Result Box
  tft.fillRect(0, 0, 240, 80, CYAN);

 //Draw First Column
 for (int i= GRID_SIZE, y= BASE_NUM_Y; ( i ) ; i--, y += SIZE_NUM_Y ) {
  for (int j = GRID_SIZE, x= BASE_NUM_X; ( j ) ; j--, x+= SIZE_NUM_X ) {
    tft.fillRect  (x,y,SIZE_NUM_X,SIZE_NUM_Y,colors[GRID_SIZE-i][GRID_SIZE-j]);
  }
 }

  //Draw Horizontal Lines

  for (int h=BASE_NUM_Y; h<=320; h+=SIZE_NUM_Y)
    { tft.drawFastHLine(BASE_NUM_X, h, GRID_SIZE * SIZE_NUM_X, WHITE); }

  //Draw Vertical Lines

  for (int v=BASE_NUM_X; v<=240; v+=SIZE_NUM_X)
   { tft.drawFastVLine(v, BASE_NUM_Y, GRID_SIZE * SIZE_NUM_Y, WHITE); }

  //Display keypad lables 

  for (int j=0;j<GRID_SIZE;j++) {
    for (int i=0;i<GRID_SIZE;i++) {
      String s= symbol[j][i] ;
      int w= 16 * s.length() ;
      tft.setCursor((BASE_NUM_X + (SIZE_NUM_X - w) / 2 ) + (SIZE_NUM_X*i), (BASE_NUM_Y +(SIZE_NUM_Y/2)-10) + (SIZE_NUM_Y*j));
      tft.setTextSize(3);
      tft.setTextColor(WHITE);
      tft.println(symbol[j][i]);
    }
  }
}
