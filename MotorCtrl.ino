#include <SPI.h>
#include <Wire.h>
#include <Encoder.h>
#include <Servo.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128    // OLED display width, in pixels
#define SCREEN_HEIGHT 32    // OLED display height, in pixels
#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

static const unsigned char PROGMEM pwm_bmp[] =
    {0b00000000,
     0b00111100,
     0b00100100,
     0b00100100,
     0b00100100,
     0b00100100,
     0b11100111,
     0b00000000};

Servo ESC; // create servo object to control the ESC
Encoder myEnc(3, 4);
//   avoid using pins with LEDs attached

void setup()
{
  Serial.begin(19200);
  Serial.println("Basic Encoder Test:");
  ESC.attach(9, 1000, 2000); // (pin, min pulse width, max pulse width in microseconds)

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }

  display.display();
  delay(1000);

  // Clear the buffer
  display.clearDisplay();
  display.setRotation(2);
}

long encoderPos = -999;
int setpoint = 0;

void loop()
{
  updateEncoder();
  updateSetpoint();
  updateESC();
  drawUI();
}

void updateEncoder()
{
  long newPosition = myEnc.read();
  if (newPosition != encoderPos)
  {
    encoderPos = newPosition;
  }
}

void updateSetpoint()
{
  int floor = -256;
  int ceiling = 0;
  if (encoderPos > ceiling)
  {
    ceiling = encoderPos;
    floor = ceiling - 256;
  }
  else if (encoderPos < floor)
  {
    floor = encoderPos;
    ceiling = floor + 256;
  }

  setpoint = map(encoderPos, ceiling, floor, 0, 100);
  Serial.print("setpoint: ");
  Serial.print(setpoint);
  Serial.println();
}

void updateESC()
{
  ESC.write(map(setpoint, 0, 100, 90, 180));
}

void drawUI(void)
{
  display.clearDisplay();

  display.drawRect(14, 12, 100, 6, SSD1306_WHITE);
  display.fillRect(14, 12, setpoint, 6, SSD1306_WHITE);
  display.fillTriangle(14 + setpoint, 20, 11 + setpoint, 25, 17 + setpoint, 25, SSD1306_WHITE);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 0);
  display.print("Duty: ");
  display.print(setpoint);
  display.print("%");
  display.println();
  display.drawBitmap(
      0,
      0,
      pwm_bmp, 8, 8, 1);
  display.display(); // Update screen with each newly-drawn rectangle

  delay(1);
}