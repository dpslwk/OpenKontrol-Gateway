/****************************************************	
 * sketch = OKMQTT_WIFLY
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
    002 - changes to mqtt topics
            topics are now
            ok/tx/<DEVID>
            ok/rx/<DEVID>
            LLAP messages are decoded/constructed using <DEVID> from topic
            messages from ok/tx/<DEVID> are auto padded with '-' to 12 char's
            Now using updated PubSubClient from https://github.com/dpslwk/pubsubclient
    003 - WiFly Example
            Same MQTT <> LLAP but using RN-XV on the gateway softserial
            Compatibly WiFly Libary can be found here https://github.com/dpslwk/WiFly
            
            
 
 Known issues:

 
 Future changes:

 
 ToDo:

 
 Authors:
 'RepRap' Matt      dps.lwk at gmail.com

 */

#define VERSION_NUM 003
#define VERSION_STRING "OKMQTT ver: 003"

// Uncomment for debug prints
#define DEBUG_PRINT

#include <SPI.h>
#include <WiFly.h>
#include <PubSubClient.h>
#include <SoftwareSerial.h>
#include "Credentials.h"
#include "OKMQTT_WIFLY.h"



/**************************************************** 
 * pushXRF 
 * Read incoming LLAP from MQTT and push to XRF 
 ****************************************************/
void pushXRF(char* topic, byte* payload, int length) {
    // pre fill buffer with padding
    memset(LLAPmsg, '-', LLAP_BUFFER_LENGTH);
	// add a null terminator to make it a string
    LLAPmsg[LLAP_BUFFER_LENGTH - 1] = 0;
    // start char for LLAP packet
    LLAPmsg[0] = 'a';
    
	// copy <DEVID> from topic
    memcpy(LLAPmsg +1, topic + strlen(S_RX_MASK), LLAP_DEVID_LENGTH);
    
    // little memory overflow protection
    if ((length) > LLAP_DATA_LENGTH) {
        length = LLAP_DATA_LENGTH;
    }
    // copy mqtt payload into messgae
    memcpy(LLAPmsg+3, payload, length);
    
    // send it out via the XRF
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
 * work out which topic was published to and handle as needed
 ****************************************************/
void callbackMQTT(char* topic, byte* payload, unsigned int length) {
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
        	// read in <DEVID>
        	char devId[LLAP_DEVID_LENGTH +1];
        	Serial.readBytes(devId, LLAP_DEVID_LENGTH);
        	
        	// build full topic
			char llapTopic[strlen(P_TX) + LLAP_DEVID_LENGTH + 1];
			strcat(llapTopic, P_TX);
			strcat(llapTopic, devId);
			
           //clear the buffer
            memset(LLAPmsg, 0, LLAP_BUFFER_LENGTH);
            char t;
            // read in rest of message for mqtt payload
            for(int pos=0; pos < LLAP_DATA_LENGTH; pos++) {
            	t = Serial.read();
            	if(t != '-')
                	LLAPmsg[pos] = Serial.read();
            }
            mqttClient.publish(llapTopic, LLAPmsg);
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
    // Setup Pins
	pinMode(XRF_POWER_PIN, OUTPUT);
    
	// Set default output's
	// turn on XRF before start serial output
    digitalWrite(XRF_POWER_PIN, HIGH);
    
    // Start Serial
	Serial.begin(XRF_BAUD);
#ifdef DEBUG_PRINT
	Serial.println(VERSION_STRING);
#endif
    
    // Start wiSerial
    wiSerial.begin(XV_BAUD);
    WiFly.setUart(&wiSerial);
    
    WiFly.begin();
    
    // Join the WiFi network
    if (!WiFly.join(ssid, passphrase, mode)) {
        while (1) {
            // Hang on failure.
        }
    } 
    
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








