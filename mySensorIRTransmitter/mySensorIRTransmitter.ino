// Example sketch showing how to control ir devices
// An IR LED must be connected to Arduino PWM pin 3.
// An optional ir receiver can be connected to PWM pin 8. 
// All receied ir signals will be sent to gateway device stored in VAR_1.
// When binary light on is clicked - sketch will send volume up ir command
// When binary light off is clicked - sketch will send volume down ir command

#include <MySensor.h>
#include <SPI.h>
#include <IRLib.h>

int RECV_PIN = 8;

//Led infrarouge = 3
const int LED=6; //declaration constante de broche  pour la led d'affichage


#define CHILD_1  5  // childId
unsigned int SAMSUNG_PWR[68]={4600,4350,700,1550,650,1550,650,1600,650,450,650,450,650,450,650,450,700,400,700,1550,650,1550,650,1600,650,450,650,450,650,450,700,450,650,450,650,450,650,1550,700,450,650,450,650,450,650,450,650,450,700,400,650,1600,650,450,650,1550,650,1600,650,1550,650,1550,700,1550,650,1550,650};


//IRsend irsend;
IRsendRaw irsendraw;
IRrecv irrecv(RECV_PIN);
IRdecode decoder;
//decode_results results;

unsigned int Buffer[RAWBUF];
MySensor gw;
MyMessage msg(CHILD_1, V_VAR1);

void setup()  
{  
  pinMode(LED, OUTPUT); //met la broche en sortie 
  digitalWrite(LED,HIGH); // met la broche au niveau haut (5V) – allume la LED
  delay(500);
  digitalWrite(LED,LOW); // met la broche au niveau haut (5V) – allume la LED
  irrecv.enableIRIn(); // Start the ir receiver
  decoder.UseExtnBuf(Buffer);
  gw.begin(incomingMessage);
  Serial.begin(57600);

  // Send the sketch version information to the gateway and Controller
  gw.sendSketchInfo("IR Sensor", "1.0");

  // Register a sensors to gw. Use binary light for test purposes.
  gw.present(CHILD_1, S_LIGHT);
}


void loop() 
{
  gw.process();
  if (irrecv.GetResults(&decoder)) {
    irrecv.resume(); 
    decoder.decode();
    decoder.DumpResults();
        
    char buffer[10];
    sprintf(buffer, "%08lx", decoder.value);
    // Send ir result to gw
    gw.send(msg.set(buffer));
  }
}



void incomingMessage(const MyMessage &message) {
  // We only expect one type of message from controller. But we better check anyway.
  if (message.type==V_LIGHT) {
     digitalWrite(LED,HIGH); // met la broche au niveau haut (5V) – allume la LED

     int incomingRelayStatus = message.getInt();
     if (incomingRelayStatus == 1) {
      //irsend.send(NEC, 0xCF20D, 32); // On Vidéoprojecteur
      setVPOn();
     }
     if (incomingRelayStatus == 0) {
      //irsend.send(NEC, 0xC728D, 32); // Off Vidéoprojecteur
      setVPOff();
     }
     if (incomingRelayStatus == 3) {
      //irsend.send(NEC, 0xC728D, 32); // Off Vidéoprojecteur
      setMuteOn();
     }
     if (incomingRelayStatus == 4) {
      //irsend.send(NEC, 0xC728D, 32); // Off Vidéoprojecteur
      setMuteOff();
     }
     if (incomingRelayStatus == 2) {
        Serial.println("Send IR SAMSUNG_PWR");
        irsendraw.send(SAMSUNG_PWR,68,38); // On/Off TV
     }

     digitalWrite(LED,LOW); // met la broche au niveau bas (0V) – éteint la LED
     
     // Start receiving ir again...
    irrecv.enableIRIn(); 
  }
}
    
// Dumps out the decode_results structure.
// Call this after IRrecv::decode()
// void * to work around compiler issue
//void dump(void *v) {
//  decode_results *results = (decode_results *)v
/*void dump(decode_results *results) {
  int count = results->rawlen;
  if (results->decode_type == UNKNOWN) {
    Serial.print("Unknown encoding: ");
  } 
  else if (results->decode_type == NEC) {
    Serial.print("Decoded NEC: ");
  } 
  else if (results->decode_type == SONY) {
    Serial.print("Decoded SONY: ");
  } 
  else if (results->decode_type == RC5) {
    Serial.print("Decoded RC5: ");
  } 
  else if (results->decode_type == RC6) {
    Serial.print("Decoded RC6: ");
  }
  else if (results->decode_type == PANASONIC) {	
    Serial.print("Decoded PANASONIC - Address: ");
    Serial.print(results->panasonicAddress,HEX);
    Serial.print(" Value: ");
  }
  else if (results->decode_type == JVC) {
     Serial.print("Decoded JVC: ");
  }
  Serial.print(results->value, HEX);
  Serial.print(" (");
  Serial.print(results->bits, DEC);
  Serial.println(" bits)");
  Serial.print("Raw (");
  Serial.print(count, DEC);
  Serial.print("): ");

  for (int i = 0; i < count; i++) {
    if ((i % 2) == 1) {
      Serial.print(results->rawbuf[i]*USECPERTICK, DEC);
    } 
    else {
      Serial.print(-(int)results->rawbuf[i]*USECPERTICK, DEC);
    }
    Serial.print(" ");
  }
  Serial.println("");
}
*/


void setVPOn() {
  Serial.write("\r*pow=on#\r");
  Serial.flush();
}

void setMuteOn() {
  Serial.write("\r*mute=on#\r");
  Serial.flush();
}

void setMuteOff() {
  Serial.write("\r*mute=off#\r");
  Serial.flush();
}

void setVPOff() {
  Serial.write("\r*pow=off#\r");
  Serial.flush();
}

