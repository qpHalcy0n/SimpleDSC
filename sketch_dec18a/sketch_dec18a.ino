#include <SoftwareSerial.h>
#include <stdlib.h>

SoftwareSerial SkySafari(10, 11);

int altCount = 0;
int azCount = 0;


void setup() 
{
  // put your setup code here, to run once:
  SkySafari.begin(9600);
  Serial.begin(9600);
  Serial.println("Cock");
}

void loop() 
{
  // put your main code here, to run repeatedly:
  altCount++;
  if(altCount >= 5000)
    altCount = -4999;

  int absAltCount = abs(altCount);
  int absAzCount = abs(azCount);

  String response;
  int nBytes = 0;
  nBytes = SkySafari.available();
  if(nBytes > 0)
  {   
    for(int i = 0; i < nBytes; i++)
    {
      char c = SkySafari.read();
//      Serial.print(c);

      // Position requested
      if(c == 'Q')
      {
        
        if(azCount >= 0)
          response.concat('+');
        else
          response.concat('-');

        if(absAzCount < 10000) response.concat('0');
        if(absAzCount < 1000) response.concat('0');
        if(absAzCount < 100) response.concat('0');
        if(absAzCount < 10) response.concat('0');
        response.concat(absAzCount);
        response.concat('\t');

        if(altCount >= 0)
          response.concat('+');
        else
          response.concat('-');
        if(absAltCount < 10000) response.concat('0');
        if(absAltCount < 1000) response.concat('0');
        if(absAltCount < 100) response.concat('0');
        if(absAltCount < 10) response.concat('0');
        response.concat(absAltCount);
        response.concat('\r');
        
        
        Serial.print(response + "\n");
        SkySafari.print(response);

        response = "";
      }
    }
  }

  delay(10);
}
