#include<SoftwareSerial.h>
SoftwareSerial SUART(4, 5); //SRX=Dpin-D2; STX=DPin-D1

void setup() {
  Serial.begin(115200); //enable Serial Monitor
  SUART.begin(9600); //enable SUART Port
}

void loop() {
  //--from NodeMCU to Arduino via SUART port--
  SUART.print("Test");
  SUART.println();

  delay(1000);

  
}
