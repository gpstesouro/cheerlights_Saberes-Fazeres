/*
  https://www.cheerlights.com
  Saberes&Afazeres
  ++++++++++++++++++++++++++++++++++++++++++++
  Descrição: 
     * v.1: vamos lá, sei que você, assim como eu, não se contentará com a monotonia de ficar olhando uma única cor (painel monocromático). 
        Portanto, façamos dois tipos de registros históricos: (i) um visual (usando o cordão de leds) com os 48 últimos registros de cores 
        amostrados (discretizados) a cada 19 segundos; e (ii) transferir dados para gerar um gráfico com a cor e o respectivo intervalo de 
        permanência (tempo de cronômetro) da cor inalterada;
     * v.1.1: utilização do FastLED v.3.3.3 e display (OLED) do ESP32 com a indicação da cor e tempo de exposição (cronômetro) da respectiva cor;
     * v.1.2: captação de um idDigital, tal como uma "assinatura em livro de visita" daqueles que instalarem e rodarem o sketch em suas placas. 
        A intenção não é de identificar, tão somente registrar a visita, como se fosse, uma rubrica;
     * v.1.3: ThingSpeak recebe os dados (idDigital, color e cronômetro) e apresenta num gráfico temporal; e
     * v.2: dashboard WEB (Qnt de idDigitais, corCurrent, chronometer e gráfico de histórico - TS). Esta parece-me a tarefa mais complicada e que 
        demandará maior tempo.
  
  Hardware: placa Heltec WIFI ESP32 V2, cordão ou fita de leds (atentar quanto a voltagem e pinagem). Nesta experimentação foi utlizado o modelo WS2811 - 12 Volts (50 leds).
   
  !!! IMPORTANTE: (i) modifique o arquivo secrets.h para o seu projeto, com sua conta e senha de conexão a rede e detalhes de configuração: ThingSpeak (channel) e MQTT; 
                  (ii) consulte o pinout de sua placa; mas cabe sugerir, se possível, não usar para outros fins as GPIO reservadas: LoRa (5, 19, 27, 14 e 18) e OLED (4 e 15) !!!!
  
  Note:
  - Requires installation of EPS32 core. See https://github.com/espressif/arduino-esp32/blob/master/docs/arduino-ide/boards_manager.md for details. 
  - Select the target hardware from the Tools->Board menu
  - This example is written for a network using WPA encryption. For WEP or WPA, change the WiFi.begin() call accordingly.
  
  >(OLED) Wire, Adafruit_GFX e Adafruit_SSD1306

  >(STRIPLEDS)FastLED

  >MQTT
  https://pubsubclient.knolleary.net/api#configoptions

  >ThingSpeak ( https://www.thingspeak.com ) is an analytic IoT platform service that allows you to aggregate, visualize, and 
  analyze live data streams in the cloud. Visit https://www.thingspeak.com to sign up for a free account and create a channel.  
  Documentation for the ThingSpeak Communication Library for Arduino is in the README.md folder where the library was installed.
  See https://www.mathworks.com/help/thingspeak/index.html for the full ThingSpeak documentation.
  For licensing information, see the accompanying license file. Copyright 2018, The MathWorks, Inc.

*/
///////////////////////////////////////////////////////////////////////////////////////////////
#include <Wire.h>              //<<G>
#include <Adafruit_GFX.h>      
#include <Adafruit_SSD1306.h>  //<<G>
#include "FastLED.h"           //<H>
#include "secrets.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include "ThingSpeak.h"        //<F>
//-----------------------------
/* Endereço I2C do display */   //<<<G>
#define OLED_ADDR   0x3c
/* distancia, em pixels, de cada linha em relacao ao topo do display */
#define OLED_LINE1     0
#define OLED_LINE2     10
#define OLED_LINE3     20
#define OLED_LINE4     30
#define OLED_LINE5     40
#define OLED_LINE6     50
/* Configuração da resolucao do display (este modulo possui display 128x64) */
#define SCREEN_WIDTH    128 
#define SCREEN_HEIGHT   64     ///<<<G>

/*Propriedades da fita ou cordão de leds*/ //<<H>
#define NUM_LEDS 50
#define DATA_PIN 17
/* Configuração da intensidade de brilho*/
#define BRIGHTNESS 32 //(0 ... 255)        //<<H> 
//-----------------------------
// Replace the next variables with your SSID/Password combination
char ssid[] = SECRET_SSID;     // your network SSID (name) 
char pass[] = SECRET_PASS;     // your network password

// Add your MQTT Broker IP address, example:
const char* mqttServer = ADDRESS_MQTTSERVER; 

//ThingSpeak channel details: cheerlights_saberes-afazeres
char thingSpeakAddress[] = "api.thingspeak.com";

unsigned long idChannel = ID_CHANNEL;
char* readAPIKey = SECRET_READAPIKEY;              //ver arquivo SECRET.h
char* writeAPIKey = SECRET_WRITEAPIKEY;            //ver arquivo SECRET.h
unsigned int idDigitalFieldNumber = 1;             // Field to write idDigital
unsigned int colorFieldNumber = 2;                 // Field to write colorBefore
unsigned int chronometerFieldNumber = 3;           // Field to write chronometer

int permanence = 0;   // <loop><D> (=10) estabelece o intervalo de revisita a função stripLeds para indicar no painel a permanência da mesma cor 

String color;         // <B> variável que acolhe o valor retornado pela função messageMQTT (callback MQTT subscribe cheerlights)
String colorBefore;   // <loop><D><F> variável global (armazena a cor antecedente)
int colorIndice = 0;
int colorIndiceBefore;//<D><F>
unsigned long beginTime, chronometer;  //<F><G>

uint64_t chipid; // corresponde ao "uint64_t chipid;" do exemplo: ESP32 => ChipID => GetChipID
unsigned int idDigital; // parte "low" (4-1 = 3 bytes) do ChipID //<setup><D><F><G>

//-----------------------------
/* Objeto do display */
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, 16); //<G>
/* Objeto do leds - arrays */
CRGB leds[NUM_LEDS];
CRGB ledsMirror[NUM_LEDS];
/*Objeto conexão WiFi*/
WiFiClient espClient;
/*Objeto conexão MQTT*/
PubSubClient client(espClient);


///////////////////////////setup//////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(115200);

  chipid = ESP.getEfuseMac();//The chip ID is essentially its MAC address(length: 6 bytes).
  //idDigitalHigh = chipid>>32; /// descartada a parte "high" do Chipid 
  // (HEX: 00 3C | AF 286F24) (DEC: 00 60 | 175 >2649892<)
  idDigital = ((chipid) & 0x0000000000FFFFFF); // utilizar apenas os 3 últimos bytes menos significativos, em deciaml.
  
   /* inicializa display OLED */
  Wire.begin(4, 15);                               //<<G>
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.clearDisplay();                          //<<G>

  FastLED.addLeds<WS2811,DATA_PIN>(leds,NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.clear();
  
  WiFi.mode(WIFI_STA);   
  setup_wifi();
  
  client.setKeepAlive(30);                   //<<B>
  client.setServer(mqttServer, 1883);
  client.setCallback(messageMQTT);           //<<B>

  ThingSpeak.begin(espClient);  // Initializa ThingSpeak // Obs; era (client) entre parenteses
  
}


/////////////////////////////////loop////////////////////////////////////////////////////////
void loop() {
  Serial.print("<loop> Status da conexão MQTT (0 == OK): "); //????????? depuração ?????????
  Serial.println(client.state());
  if(client.state() != 0){  // sinalizar haver algum problema na conexão MQTT
    leds[0] = CRGB::White;
    FastLED.show();
    delay(500);
    leds[0] = CRGB::Black;
    FastLED.show();
    delay(500);
  }               
  
  if (!client.connected()) {
    reconnectMQTT();
  }
  client.loop();

  if (color != colorBefore || (permanence == 19)){  
    stripLeds(color); 
  }
  
  displayChronometer(color);  ///// mostra o tempo de pemanência (cronômetro) da cor no display do ESP32 
  permanence++;
  delay(1000); // time do cronômetro (delay = 1000 corresponde a APROXIMADAMENTE 1 seg).
}

//////////////////////////////////<A>///////////////////////////////////////////////////////
//Função encarregada de manter a conexão WiFi
void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

   while (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid, pass);
    Serial.print(".");
    delay(500);
  }

  Serial.println("WiFi connected. IP address: ");
  Serial.println(WiFi.localIP());
}

/////////////////////////////////<B>////////////////////////////////////////////////////////
//Função CallBack a qual é chamada quando há mensagens publicadas sobre o tópico assinado
void messageMQTT(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: " + String(topic));
  //Serial.print(topic);
  Serial.print(". Message: ######## ");
  String messageRX;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageRX += (char)message[i];
  }
  Serial.println();

  // If a message is received on the topic cheerlights/okStoreChrono, you check if the message is either "on" or "off". 
  if (String(topic) == "cheerlights/okStoreChrono") {
    Serial.print("Changing output to ");
    if(messageRX.substring(0,length-2) == "on"){
      Serial.println("on");
    }
    else if(messageRX.substring(0,length-2) == "off"){
      Serial.println("off");
    }
  }
  else if (String(topic) == "cheerlights") {
    Serial.println(messageRX);
    color = messageRX;
  }
}

////////////////////////////////<C>/////////////////////////////////////////////////////////
// Reconecta ao Broker MQTT, caso precise
void reconnectMQTT() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP32Client")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("cheerlights/okStoreChrono");
      client.subscribe("cheerlights");
      
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


/////////////////////////////////////<D>/////////////////////////////////////////////////////////////////
//Função que recebe a "color" obtida do Broker MQTT e lança no painel. Também realiza o espelhamento da informação de cor no array ledsMirror[]. 
void stripLeds (String colorCurrent){
  
////////////////////////////
  static int i;
  (i < NUM_LEDS-2)?i++:i=1;
  //////////////????????? depuração ?????????
 //Faça uma experiência, retire o "static" da declaração da variável "i" e verifique o resultado.
  Serial.print("(i): ");
  Serial.println(i);
  //char *myColor[] = {"White", "Red", "Green", "Blue", "Cyan", "Magenta", "Yellow", "Purple", "Orange", "Warmwhite", "OldLace", "Pink"}; 
  //char *myCode[] = {"16729309", "16729309"};
////////////////////////////

  if (colorCurrent == "white")
    {  
           
        colorIndice = 1;
        leds[i] = CRGB::White;
        ledsMirror[i] = leds[i];
        leds[(i != NUM_LEDS-2)?i+1:1] = CRGB::Black;
        FastLED.show();
        delay(30);
      
    }
    else if (colorCurrent == "red")
    {  
      
        colorIndice = 2;
        leds[i] = CRGB::Red;
        ledsMirror[i] = leds[i];
        leds[(i != NUM_LEDS-2)?i+1:1] = CRGB::Black;
        FastLED.show();
        delay(30);
    
    }
   else if (colorCurrent == "green")
    {  
      
        colorIndice = 3;
        leds[i] = CRGB::Green;
        ledsMirror[i] = leds[i];
        leds[(i != NUM_LEDS-2)?i+1:1] = CRGB::Black;
        FastLED.show();
        delay(30);
      
    }
    else if (colorCurrent == "blue")
    {  
      
        colorIndice = 4;
        leds[i] = CRGB::Blue;
        ledsMirror[i] = leds[i];
        leds[(i != NUM_LEDS-2)?i+1:1] = CRGB::Black;
        FastLED.show();
        delay(30);
     
    }
    else if (colorCurrent == "cyan")
    {  
      
        colorIndice = 5;
        leds[i] = CRGB::Cyan;
        ledsMirror[i] = leds[i];
        leds[(i != NUM_LEDS-2)?i+1:1] = CRGB::Black;
        FastLED.show();
        delay(30);
      
    }
    else if (colorCurrent == "magenta")
    {  
      
        colorIndice = 6;
        leds[i] = CRGB::Magenta;
        ledsMirror[i] = leds[i];
        leds[(i != NUM_LEDS-2)?i+1:1] = CRGB::Black;
        FastLED.show();
        delay(30);
    
    }
    else if (colorCurrent == "yellow")
    {  
      
        colorIndice = 7;
        leds[i] = CRGB::Yellow;
        ledsMirror[i] = leds[i];
        leds[(i != NUM_LEDS-2)?i+1:1] = CRGB::Black;
        FastLED.show();
        delay(30);
     
    }
    else if (colorCurrent == "purple")
    {  
      
        colorIndice = 8;
        leds[i] = CRGB::Purple;
        ledsMirror[i] = leds[i];
        leds[(i != NUM_LEDS-2)?i+1:1] = CRGB::Black;
        FastLED.show();
        delay(30);
      
    }
    else if (colorCurrent == "orange")
    {  
      
        colorIndice = 9;
        leds[i] = CRGB::Orange;
        ledsMirror[i] = leds[i];
        leds[(i != NUM_LEDS-2)?i+1:1] = CRGB::Black;
        FastLED.show();
        delay(30);
     
    }
    else if (colorCurrent == "warmwhite")
    {  
      
        colorIndice = 10;
        leds[i] = CRGB::OldLace;
        ledsMirror[i] = leds[i];
        leds[(i != NUM_LEDS-2)?i+1:1] = CRGB::Black;
        FastLED.show();
        delay(30);

    }
    else if (colorCurrent == "oldlace")
    {  
     
        colorIndice = 10;
        leds[i] = CRGB::OldLace;
        ledsMirror[i] = leds[i];
        leds[(i != NUM_LEDS-2)?i+1:1] = CRGB::Black;
        FastLED.show();
        delay(30);
     
    }
    else if (colorCurrent == "pink")
    {  
      
        colorIndice = 11;
        leds[i] = CRGB::Pink;
        ledsMirror[i] = leds[i];
        leds[(i != NUM_LEDS-2)?i+1:1] = CRGB::Black;
        FastLED.show();
        delay(30);
    
    }
    else
    {
      leds[i] = CRGB::Black;
    }
////////////////  
    
  if(colorCurrent != colorBefore){
      alertColorChange(i);
      transferTSData();    /////// transfere dados (idDigital, color, chronometer) para o ThingSpeak (TS)
      colorBefore = colorCurrent;
      colorIndiceBefore = colorIndice;
      beginTime = millis();  // Dispara o cronômetro
  } 
  
  permanence = 0;
  
  //////////////????????? depuração ?????????
  // Serial.print(colorCurrent);
  // Serial.println(" - final do processo: registrado como colorBefore");
  // Serial.println(".................................................................");
}


/////////////////////////////////////<E>//////////////////////////////////////////
//Função que alerta no painel de leds o momento da transição de cor, fazendo um preenchimento (leds 1:48) unicolor na respectiva cor. 
void alertColorChange(int i){
  Serial.println("alertColorChange");  //////////////????????? depuração ?????????

//Varredura dos led (1:48) tornando-os todos na mesma cor mais recentemente definida.
  for(int j = 1; j < NUM_LEDS-2; j++) { 
       leds[j] = leds[i]; 
       FastLED.delay(66);
  }

//Recuperação do painel com o histórico de leds (1:48) de cores, valendo-se do array ledsMirror[].    
  for(int j = 1; j < NUM_LEDS-2; j++) {
    leds[j] = ledsMirror[j];
    //FastLED.show();
  }
  leds[(i != NUM_LEDS-2)?i+1:1] = CRGB::Black;
  FastLED.show();
}


/////////////////////////////////<F>/////////////////////////////////////////
//Função encarregada de transferir dados (idDigital, color e crhonometer) para o ThingSpeak (TS), de modo a serem exibidos num gráfico.   
void transferTSData() {
      //chronometer = (millis() - beginTime)/1000;
      ThingSpeak.setField(idDigitalFieldNumber, (int) idDigital);
      ThingSpeak.setField(colorFieldNumber, (int) colorIndiceBefore);
      ThingSpeak.setField(chronometerFieldNumber, (long) chronometer);
      int writeTSSucess = ThingSpeak.writeFields( idChannel, writeAPIKey);
      if(writeTSSucess == 200){
         leds[49] = CRGB::White;
         FastLED.show();
         delay(500);
         leds[49] = CRGB::Black;
         FastLED.show();
         delay(500);
      }
      
      Serial.print("<F> MQTT: ");      //////////////????????? depuração ?????????
      Serial.print(idDigital);
      Serial.print(" :::::::::::: ");
      Serial.print(colorBefore);  
      Serial.print(" - ");
      Serial.print(colorIndiceBefore);
      Serial.print(" ======== ");
      Serial.println(chronometer);

}


/////////////////////////////////<G>/////////////////////////////////////////
//Função encarregada de registrar no display da placa (OLED) a cor corrente e tempo de cronômetro na respectiva cor.   
void displayChronometer(String colorCurrent) {
    chronometer = (millis() - beginTime)/1000;
     //////////////????????? depuração ?????????
    Serial.print("<G> Display OLED: ");
    Serial.print(colorCurrent);
    Serial.print("<--------------> ");
    Serial.println(chronometer); 

    display.clearDisplay();
    //
    display.setCursor(0, OLED_LINE1);
    display.println("@CheerLights!");
    //
    display.setCursor(0, OLED_LINE2);
    display.println("Saberes&Afazeres");
    //
    display.setCursor(0, OLED_LINE3);
    display.println("<--------------> ");
    //
    display.setCursor(0, OLED_LINE4);
    display.print("id: ");
    //display.print(idDigitalHigh);                  ///!!!!!!! (idDigital) assim ... nem compila o sketch
    display.println(idDigital);                       
    //
    display.setCursor(0, OLED_LINE5);
    display.print("cor: ");
    display.println(colorCurrent);
    //
    display.setCursor(0, OLED_LINE6);
    display.print("Tempo: ");
    display.print(chronometer);
    display.println(" seg");
    //
    display.display();
}
