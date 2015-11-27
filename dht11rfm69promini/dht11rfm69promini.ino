/*
 * Wireless node that sends temperature and humidity every SENDING_PERIOD milliseconds.
 * 
 * It uses lowpowerlab.com library to manage the RFM69CW chip.
 * Using also DHT11 library from official arduino.
 * 
 * Tested in a Arduino Mini, ATmega328(5V, 16 MHz) using a USB to serial device.
*/


//RFM69  --------------------------------------------------------------------------------------------------
#include <RFM69.h>
#define NODEID        2    //unique for each node on same network
#define NETWORKID     101  //the same on all nodes that talk to each other
#define GATEWAYID     1
//Match frequency to the hardware version of the radio on your Moteino (uncomment one):
//#define FREQUENCY   RF69_433MHZ
#define FREQUENCY   RF69_868MHZ
//#define FREQUENCY     RF69_915MHZ
#define ENCRYPTKEY    "xxxxxxxxxxxxxxxx" //exactly the same 16 characters/bytes on all nodes!
//#define IS_RFM69HW    //uncomment only for RFM69HW! Leave out if you have RFM69W!
#define ACK_TIME      30 // max # of ms to wait for an ack
#define LED           9  // Moteinos have LEDs on D9
#define SERIAL_BAUD   9600  //must be 9600 for GPS, use whatever if no GPS

#define SENDING_PERIOD 2000

typedef struct {    
  int           nodeID;     //node ID (1xx, 2xx, 3xx);  1xx = basement, 2xx = main floor, 3xx = outside
  int     deviceID;   //sensor ID (2, 3, 4, 5)
  unsigned long var1_usl;     //uptime in ms
  float         var2_float;     //sensor data?
  float     var3_float;   //battery condition?
} Payload;
Payload theData;

char buff[20];
byte sendSize=0;
boolean requestACK = false;
RFM69 radio;

//end RFM69 ------------------------------------------



//device DHT11 Temperature/Humidity
#include <dht11.h>
#define DHT11PIN 3     // what pin we're connected to
dht11 DHT11;


//round robbin
int dev2_trans_period = 7000;
long dev2_last_period = -1;
int dev3_trans_period = 5000;
long dev3_last_period = -1;
int dev4_trans_period = 9000;
long dev4_last_period = -1;
int dev5_trans_period = 11000;
long dev5_last_period = -1;





  // 3 = temperature
  const unsigned long dev4_period = 5;  //700000     //send data every X seconds
  unsigned long dev4_period_time; //seconds since last period


  
  
  // 4 = temperature_F/humidity
  // 5 = presence sensor PIR



void setup()
{
  Serial.begin(SERIAL_BAUD);  //Begin serial communcation
  
  //RFM69-------------------------------------------
  
  radio.initialize(FREQUENCY,NODEID,NETWORKID);
  #ifdef IS_RFM69HW
    radio.setHighPower(); //uncomment only for RFM69HW!
  #endif
  radio.encrypt(ENCRYPTKEY);
  char buff[50];
  sprintf(buff, "\nTransmitting at %d Mhz...", FREQUENCY==RF69_433MHZ ? 433 : FREQUENCY==RF69_868MHZ ? 868 : 915);
  Serial.println(buff);
  theData.nodeID = 1;  //this node id should be the same for all devices in this node
  

  
  //time:
  // dev4 is temperature_F/humidity
  dev4_period_time = millis();  //seconds since last period
  
  // 5 = presence sensor PIR
  
}
//---------------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------------
void loop()
{
  
  //---- this is what data looks like -----
  /*
    theData.nodeID = 40;
  theData.sensorID = xx;  
                                      2 = lights sensor var1 = 0-1024, higher means more light
                                      3 = ultrasonic  var1 = inches
                                      4 = temperature_F/humidity  var1= degree F, var2=% humidity
                                      5 = presence sensor var: = 1 = presence, 0=absence
  theData.var1_usl = millis();
        theData.var2_float = ;
        theData.var3_float = ;
  */


  
  
  //---------------- start devices ------------------------------------------------
  
  
  delay(SENDING_PERIOD); 

  int chk = DHT11.read(DHT11PIN);

  Serial.print("Read sensor: ");
  switch (chk)
  {
    case DHTLIB_OK: 
    Serial.println("OK"); 
    break;
    case DHTLIB_ERROR_CHECKSUM: 
    Serial.println("Checksum error"); 
    break;
    case DHTLIB_ERROR_TIMEOUT: 
    Serial.println("Time out error"); 
    break;
    default: 
    Serial.println("Unknown error"); 
    break;
  }

  Serial.print("Humidity (%): ");
  Serial.println((float)DHT11.humidity, 2);
  Serial.print("Temperature (*C): ");
  Serial.println((float)DHT11.temperature, 2);
  
  float h = (float)DHT11.humidity;
  float t = (float)DHT11.temperature;
  float temp_F;

  if (isnan(t) || isnan(h)) {
    Serial.println("Failed to read from DHT");
  } 

  //send data
  theData.deviceID = 4;
  theData.var1_usl = millis();
  theData.var2_float = t;
  theData.var3_float = h;
  radio.sendWithRetry(GATEWAYID, (const void*)(&theData), sizeof(theData));

  
  
}//end loop


