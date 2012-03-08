/****************************************************	
 * sketch = OKMQTT
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

#ifndef  OKMQTT_h
#define  OKMQTT_h

/**************************************************** 
 * genral defines
 * use for staic config bits
 ****************************************************/
// baud rate for the XRF
#define XRF_BAUD 9600

//LLAP messgaes are 12 char's long + NULL terminator
#define LLAP_BUFFER_LENGHT 13


/**************************************************** 
 * Ethernet settings
 * Update these with values suitable for your network.
 *
 ****************************************************/

byte mac[] = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xAF }; // Grab the mac from your wiznet chip

//uncomment and fill in for static ip
byte ip = false;
//IPAddress ip(0,0,0,0);


/**************************************************** 
 * MQTT settings
 *
 ****************************************************/

// MQTT server 
byte mqttIp[] = {10,0,0,2};
#define MQTT_PORT 1883

// ClientId for connecting to MQTT
#define CLIENT_ID "OpenKnotrol"

// Subscribe to topics
#define S_RX		"ok/tx/#"
#define S_RX_MASK   "ok/tx/"

// Publish Topics
#define P_TX		"ok/rx"

// Status Topic, use to say we are alive or DEAD (will)
#define S_STATUS "ok/status"
#define P_STATUS "ok/status"
#define STATUS_STRING "STATUS"
#define RUNNING "Running: OKMQTT"
#define RESTART "Restart: OKMQTT"

#endif
