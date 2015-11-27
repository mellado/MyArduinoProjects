/*

This sketch receives RFM wireless data and forwards it to serial interface.

TODO: Send the data to the serial interface

 
*/

/*
RFM69 Pinout:
    MOSI = 11
    MISO = 12
    SCK = 13
    SS = 10
*/


//general --------------------------------
#define SERIAL_BAUD   9600



//RFM69  ----------------------------------
#include <RFM69.h>
#define NODEID        1    //unique for each node on same network
#define NETWORKID     101  //the same on all nodes that talk to each other
#define FREQUENCY     RF69_868MHZ
#define ENCRYPTKEY    "xxxxxxxxxxxxxxxx" //exactly the same 16 characters/bytes on all nodes!
//#define IS_RFM69HW    //uncomment only for RFM69HW! Leave out if you have RFM69W!
#define ACK_TIME      30 // max # of ms to wait for an ack

RFM69 radio;
bool promiscuousMode = false; //set to 'true' to sniff all packets on the same network

typedef struct {		
  int                   nodeID; 
  int			              sensorID;
  unsigned long         var1_usl; 
  float                 var2_float; 
  float			            var3_float;	
} Payload;
Payload theData;


void setup() 
{  
  Serial.begin(9600); 

  //RFM69 ---------------------------
  bool init = radio.initialize(FREQUENCY,NODEID,NETWORKID);
  Serial.println(init);
  #ifdef IS_RFM69HW
    radio.setHighPower(); //uncomment only for RFM69HW!
  #endif
  radio.encrypt(ENCRYPTKEY);
  radio.promiscuous(promiscuousMode);
  char buff[50];
  sprintf(buff, "\nListening at %d Mhz...", FREQUENCY==RF69_433MHZ ? 433 : FREQUENCY==RF69_868MHZ ? 868 : 915);
  Serial.println(buff);

}  // end of setup

byte ackCount=0;


void loop() 
{
  
  if (radio.receiveDone())
  {
    //Serial.print('[');Serial.print(radio.SENDERID, DEC);Serial.print("] ");
    if (promiscuousMode)
    {
      Serial.print("to [");Serial.print(radio.TARGETID, DEC);Serial.print("] ");
    }
	

    if (radio.DATALEN != sizeof(Payload))
      Serial.println("Invalid payload received, not matching Payload struct!");
    else
    {
      theData = *(Payload*)radio.DATA; //assume radio.DATA actually contains our struct and not something else
      Serial.print("Sensor ID: ");      
      Serial.print(theData.sensorID);
      Serial.print(", millis: ");
      Serial.print(theData.var1_usl);
      Serial.print(", temperature (*C): ");
      Serial.print(theData.var2_float);
      Serial.print(", humidity(%):");
      Serial.print(theData.var3_float);
      
      //printFloat(theData.var2_float, 5); Serial.print(", "); printFloat(theData.var3_float, 5);
      
      Serial.print(", RSSI= ");
      Serial.println(radio.RSSI);
      
      
    }
	
	
    if (radio.ACK_REQUESTED)
    {
      byte theNodeID = radio.SENDERID;
      radio.sendACK();
      // Serial.println(" - ACK sent.");

    }//end if radio.ACK_REQESTED
    
    
    
    
  } //end if radio.receive
  
}//end loop
