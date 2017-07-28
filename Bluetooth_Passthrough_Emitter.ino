/*
 * Controller for the IR Emitter of the Bluetooth Passthrough
 * Receives delay timings as an array from the receiver, and blinks hte LED at 38kHz
 * Based off of a tutorial at  http://www.ladyada.net/learn/sensors/ir.html
 */

#include <SoftwareSerial.h>  

int bluetoothTx = 12;  // TX-O pin of bluetooth mate silver
int bluetoothRx = 13;  // RX-I pin of bluetooth mate silver
#define BLUETOOTH_PWR 11
#define BLUETOOTH_GND 10

SoftwareSerial bluetooth(bluetoothTx, bluetoothRx);


#define IRledPin      3    
#define LED_GND       2
 
void setup()   {                
  // initialize the IR LED
  pinMode(IRledPin, OUTPUT);      
  pinMode(LED_GND, OUTPUT);

  digitalWrite(LED_GND, LOW);
  
  // Initialize bluetooth pins
  pinMode(BLUETOOTH_PWR, OUTPUT);
  pinMode(BLUETOOTH_GND, OUTPUT);
  digitalWrite(BLUETOOTH_GND,LOW);
  digitalWrite(BLUETOOTH_PWR, HIGH);

  bluetooth.begin(9600);
  Serial.begin(9600);
}
 
void loop()                     
{
  uint16_t numbers[400]; // Hold the read values, is currently 2x the dimension of the array on receiver
  int currNumber = 0;
  if(bluetooth.available()){
    Serial.println("Received New Code");
    while(bluetooth.available()){
      numbers[currNumber] = bluetooth.parseInt();
      Serial.println(numbers[currNumber]);
      currNumber++;
    }

    for(int i = 0; i < currNumber - 1; i+=2){
      pulseIR(numbers[i]);
      delayMicroseconds(numbers[i+1]);
    }
  }
}
 
// This procedure sends a 38KHz pulse to the IRledPin 
// for a certain # of microseconds. We'll use this whenever we need to send codes
void pulseIR(long microsecs) {
  // we'll count down from the number of microseconds we are told to wait
 
  cli();  // this turns off any background interrupts
 
  while (microsecs > 0) {
    // 38 kHz is about 13 microseconds high and 13 microseconds low
   digitalWrite(IRledPin, HIGH);  // this takes about 3 microseconds to happen
   delayMicroseconds(9);         // hang out for 10 microseconds, you can also change this to 9 if its not working
   digitalWrite(IRledPin, LOW);   // this also takes about 3 microseconds
   delayMicroseconds(9);         // hang out for 10 microseconds, you can also change this to 9 if its not working
 
   // so 26 microseconds altogether
   microsecs -= 26;
  }

  sei();  // this turns them back on
}
