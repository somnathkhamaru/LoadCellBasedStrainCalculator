#include <Arduino.h>
#include<stdio.h>
#include "HX711.h"
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,20,4);  // pin D2-SDA and pin D1-SCL

// HX711 circuit wiring for 1st load cell
const int LOADCELL_DOUT_PIN_1 = 12; // Pin D6
const int LOADCELL_SCK_PIN_1 = 13; // Pin D7

// HX711 circuit wiring for 2nd load cell
const int LOADCELL_DOUT_PIN_2 = 0; //Pin D3
const int LOADCELL_SCK_PIN_2 = 2; // Pin D4

// HX711 circuit wiring for 3rd load cell
const int LOADCELL_DOUT_PIN_3 = 14; //Pin D5
const int LOADCELL_SCK_PIN_3 = 15; //Pin D8
double calibrationFactor = -219.58;

HX711 scale1;
HX711 scale2;
HX711 scale3;

void setup() {
  Serial.begin(115200);
  lcd.init();                      // initialize the lcd 
  lcd.backlight();
  lcd.clear();
  init7X11(scale1,LOADCELL_DOUT_PIN_1 ,LOADCELL_SCK_PIN_1 ,1);
 init7X11(scale2, LOADCELL_DOUT_PIN_2 ,LOADCELL_SCK_PIN_2 ,2);
  init7X11(scale3, LOADCELL_DOUT_PIN_3 ,LOADCELL_SCK_PIN_3,3);
}
void init7X11(HX711& scale, int LOADCELL_DOUT_PIN, int LOADCELL_SCK_PIN,int no)
{
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  Serial.print("Before setting up the scale No :");
  Serial.println(no);
  Serial.print("read: \t\t");
  Serial.println(scale.read());      // print a raw reading from the ADC
  Serial.print("read average: \t\t");
  Serial.println(scale.read_average(20));   // print the average of 20 readings from the ADC
  Serial.print("get value: \t\t");
  Serial.println(scale.get_value(5));   // print the average of 5 readings from the ADC minus the tare weight (not set yet)  
  scale.set_scale(calibrationFactor);          
  scale.tare();               // reset the scale to 0
  Serial.print("After setting up the scale No :");
  Serial.println(no);
  Serial.print("read:");
  Serial.println(scale.read());                 // print a raw reading from the ADC
  Serial.print("read average: ");
  Serial.println(scale.read_average(20));       // print the average of 20 readings from the ADC
  Serial.print("get value: \t\t");
  Serial.println(scale.get_value(5));   // print the average of 5 readings from the ADC minus the tare weight,
}
double calculateStrain(double avgps)
{
  Serial.print("mv/V:");
  double val =(avgps/8388608)*(3.90625);
  Serial.println(val,5);
  Serial.print("uv/V:");
  double uv=(val*1000);
  Serial.println(uv);
  double strain = (uv*2)/pow(10,6);
  Serial.print("Strain actual : ");
  Serial.print(strain,8);
  Serial.println(" strain");
  return strain;
}
void printStrainValInLCD(double strain , int no)
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("LC ");
  lcd.print(no);
  lcd.print(" Strain =");
  lcd.setCursor(0,1);
  lcd.print(strain,8);
}
void getRawScaleValue(HX711& scale,int no)
{
  long avg = scale.read_average(20);
  double avgps = abs(avg); 
  double strain = calculateStrain(avgps);
  printStrainValInLCD(strain,no);
  scale.power_down();             // put the ADC in sleep mode
  delay(5000);
  scale.power_up();
}
void loop() {
  getRawScaleValue(scale1,1);
  getRawScaleValue(scale2,2);
  getRawScaleValue(scale3,3);
}
