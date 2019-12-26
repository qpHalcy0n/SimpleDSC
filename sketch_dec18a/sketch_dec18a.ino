#include "digitalWriteFast.h"
#include <stdlib.h>

const int AZ_RES = 10000;
const int ALT_RES = 10000;
const int HALF_RES = 5000;
const int ALT_CH_A_PIN = 3;
const int ALT_CH_B_PIN = 2;
const int AZ_CH_A_PIN = 18;
const int AZ_CH_B_PIN = 19;

volatile int AZ_POS = AZ_RES / 2;
volatile int ALT_POS = ALT_RES / 2;
volatile bool AZ_CH_A_SET;
volatile bool AZ_CH_B_SET;
volatile bool ALT_CH_A_SET;
volatile bool ALT_CH_B_SET;

volatile bool IS_UPDATED = LOW;


int correct_sign(int val, int resolution)
{
  int res = abs(resolution); 
  val = val % res;
  if(val >= 0)
  {
    if(val <= (HALF_RES - 1))
      return val;

    else
      return -HALF_RES - (HALF_RES - val);
  }

  else
  {
    if(abs(val) <= HALF_RES)
      return val;
    else 
      return HALF_RES + (HALF_RES + val);
  }
}

void AltAISR()
{
  ALT_CH_A_SET = digitalReadFast(ALT_CH_A_PIN) == HIGH;
  ALT_CH_B_SET = digitalReadFast(ALT_CH_B_PIN);

  ALT_POS += (ALT_CH_A_SET != ALT_CH_B_SET) ? +1 : -1;
  IS_UPDATED = HIGH;

  ALT_POS = correct_sign(ALT_POS, ALT_RES);
}

void AltBISR()
{
  ALT_CH_A_SET = digitalReadFast(ALT_CH_A_PIN);
  ALT_CH_B_SET = digitalReadFast(ALT_CH_B_PIN) == HIGH;

  ALT_POS += (ALT_CH_A_SET == ALT_CH_B_SET) ? +1 : -1;
  IS_UPDATED = HIGH;

  ALT_POS = correct_sign(ALT_POS, ALT_RES);
}

void AzAISR()
{
  AZ_CH_A_SET = digitalReadFast(AZ_CH_A_PIN) == HIGH;
  AZ_CH_B_SET = digitalReadFast(AZ_CH_B_PIN);

  AZ_POS += (AZ_CH_A_SET != AZ_CH_B_SET) ? +1 : -1;
  IS_UPDATED = HIGH;

  AZ_POS = correct_sign(AZ_POS, AZ_RES);
}

void AzBISR()
{
  AZ_CH_A_SET = digitalReadFast(AZ_CH_A_PIN);
  AZ_CH_B_SET = digitalReadFast(AZ_CH_B_PIN) == HIGH;

  AZ_POS += (AZ_CH_A_SET == AZ_CH_B_SET) ? +1 : -1;
  IS_UPDATED = HIGH;

  AZ_POS = correct_sign(AZ_POS, AZ_RES);
}

void setup() 
{
  // put your setup code here, to run once:
  Serial3.begin(9600);
  Serial.begin(9600);
  Serial.println("Begin Simple DSC");

  pinMode(ALT_CH_B_PIN, INPUT_PULLUP);
  pinMode(ALT_CH_A_PIN, INPUT_PULLUP);
  pinMode(AZ_CH_A_PIN, INPUT_PULLUP);
  pinMode(AZ_CH_B_PIN, INPUT_PULLUP);
  
  attachInterrupt(digitalPinToInterrupt(ALT_CH_B_PIN), AltBISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ALT_CH_A_PIN), AltAISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(AZ_CH_B_PIN), AzBISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(AZ_CH_A_PIN), AzAISR, CHANGE); 
}

void loop() 
{
  // put your main code here, to run repeatedly:
  int absAltCount = abs(ALT_POS);
  int absAzCount = abs(AZ_POS);

  String response;
  int nBytes = 0;
  nBytes = Serial3.available();
  if(nBytes > 0)
  {   
      char c = Serial3.read();

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

        Serial3.print(response);
        response = "";
      }
  }
}
