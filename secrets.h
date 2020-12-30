// Use this file to store all of the private credentials 
// and connection details

#define SECRET_SSID "***********"		// replace MySSID with your WiFi network name
#define SECRET_PASS "**********"		// replace MyPassword with your WiFi password

//Broker MQTT - Mosquitto
#define ADDRESS_MQTTSERVER "67.225.227.165" // MQTT Broker OFICIAL "mqtt.cheerlights.com" (67.225.227.165:1883) // broker desabilitado a fazer outros Publisher.
//#define ADDRESS_MQTTSERVER "5.196.95.208"     // MQTT Broker "test.mosquitto.org" (5.196.95.208:1883) // aconteceu de insistir (erroneamente) em voltar para a cor blue.

//MathWorks-ThingSpeak:
#define ID_CHANNEL 1270120                     //cheerlights_saberes-afazeres
#define SECRET_READAPIKEY "76DP3IOOV6ECRTCO"	 //API Key for Test channel
#define SECRET_WRITEAPIKEY "698HWRJZTNEEX2P8"  //API Key for Test channel
