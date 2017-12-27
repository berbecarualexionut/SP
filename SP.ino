#include <LiquidCrystal.h>

LiquidCrystal lcd(0);
#include <Wire.h>

#define DS1307_I2C_ADDRESS 0x68
#include "SoftwareSerial.h";

int bluetoothTx = A1;

int bluetoothRx = A0;

SoftwareSerial bluetooth(bluetoothTx, bluetoothRx);
#include <SimpleDHT.h>

int pinDHT11 = 2;

SimpleDHT11 dht11;
volatile int sec_afis=1;

volatile boolean afis=true;

void setup() {
  // put your setup code here, to run once:

  bluetooth.begin(9600);

  digitalWrite(9,LOW);

  Wire.begin();

  lcd.begin(16,2);

  lcd.noCursor();

  lcd.clear();

  TIMSK1 = 0x01;

  TCCR1A = 0x00;

  TCNT1 = 0x0BDC;

  TCCR1B = 0x04;

  
}

void loop() {
 
  // put your main code here, to run repeatedly:
byte temp,hum;
byte second,minute,hour,dayOfWeek,dayOfMonth,month,year;
getDateDs1307(&second,&minute,&hour,&dayOfWeek,&dayOfMonth,&month,&year);
if(bluetooth.available()){
  char comanda=(char)bluetooth.read();
  switch(comanda){
    case'y':
    while(bluetooth.available()==0){delay(10);}
    comanda=(char)bluetooth.read();
    if(comanda=='+') year++;
    else if(comanda=='-')year--;
    setDateDs1307(second,minute,hour,dayOfWeek,dayOfMonth,month,year);
    break;
    case 'm':
    while(bluetooth.available()==0){delay(10);}
    comanda=(char)bluetooth.read();
    if(comanda=='+')month++;
    else if(comanda=='-')month--;
    setDateDs1307(second,minute,hour,dayOfWeek,dayOfMonth,month,year);
    break;
    case 'd':
    while(bluetooth.available()==0){delay(10);}
    comanda=(char)bluetooth.read();
    if(comanda=='+')dayOfMonth++;
    else if(comanda=='-')dayOfMonth--;
    setDateDs1307(second,minute,hour,dayOfWeek,dayOfMonth,month,year);
    break;
    case 'w':
    while(bluetooth.available()==0){delay(10);}
    comanda=(char)bluetooth.read();
    if(comanda=='+')dayOfWeek++;
    else if(comanda=='-')dayOfWeek--;
    setDateDs1307(second,minute,hour,dayOfWeek,dayOfMonth,month,year);
    break;
    case 'h':
    while(bluetooth.available()==0){delay(10);}
    comanda=(char)bluetooth.read();
    if(comanda=='+')hour++;
    else if(comanda=='-')hour--;
    setDateDs1307(second,minute,hour,dayOfWeek,dayOfMonth,month,year);
    break;
    case 't':
    while(bluetooth.available()==0){delay(10);}
    comanda=(char)bluetooth.read();
    if(comanda=='+')minute++;
    else if(comanda=='-') minute--;
    setDateDs1307(second,minute,hour,dayOfWeek,dayOfMonth,month,year);
    break;
  }}
lcd.setCursor(0,0);
switch(dayOfWeek){
  case 1:
  lcd.print("LUN");
  break;
  case 2:
  lcd.print("MAR");
  break;
  case 3:
  lcd.print("MIE");
  break;
  case 4:
  lcd.print("JOI");
  break;
  case 5:
  lcd.print("VIN");
  break;
  case 6:
  lcd.print("SAM");
  break;
  case 7:
  lcd.print("DUM");
  break;}
  lcd.print(" ");
  lcd.print(dayOfMonth,DEC);
  lcd.print(" ");
  switch(month) {
    case 1:
    lcd.print("ian");
    break;
    case 2:
    lcd.print("feb");
    break;          
    case 3:
    lcd.print("mar");
    break;          
    case 4:
    lcd.print("apr");
    break;          
    case 5:
    lcd.print("mai");
    break;          
    case 6:
    lcd.print("iun");
    break;          
    case 7:
    lcd.print("iul");
    break;          
    case 8:
    lcd.print("aug");
    break;          
    case 9:
    lcd.print("sep");
    break;          
    case 10:
    lcd.print("oct");
    break;          
    case 11:
    lcd.print("noi");
    break;          
    case 12:        
    lcd.print("dec");
    break;                 
}
lcd.print("20");
lcd.print(year,DEC);
lcd.setCursor(0,1);
if(hour<10)lcd.print(" ");
lcd.print(hour,DEC);
lcd.print(":");
if(minute<10)lcd.print("0");
lcd.print(minute,DEC);
lcd.print(":");
if(second<10)lcd.print("0");
lcd.print(second,DEC);
lcd.print(" ");



if(((sec_afis%10)==0)&&afis){
  byte chk=dht11.read(pinDHT11,&temp,&hum,NULL);
  lcd.setCursor(12,1);
  switch(chk){
    case 0:
    if(sec_afis==10){
      lcd.print(temp);
      lcd.print(char(223));
      lcd.print("C");
    }
    else{
      lcd.print(" ");
      lcd.print(hum);
      lcd.print("%");
      }
      break;
      default:
      lcd.print("Error");
      break;
     }
     afis=false;
}
}
ISR(TIMER1_OVF_vect){
  TCNT1=0x0BDC;
  sec_afis++;
  if(sec_afis==21)sec_afis=1;
 }

byte bcdToDec(byte val){return ((val/16*10)+(val%16));}
byte decToBcd(byte val){return ((val/10*16)+(val%10));}
void getDateDs1307(byte *second,byte *minute,byte *hour,byte *dayOfWeek,byte *dayOfMonth,byte *month,byte *year)
{
  Wire.beginTransmission(DS1307_I2C_ADDRESS);
  Wire.write(0);
  Wire.endTransmission();
  Wire.requestFrom(DS1307_I2C_ADDRESS,7);
  *second=bcdToDec(Wire.read()&0x7f);
  *minute=bcdToDec(Wire.read());
  *hour=bcdToDec(Wire.read()&0x3f);
  *dayOfWeek=bcdToDec(Wire.read());
  *dayOfMonth=bcdToDec(Wire.read());
  *month=bcdToDec(Wire.read());
  *year=bcdToDec(Wire.read());
}
void setDateDs1307(byte second,byte minute,byte hour,byte dayOfWeek,byte dayOfMonth,byte month,byte year){
  Wire.beginTransmission(DS1307_I2C_ADDRESS);
  Wire.write(byte(0));
  Wire.write(decToBcd(second));
  Wire.write(decToBcd(minute));
  Wire.write(decToBcd(hour));
  Wire.write(decToBcd(dayOfWeek));
  Wire.write(decToBcd(dayOfMonth));
  Wire.write(decToBcd(month));
  Wire.write(decToBcd(year));
  Wire.write(byte(0));
  Wire.endTransmission();
}

   
