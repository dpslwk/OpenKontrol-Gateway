/****************************************************	
 * sketch = OKMQTT_WIFLY
 *
 * Nottingham Hackspace
 * CC-BY-SA
 *
 * Source = http://wiki.nottinghack.org.uk/wiki/...
 * Target controller = Arduino 328 (Ciseco OpenKnotrol Gateway)
 * Clock speed = 16 MHz
 * Development platform = Arduino IDE 1.0
 * C compiler = WinAVR from Arduino IDE 1.0
 * 
 * 
 * This code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 *
 * Simple LLAP<->MQTT bridge example for the Ciseco Open
 *
 * 
 ****************************************************/

/*
 These are the global config parameters
 Including Pin Outs, IP's, TimeOut's etc
 
 */

#ifndef  OKMQTT_WIFLY_h
#define  OKMQTT_WIFLY_h

/**************************************************** 
 * genral defines
 * use for staic config bits
 ****************************************************/
// Status LED
#define STATUS_LED 17
#define STATUS_TOGGLE_TIMEOUT 500
unsigned long statusTimeOut = 0;
byte statusState = 0;

// baud rate for the XRF
#define XRF_BAUD 9600


// WiFly RN-XV details
#define XV_BAUD 9600
#define XV_TX 6
#define XV_RX 5


//LLAP messgaes are 12 char's long + NULL terminator
#define LLAP_BUFFER_LENGTH 13
#define LLAP_DEVID_LENGTH 2
#define LLAP_DATA_LENGTH 9

/**************************************************** 
 * global vars
 * 
 ****************************************************/
char LLAPmsg[LLAP_BUFFER_LENGTH];
SoftwareSerial wiSerial(XV_RX, XV_TX);
WiFlyClient ethClient;

/**************************************************** 
 * Ethernet settings
 * Update these with values suitable for your network.
 *
 ****************************************************/
// WiFly library currently only uses DHCP

/**************************************************** 
 * MQTT settings
 *
 ****************************************************/
byte mqttIp[] = {85,119,83,194};
// or
//char* mqttIp = "test.moqsuitto.org";

#define MQTT_PORT 1883
// forward def of callback
void callbackMQTT(char*, byte*, unsigned int);
PubSubClient mqttClient(mqttIp, MQTT_PORT, callbackMQTT, ethClient);

// ClientId for connecting to MQTT
#define CLIENT_ID "OpenKnotrol"

// Subscribe to topics
#define S_RX		"ok/tx/#"
#define S_RX_MASK   "ok/tx/"

// Publish Topics
#define P_TX		"ok/rx/"

// Status Topic, use to say we are alive or DEAD (will)
#define S_STATUS "ok/status"
#define P_STATUS "ok/status"
#define STATUS_STRING "STATUS"
#define RUNNING "Running: OKMQTT"
#define RESTART "Restart: OKMQTT"

#endif
