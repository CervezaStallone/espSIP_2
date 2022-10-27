#include <Arduino.h>
#include <wifi.h>
#include <SipMachine.h>
#include "SPIFFS.h"
#include <Debug.h>

//Set WiFi params
const char* ssid = "Yer A Wifi Harry";
const char* password = "Pannekoekenplantdealer1996!";

//Set SIP params &
String telNr = "1000";
String serverIp = "10.13.37.7";
String localIp = "10.13.37.53";
String sipUser = "Deurbel";
String sipPwd = "12345";
int sipPort = 5062;
SipMachine sipMachine = SipMachine(sipUser, sipPwd, telNr, serverIp, serverIp, sipPort);

void connectToNetwork() {
  WiFi.begin(ssid, password);
  Serial.println("\nConnecting");
  
  while(WiFi.status() != WL_CONNECTED) { 
  Serial.println(".");
  delay(100);
  }

  Serial.println("\nNetwork connection established");
  Serial.print("IP-address assigned by DHCP server: ");
  Serial.println(WiFi.localIP());
}

void configSip() {
  sipMachine.setup(WiFi.localIP().toString(), serverIp);
}
fs::File f;
fs::File fo;

void setup() {
  Serial.begin(9600);
  connectToNetwork();
  configSip();
}

bool b = true;
unsigned long t = millis();
unsigned long t2 = millis();
unsigned long x = 0;
unsigned long t3 = millis();
unsigned long t4 = micros();
unsigned long t5 = millis();

SipMachine::Status status;
int16_t pcmOut=0;
int16_t pcmIn;

int16_t minP=0;
int16_t maxP=0;

void loop()
{

    pcmIn = sipMachine.loop(pcmOut);
    pcmOut=0;
    status=sipMachine.getStatus();

    switch (status)
    {
    case SipMachine::ringIn:
        // debug_println(String("Ringing Call Nr. ") + sipMachine.getTelNr());
        if (sipMachine.getTelNrIncomingCall().toInt() < 300)
        {
            debug_println(String("Accept incoming Call ") + sipMachine.getTelNrIncomingCall());
            sipMachine.acceptIncomingCall();
        }
        break;
    case SipMachine::idle:
        if ((t2 + 5000 < millis()) & (t2 + 6000 > millis()))
            if (!telNr.equals("222"))
            {
                sipMachine.makeCall("222");
            }
        if (f)
        {
            f.close();
        }
        if (fo)
        {
            fo.close();
        }
        break;
    case SipMachine::call:
        if ((t5+20000)<millis())
        {
            t5+=20000;
            sipMachine.bye();
        }
        if (t4<micros())
        {
            t4+=125;
            if (!f)
            {
                f=SPIFFS.open("/Data.pcm",FILE_WRITE);

            }
            if (!fo)
            {
                fo=SPIFFS.open("/wilk.pcm");
            }
            else
            {
                if (pcmIn==(688))
                {
                    pcmIn=0;
                }
                if (pcmIn>maxP)
                {
                    maxP=pcmIn;
                }
                if (pcmIn<minP)
                {
                    minP=pcmIn;
                }
                f.write((uint8_t*)&pcmIn,2);
            }

            if(fo.available()>1)
            {
                fo.read((uint8_t*)&pcmOut,2);
            }
            else
            {
                fo.close();
            }
        }
        break;
    default:
        break;
    }
    x++;

  delay(500);
}

//#ifdef __ESP_ARDU

int main()
{
    setup();
    while (true==true)
    {
        loop();
    }
}

//#endif