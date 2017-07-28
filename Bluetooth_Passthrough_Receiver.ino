/*
 * Controller for IR Receiver of the Bluetooth Passthrough
  Infrared Reading is taken from an example provided on https://learn.adafruit.com/ir-sensor?view=all#using-an-ir-sensor
 */
#include <SoftwareSerial.h>  


int bluetoothTx = 12;  // TX-O pin of bluetooth mate silver
int bluetoothRx = 13;  // RX-I pin of bluetooth mate silver
#define BLUETOOTH_PWR 11
#define BLUETOOTH_GND 10

SoftwareSerial bluetooth(bluetoothTx, bluetoothRx);

// Pin mapping for pro mini https://www.arduino.cc/en/Hacking/PinMapping168 for raw reading
#define IRpin_PIN PIND
#define IRpin 2
#define IR_PWR 4
#define IR_GND 3


// the maximum pulse we'll listen for - 65 milliseconds is a long time
#define MAXPULSE 65000
 
// what our timing resolution should be, larger is better
// as its more 'precise' - but too large and you wont get
// accurate timing
#define RESOLUTION 20
 
// we will store up to 200 pulse pairs
uint16_t pulses[200][2]; // pair is high and low pulse
uint8_t currentpulse = 0; // index for pulses we're storing 

void setup()
{
  // Setup IR receiver
  pinMode(IR_PWR, OUTPUT);
  pinMode(IR_GND, OUTPUT);
  digitalWrite(IR_GND,LOW);
  digitalWrite(IR_PWR, HIGH);

  // Setup bluetooth
  pinMode(BLUETOOTH_PWR, OUTPUT);
  pinMode(BLUETOOTH_GND, OUTPUT);
  digitalWrite(BLUETOOTH_GND,LOW);
  digitalWrite(BLUETOOTH_PWR, HIGH);
  
  bluetooth.begin(9600);  // The Bluetooth Mate 
}

void loop()
{ 
uint16_t highpulse, lowpulse; // temporary storage timing
  highpulse = lowpulse = 0; // start out with no pulse length
 
    while (IRpin_PIN & (1 << IRpin)) {
     // pin is still HIGH
     // count off another few microseconds
     highpulse++;
     delayMicroseconds(RESOLUTION);
 
     // If the pulse is too long, we 'timed out' - either nothing
     // was received or the code is finished, so print what
     // we've grabbed so far, and then reset
     if ((highpulse >= MAXPULSE) && (currentpulse != 0)) {
      sendBluetooth();
      currentpulse=0;
      return;
     }
  }
  // we didn't time out so lets stash the reading
  pulses[currentpulse][0] = highpulse;

  // same as above
  while (! (IRpin_PIN & _BV(IRpin))) {
     // pin is still LOW
     lowpulse++;
     delayMicroseconds(RESOLUTION);
     if ((lowpulse >= MAXPULSE) && (currentpulse != 0)) {
       sendBluetooth();
       currentpulse=0;
       return;
     }
  }
  pulses[currentpulse][1] = lowpulse;
 
  // we read one high-low pulse successfully, continue!
  currentpulse++;
}

// Send the entire array over bluetooth to be repeated by the other end.
void sendBluetooth(){
  for (uint8_t i = 0; i < currentpulse-1; i++) {
    bluetooth.print(pulses[i][1] * RESOLUTION, DEC);
    bluetooth.print(",");
    bluetooth.print(pulses[i+1][0] * RESOLUTION, DEC);
    bluetooth.println(",");
  }
  bluetooth.print(pulses[currentpulse-1][1] * RESOLUTION, DEC);
  bluetooth.print(",");
  bluetooth.println(0,DEC);
}
