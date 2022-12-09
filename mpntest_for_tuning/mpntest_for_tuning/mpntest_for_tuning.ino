//define MPU pins
#define LCD_RS 7
#define LCD_E 8
#define LCD_D4 9
#define LCD_D5 10
#define LCD_D6 11
#define LCD_D7 12

#define Uout_meas A0 
#define U_sw 15
#define start_sw 4
#define red_led 3
#define green_led 2

#define load_on A2
#define load_sens A3
#define LV_on A4  //LV=10V
#define HV_on A5  //HV=48V

//calibrating constants for 6.3v- and 5v- mode:
//"norm" - measured
const int v_overvoltage =1;

int MODE=6; //6.3V=6 5V=5
int Vmin=0; //temp for compare
int Vmax=0; //temp for compare
bool bad=true; //bad flag
bool dbg=true; //dbg mode flag (for developers)

//min6.0 max 6.5:
const int v6_max_LV_open =518;//norm=504/6.32
const int v6_min_LV_open =478;//norm=504/6.32
const int v6_max_LV_load =509;//norm=485/6.19 
const int v6_min_LV_load =470;//norm=485/6.19
//min6.0 max 6.6:
const int v6_max_HV_open =525;//norm=512/6.44
const int v6_min_HV_open =477;//norm=512/6.44
const int v6_max_HV_load =519;//norm=504/6.41
const int v6_min_HV_load =471;//norm=504/6.41

//min4.8 max5.3:
const int v5_max_LV_open =418;//norm=407/5.15
const int v5_min_LV_open =379;//norm=407/5.15
const int v5_max_LV_load =411;//norm=398/5.13
const int v5_min_LV_load =372;//norm=398/5.13
//min4.8 max5.4:
const int v5_max_HV_open =428;//norm=415/5.24
const int v5_min_HV_open =380;//norm=415/5.24
const int v5_max_HV_load =420;//norm=404/5.20
const int v5_min_HV_load =373;//norm=404/5.20




#include <LiquidCrystal.h>  // Лобавляем необходимую библиотеку для LCD
LiquidCrystal lcd(LCD_RS, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7); // (RS, E, DB4, DB5, DB6, DB7) //physical pin

void setup() {
  // put your setup code here, to run once:

  digitalWrite(LV_on, LOW); //set 48v off
  digitalWrite(HV_on, LOW); //ste 10v off

  analogReference(INTERNAL);
  Serial.begin(115200);

  pinMode(red_led, OUTPUT);
  pinMode(green_led, OUTPUT);
  pinMode(Uout_meas, INPUT);
  pinMode(U_sw, INPUT_PULLUP);
  pinMode(start_sw, INPUT_PULLUP);

  pinMode(load_on, OUTPUT);
  pinMode(load_sens, INPUT_PULLUP);
  pinMode(LV_on, OUTPUT);
  pinMode(HV_on, OUTPUT);


  lcd.begin(16, 2);           // Задаем размерность экрана
  lcd.setCursor(0, 0);        // Устанавливаем курсор в начало 1 строки
  lcd.print("test LCD");      // Выводим текст 
  lcd.setCursor(0, 1);        // Устанавливаем курсор в начало 2 строки
  lcd.print("mpntest");       // Выводим текст 

  Serial.println("");
  Serial.println("***********Start************");
  analogRead(Uout_meas);//first read for reduce error

  startscreen();


}

void loop() {
  // put your main code here, to run repeatedly:

  if(MODE==5 && digitalRead(U_sw)==LOW)//mode switched up=6.3V
  {
    MODE=6;
    startscreen();
  }

  if(MODE==6 && digitalRead(U_sw)==HIGH)//mode switched down=5V
  {
    MODE=5;
    startscreen();
  }


  //if(digitalRead(start_sw)==LOW) test(); //start btn pressed
  test();



}

void test()
{ 

  bad=false;
  
  //blink leds:
  digitalWrite(red_led, LOW);
  digitalWrite(green_led, LOW);
  delay(10);
  digitalWrite(red_led, HIGH);
  digitalWrite(green_led, HIGH);
  delay(10);
  digitalWrite(red_led, LOW);
  digitalWrite(green_led, LOW);

//*********test 1*******
  //10v open (LV):
  if (MODE==6) {Vmin=v6_min_LV_open; Vmax=v6_max_LV_open;}
  if (MODE==5) {Vmin=v5_min_LV_open; Vmax=v5_max_LV_open;}
  digitalWrite(HV_on, LOW); //set 48v off
  digitalWrite(LV_on, HIGH); //ste 10v on
  if(overvoltage_test()==true)return;
  digitalWrite(load_on, LOW); //set load off
  delay(200);  

  lcd.begin(16, 2);           // Задаем размерность экрана
  lcd.setCursor(0, 0);        // Устанавливаем курсор в начало 1 строки
  
  if((Vmin < analogRead(Uout_meas)) && (analogRead(Uout_meas) < Vmax)) lcd.print("10-OK"); else {lcd.print("10-BAD"); bad=true;};   
  Serial.println(""); 
  Serial.print("10v open Uout_meas: "); Serial.print(Vmin); Serial.print(" < "); Serial.print(analogRead(Uout_meas)); Serial.print(" < "); Serial.print(Vmax); Serial.print(" | load=");Serial.println(digitalRead(load_sens));

  if (dbg==true) delay(4000);
  
  //if(digitalRead(load_sens)==0) {message("LOAD FAIL!");return;}

  digitalWrite(LV_on, LOW); //set 10v off
  digitalWrite(HV_on, LOW); //set 48v on  
  digitalWrite(load_on, LOW); //set load off
  delay(300);

//*********test 2*******
  //10v load (LV):
  if (MODE==6) {Vmin=v6_min_LV_load; Vmax=v6_max_LV_load;}
  if (MODE==5) {Vmin=v5_min_LV_load; Vmax=v5_max_LV_load;}
  digitalWrite(HV_on, LOW); //set 48v off
  digitalWrite(LV_on, HIGH); //ste 10v on
  if(overvoltage_test()==true)return;
  digitalWrite(load_on, HIGH); //set load on
  delay(200);

  lcd.setCursor(8, 0);        // Устанавливаем курсор в середину 1 строки
    
  if((Vmin < analogRead(Uout_meas)) && (analogRead(Uout_meas) < Vmax)) lcd.print("10L-OK"); else {lcd.print("10-BAD"); bad=true;};   
  Serial.print("10v load Uout_meas: "); Serial.print(Vmin); Serial.print(" < "); Serial.print(analogRead(Uout_meas)); Serial.print(" < "); Serial.print(Vmax); Serial.print(" | load=");Serial.println(digitalRead(load_sens)); 

  if (dbg==true) delay(4000);

  //if(digitalRead(load_sens)==1) {message("LOAD FAIL!");return;}
  
  digitalWrite(LV_on, LOW); //set 10v off
  digitalWrite(HV_on, LOW); //set 48v on 
  digitalWrite(load_on, LOW); //set load off
  delay(300);

//*********test 3*******
  //48v open (HV):
  if (MODE==6) {Vmin=v6_min_HV_open; Vmax=v6_max_HV_open;}
  if (MODE==5) {Vmin=v5_min_HV_open; Vmax=v5_max_HV_open;}  
  digitalWrite(LV_on, LOW); //set 10v off
  digitalWrite(HV_on, HIGH); //set 48v on
  if(overvoltage_test()==true)return;
  digitalWrite(load_on, LOW); //set load off
  delay(200);  

  lcd.setCursor(0, 2);        // Устанавливаем курсор в начало 2 строки

  if((Vmin < analogRead(Uout_meas)) && (analogRead(Uout_meas) < Vmax)) lcd.print("48-OK"); else {lcd.print("48-BAD"); bad=true;};   
  Serial.print("48v open Uout_meas: "); Serial.print(""); Serial.print(Vmin); Serial.print(" < "); Serial.print(analogRead(Uout_meas)); Serial.print(" < "); Serial.print(Vmax); Serial.print(" | load=");Serial.println(digitalRead(load_sens)); 

  if (dbg==true) delay(4000);

  //if(digitalRead(load_sens)==0) {message("LOAD FAIL!");return;}
  
  digitalWrite(LV_on, LOW); //set 10v off
  digitalWrite(HV_on, LOW); //set 48v on    
  digitalWrite(load_on, LOW); //set load off
  delay(300);

//*********test 4*******
  //48v load (HV):
  if (MODE==6) {Vmin=v6_min_HV_load; Vmax=v6_max_HV_load;}
  if (MODE==5) {Vmin=v5_min_HV_load; Vmax=v5_max_HV_load;}  
  digitalWrite(LV_on, LOW); //set 10v off
  digitalWrite(HV_on, HIGH); //set 48v on
  if(overvoltage_test()==true)return;
  digitalWrite(load_on, HIGH); //set load on
  delay(200);  

  lcd.setCursor(8, 2);        // Устанавливаем курсор в середину 2 строки

  if((Vmin < analogRead(Uout_meas)) && (analogRead(Uout_meas) < Vmax)) lcd.print("48L-OK"); else {lcd.print("48-BAD"); bad=true;};  
  Serial.print("48v load Uout_meas: "); Serial.print(""); Serial.print(Vmin); Serial.print(" < "); Serial.print(analogRead(Uout_meas)); Serial.print(" < "); Serial.print(Vmax); Serial.print(" | load=");Serial.println(digitalRead(load_sens)); 

  if (dbg==true) delay(4000);

  //if(digitalRead(load_sens)==1) {message("LOAD FAIL!");return;}
  
  digitalWrite(LV_on, LOW); //set 10v off
  digitalWrite(HV_on, LOW); //set 48v on  
  digitalWrite(load_on, LOW); //set load off
  delay(300);




  if (bad==true) {    //"not ok" led 
    digitalWrite(red_led, HIGH);
    digitalWrite(green_led, LOW);
  }
  if (bad==false) {   //"ok" led
    digitalWrite(red_led, LOW);
    digitalWrite(green_led, HIGH);
  }  

  
}


void startscreen()
{
  if(digitalRead(U_sw)==LOW) //switch up=6.3V
  {
  lcd.begin(16, 2);           // Задаем размерность экрана
  lcd.setCursor(0, 0);        // Устанавливаем курсор в начало 1 строки
  lcd.print("Connect MPN 6.3V");      // Выводим текст 
  lcd.setCursor(0, 1);        // Устанавливаем курсор в начало 2 строки
  lcd.print("Press Start");       // Выводим текст 
  }

  if(digitalRead(U_sw)==HIGH) //switch down=5V
  {
  lcd.begin(16, 2);                // Задаем размерность экрана
  lcd.setCursor(0, 0);            // Устанавливаем курсор в начало 1 строки
  lcd.print("Connect MPN 5V");    // Выводим текст 
  lcd.setCursor(0, 1);            // Устанавливаем курсор в начало 2 строки
  lcd.print("Press Start");       // Выводим текст 
  }
}

void message(String s)
{
  lcd.begin(16, 2);           // Задаем размерность экрана
  lcd.setCursor(0, 0);        // Устанавливаем курсор в начало 1 строки
  lcd.print(s);               // Выводим текст 
}

bool overvoltage_test()
{
  if(analogRead(Uout_meas)>v6_max_HV_open)
  {
    digitalWrite(LV_on, LOW); //set 10v off
    digitalWrite(HV_on, LOW); //set 48v on  
    message("OVERVOLTAGE !!!");
    return true;
  }
  else return false;
}

   
