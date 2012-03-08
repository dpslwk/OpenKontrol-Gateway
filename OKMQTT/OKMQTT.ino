/****************************************************	
 * sketch = OKMQTT
 *
 * Nottingham Hackspace
 * CC-BY-SA
 *
 * Source = http://wiki.nottinghack.org.uk/wiki/...
 * Target controller = Arduino 328 (Ciseco OpenKontrol Gateway)
 * Clock speed = 16 MHz
 * Development platform = Arduino IDE 1.0
 * C compiler = WinAVR from Arduino IDE 1.0
 * 
 * 
 * This code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 *
 * Simple LLAP<->MQTT bridge example for the Ciseco OpenKontrol Gateway
 *
 * 
 ****************************************************/

/*  
 History
    000 - Started 07/03/2012
 	001 - Initial release
            first pass at a basic bridge
            assumes XRF is on hardware UART and using wiznet ethernet
            LLAP message have a max length of 12 char's
            need to publish each LLAP individually via MQTT
 
 Known issues:

 
 Future changes:

 
 ToDo:

 
 Authors:
 'RepRap' Matt      dps.lwk at gmail.com

 */

#define VERSION_NUM 001
#define VERSION_STRING "OKMQTT ver: 001"

// Uncomment for debug prints
#define DEBUG_PRINT

#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include "OKMQTT.h"

/**************************************************** 
 * global vars
 * 
 ****************************************************/
PubSubClient mqttClient(mqttIp, MQTT_PORT, callbackMQTT);
char LLAPmsg[LLAP_BUFFER_LENGHT];

/**************************************************** 
 * pushXRF 
 * Read incoming LLAP from MQTT and push to XRF 
 ****************************************************/
void pushXRF(char* topic, byte* payload, int length) {
    // basic implementation
    
    // clear the buffer
    memset(LLAPmsg, 0, LLAP_BUFFER_LENGHT);
    // little memory overflow protection
    if ((length) > LLAP_BUFFER_LENGHT-1) {
        length = LLAP_BUFFER_LENGHT-1;
    }
    memcpy(LLAPmsg, payload, length);
    
    Serial.print(LLAPmsg);
} 

/**************************************************** 
 * statusUpdate
 * given MQTT request for STATUS respond as needed
 *
 ****************************************************/
void statusUpdate(byte* payload, int length) {
    // check for Status request
    if (strncmp(STATUS_STRING, (char*)payload, strlen(STATUS_STRING)) == 0) {
#ifdef DEBUG_PRINT
        Serial.println("Status Request");
#endif
        mqttClient.publish(P_STATUS, RUNNING);
    }
}

/**************************************************** 
 * callbackMQTT
 * called when we get a new MQTT
 * work out which topic was published to and handel as needed
 ****************************************************/
void callbackMQTT(char* topic, byte* payload, int length) {
    if (strncmp(S_RX_MASK, topic, strlen(S_RX_MASK)) == 0) {
        pushXRF(topic, payload, length);
    } else  if (!strcmp(S_STATUS, topic)) {
        statusUpdate(payload, length);
    }
}

/**************************************************** 
 * pollXRF 
 * Read incoming LLAP from XRF and push to MQTT 
 ****************************************************/
void pollXRF() {
    if (Serial.available() >= 12){
        if (Serial.read() == 'a') {
            //clear the buffer
            memset(LLAPmsg, 0, LLAP_BUFFER_LENGHT);
            int pos = 0;
            LLAPmsg[pos++] = 'a';
            while (pos < 12) {
                LLAPmsg[pos++] = Serial.read();
            }
            mqttClient.publish(P_TX, LLAPmsg);
        }
    }
} 

/**************************************************** 
 * check we are still connected to MQTT
 * reconnect if needed
 *  
 ****************************************************/
void checkMQTT()
{
  	if(!mqttClient.connected()){
		if (mqttClient.connect(CLIENT_ID)) {
			mqttClient.publish(P_STATUS, RESTART);
			mqttClient.subscribe(S_RX);
			mqttClient.subscribe(S_STATUS);
#ifdef DEBUG_PRINT
			Serial.println("MQTT Reconnect");
#endif
		}
	}
} 


void setup() {
    // Start Serial
	Serial.begin(XRF_BAUD);
#ifdef DEBUG_PRINT
	Serial.println(VERSION_STRING);
#endif
    
    // Start ethernet on the OpenKontrol Gateway
    // handels static ip or DHCP automatically
    if(ip){
        Ethernet.begin(mac, ip);
    } else {    
        if (Ethernet.begin(mac) == 0) {
#ifdef DEBUG_PRINT
            Serial.println("Failed to configure Ethernet using DHCP");
#endif
            // no point in carrying on, so do nothing forevermore:
            for(;;)
                ;
        }
    }
    
    // Setup Pins
    
    // Set default output's
    
    // Start MQTT and say we are alive
    checkMQTT();
    
    // let everything else settle
    delay(100);
}

void loop() {
    // poll XRF for incoming LLAP
	pollXRF();
	
	// Poll MQTT
	// should cause callback if theres a new message
	mqttClient.loop();

	// are we still connected to MQTT
	checkMQTT();

}








