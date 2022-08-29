#include <SPI.h>
#include <Servo.h>
#include <WiFiNINA.h>
#include <ArduinoJson.h>
#include <ArduinoHttpClient.h>
#include <Adafruit_NeoPixel.h>

// Melody Importing
#define tone_one  1300   // Frequency 1263Hz
#define tone_two  1350   // Frequency 1350Hz
#define tone_three  1523   // Frequency 1523Hz
int melody[] = {tone_one, tone_two, tone_three};
int noteDurations[] = {18, 18, 3};

char ssid[] = "JAI_BALAYYA";
char pass[] = "48GOODWOOD";
int status = WL_IDLE_STATUS;     // the Wifi radio's status

char serverAddress[] = "192.168.0.17"; // server address
int port = 8000;

WiFiClient wifi;
HttpClient client = HttpClient(wifi, serverAddress, port);

StaticJsonDocument<200> doc;

Servo panServo;
Servo tiltServo;

// Neopixel Setup----------------------------------------------------------------------------------------------
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(8, 11, NEO_GRB + NEO_KHZ800);
class Strip {
public:
  uint8_t   effect;
  uint8_t   effects;
  uint16_t  effStep;
  unsigned long effStart;
  Adafruit_NeoPixel strip;
  Strip(uint16_t leds, uint8_t pin, uint8_t toteffects, uint16_t striptype) : strip(leds, pin, striptype) {
    effect = -1;
    effects = toteffects;
    Reset();
  }
  void Reset(){
    effStep = 0;
    effect = (effect + 1) % effects;
    effStart = millis();
  }
};

struct Loop {
  uint8_t currentChild;
  uint8_t childs;
  bool timeBased;
  uint16_t cycles;
  uint16_t currentTime;
  Loop(uint8_t totchilds, bool timebased, uint16_t tottime) {
    currentTime=0;
    currentChild=0;
    childs=totchilds;
    timeBased=timebased;
    cycles=tottime;
    }
};

Strip strip_0(8, 11, 8, NEO_GRB + NEO_KHZ800);
struct Loop strip0loop0(1, false, 1);
//-------------------------------------------------------------------------------------------------------------

const byte buffSize = 40;
char inputBuffer[buffSize];
const char startMarker = '<';
const char endMarker = '>';
byte bytesRecvd = 0;
boolean readInProgress = false;
boolean newDataFromPC = false;

float panServoAngle = 90.0;
float tiltServoAngle = 90.0;
// int LED_state = 11;

String url = "http://192.168.1.168:8000/";

void setup()
{
  Serial.begin(9600);
  panServo.attach(10);
  tiltServo.attach(9);

  strip_0.strip.begin();

  // moveServo();
  start_sequence();

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv != "1.1.0") {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }

  // you're connected now, so print out the data:
  Serial.print("You're connected to the network");
  printCurrentNet();
  printWifiData();

    /*WiFi.begin(ssid, pass);
    Serial.println("Connecting..");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(5000);
    }
    Serial.println("Goodbye, IP:" + WiFi.localIP());*/
}

void loop()
{
    strips_loop_rainbow();
    getDataFromPC();
    replyToPC();
    moveServo();
    setLED();
}

void getDataFromPC()
{
    // receive data from PC and save it into inputBuffer

    if (wifi.status() == WL_CONNECTED)
    {
        client.get("/");

        // read the status code and body of the response
        int statusCode = client.responseStatusCode();
        String json = client.responseBody();

        if (statusCode == 200)
        {
            DeserializationError error = deserializeJson(doc, json);
            //Serial.println(data);

            if (error)
            {
                Serial.print(F("deserializeJson() failed: "));
                Serial.println(error.f_str());
                return;
            }

            panServoAngle = doc["target_pan"];
            panServoAngle = doc["target_tilt"];
            LED_state = doc["led_mode"];
        }
        else
        {
            Serial.println("Error: " + statusCode);
        }
    }
}

void processData() // for data type "<float, float, int>"
{
    char *strtokIndx; // this is used by strtok() as an index

    strtokIndx = strtok(inputBuffer, ","); // get the first part
    panServoAngle = atof(strtokIndx);      // convert this part to a float

    strtokIndx = strtok(NULL, ",");    // get the second part(this continues where the previous call left off)
    tiltServoAngle = atof(strtokIndx); // convert this part to a float

    strtokIndx = strtok(NULL, ","); // get the last part
    LED_state = atoi(strtokIndx);   // convert this part to an integer (string to int)
}

void replyToPC()
{

    if (newDataFromPC)
    {
        newDataFromPC = false;
        Serial.print("<");
        Serial.print(panServo.read());
        Serial.print(",");
        Serial.print(tiltServo.read());
        Serial.println(">");
    }
}

void moveServo()
{
    panServo.write(panServoAngle);
    tiltServo.write(tiltServoAngle);
}

// -----------------------------------------RAINBOW-------------------------------------------------------------------------
void strips_loop_rainbow() {
  if(strip0_loop0() & 0x01)
    strip_0.strip.show();
}

uint8_t strip0_loop0() {
  uint8_t ret = 0x00;
  switch(strip0loop0.currentChild) {
    case 0: 
           ret = strip0_loop0_eff0();break;
  }
  if(ret & 0x02) {
    ret &= 0xfd;
    if(strip0loop0.currentChild + 1 >= strip0loop0.childs) {
      strip0loop0.currentChild = 0;
      if(++strip0loop0.currentTime >= strip0loop0.cycles) {strip0loop0.currentTime = 0; ret |= 0x02;}
    }
    else {
      strip0loop0.currentChild++;
    }
  };
  return ret;
}

uint8_t strip0_loop0_eff0() {
    // Strip ID: 0 - Effect: Rainbow - LEDS: 8
    // Steps: 103 - Delay: 52
    // Colors: 3 (255.0.0, 0.255.0, 0.0.255)
    // Options: rainbowlen=168, toLeft=true, 
  if(millis() - strip_0.effStart < 52 * (strip_0.effStep)) return 0x00;
  float factor1, factor2;
  uint16_t ind;
  for(uint16_t j=0;j<8;j++) {
    ind = strip_0.effStep + j * 0.6130952380952381;
    switch((int)((ind % 103) / 34.333333333333336)) {
      case 0: factor1 = 1.0 - ((float)(ind % 103 - 0 * 34.333333333333336) / 34.333333333333336);
              factor2 = (float)((int)(ind - 0) % 103) / 34.333333333333336;
              strip_0.strip.setPixelColor(j, 255 * factor1 + 0 * factor2, 0 * factor1 + 255 * factor2, 0 * factor1 + 0 * factor2);
              break;
      case 1: factor1 = 1.0 - ((float)(ind % 103 - 1 * 34.333333333333336) / 34.333333333333336);
              factor2 = (float)((int)(ind - 34.333333333333336) % 103) / 34.333333333333336;
              strip_0.strip.setPixelColor(j, 0 * factor1 + 0 * factor2, 255 * factor1 + 0 * factor2, 0 * factor1 + 255 * factor2);
              break;
      case 2: factor1 = 1.0 - ((float)(ind % 103 - 2 * 34.333333333333336) / 34.333333333333336);
              factor2 = (float)((int)(ind - 68.66666666666667) % 103) / 34.333333333333336;
              strip_0.strip.setPixelColor(j, 0 * factor1 + 255 * factor2, 0 * factor1 + 0 * factor2, 255 * factor1 + 0 * factor2);
              break;
    }
  }
  if(strip_0.effStep >= 103) {strip_0.Reset(); return 0x03; }
  else strip_0.effStep++;
  return 0x01;
}

// ------------------------------------------------------------------------------------------------------------------
void setLED() {
    if (LED_state == 2) { // Yellow ON

    }
    else if (LED_state == 1) { // Face Found, Green ON

    }
    else if (LED_state == 0) { // No Face, Red ON

    }
    else if (LED_state == 3) { // All ON
        strips_loop_rainbow();
    }
    else { // All OFF
        
    }
}

// ------------------------------------------------------------------------------------------------------------------
void startup_sound() {
  for (int thisNote = 0; thisNote < 3; thisNote++) {
    int noteDuration = 1000 / noteDurations[thisNote];  // Play note
    tone(8, melody[thisNote], noteDuration*0.8);
    
    int pauseBetweenNotes = noteDuration * 1.30;  // Pause note
    delay(pauseBetweenNotes);
    
    noTone(8); // Stop sound
  }
}

void printWifiData() {
  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  Serial.println(ip);

  // print your MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  Serial.print(mac[5], HEX);
  Serial.print(":");
  Serial.print(mac[4], HEX);
  Serial.print(":");
  Serial.print(mac[3], HEX);
  Serial.print(":");
  Serial.print(mac[2], HEX);
  Serial.print(":");
  Serial.print(mac[1], HEX);
  Serial.print(":");
  Serial.println(mac[0], HEX);

}

void printCurrentNet() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print the MAC address of the router you're attached to:
  byte bssid[6];
  WiFi.BSSID(bssid);
  Serial.print("BSSID: ");
  Serial.print(bssid[5], HEX);
  Serial.print(":");
  Serial.print(bssid[4], HEX);
  Serial.print(":");
  Serial.print(bssid[3], HEX);
  Serial.print(":");
  Serial.print(bssid[2], HEX);
  Serial.print(":");
  Serial.print(bssid[1], HEX);
  Serial.print(":");
  Serial.println(bssid[0], HEX);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.println(rssi);

  // print the encryption type:
  byte encryption = WiFi.encryptionType();
  Serial.print("Encryption Type:");
  Serial.println(encryption, HEX);
  Serial.println();
}

// LED

void start_sequence()
{
    startup_sound();
    for(int i=8; i>=0; i--){
      pixels.setPixelColor(i, pixels.Color(0,255,35));
      pixels.show();
      delay(100);
    }
    panServo.write(90);
    tiltServo.write(90);
    delay(300);
}
