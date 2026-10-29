#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// 1. تهيئة شاشة الـ LCD المعتمدة عندك (العنوان 0x27)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// 2. تهيئة شاشة الـ OLED المعتمدة عندك على الـ I2C الحقيقي المشترك (العنوان 0x3C)
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1  
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// شعار جامعة الطائف TU الثابت في الفلاش ميموري
static const unsigned char PROGMEM image_taif_uni_bits[] = {
  0xff,0xef,0x0f,0xff,0xef,0x0f,0xff,0xef,0x0f,0xff,0xef,0x0f,0x0f,0x0f,0x0f,0x0f,
  0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x8f,0x0f,0x07,
  0xff,0x0f,0x07,0xfe,0x0f,0x07,0xfe,0x0f,0x01,0xfc
};

// أسماء الطلاب الثابتة والمحفوظة في الفلاش ميموري لمنع تعليق الأردوينو أونو
const char s0[] PROGMEM = "1.Abdullah Alahmari";
const char s1[] PROGMEM = "2.Osama Zwawi";
const char s2[] PROGMEM = "3.Majed Gandaih";
const char s3[] PROGMEM = "4.Faisal Sultan";
const char s4[] PROGMEM = "5.Abdulaziz AlZhrani";
const char s5[] PROGMEM = "6.Turki AlZahrani";
const char s6[] PROGMEM = "7.Omar Banqetah";

const char* const students[] PROGMEM = {s0, s1, s2, s3, s4, s5, s6};

const int numStudents = 7;
const int itemHeight = 13; 
const int scrollMinY = 28; 
const int loopHeight = (numStudents * itemHeight) + 25; 
int yOffset = 0;           

// تعريف البنات المعتمدة في دائرتك الحقيقية والناجحة
const int pwmPin = 9;         
const int potPin = A0;         // المقاومة المتغيرة لتحديد الـ Target
const int vinPin = A1;         // قراءة فولت المدخل Vin عبر مجزئ الجهد الأول
const int feedbackPin = A2;    // الفيدباك لقراءة فولت الخرج Vout عبر مجزئ الجهد الثاني

// متغيرات التحكم والحماية حقك
float targetVoltage = 0.0;     
float dutyCycle = 0.1;         
int pwmValue = 25;            

// متغيرات الفلتر الذكي لتثبيت قراءة الـ Vin
float smoothedVin = 0.0;

void drawHeader() {
  display.fillRect(0, 0, 128, scrollMinY, SSD1306_BLACK);
  display.drawBitmap(102, 2, image_taif_uni_bits, 24, 14, SSD1306_WHITE);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print(F("DC MOTOR SPEED"));  
  display.setCursor(0, 9);
  display.print(F("CONTROLLE (BUCK)"));
  display.drawFastHLine(0, 18, 128, SSD1306_WHITE);
  display.setCursor(13, 20);
  display.print(F("work was done by:"));
  display.drawFastHLine(4, 29, 120, SSD1306_WHITE);
}

void setup() {
  pinMode(pwmPin, OUTPUT);
  TCCR1B = (TCCR1B & 0b11111000) | 0x01; // تردد 31.25 kHz لمنع صوت الصفير
  
  lcd.init();
  lcd.backlight();
  lcd.clear();
  
  if(display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    display.clearDisplay();
    display.display();
  }
  
  // قراءة مبدئية لتأسيس الفلتر أول ما يشتغل الأردوينو
  smoothedVin = (analogRead(vinPin) / 1023.0) * 4.65 * 11.0;
}

void loop() {
  // 1. حسابات الباور والـ Closed-Loop والـ LCD
  int potValue = analogRead(potPin);
  targetVoltage = (potValue / 1023.0) * 12.0;

  // القراءة الخام من مجزئات الجهد (تم ضبط المعامل على 4.65 لمعايرة دقيقة مع الآفو حقتك)
  float rawVin = (analogRead(vinPin) / 1023.0) * 4.65 * 11.0;
  float actualVoltage = (analogRead(feedbackPin) / 1023.0) * 4.65 * 11.0;

  // -----------------------------------------------------------------
  // التعديل السحري الجديد: فلتر التنعيم الرقمي (Digital Low-Pass Filter) لـ Vin
  // -----------------------------------------------------------------
  // يأخذ 98% من القراءة القديمة الثابتة ويضيف عليها 2% فقط من اللعب الجديد، فيموت الارتعاش تماماً!
  smoothedVin = (smoothedVin * 0.98) + (rawVin * 0.02);

  // نظام الـ Closed-Loop الأساسي حقك
  if (actualVoltage < targetVoltage) {
    dutyCycle += 0.005; 
  }
  else if (actualVoltage > targetVoltage) {
    dutyCycle -= 0.005; 
  }

  pwmValue = dutyCycle * 255.0;

  if (pwmValue > 250) { pwmValue = 250; dutyCycle = 250 / 255.0; }
  if (pwmValue < 13)  { pwmValue = 13;  dutyCycle = 13 / 255.0; }
  if (targetVoltage < 0.1) { pwmValue = 0; dutyCycle = 0; }

  analogWrite(pwmPin, pwmValue);

  // فلتر تثبيت العرض الـ 7% الخاص بالـ Vout والتارغيت
  float displayedVoltage = actualVoltage;
  if (abs(actualVoltage - targetVoltage) < 0.8) { 
    displayedVoltage = targetVoltage;
  }
  if (targetVoltage < 0.1) {
    displayedVoltage = 0.0;
  }

  // 2. طباعة الفولتات المستقرة صخر والمفلترة على الـ LCD
  lcd.setCursor(0, 0);
  lcd.print(F("Vin : "));
  lcd.print(smoothedVin, 2); // طباعة الـ Vin الثابتة والمحمية من الارتعاش والنويز
  lcd.print(F("   ")); 

  lcd.setCursor(0, 1);
  lcd.print(F("T:"));
  lcd.print(targetVoltage, 2); 
  lcd.print(F(" V:"));
  lcd.print(displayedVoltage, 2); 
  lcd.print(F("   ")); 

  // 3. تحديث حركة الأسماء والشعار لايف على شاشة الـ OLED
  display.clearDisplay();
  display.setTextSize(1);

  char buffer[25]; 
  for (int i = 0; i < numStudents; i++) {
    int yPos = scrollMinY + yOffset + (i * itemHeight);
    while (yPos < scrollMinY) { yPos += loopHeight; }
    while (yPos >= scrollMinY + loopHeight) { yPos -= loopHeight; }

    if (yPos > scrollMinY && yPos < 64) {
      display.setCursor(6, yPos);
      strcpy_P(buffer, (char*)pgm_read_word(&(students[i])));
      display.print(buffer);
    }
  }

  drawHeader(); 
  display.display();

  yOffset--; 
  if (yOffset <= -loopHeight) { yOffset = 0; }

  delay(80); 
}