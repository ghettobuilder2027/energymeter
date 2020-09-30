#include <TFT_eSPI.h>
#include <SPI.h>
#include <Button2.h>
#include <Time.h>
#include <PZEM004Tv30.h>



// Display
#ifndef TFT_DISPOFF
#define TFT_DISPOFF 0x28
#endif
#ifndef TFT_SLPIN
#define TFT_SLPIN   0x10
#endif
#define TFT_MOSI            19
#define TFT_SCLK            18
#define TFT_CS              5
#define TFT_DC              16
#define TFT_RST             23
#define TFT_BL              4   // Display backlight control pin
#define ADC_EN              14  //ADC_EN is the ADC detection enable port
#define ADC_PIN             34
#define BUTTON_1            35
#define BUTTON_2            0
TFT_eSPI tft = TFT_eSPI(135, 240); // Invoke custom library

PZEM004Tv30 pzem(&Serial2);

Button2 btn1(BUTTON_1);
Button2 btn2(BUTTON_2);

int new_now;
int time_now = now();
char buff[512];
int i = 0;


void button_init(){ 
    btn1.setPressedHandler([](Button2& b ) {
        Serial.println("bouton 1.. reset energy");
        setTime(0);
        time_now = 0;
        pzem.resetEnergy();
        tft.fillScreen(TFT_BLACK);
    });
    btn2.setPressedHandler([](Button2& b) {
        Serial.println("bouton 2"); 
    });
}

void button_loop(){
    btn1.loop();
    btn2.loop();
}

String formate (int t) {
  String retour;
  if (t<10) {
    retour = "0"+ String (t);
  }
  else {
    retour = String(t);
  }
  return retour;
}

void measure() {
    String d_voltage;
    String d_current;
    String d_power;
    String d_energy;
    String d_frequency;
    String d_pf;

    String temps;
    time_t t = now();   //
    temps = "Time passed "+formate(hour())+":"+formate(minute())+":"+formate(second());

    float voltage = pzem.voltage();
    if( !isnan(voltage) ){
        Serial.print("Voltage:    "); Serial.print(voltage); Serial.println("V");
        d_voltage = "Voltage:    " + String(voltage) + " V";
    
    } else {
        Serial.println("Error reading voltage");
    }

    float current = pzem.current();
    if( !isnan(current) ){
        Serial.print("Current:    "); Serial.print(current); Serial.println("A");
        d_current = "Current:    " + String(current) + " A";
    
    } else {
        Serial.println("Error reading current");
    }

    float power = pzem.power();
    if( !isnan(power) ){
        Serial.print("Power:      "); Serial.print(power); Serial.println("W");
        d_power = "Power:      " + String(power) + " W";
    
    } else {
        Serial.println("Error reading power");
    }

    float energy = pzem.energy();
    if( !isnan(energy) ){
        Serial.print("Energy:     "); Serial.print(energy,3); Serial.println("kWh");
        d_energy = "Energy:     " + String(energy) + " kWh";
    
    } else {
        Serial.println("Error reading energy");
    }

    float frequency = pzem.frequency();
    if( !isnan(frequency) ){
        Serial.print("Frequency:  "); Serial.print(frequency, 1); Serial.println("Hz");
        d_frequency = "Frequency:  " + String(frequency) + " Hz";
    
    } else {
        Serial.println("Error reading frequency");
    }

    float pf = pzem.pf();
    if( !isnan(pf) ){
        Serial.print("PF:  "); Serial.println(pf);
        d_pf = "Power Factor:  " + String(pf);
    } else {
        Serial.println("Error reading power factor");
    }
    tft.fillScreen(TFT_BLACK);
    tft.setTextDatum(TL_DATUM);
    tft.drawString("Ghetto Power Meter",0,0);
    tft.drawString(d_voltage, 0,18);
    tft.drawString(d_frequency, 0,36);
    tft.drawString(d_current, 0,60);
    tft.drawString(d_power,  0, 78);
    tft.drawString(d_energy, 0,96);
    tft.drawString(temps, 0,116);
    
}


void setup(){
  setTime(0);
  pzem.resetEnergy();

  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, 25, 26);
     

  // init TFT
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(2);
  tft.setTextColor(TFT_GREEN);
  tft.setCursor(0, 0);
  tft.setTextDatum(MC_DATUM);
  tft.setTextSize(2);
  if (TFT_BL > 0) {                           // TFT_BL has been set in the TFT_eSPI library in the User Setup file TTGO_T_Display.h
      pinMode(TFT_BL, OUTPUT);                // Set backlight pin to output mode
      digitalWrite(TFT_BL, TFT_BACKLIGHT_ON); // Turn backlight on. TFT_BACKLIGHT_ON has been set in the TFT_eSPI library in the User Setup file TTGO_T_Display.h
  }
  
  button_init();

  tft.setSwapBytes(true);
 
  tft.drawString("Power meter ", tft.width() / 2, tft.height() / 2 -30 );
  tft.drawString("by ", tft.width() / 2, tft.height() / 2 -10 );
  tft.drawString("Ghetto Builder ", tft.width() / 2, tft.height() / 2 + 20 );
  
  
  Serial.println("Serial succes");
  delay(700);
  tft.fillScreen(TFT_BLACK);
    
 
}

void loop(){
  
  button_loop();
  
  new_now = now();
  //Serial.println(new_now);
  if (new_now - time_now > 2) {
     //tft.fillScreen(TFT_BLACK);
     i++;
     Serial.println(i);
     time_now = new_now;
     measure();

  }
  
}
