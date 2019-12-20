#include <SoftwareSerial.h>
#include <stdlib.h>

SoftwareSerial SkySafari(10, 11);


const int AZ_RES = 10000;
const int ALT_RES = 10000;
const int ALT_CH_A_PIN = 3;
const int ALT_CH_B_PIN = 2;
const int AZ_CH_A_PIN = 18;
const int AZ_CH_B_PIN = 19;

int AZ_POS = AZ_RES / 2;
int ALT_POS = ALT_RES / 2;
char AZ_STATE = 0;
char ALT_STATE = 0;

int correct_sign(int val, int res)
{
  val = val % res;
  if(val <= 0)
  {
    if(val <= ((res / 2) - 1))
      return val;
    else
      return -(res / 2) - ((res / 2) - val);
  }

  else
  {
    if(abs(val) <= (res / 2))
      return val;
    else
      return (res / 2) + ((res / 2) + val);
  }
}

void DecISR()
{
  char s = ALT_STATE & 3;
  if(digitalRead(ALT_CH_A_PIN))
    s = s | 4;
  if(digitalRead(ALT_CH_B_PIN))
    s = s | 8;

  if(s == 1 || s == 7 || s == 8 || s == 14)
    ALT_POS++;
  else if(s == 2 || s == 4 || s == 11 || s == 13)
    ALT_POS--;
  else if(s == 3 || s == 12)
    ALT_POS += 2;
  else if(s == 0 || s == 5 || s == 10 || s == 15)
    ALT_POS += 0;
  else
    ALT_POS -= 2;

  ALT_STATE = s >> 2;
  ALT_POS = correct_sign(ALT_POS, ALT_RES);
}

void AzISR()
{
  char s = AZ_STATE & 3;
  if(digitalRead(AZ_CH_A_PIN))
    s = s | 4;
  if(digitalRead(AZ_CH_B_PIN))
    s = s | 8;

  if(s == 1 || s == 7 || s == 8 || s == 14)
    AZ_POS++;
  else if(s == 2 || s == 4 || s == 11 || s == 13)
    AZ_POS--;
  else if(s == 3 || s == 12)
    AZ_POS += 2;
  else if(s == 0 || s == 5 || s == 10 || s == 15)
    AZ_POS += 0;
  else
    AZ_POS -= 2;

  AZ_STATE = s >> 2;
  AZ_POS = correct_sign(AZ_POS, AZ_RES);
}



void setup() 
{
  // put your setup code here, to run once:
  SkySafari.begin(9600);
  Serial.begin(9600);
  Serial.println("Begin Simple DSC");

  pinMode(ALT_CH_B_PIN, INPUT);
  pinMode(ALT_CH_A_PIN, INPUT);
  pinMode(AZ_CH_A_PIN, INPUT);
  pinMode(AZ_CH_B_PIN, INPUT);

  attachInterrupt(digitalPinToInterrupt(ALT_CH_B_PIN), DecISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ALT_CH_A_PIN), DecISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(AZ_CH_B_PIN), AzISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(AZ_CH_A_PIN), AzISR, CHANGE); 
}

void loop() 
{
  // put your main code here, to run repeatedly:
  int absAltCount = abs(ALT_POS);
  int absAzCount = abs(AZ_POS);

  String response;
  int nBytes = 0;
  nBytes = SkySafari.available();
  if(nBytes > 0)
  {   
    for(int i = 0; i < nBytes; i++)
    {
      char c = SkySafari.read();

      // Position requested
      if(c == 'Q')
      {
        
        if(AZ_POS >= 0)
          response.concat('+');
        else
          response.concat('-');

        if(absAzCount < 10000) response.concat('0');
        if(absAzCount < 1000) response.concat('0');
        if(absAzCount < 100) response.concat('0');
        if(absAzCount < 10) response.concat('0');
        response.concat(absAzCount);
        response.concat('\t');

        if(ALT_POS >= 0)
          response.concat('+');
        else
          response.concat('-');
        if(absAltCount < 10000) response.concat('0');
        if(absAltCount < 1000) response.concat('0');
        if(absAltCount < 100) response.concat('0');
        if(absAltCount < 10) response.concat('0');
        response.concat(absAltCount);
        response.concat('\r');

        SkySafari.print(response);

        response = "";
      }
    }
  }
}
