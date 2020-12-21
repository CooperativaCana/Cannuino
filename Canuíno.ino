////////////COMUNICAÇÃO INTERNET
#define BLYNK_PRINT Serial
#include <BlynkSimpleShieldEsp8266.h>
#include <WiFi.h>
char auth[] = "_v1WgCwcT98Ax8eYHhU4gJxoIowEexYh";
char ssid[] = "NOS_Internet_Movel_C254";
char pass[] = "04047761";
BlynkTimer timer;

////////////ESP8266 WIFI
#include <ESP8266_Lib.h>
#define EspSerial Serial1
ESP8266 wifi(&EspSerial);
#define ESP8266_BAUD 115200

////////////COMUNICAÇÃO COM I2C
#include <Wire.h> //SCL,SDA library

///////////DS1307 TINY REAL TIME CLOCK
#include "RTClib.h" //RTC1307 library
#include <DS1307RTC.h> //RTC1307, Clock
RTC_DS1307 rtc;
#if defined(ARDUINO_ARCH_SAMD)
#define Serial SerialUSB
#endif

///////////SENSORES DE HUMIDADE E TEMPERATURA DO AR DHT22 E DHT11
#include "DHT.h"
#define DHTPIN_11 10     // what pin we're connected to
#define DHTPIN_22 33
#define DHTTYPE DHT11   // DHT 11 
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht11(DHTPIN_11, DHT11);
DHT dht22(DHTPIN_22, DHT22);

///////////////////VISOR OLED
#include <U8g2lib.h>
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#endif
U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

//////////////////////////RELÉS do canal D2 ao D7
#define balastro 8 /////////check ficha branca
int balastroOut = 8;

#define Led_vegetativo 3 // check ficha 3
int Led_vegetativoOut = 3;

#define ventoinhaGrow 2 // ///////check ficha 1
int ventoinhaGrowOut = 2;

#define ventoinha_Bloom 6 //////////check  shuko femea preta
int ventoinha_BloomOut = 6;

#define mm125tube 7 /////////check ficha 2 ///ok
int mm125tubeOut = 7;

#define bomba 9 /////// check ficha 4
int bombaOut = 9;

/////////////////////// 12V

#define Piezo24v 4 // check ficha 24v
int Piezo24vOut = 4;

#define intakefan 5 // check ficha 12v 
int intakefanOut = 5;

// current state of the button
const int  buttonPin = 13;
int buttonState = 0;         
int lastButtonState = 0;

/////////////////////IDE SETUP
void setup() {
  Serial.begin(9600);
  Wire.begin();
  rtc.begin();
  EspSerial.begin(ESP8266_BAUD);
  Blynk.begin(auth, wifi, ssid, pass);
  //rtc.adjust(DateTime(2020, 12, 10, 1, 34 , 0));
  dht11.begin();
  dht22.begin();
  u8g2.begin();

  /////////////RELÈS
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);

  //////PORQUE ESTÃO NORMALMENTE ABERTAS EM LOW
  digitalWrite(2, HIGH);
  digitalWrite(3, HIGH);
  digitalWrite(4, HIGH);
  digitalWrite(5, HIGH);
  digitalWrite(6, HIGH);
  digitalWrite(7, HIGH);
  digitalWrite(8, HIGH);
  digitalWrite(9, HIGH);

  pinMode(buttonPin, INPUT);
}

void loop() {
  Blynk.run();
  luzes();
  ambiente_bloom();
  Humidade_Automatica_grow();
  MenuPrincipal();
  blynk_solo();
  
  buttonState = digitalRead(buttonPin);
  if (buttonState != lastButtonState) {
    if (buttonState == LOW) {
      digitalWrite(12, HIGH);
    } else {
      digitalWrite(12, LOW);
    }
  }
  lastButtonState = buttonState;
}

void blynk_solo() {
  int sensor1Pin = A13;
  int sensor1Value = 0;
  int sensor1map = 0;
  sensor1Value = analogRead(sensor1Pin);
  sensor1map = map(sensor1Value, 0, 620, 0, 100); //////map(valor/input a ser mapeado, fromLow, fromHigh, toLow, toHigh)

  int sensor2Pin = A12;
  int sensor2Value = 0;
  int sensor2map = 0;
  sensor2Value = analogRead(sensor2Pin);
  sensor2map = map(sensor2Value,  0, 620, 0, 100); //////map(valor/input a ser mapeado, fromLow, fromHigh, toLow, toHigh)

  int sensor3Pin = A11;
  int sensor3Value = 0;
  int sensor3map = 0;
  sensor3Value = analogRead(sensor3Pin);
  sensor3map = map(sensor3Value,  0, 620, 0, 100); //////map(valor/input a ser mapeado, fromLow, fromHigh, toLow, toHigh)

  Blynk.virtualWrite(V5, sensor1map);
  Blynk.virtualWrite(V6, sensor2map);
  Blynk.virtualWrite(V7, sensor3map);
}

void MenuPrincipal() {
  const int potPin = A6; //6
  int potValue = 0;
  int potMap = 0;

  const int pot2Pin = A15; //
  int pot2Value = 0;
  int pot2Map = 0;

  int temp_dht22 = dht22.readTemperature();
  int hum_dht22 = dht22.readHumidity() - 9;
  int temp_dht11 = dht11.readTemperature();
  int hum_dht11 = dht11.readHumidity() ;

  int sensor1Pin = A13;
  int sensor1Value = 0;
  int sensor1map = 0;
  sensor1Value = analogRead(sensor1Pin);
  sensor1map = map(sensor1Value, 0, 620, 0, 100); //////map(valor/input a ser mapeado, fromLow, fromHigh, toLow, toHigh)

  int sensor2Pin = A12;
  int sensor2Value = 0;
  int sensor2map = 0;
  sensor2Value = analogRead(sensor2Pin);
  sensor2map = map(sensor2Value,  0, 620, 0, 100); //////map(valor/input a ser mapeado, fromLow, fromHigh, toLow, toHigh)

  int sensor3Pin = A11;
  int sensor3Value = 0;
  int sensor3map = 0;
  sensor3Value = analogRead(sensor3Pin);
  sensor3map = map(sensor3Value,  0, 620, 0, 100); //////map(valor/input a ser mapeado, fromLow, fromHigh, toLow, toHigh)

  int map_hum_dht11 = 0;
  map_hum_dht11 = map(hum_dht11, 0, 130, 0, 100);

  potValue = analogRead(potPin);
  potMap = map(potValue, 10, 1000, 1, 3);
  switch (potMap) {
    case 1:
      rega();
      oled_rega();
      break;

    case 2:
      rega_parada();
      oled_rega_Stop();
      break;

    case 3:
      pot2Value = analogRead(pot2Pin);
      pot2Map = map(pot2Value, 0, 1023, 1, 7);
      switch (pot2Map) {
        case 1:
          u8g2.firstPage();
          do {
            u8g2.setFont(u8g2_font_helvB14_te);
            u8g2.setCursor(12, 26);
            u8g2.print("Bloom Temp");
            u8g2.setFont(u8g2_font_helvB24_te);
            u8g2.setCursor(42, 56);
            u8g2.print(temp_dht22);
          }  while (u8g2.nextPage());
          break;

        case 2:
          u8g2.firstPage();
          do {
            u8g2.setFont(u8g2_font_helvB14_te);
            u8g2.setCursor(12, 26);
            u8g2.print("Grow Temp");
            u8g2.setFont(u8g2_font_helvB24_te);
            u8g2.setCursor(42, 56);
            u8g2.print(temp_dht11);
          }  while (u8g2.nextPage());
          break;

        case 3:
          u8g2.firstPage();
          do {
            u8g2.setFont(u8g2_font_helvB14_te);
            u8g2.setCursor(12, 26);
            u8g2.print("Grow Hum");
            u8g2.setFont(u8g2_font_helvB24_te);
            u8g2.setCursor(42, 56);
            u8g2.print(map_hum_dht11);
          }  while (u8g2.nextPage());
          break;

        case 4:
          u8g2.firstPage();
          do {
            u8g2.setFont(u8g2_font_helvB14_te);
            u8g2.setCursor(12, 26);
            u8g2.print("Bloom Hum");
            u8g2.setFont(u8g2_font_helvB24_te);
            u8g2.setCursor(42, 56);
            u8g2.print(hum_dht22);
          }  while (u8g2.nextPage());
          break;

        case 5:
          u8g2.firstPage();
          do {
            u8g2.setFont(u8g2_font_helvB14_te);
            u8g2.setCursor(22, 26);
            u8g2.print("Sensor1");
            u8g2.setFont(u8g2_font_helvB24_te);
            u8g2.setCursor(42, 56);
            u8g2.print(sensor1map);
          }  while (u8g2.nextPage());
          break;

        case 6:
          u8g2.firstPage();
          do {
            u8g2.setFont(u8g2_font_helvB14_te);
            u8g2.setCursor(22, 26);
            u8g2.print("Sensor2");
            u8g2.setFont(u8g2_font_helvB24_te);
            u8g2.setCursor(42, 56);
            u8g2.print(sensor2map);
          }  while (u8g2.nextPage());
          break;

        case 7:
          u8g2.firstPage();
          do {
            u8g2.setFont(u8g2_font_helvB14_te);
            u8g2.setCursor(22, 26);
            u8g2.print("Sensor3");
            u8g2.setFont(u8g2_font_helvB24_te);
            u8g2.setCursor(42, 56);
            u8g2.print(sensor3map);
          }  while (u8g2.nextPage());
          break;
      }
  }
}

void rega() {
  digitalWrite(bomba, LOW);
  digitalWrite(15, HIGH);
}

void rega_parada() {
  digitalWrite(bomba, HIGH);
  digitalWrite(15, LOW);
}

void luzes() {
  DateTime now = rtc.now();
  ///////12 horas
  if ((now.hour() >= 20 && now.hour() <= 23 ) or
      (now.hour() >= 0 && now.hour() <= 8 ))
  {
    digitalWrite(balastro, LOW);
  } else {
    digitalWrite(balastro, HIGH);
  }

  if ((now.hour() >= 20 && now.hour() <= 23 ) or
      (now.hour() >= 0 && now.hour() <= 14 ))
  {
    digitalWrite(Led_vegetativo, LOW);
  } else {
    digitalWrite(Led_vegetativo, HIGH);
  }
}

void quente_bloom() {
  digitalWrite(mm125tube, LOW);
  digitalWrite(intakefan, LOW);
  digitalWrite(ventoinha_Bloom, LOW);
}
void estado_normal_bloom() {
  digitalWrite(mm125tube, HIGH);
  digitalWrite(intakefan, LOW);
  digitalWrite(ventoinha_Bloom, LOW);
}
void frio_bloom_dia() {
  digitalWrite(intakefan, HIGH);
  digitalWrite(mm125tube, HIGH);
  digitalWrite(ventoinha_Bloom, HIGH);
}

///////////////////////////TEMPERATURA E HUMIDADE
void ambiente_bloom() {
  int temp_dht22 = dht22.readTemperature();
  Blynk.virtualWrite(V2, temp_dht22, " ºC");
  int hum_dht22 = dht22.readHumidity() - 9;
  Blynk.virtualWrite(V1, hum_dht22, " %");
  if ( temp_dht22 >= 25 or hum_dht22 >= 60 )
  {
    quente_bloom();
  }

  if ( temp_dht22 >= 18 && temp_dht22 <= 24 )
  {
    estado_normal_bloom();
  }

  if (balastro == LOW && temp_dht22 <= 17)
  {
    frio_bloom_dia();
  }

  if ( hum_dht22 >= 65)
  {
    quente_bloom();
  }
}

void Humidade_Automatica_grow() {
  int temp_dht11 = dht11.readTemperature();
  int hum_dht11 = dht11.readHumidity();

  int map_hum_dht11 = 0;
  map_hum_dht11 = map(hum_dht11, 0, 130, 0, 100);
  Blynk.virtualWrite(V3, map_hum_dht11, " %");
  Blynk.virtualWrite(V4, temp_dht11, " ºC");

  if (map_hum_dht11 <= 65) {
    digitalWrite(4, LOW);
    digitalWrite(ventoinhaGrow, HIGH);
  } else {
    digitalWrite(4, HIGH);
    digitalWrite(ventoinhaGrow, LOW);
  }
}

void oled_rega() {
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_helvB18_te);
    u8g2.setCursor(0, 42);
    u8g2.print("A regar");
  }  while (u8g2.nextPage());
}

void oled_rega_Stop() {
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_helvB18_te);
    u8g2.setCursor(0, 42);
    u8g2.print("Bomba Parada");
  }  while (u8g2.nextPage());
}
