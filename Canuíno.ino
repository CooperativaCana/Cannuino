#define BLYNK_PRINT Serial
#include <BlynkSimpleShieldEsp8266.h>
#include <WiFi.h>
char auth[] = "_v1WgCwcT98Ax8eYHhU4gJxoIowEexYh";
char ssid[] = "NOS_Internet_Movel_C254";
char pass[] = "04047761";

#include <ESP8266_Lib.h>
#define EspSerial Serial1
ESP8266 wifi(&EspSerial);
#define ESP8266_BAUD 115200
BlynkTimer timer;

#include <Wire.h> //SCL,SDA library
#include "RTClib.h" //RTC1307 library
#include <DS1307RTC.h> //RTC1307, Clock

RTC_DS1307 rtc;
#if defined(ARDUINO_ARCH_SAMD)
#define Serial SerialUSB
#endif

#include <U8g2lib.h>
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#endif
U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

int lemon_kush_scrog = 0;

#include "DHT.h" //Temperature and humidity sensor library
#define DHTPIN_sensor_dht22 3
#define DHTTYPE DHT22
DHT dht_sensor_dht22(DHTPIN_sensor_dht22, DHTTYPE);

#define balastro 39 /////////check ficha branca
int balastroOut = 39;

#define cooltubefan 45 // check ficha 12v fio separado
int cooltubefanOut = 45;

#define mm125tube 35 /////////check ficha 2 ///ok
int mm125tubeOut = 35;

#define intakefan 43 // check ficha 12v com nó
int intakefanOut = 43;

#define ventoinha_grande 41 //////////check  shuko femea preta
int ventoinha_grandeOut = 41;

#define outake100mm 31 // ///////check ficha 1
int outake100mmOut = 31;

#define bomba 37 /////// check ficha 4
int bombaOut = 37;

#define humificador 33 // check ficha 3
int humificadorOut = 33;

void setup() {

  Serial.begin(9600);
  EspSerial.begin(ESP8266_BAUD);
  delay(10);
  Blynk.begin(auth, wifi, ssid, pass);
  u8g2.begin();
  Wire.begin();
  rtc.begin();
  //rtc.adjust(DateTime(2020, 10, 27, 23, 56 , 0)); ///  to get the rtc on'clock, run just one time the code with and then without
  dht_sensor_dht22.begin();
  DateTime now = rtc.now();
  pinMode(31, OUTPUT);
  pinMode(33, OUTPUT);
  pinMode(35, OUTPUT);
  pinMode(37, OUTPUT);
  pinMode(39, OUTPUT);
  pinMode(41, OUTPUT);
  pinMode(43, OUTPUT);
  pinMode(45, OUTPUT);

  digitalWrite(31, HIGH);
  digitalWrite(33, HIGH);
  digitalWrite(35, HIGH);
  digitalWrite(37, HIGH);
  digitalWrite(39, HIGH);
  digitalWrite(41, HIGH);
  digitalWrite(43, HIGH);
  digitalWrite(45, HIGH);
  timer.setInterval(1000L, Temperatura_Automatica);
  timer.setInterval(2000L, timerLampadas_12);
}

void loop() {
  Blynk.run();
  timer.run();
  //rega();
  timerLampadas_18();
  Temperatura_Automatica();
  u8g2.firstPage();
  do {
    draw();
  } while (u8g2.nextPage());
}

void draw() {
  DateTime now = rtc.now();
  float hum_sensor_dht22 = dht_sensor_dht22.readHumidity() - 12;
  float temp_sensor_dht22 = dht_sensor_dht22.readTemperature();
  // Calibration and data from sensor
  ///Dry: (520 430]     //// ( 609 - 590 - 560 ) | 0% a 23% dry
  ///Wet: (430 350]     //// ( 560 - 455 - 350 ) | 24% a 64% wet
  ///Water: (350 260]   //// ( 350 - 305 - 276 ) | 65% a 100% water
  lemon_kush_scrog = map(analogRead(A15), 656, 276, 0, 100); //////map(valor/input a ser mapeado, fromLow, fromHigh, toLow, toHigh)

  u8g2.setFont(u8g2_font_inb16_mr);
  u8g2.setCursor(0, 16);
  u8g2.print("Solo="); u8g2.print(lemon_kush_scrog); u8g2.print(" %");

  u8g2.setFont(u8g2_font_courB14_tn);
  u8g2.setCursor(0, 34);
  u8g2.print("T"); u8g2.print(temp_sensor_dht22);

  u8g2.setCursor(0, 50);
  u8g2.print("H"); u8g2.print(hum_sensor_dht22);

  u8g2.setCursor(0, 64);
  u8g2.print(now.hour()); u8g2.print(":"); u8g2.print(now.minute());
}


void rega() {
  if (digitalRead(6) == LOW) {
    digitalWrite(bomba, LOW);
  } else {
    digitalWrite(bomba, HIGH);
  }
}

void timerLampadas_12() {
  DateTime now = rtc.now();
  /// 12 hours - Flowering Stage
  if ((now.hour() >= 20 && now.hour() <= 23 ) or
      (now.hour() >= 0 && now.hour() <= 20 ))
  {
    digitalWrite(balastro, LOW);
    Blynk.virtualWrite(V8, "De dia | 400W HPS");
  } else {
    digitalWrite(balastro, HIGH);
    Blynk.virtualWrite(V8, "Noite");
  }
}

void timerLampadas_18() {
  DateTime now = rtc.now();
  // Serial.println("Vegetativo");
  /// 18 hours - Flowering Stage
  if ((now.hour() >= 20 && now.hour() <= 23 ) or
      (now.hour() >= 0 && now.hour() <= 14 ))
  {
    digitalWrite(balastro, LOW);
    Blynk.virtualWrite(V8, "De dia | 400W HPS");
  } else {
    digitalWrite(balastro, HIGH);
    Blynk.virtualWrite(V8, "Noite");
  }
}

void estado_normal() {
  digitalWrite(mm125tube, HIGH);
  digitalWrite(outake100mm, HIGH);
  digitalWrite(cooltubefan, LOW);
  digitalWrite(intakefan, LOW);
  digitalWrite(ventoinha_grande, LOW);
 // Blynk.virtualWrite(V7, "Ambiente a Ameno");
}

void pressao_negativa() {
  digitalWrite(mm125tube, LOW);
  digitalWrite(outake100mm, LOW);
  digitalWrite(cooltubefan, LOW);
  digitalWrite(intakefan, LOW);
  digitalWrite(ventoinha_grande, HIGH);
  Blynk.virtualWrite(V7, "Ambiente Quente");
}

void frio() {
  digitalWrite(intakefan, HIGH);
  digitalWrite(cooltubefan, HIGH);
  digitalWrite(mm125tube, HIGH);
  digitalWrite(outake100mm, HIGH);
  digitalWrite(ventoinha_grande, HIGH);
  Blynk.virtualWrite(V7, "Ambiente Frio");
}

////////////////////////////////////////////////////////////////////FUNÇÕES
///////////////////////////TEMPERATURA E HUMIDADE
void Temperatura_Automatica() {
  float temp_sensor_dht22 = dht_sensor_dht22.readTemperature();
  float hum_sensor_dht22 = dht_sensor_dht22.readHumidity() - 12;
  //////////////////////////// MUITO CALOR
  DateTime now = rtc.now();
  if (temp_sensor_dht22 >= 27 )
  {
    pressao_negativa();
  } 
  //////////////////////////// TEMPERATURA a aquecer
  if (temp_sensor_dht22 >= 24.90  && temp_sensor_dht22 <= 26.90)
  {
    estado_normal();
    Blynk.virtualWrite(V7, "Ambiente a Aquecer");
  }

  if (temp_sensor_dht22 >= 21.90 && temp_sensor_dht22 <= 23.90 )
  {
    estado_normal();
   Blynk.virtualWrite(V7, "Ambiente a Ameno");
  }

  if (temp_sensor_dht22 >= 18.90 && temp_sensor_dht22 <= 20.90 )
  {
    estado_normal();
    Blynk.virtualWrite(V7, "Ambiente a Arrefecer");
  }

  //////////////////////////// MUITO FRIO
  if (temp_sensor_dht22 <= 17.90)
  {
    frio();
 //Blynk.virtualWrite(V7, "Ambiente Frio");
  } 
  
  int horas = now.hour();
  int minutos = now.minute();
  Blynk.virtualWrite(V5, hum_sensor_dht22);
  Blynk.virtualWrite(V6, temp_sensor_dht22);
  Blynk.virtualWrite(V10, lemon_kush_scrog);
  Blynk.virtualWrite(V11, "Arduino ", horas, "h", ":", "", minutos, "m");

}
