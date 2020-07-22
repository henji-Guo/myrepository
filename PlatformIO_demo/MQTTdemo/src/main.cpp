/**
 *  物联网项目
 *  MQTT
 *  LED
 *  APP 
**/
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Ethernet.h>
#include <SPI.h>
#include <MFRC522.h>


#define BLINKER_WIFI
#define BLINKER_MIOT_LIGHT
//#include <Blinker.h>
#define SS_PIN 5
#define RST_PIN 4



#define LED_1 16 //GPIO16
#define LED_2 2 //GPIO02 

WiFiClient espClient;
PubSubClient client(espClient);
String data;
char buffer[64];

MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
MFRC522::MIFARE_Key key; 
// Init array that will store new NUID 
byte nuidPICC[4];


void LED_1_pwd()
{
  //analogWrite();


}
void LED_2_pwd()
{
  
}

void callback(char *topics, byte *payload, unsigned int length)
{
  String topic;
  topic = topics;
  if (topic == "/LED_ON")
  {
    digitalWrite(LED_2,LOW);
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    for (int i = 0; i < length; i++)
    {
      Serial.print((char)payload[i]);
    }
    Serial.println();
    
  }
  else if (topic == "/LED_OFF")
  {
    digitalWrite(LED_2,HIGH);
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    for (int i = 0; i < length; i++)
    {
      Serial.print((char)payload[i]);
    }
    Serial.println();
    
  }
}
void wifi_connect()
{
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin("360WiFi-3F8418", "wzlwggw001001");
  while (!WiFi.isConnected())
  {
    Serial.println("wifi is connecting ...");
    delay(1000);
  }
  Serial.println("wifi was connected");
  Serial.println(WiFi.localIP());
}

void mqtt_connect()
{
  client.setServer("47.95.249.141", 1883);
  client.setKeepAlive(120);
  client.setCallback(callback);
  while (!client.connect("MQTT_node", "NodeMCU", "NodeMCU"))
  {
    Serial.println("MQTT is connecting ...");
    delay(1000);
  }
  Serial.println("MQTT is connected");
  client.subscribe("/topic");
  client.publish("/topic", "this is a test ....");
}

/**
 * Helper routine to dump a byte array as hex values to Serial. 
 */
void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

/**
 * Helper routine to dump a byte array as dec values to Serial.
 */
void printDec(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);
  }
}

void test()
{
  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if ( ! rfid.PICC_IsNewCardPresent())
    return;

  // Verify if the NUID has been readed
  if ( ! rfid.PICC_ReadCardSerial())
    return;

  Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  Serial.println(rfid.PICC_GetTypeName(piccType));

  // Check is the PICC of Classic MIFARE type
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&  
    piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
    piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println(F("Your tag is not of type MIFARE Classic."));
    return;
  }

  if (rfid.uid.uidByte[0] != nuidPICC[0] || 
    rfid.uid.uidByte[1] != nuidPICC[1] || 
    rfid.uid.uidByte[2] != nuidPICC[2] || 
    rfid.uid.uidByte[3] != nuidPICC[3] ) {
    Serial.println(F("A new card has been detected."));

    // Store NUID into nuidPICC array
    for (byte i = 0; i < 4; i++) {
      nuidPICC[i] = rfid.uid.uidByte[i];
    }
   
    Serial.println(F("The NUID tag is:"));
    Serial.print(F("In hex: "));
    printHex(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();
    Serial.print(F("In dec: "));
    printDec(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();
  }
  else Serial.println(F("Card read previously."));

  // Halt PICC
  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
}

void uart()
{
  while (Serial.available() > 0)
  {

    char ch = Serial.read();
    //Serial.println(ch);
    data = data + ch;
    //Serial.println("hhhhhhhhhhhhhhhhhhhhh"+data);
  }

  if (data.equals("help"))
  {
    Serial.println(data);
  }
  data.clear();
}

void setup()
{
  // put your setup code here, to run once:
  pinMode(LED_1, OUTPUT);
  //digitalWrite(LED_1, LOW);
  pinMode(LED_2, OUTPUT);
  //digitalWrite(LED_2, LOW);
  SPI.begin(); // Init SPI bus

  wifi_connect();
  mqtt_connect();
  
  // rfid.PCD_Init(); // Init MFRC522
  // for (byte i = 0; i < 6; i++) 
  // {
  //   key.keyByte[i] = 0xFF;
  // }

  // Serial.println(F("This code scan the MIFARE Classsic NUID."));
  // Serial.print(F("Using the following key:"));
  // printHex(key.keyByte, MFRC522::MF_KEY_SIZE); 
}

void loop()
{
  // put your main code here, to run repeatedly:
  client.loop();
  if (!WiFi.isConnected() || !client.connected())
  {
    Serial.println("失联了...........");
    wifi_connect();
    mqtt_connect();
  }
  char c;
  //digitalWrite(SS, LOW);    // SS - pin 10
  // 循环发送字节，实现字符串的发送
  for (const char * p = "Hello,world!\n" ; c = *p; p++) {
    SPI.transfer (c);//主机SPI发送
    Serial.print(c);
  }
  
  
  
  
  
  
  
  //test();
  
  delay(1000);
}
