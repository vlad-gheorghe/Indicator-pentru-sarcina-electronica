
/*-----Samodelkin YouTube Channel----------
-----------V.A.W_meter V-1.0--------------- 
*/
//------ Здесь приведены все цвета которые можно менять в тексте .
//#define ST7735_BLACK       0x0000      /*   0,   0,   0 */
//#define ST7735_NAVY        0x000F      /*   0,   0, 128 */
//#define ST7735_DARKGREEN   0x03E0      /*   0, 128,   0 */
//#define ST7735_DARKCYAN    0x03EF      /*   0, 128, 128 */
//#define ST7735_MAROON      0x7800      /* 128,   0,   0 */
//#define ST7735_PURPLE      0x780F      /* 128,   0, 128 */
//#define ST7735_OLIVE       0x7BE0      /* 128, 128,   0 */
//#define ST7735_LIGHTGREY   0xC618      /* 192, 192, 192 */
//#define ST7735_LIGHTGREY     0x7BEF      /* 128, 128, 128 */
//#define ST7735_BLUE        0x001F      /*   0,   0, 255 */
//#define ST7735_GREEN       0x07E0      /*   0, 255,   0 */
//#define ST7735_CYAN        0x07FF      /*   0, 255, 255 */
//#define ST7735_RED         0xF800      /* 255,   0,   0 */
//#define ST7735_MAGENTA     0xF81F      /* 255,   0, 255 */
//#define ST7735_YELLOW      0xFFE0      /* 255, 255,   0 */
//#define ST7735_WHITE       0xFFFF      /* 255, 255, 255 */
//#define ST7735_ORANGE      0xFD20      /* 255, 165,   0 */
//#define ST7735_GREENYELLOW 0xAFE5      /* 173, 255,  47 */
//#define ST7735_PINK        0xF81F 
#define  BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define LIGHTGREY   0xC618      /* 192, 192, 192 */
#define LIGHTGREY     0x7BEF      /* 128, 128, 128 */
#define ORANGE      0xFD20      /* 255, 165,   0 */
//------
//------Добавляем все необходимые библиотеки
#include <SPI.h>
//#include <TFT_ST7735.h> 
#include <Wire.h>
#include <OneWire.h> 
//#include <Adafruit_ADS1015.h> 
#include <Adafruit_ADS1X15.h>
#include <Adafruit_GFX.h>
//#include <Adafruit_SSD1306.h>
#include <Fonts/FreeSans12pt7b.h>
#include <SSD1283A.h> //Hardware-specific library

//------
 
 Adafruit_ADS1115 ads;              //  Выбираем необходимый модуль АЦП, можно выбрать между asd1115  или  ads1015. 
//#define RATE_ADS1015_128SPS (0x0000)                      //  Указываем номер ножки, куда подключено датчик температуры 18b20.
OneWire  ds(7);
 //TFT_ST7735 tft = TFT_ST7735();
 SSD1283A tft(/*CS=10*/ SS, /*DC=*/ 8, /*RST=*/ 9, /*LED=*/ 7); //hardware spi,cs,cd,reset,led
// Uncomment this block to use hardware SPI
//#define TFT_CS 10 // Chip select control pin
//#define TFT_DC  7  // Data Command control pin
//#define TFT_RST 9
  //Adafruit_ST7735_AS tft = Adafruit_ST7735_AS(cs, dc, rst);
 //------Место для настроек.
  float  V_max = 50 ;   // При изменении этого значения нужно сделать расчет  делителя напряжения и учесть коэффициент усиления АЦП.  K = Vin / Vацп. K-коэффициент делителя напряжения ,Vin-максимально измеряемое напряжение, Vацп-напряжение усиления АЦП   K=35/1.024 K=34,17.             
  float  A_max = 10.12; // Этот параметр настраивается программно. I=U/R    U= 1.024 усиления АЦП  , R=0.2 сопротивление шунта.    I=1.024 / 0.2 ,  I= 5.12 A. 
  float  Temp_min = 30 ; // Минимальная температура при которой начнет работать ШИМ вентилятора.
  float  Temp_max = 60 ; // Температура при которой скорость вентилятора будет максимальной.
 //------
 //-------Здесь хранятся все переменные
 float  ASP = 32768 ;// // Переміна розрядності  
 float  temperature = 0 ;
 float  V , A , W , mAh , Wh ;
 float  fil_A ,fil_V ;
 float  adc_V , adc_A;
 int    V_graf , A_graf , PWM_out ;
 int   PWM = 0 ;
 unsigned long  new_Millis ;
   // переменные фильтра для калмана.
    float varVolt = 40;  // среднее отклонение (ищем в excel)
    float varProcess = 15; // скорость реакции на изменение (подбирается вручную)
    float Pc = 0.0;
    float G = 0.0;
    float P = 1.0;
    float Xp = 0.0;
    float Zp = 0.0;
    float Xe = 0.0;
   // переменные фильтра  для калмана.
 //------


void setup()
{
  TCCR2B = TCCR2B & 0b11111000 | 0x06;     //Включаем частоту ШИМ'а  вентилятора на ногах 3 и 11: 31250 Гц. Это позволит избавиться от неприятного писка в работе вентилятора.
  Serial.begin(9600);
 //----- Здесь можно выбрать коэффициент усиления АЦП
  //                                                              ADS1015  ADS1115
  //                                                              -------  -------
    // ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
 //ads.setGain(GAIN_ONE);         // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
   ads.setGain(GAIN_TWO);         // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
 //ads.setGain(GAIN_FOUR);        // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
  // ads.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
  // ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV
 //-----
 
   ads.begin();                // Инициализация модуля АЦП. 
   tft.init();                 // Инициализация дисплея.
   tft.setRotation(1);         // Переворачиваем дисплей. 
   tft.fillScreen(BLACK);  // Указываем цвет заливки дисплея
//-----В этом месте все статические данные, которые будут отображаться на дисплее.
    //----- Рисуем рамку.
  //tft.fillRect(-1, -2,128, 128, LIGHTGREY);
  tft.fillRect(2, 1, 124, 122, BLACK);
  //tft.fillRect(-1, 109, 128, 3, LIGHTGREY); 
     //----- Отображение символов величин. 
     tft.setTextColor(RED,BLACK);       // ( цвет текста , цвет заливки текста )
     tft.setCursor(115,30);
     tft.print("A");           // ( "Текст" , положение по оси Х , положение по оси Y , размер шрифта)  
     tft.setTextColor(CYAN,BLACK);       // ( цвет текста , цвет заливки текста )
     tft.setCursor(115,79);
     tft.print("V");        
     tft.setTextColor(YELLOW,BLACK  );
     tft.setCursor(115, 90);
     tft.print("W");
     tft.setTextColor(WHITE,BLACK);
     tft.setCursor(100,9);
     tft.print("mAh");
     tft.setCursor(3,114);
     tft.print("Temp ");
     tft.setCursor(55,114);
     tft.print("C  PWM");
     tft.setTextColor(ORANGE,BLACK);
     tft.setCursor(120,114);
     tft.print("%");
      
 
//-----
   
  new_Millis = millis();  

  delay (1000);
} 
void loop()
{
 
  //adc_V = ads.readADC_Differential_2_3();   // Включаем дифференциальный режим АЦП на входах 2,3.
  //adc_A = ads.readADC_Differential_0_1();   // Включаем дифференциальный режим АЦП на входах 0,1.

  adc_V = ads.readADC_SingleEnded(0);
  adc_A = ads.readADC_SingleEnded(2);
 // adc2 = ads.readADC_SingleEnded(2);
 // adc3 = ads.readADC_SingleEnded(3);
  // fil_V = filter(adc_V);
   fil_V = adc_V;
   fil_A = adc_A;
  
  //----- Определяем температуру на датчике.
   byte data[2]; // Место для значения температуры
  ds.reset(); // Начинаем взаимодействие со сброса всех предыдущих команд и параметров
  ds.write(0xCC); // Даем датчику DS18b20 команду пропустить поиск по адресу. В нашем случае только одно устрйоство 
  ds.write(0x44); // Даем датчику DS18b20 команду измерить температуру. Само значение температуры мы еще не получаем - датчик его положит во внутреннюю память
  ds.reset(); // Теперь готовимся получить значение измеренной температуры
  ds.write(0xCC); 
  ds.write(0xBE); // Просим передать нам значение регистров со значением температуры
    data[0] = ds.read(); // Читаем младший байт значения температуры
    data[1] = ds.read(); // А теперь старший 
   temperature =  ((data[1] << 8) | data[0]) * 0.0625;
  //-----
  
  //----- Определяем скорость вентилятора в зависимости от температуры.
   if (temperature >= Temp_min && temperature <= Temp_max )  {PWM = ( temperature - Temp_min )*255/( Temp_max - Temp_min );} 
      else if (temperature < Temp_min)  { PWM=0;}
      else if (temperature >= Temp_max)  { PWM=255;}
      analogWrite (6,PWM);
   //-----   
    
 //----- Расчет и отображение всех динамических данных.
   V = fil_V * V_max / ASP;
   A = fil_A * A_max / ASP;
  if ( A<0) {A=0;}
  if (V<0) {V=0;}

 W = A * V*1.344;
   char mAh_out[8]; 
    // tft.setTextSize(2); 
   mAh += A * (millis() - new_Millis)/3600000*1000; //расчет емкости  в мАч
   new_Millis = millis();
     if (mAh<100) {
         dtostrf( mAh , 5, 1, mAh_out); tft.setTextSize(2); 
         tft.setTextColor(WHITE,BLACK);      // ( цвет текста , цвет заливки текста )
     //tft.setCursor(100,45);tft.setTextSize(1); 
     //tft.print("Amp");        
         tft.setCursor(16,5);
         tft.print(mAh_out);tft.setTextSize(1); 
       }
      else if (mAh>=100 && mAh<1000 )   {
          dtostrf( mAh , 5, 1, mAh_out); 
          tft.setTextColor(WHITE,BLACK);
          tft.setCursor(16,5);tft.setTextSize(2);
          tft.print(mAh_out);tft.setTextSize(1); 
       }         
       else  if  (mAh>=1000 && mAh<10000){
           itoa (mAh,mAh_out,10);tft.setTextSize(2);
           dtostrf( mAh , 5, 1, mAh_out);
           tft.setTextColor(WHITE,BLACK);
           tft.setCursor(16,5);
           tft.print(mAh_out);tft.setTextSize(1); 
       }
       else  if  (mAh>=10000 ){
           dtostrf( mAh , 1, 1, mAh_out);
           tft.setTextColor(WHITE,BLACK);
           tft.setCursor(16,5);tft.setTextSize(2);
           tft.print(mAh_out);tft.setTextSize(1); 
       }  
       tft.setTextSize(1); 
       //CORECTIE VALOARE U SI I FUNCTIE DE REZISTENTE DIVIZOR SI SUNT     
  char V_out[7]; dtostrf( ((V*0.9429)-(0.135*A*2.3636)) , 5, 2, V_out);
  char A_out[8]; dtostrf( A*2.3636 , 7, 2, A_out);
  char W_out[8]; dtostrf( W ,7, 2, W_out);
  char Temperature[4]; dtostrf(temperature, 4, 1, Temperature);
  char PWM_out[4]; dtostrf(PWM/2.25, 3, 0, PWM_out);

    tft.setTextColor( CYAN,BLACK); tft.setFont(&FreeSans12pt7b);
    tft.fillRect(5,43,120,34,BLACK);
    tft.setCursor(6,75);tft.setTextSize(2);
    tft.print(V_out);tft.setTextSize(1);tft.setFont();
       // if (V < 10)  {
        //  tft.setCursor(35,35);
        //  tft.print("  ");
        //  tft.setTextSize(1);
        //  }
          
    tft.setTextColor(RED,BLACK );
    tft.setCursor(5,25);tft.setTextSize(2);
    tft.print(A_out);tft.setTextSize(1);
    tft.setTextColor(YELLOW,BLACK );
    tft.setCursor(7,85);tft.setTextSize(2);
    tft.print(W_out);tft.setTextSize(1);
       // if (W < 100)  { 
        //  tft.setCursor(46,83);
        //  tft.print(" ");
        //  } 
                         
    tft.setTextColor(ORANGE,BLACK);
    tft.setCursor(28,114);
    tft.print(Temperature);
    tft.setTextColor(ORANGE,BLACK);
    tft.setCursor(95,114);
    tft.print(PWM_out );
   
       
 analogWrite(3, PWM); 
 //-----       
   //----- Отображение шкал заполнения.      
          V_graf = V / V_max * 100 ;
  if (V_graf < 0) {V_graf = -V_graf;}
  tft.fillRect(1, 5, 4, 100- V_graf, BLACK);
  tft.fillRect(1, 105 - V_graf, 4, V_graf+1, CYAN);

  A_graf = A / A_max * 100 ;
  if (A_graf < 0) {A_graf = -A_graf;}
  tft.fillRect(125, 6, 2, 100- A_graf, BLACK);
  tft.fillRect(125, 106 - A_graf, 2, A_graf+1, RED);
  //-----
      
      Serial.print  ("  A " );
      Serial.print  ( A_out );
      Serial.print  ("    " );
      Serial.print  (  adc_A );
      Serial.print  ("  V " );
      Serial.print(  V );
      Serial.print  ("    " );
      Serial.println (  adc_V );
  

       delay (10);
}
    float filter(float val) {  //функция фильтрации
  Pc = P + varProcess;
  G = Pc/(Pc + varVolt);
  P = (1-G)*Pc;
  Xp = Xe;
  Zp = Xp;
  Xe = G*(val-Zp)+Xp; // "фильтрованное" значение
  return(Xe); 
   }
