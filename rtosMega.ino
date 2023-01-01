#include <Wire.h>

#include <Arduino_FreeRTOS.h>
#include <TouchScreen.h>    //touch library
#include <LCDWIKI_GUI.h>    //Core graphics library
#include <LCDWIKI_KBV.h>    //Hardware-specific library
#include <SD.h>             // need to include the SD library
#define SD_ChipSelectPin 53 // connect pin 4 of arduino to cs pin of sd card
#include <TMRpcm.h>         //Arduino library for asynchronous playback of PCM/WAV files
#include <SPI.h>            //  need to include the SPI library

LCDWIKI_KBV my_lcd(ILI9341, A3, A2, A1, A0, A4); // model,cs,cd,wr,rd,reset

#define BLACK 0x0000       /*   0,   0,   0 */
#define BLUE 0x001F        /*   0,   0, 255 */
#define RED 0xF800         /* 255,   0,   0 */
#define GREEN 0x07E0       /*   0, 255,   0 */
#define CYAN 0x07FF        /*   0, 255, 255 */
#define MAGENTA 0xF81F     /* 255,   0, 255 */
#define YELLOW 0xFFE0      /* 255, 255,   0 */
#define WHITE 0xFFFF       /* 255, 255, 255 */
#define NAVY 0x000F        /*   0,   0, 128 */
#define DARKGREEN 0x03E0   /*   0, 128,   0 */
#define DARKCYAN 0x03EF    /*   0, 128, 128 */
#define MAROON 0x7800      /* 128,   0,   0 */
#define PURPLE 0x780F      /* 128,   0, 128 */
#define OLIVE 0x7BE0       /* 128, 128,   0 */
#define LIGHTGREY 0xC618   /* 192, 192, 192 */
#define DARKGREY 0x7BEF    /* 128, 128, 128 */
#define ORANGE 0xFD20      /* 255, 165,   0 */
#define GREENYELLOW 0xAFE5 /* 173, 255,  47 */
#define PINK 0xF81F        /* 255,   0, 255 */

/******************* UI details */
#define BUTTON_R 25         // the radius of button
#define BUTTON_SPACING_X 25 // the horizontal distance between button
#define BUTTON_SPACING_Y 5  // the vertical distance between button
#define EDG_Y 5             // lower edge distance
#define EDG_X 20            // left and right distance

#define YP A3 // must be an analog pin, use "An" notation!
#define XM A2 // must be an analog pin, use "An" notation!
#define YM 9  // can be a digital pin
#define XP 8  // can be a digital pin

// touch sensitivity for X
#define TS_MINX 911
#define TS_MAXX 117

// touch sensitivity for Y
#define TS_MINY 87
#define TS_MAXY 906

// We have a status line for like, is FONA working
#define STATUS_X 10
#define STATUS_Y 65

// touch sensitivity for press
#define MINPRESSURE 10
#define MAXPRESSURE 1000

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

TMRpcm tmrpcm; // create an object for use in this sketch
int temp = 1;

typedef struct _button_info
{
  uint8_t button_name[10];
  uint8_t button_name_size;
  uint16_t button_name_colour;
  uint16_t button_colour;
  uint16_t button_x;
  uint16_t button_y;
} button_info;

// the definition of buttons
button_info phone_button[12] =
    {
        "1",
        3,
        BLACK,
        CYAN,
        EDG_X + BUTTON_R - 1,
        my_lcd.Get_Display_Height() - EDG_Y - 4 * BUTTON_SPACING_Y - 9 * BUTTON_R - 1,
        "2",
        3,
        BLACK,
        CYAN,
        EDG_X + 3 * BUTTON_R + BUTTON_SPACING_X - 1,
        my_lcd.Get_Display_Height() - EDG_Y - 4 * BUTTON_SPACING_Y - 9 * BUTTON_R - 1,
        "3",
        3,
        BLACK,
        CYAN,
        EDG_X + 5 * BUTTON_R + 2 * BUTTON_SPACING_X - 1,
        my_lcd.Get_Display_Height() - EDG_Y - 4 * BUTTON_SPACING_Y - 9 * BUTTON_R - 1,
        "4",
        3,
        BLACK,
        CYAN,
        EDG_X + BUTTON_R - 1,
        my_lcd.Get_Display_Height() - EDG_Y - 3 * BUTTON_SPACING_Y - 7 * BUTTON_R - 1,
        "5",
        3,
        BLACK,
        CYAN,
        EDG_X + 3 * BUTTON_R + BUTTON_SPACING_X - 1,
        my_lcd.Get_Display_Height() - EDG_Y - 3 * BUTTON_SPACING_Y - 7 * BUTTON_R - 1,
        "6",
        3,
        BLACK,
        CYAN,
        EDG_X + 5 * BUTTON_R + 2 * BUTTON_SPACING_X - 1,
        my_lcd.Get_Display_Height() - EDG_Y - 3 * BUTTON_SPACING_Y - 7 * BUTTON_R - 1,
        "7",
        3,
        BLACK,
        CYAN,
        EDG_X + BUTTON_R - 1,
        my_lcd.Get_Display_Height() - EDG_Y - 2 * BUTTON_SPACING_Y - 5 * BUTTON_R - 1,
        "8",
        3,
        BLACK,
        CYAN,
        EDG_X + 3 * BUTTON_R + BUTTON_SPACING_X - 1,
        my_lcd.Get_Display_Height() - EDG_Y - 2 * BUTTON_SPACING_Y - 5 * BUTTON_R - 1,
        "9",
        3,
        BLACK,
        CYAN,
        EDG_X + 5 * BUTTON_R + 2 * BUTTON_SPACING_X - 1,
        my_lcd.Get_Display_Height() - EDG_Y - 2 * BUTTON_SPACING_Y - 5 * BUTTON_R - 1,
        // "*",3,BLACK,PINK,EDG_X+BUTTON_R-1,my_lcd.Get_Display_Height()-EDG_Y-BUTTON_SPACING_Y-3*BUTTON_R-1,
        // "0",3,BLACK,CYAN,EDG_X+3*BUTTON_R+BUTTON_SPACING_X-1,my_lcd.Get_Display_Height()-EDG_Y-BUTTON_SPACING_Y-3*BUTTON_R-1,
        // "#",3,BLACK,PINK,EDG_X+5*BUTTON_R+2*BUTTON_SPACING_X-1,my_lcd.Get_Display_Height()-EDG_Y-BUTTON_SPACING_Y-3*BUTTON_R-1,
        "Prev",
        2,
        BLACK,
        RED,
        EDG_X + BUTTON_R - 1,
        my_lcd.Get_Display_Height() - EDG_Y - BUTTON_R - 1,
        "Play",
        2,
        BLACK,
        GREEN,
        EDG_X + 3 * BUTTON_R + BUTTON_SPACING_X - 1,
        my_lcd.Get_Display_Height() - EDG_Y - BUTTON_R - 1,
        "Next",
        2,
        BLACK,
        LIGHTGREY,
        EDG_X + 5 * BUTTON_R + 2 * BUTTON_SPACING_X - 1,
        my_lcd.Get_Display_Height() - EDG_Y - BUTTON_R - 1,
};

// display string
void show_string(uint8_t *str, int16_t x, int16_t y, uint8_t csize, uint16_t fc, uint16_t bc, boolean mode)
{
  my_lcd.Set_Text_Mode(mode);
  my_lcd.Set_Text_Size(csize);
  my_lcd.Set_Text_colour(fc);
  my_lcd.Set_Text_Back_colour(bc);
  my_lcd.Print_String(str, x, y);
}

// Check whether to press or not
boolean is_pressed(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t px, int16_t py)
{
  if ((px > x1 && px < x2) && (py > y1 && py < y2))
  {
    return true;
  }
  else
  {
    return false;
  }
}

// display the main menu
void show_menu(void)
{
  uint16_t i;
  for (i = 0; i < sizeof(phone_button) / sizeof(button_info); i++)
  {
    my_lcd.Set_Draw_color(phone_button[i].button_colour);
    my_lcd.Fill_Circle(phone_button[i].button_x, phone_button[i].button_y, BUTTON_R);
    show_string(phone_button[i].button_name, phone_button[i].button_x - strlen(phone_button[i].button_name) * phone_button[i].button_name_size * 6 / 2 + 1, phone_button[i].button_y - phone_button[i].button_name_size * 8 / 2 + 1, phone_button[i].button_name_size, phone_button[i].button_name_colour, BLACK, 1);
  }
  my_lcd.Set_Draw_color(BLACK);
  my_lcd.Fill_Rectangle(1, 1, my_lcd.Get_Display_Width() - 2, 3);
  my_lcd.Fill_Rectangle(1, 29, my_lcd.Get_Display_Width() - 2, 31);
  my_lcd.Fill_Rectangle(1, 1, 3, 31);
  my_lcd.Fill_Rectangle(my_lcd.Get_Display_Width() - 4, 1, my_lcd.Get_Display_Width() - 2, 31);
}

uint16_t text_x = 10, text_y = 6, text_x_add = 6 * phone_button[0].button_name_size, text_y_add = 8 * phone_button[0].button_name_size;
uint16_t n = 0;

int Apin = 24;
int Bpin = 25;
int Cpin = 26;
int Dpin = 31;
int Epin = 30;
int Fpin = 29;
int Gpin = 28;
int Dotpin = 27;
int gearButtonpin = 10;
int buttonState = 0;

int xValue = 0;
int yValue = 0;
int bValue = 0;

void setup()
{
  // set segment pins as OUTPUT
  Serial.begin(9600);

  pinMode(Apin, OUTPUT);
  pinMode(Bpin, OUTPUT);
  pinMode(Cpin, OUTPUT);
  pinMode(Dpin, OUTPUT);
  pinMode(Epin, OUTPUT);
  pinMode(Fpin, OUTPUT);
  pinMode(Gpin, OUTPUT);
  pinMode(Dotpin, OUTPUT);

  my_lcd.Init_LCD();
  Serial.println(my_lcd.Read_ID(), HEX);
  my_lcd.Fill_Screen(BLUE);
  show_menu();

  tmrpcm.speakerPin = 46; // 5,6,11 or 46 on Mega, 9 on Uno, Nano, etc

  if (!SD.begin(SD_ChipSelectPin)) // returns 1 if the card is present
  {
    Serial.println("SD fail");
    return;
  }
  if (!SD.exists("song1.wav")) // returns 1 if the song is present
  {
    Serial.println("Song fail");
    return;
  }

  tmrpcm.setVolume(5); //

  tmrpcm.play("song1.WAV"); // the sound file "song" will play each time the arduino powers up, or is reset
                            // try to provide the file name with extension

  Serial.println(tmrpcm.isPlaying());

  // Wire.begin();
  xTaskCreate(Task1, "task1", 512, NULL, 1, NULL);
  xTaskCreate(Task2, "task2", 512, NULL, 1, NULL);

  vTaskStartScheduler();
}

void loop()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Task1(void *pvParameters)
{

  while (1)
  {
    uint16_t i;
    digitalWrite(13, HIGH);
    TSPoint p = ts.getPoint();
    digitalWrite(13, LOW);

    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);
    if (p.z > MINPRESSURE && p.z < MAXPRESSURE)
    {
      // p.x = my_lcd.Get_Display_Width()-map(p.x, TS_MINX, TS_MAXX, my_lcd.Get_Display_Width(), 0);
      // p.y = my_lcd.Get_Display_Height()-map(p.y, TS_MINY, TS_MAXY, my_lcd.Get_Display_Height(), 0);
      p.x = map(p.x, TS_MINX, TS_MAXX, 0, my_lcd.Get_Display_Width());
      p.y = map(p.y, TS_MINY, TS_MAXY, 0, my_lcd.Get_Display_Height());

      for (i = 0; i < sizeof(phone_button) / sizeof(button_info); i++)
      {
        // press the button
        if (is_pressed(phone_button[i].button_x - BUTTON_R, phone_button[i].button_y - BUTTON_R, phone_button[i].button_x + BUTTON_R, phone_button[i].button_y + BUTTON_R, p.x, p.y))
        {
          my_lcd.Set_Draw_color(DARKGREY);
          my_lcd.Fill_Circle(phone_button[i].button_x, phone_button[i].button_y, BUTTON_R);
          show_string(phone_button[i].button_name, phone_button[i].button_x - strlen(phone_button[i].button_name) * phone_button[i].button_name_size * 6 / 2 + 1, phone_button[i].button_y - phone_button[i].button_name_size * 8 / 2 + 1, phone_button[i].button_name_size, WHITE, BLACK, 1);
          delay(100);
          my_lcd.Set_Draw_color(phone_button[i].button_colour);
          my_lcd.Fill_Circle(phone_button[i].button_x, phone_button[i].button_y, BUTTON_R);
          show_string(phone_button[i].button_name, phone_button[i].button_x - strlen(phone_button[i].button_name) * phone_button[i].button_name_size * 6 / 2 + 1, phone_button[i].button_y - phone_button[i].button_name_size * 8 / 2 + 1, phone_button[i].button_name_size, phone_button[i].button_name_colour, BLACK, 1);

          // if(i < 12)
          // {
          //     if(n < 13)
          //     {
          //       show_string(phone_button[i].button_name,text_x,text_y,phone_button[i].button_name_size,GREENYELLOW, BLACK,1);
          //       text_x += text_x_add-1;
          //       n++;
          //     }
          // }
          if (i == 0)
          {
            Serial.println("Play Song 1");
            temp = 1;
          }
          else if (i == 1)
          {
            Serial.println("Play Song 2");
            temp = 2;
          }
          else if (i == 2)
          {
            Serial.println("Play Song 3");
            temp = 3;
          }
          else if (i == 3)
          {
            Serial.println("Play Song 4");
            temp = 4;
          }
          else if (i == 4)
          {
            Serial.println("Play Song 5");
            temp = 5;
          }
          else if (i == 5)
          {
            Serial.println("Play Song 6");
            temp = 6;
          }
          else if (i == 6)
          {
            Serial.println("Play Song 7");
            temp = 7;
          }
          else if (i == 7)
          {
            Serial.println("Play Song 8");
            temp = 8;
          }
          else if (i == 8)
          {
            Serial.println("Play Song 9");
            temp = 9;
          }
          else if (9 == i) // show calling ended
          {
            Serial.println("Prev");
            my_lcd.Set_Draw_color(BLUE);
            my_lcd.Fill_Rectangle(0, 33, my_lcd.Get_Display_Width() - 1, 42);
            show_string("Starting Prev", CENTER, 33, 1, OLIVE, BLACK, 1);
            temp--;
            song();
          }
          else if (10 == i) // show calling
          {
            if (tmrpcm.isPlaying())
            {
              tmrpcm.pause();
              my_lcd.Fill_Rectangle(0, 33, my_lcd.Get_Display_Width() - 1, 42);
              show_string("Pausing...", CENTER, 33, 1, OLIVE, BLACK, 1);
            }
            else
            {
              Serial.println("Play");
              my_lcd.Set_Draw_color(BLUE);
              my_lcd.Fill_Rectangle(0, 33, my_lcd.Get_Display_Width() - 1, 42);
              show_string("Starting.....", CENTER, 33, 1, OLIVE, BLACK, 1);
              song();
            }
          }
          else if (11 == i) // delete button
          {

            Serial.println("Next");
            my_lcd.Set_Draw_color(BLUE);
            text_x -= (text_x_add - 1);
            my_lcd.Fill_Rectangle(text_x, text_y, text_x + text_x_add - 1, text_y + text_y_add - 2);
            temp++;
            song();
            show_string("Starting Next", CENTER, 33, 1, OLIVE, BLACK, 1);
          }
        }
      }
    }
  }
}

void Task2(void *pvParameters)
{
  while (1)
  {
    xValue = analogRead(A10);
    yValue = analogRead(A11);

    if (xValue > 700 && yValue > 250)
    {
      printP();
    }
    else
    {
      if (xValue < 100 && yValue > 450)
      {
        printR();
      }
      else
      {
        if (xValue > 450 && yValue > 500)
        {
          printN();
        }
      }
    }
    if (xValue > 30 && yValue > 950)
    {
      printD();
    }
  }
}

void printP()
{
  digitalWrite(Apin, HIGH);
  digitalWrite(Bpin, HIGH);
  digitalWrite(Cpin, LOW);
  digitalWrite(Dpin, LOW);
  digitalWrite(Epin, HIGH);
  digitalWrite(Fpin, HIGH);
  digitalWrite(Gpin, HIGH);
  digitalWrite(Dotpin, HIGH);
}

void printD()
{
  digitalWrite(Apin, LOW);
  digitalWrite(Bpin, HIGH);
  digitalWrite(Cpin, HIGH);
  digitalWrite(Dpin, HIGH);
  digitalWrite(Epin, HIGH);
  digitalWrite(Fpin, LOW);
  digitalWrite(Gpin, HIGH);
  digitalWrite(Dotpin, HIGH);
}

void printN()
{
  digitalWrite(Apin, LOW);
  digitalWrite(Bpin, LOW);
  digitalWrite(Cpin, HIGH);
  digitalWrite(Dpin, LOW);
  digitalWrite(Epin, HIGH);
  digitalWrite(Fpin, LOW);
  digitalWrite(Gpin, HIGH);
  digitalWrite(Dotpin, HIGH);
}
void printR()
{
  digitalWrite(Apin, LOW);
  digitalWrite(Bpin, LOW);
  digitalWrite(Cpin, LOW);
  digitalWrite(Dpin, LOW);
  digitalWrite(Epin, HIGH);
  digitalWrite(Fpin, LOW);
  digitalWrite(Gpin, HIGH);
  digitalWrite(Dotpin, HIGH);
}

void song(void)
{
  if (temp == 1)
  {
    tmrpcm.play("song1.WAV");
  }
  else if (temp == 2)
  {
    tmrpcm.play("song2.WAV");
  }
  else if (temp == 3)
  {
    tmrpcm.play("song3.WAV");
  }
  else if (temp == 4)
  {
    tmrpcm.play("song4.WAV");
  }
  else if (temp == 5)
  {
    tmrpcm.play("song5.WAV");
  }
  else if (temp == 6)
  {
    tmrpcm.play("song6.WAV");
  }
  else if (temp == 7)
  {
    tmrpcm.play("song7.WAV");
  }
  else if (temp == 8)
  {
    tmrpcm.play("song8.WAV");
  }
  else if (temp == 9)
  {
    tmrpcm.play("song9.WAV");
  }
}
