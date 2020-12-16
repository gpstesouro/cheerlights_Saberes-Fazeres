/*
  https://www.cheerlights.com
  Saberes&Afazeres
  ++++++++++++++++++++++++++++++++++++++++++++
  Descrição: - v.1: vamos lá, sei que você não se contentará com a monotonia de uma única cor (monocromático). Façamos dois tipos de registros históricos: 
                   (i) um visual (fita de leds) com os 48 últimos registros de cores amostrados (discretizados) a cada 10 segundos; e 
                   (ii) prover dados para um banco de dados com a cor e o respectivo intervalo (tempo de cronômetro) de permanência da cor inalterada;
             - v.1.1: display do ESP32 com a indicação da cor e tempo de exposição da respectiva cor; 
             - v.1.2: captação de um idDigital, tal como uma "assinatura em livro de visita" daqueles que instalarem e rodarem o sketch em suas placas;
             - v.1.3: utilização do FastLED v.3.3.3, ThingSpeak (receber a cor da vez "tweetada" pelo além da Internet e analisar os registros) e MQTT (idDigital e publicar registros); e
             - v.2: dashboard WEB (Qnt de idDigitais, corCurrent e gráfico de histórico). 
  
  Hardware: placa Heltec WIFI ESP32 V2, cordão ou fita de leds (atentar quanto a voltagem e pinagem). Nesta experimentação foi utlizado o modelo WS2811 - 12 Volts (50 leds).
   
  !!! IMPORTANTE: (i) modifique o arquivo secrets.h para o seu projeto, com sua conta e senha de conexão a rede e detalhes de configuração: ThingSpeak (channel) e MQTT!!!
  
  Note:
  - Requires installation of EPS32 core. See https://github.com/espressif/arduino-esp32/blob/master/docs/arduino-ide/boards_manager.md for details. 
  - Select the target hardware from the Tools->Board menu
  - This example is written for a network using WPA encryption. For WEP or WPA, change the WiFi.begin() call accordingly.
  
  >ThingSpeak ( https://www.thingspeak.com ) is an analytic IoT platform service that allows you to aggregate, visualize, and 
  analyze live data streams in the cloud. Visit https://www.thingspeak.com to sign up for a free account and create a channel.  
  Documentation for the ThingSpeak Communication Library for Arduino is in the README.md folder where the library was installed.
  See https://www.mathworks.com/help/thingspeak/index.html for the full ThingSpeak documentation.
  For licensing information, see the accompanying license file. Copyright 2018, The MathWorks, Inc.

  >FastLED

  >Adafruit

  >MQTT

*/

///////////////////////////////////////////////////////////////////////////////////////////////
#include <Wire.h>              //<<D>
#include <Adafruit_GFX.h>      
#include <Adafruit_SSD1306.h>  //<<D>

#include "ThingSpeak.h"
#include "secrets.h"
#include "FastLED.h"
#include <WiFi.h>

char ssid[] = SECRET_SSID;     // your network SSID (name) 
char pass[] = SECRET_PASS;     // your network password
//int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;

// CheerLights channel details
unsigned long cheerlightsChannelNumber = SECRET_CH_ID_COLOR;
unsigned int corFieldNumber = 1; // Fields: 1 (cor) ou 2 (#código)

String color;       // variável que acolhe a consulta ao ThingSpeak sobre o status cheerlights
String colorBefore; // variável global (armazena a cor antecedente)
unsigned long beginTime, chronometer;  //<C><D>

/* Endereço I2C do display */   //<<<D>
#define OLED_ADDR   0x3c
/* distancia, em pixels, de cada linha em relacao ao topo do display */
#define OLED_LINE1     0
#define OLED_LINE2     10
#define OLED_LINE3     20
#define OLED_LINE4     30
#define OLED_LINE5     40
/* Configuração da resolucao do display (este modulo possui display 128x64) */
#define SCREEN_WIDTH    128 
#define SCREEN_HEIGHT   64     ///<<<D>

/*Propriedades da fita ou cordão de leds*/ 
#define NUM_LEDS 50
#define DATA_PIN 13
/* Configuração da intensidade de brilho*/
#define BRIGHTNESS 32 //(0 ... 255)

/* Objeto do display */
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, 16); //<D>

/* Objeto do leds - arrays */
CRGB leds[NUM_LEDS];
CRGB ledsMirror[NUM_LEDS];


/////////////////////////////////////setup//////////////////////////////////////////////////////////
void setup() {
  Serial.begin(115200);  // Initializa serial
  beginTime = millis();  // Dispara o cronômetro

  /* inicializa display OLED */
  Wire.begin(4, 15);
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.clearDisplay();
  
  WiFi.mode(WIFI_STA);   
  ThingSpeak.begin(client);  // Initializa ThingSpeak

  FastLED.addLeds<WS2811,DATA_PIN>(leds,NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.clear();
  }

/////////////////////////////////////loop//////////////////////////////////////////////////////////////

void loop() {

  int statusCode = 0;
  
  // Connect or reconnect to WiFi
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass); // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);     
    } 
    Serial.println("\nConnected");
  }

  // Read in field 1 ou 2 of the public channel recording the color
  String color = ThingSpeak.readStringField(cheerlightsChannelNumber, corFieldNumber);
   
  // Check the status of the read operation to see if it was successful
  statusCode = ThingSpeak.getLastReadStatus();
  if(statusCode == 200){
    leds[0] = CRGB::White;
    FastLED.show();
     //////////////????????? depuração ?????????
    Serial.println("Consulta a cor pelo ThingSpeak - Ok: " + String(color));
    
    stripLeds(color); 
  }
  else{
    leds[0] = CRGB::Black;
    FastLED.show();
    Serial.println("Problem reading channel. HTTP error code " + String(statusCode)); 
  }
  
  delay(10000); // frequência de verificação da cor.
}


/////////////////////////////////////<A>/////////////////////////////////////////////////////////////////
//Função que recebe a "color" obtida do ThingSpeak e lança no painel. Também realiza o espelahmento da informação de cor no array ledsMirror[]. 
void stripLeds (String colorCurrent){
  
////////////////////////////
  static int i;
  (i < NUM_LEDS-2)?i++:i=1;
  //////////////????????? depuração ?????????
 //Faça uma experiência, retire o "static" da declaração da variável "i" e verifique o resultado.
  Serial.print("(i): ");
  Serial.println(i);
////////////////////////////

  if (colorCurrent == "white")
    {  
           
        leds[i] = CRGB::White;
        ledsMirror[i] = leds[i];
        leds[(i != NUM_LEDS-2)?i+1:1] = CRGB::Black;
        FastLED.show();
        delay(30);
      
    }
    else if (colorCurrent == "red")
    {  
      
        leds[i] = CRGB::Red;
        ledsMirror[i] = leds[i];
        leds[(i != NUM_LEDS-2)?i+1:1] = CRGB::Black;
        FastLED.show();
        delay(30);
    
    }
   else if (colorCurrent == "green")
    {  
      
        leds[i] = CRGB::Green;
        ledsMirror[i] = leds[i];
        leds[(i != NUM_LEDS-2)?i+1:1] = CRGB::Black;
        FastLED.show();
        delay(30);
      
    }
    else if (colorCurrent == "blue")
    {  
      
        leds[i] = CRGB::Blue;
        ledsMirror[i] = leds[i];
        leds[(i != NUM_LEDS-2)?i+1:1] = CRGB::Black;
        FastLED.show();
        delay(30);
     
    }
    else if (colorCurrent == "cyan")
    {  
      
        leds[i] = CRGB::Cyan;
        ledsMirror[i] = leds[i];
        leds[(i != NUM_LEDS-2)?i+1:1] = CRGB::Black;
        FastLED.show();
        delay(30);
      
    }
    else if (colorCurrent == "magenta")
    {  
      
        leds[i] = CRGB::Magenta;
        ledsMirror[i] = leds[i];
        leds[(i != NUM_LEDS-2)?i+1:1] = CRGB::Black;
        FastLED.show();
        delay(30);
    
    }
    else if (colorCurrent == "yellow")
    {  
      
        leds[i] = CRGB::Yellow;
        ledsMirror[i] = leds[i];
        leds[(i != NUM_LEDS-2)?i+1:1] = CRGB::Black;
        FastLED.show();
        delay(30);
     
    }
    else if (colorCurrent == "purple")
    {  
      
        leds[i] = CRGB::Purple;
        ledsMirror[i] = leds[i];
        leds[(i != NUM_LEDS-2)?i+1:1] = CRGB::Black;
        FastLED.show();
        delay(30);
      
    }
    else if (colorCurrent == "orange")
    {  
      
        leds[i] = CRGB::Orange;
        ledsMirror[i] = leds[i];
        leds[(i != NUM_LEDS-2)?i+1:1] = CRGB::Black;
        FastLED.show();
        delay(30);
     
    }
    else if (colorCurrent == "warmwhite")
    {  
      
        leds[i] = CRGB::OldLace;
        ledsMirror[i] = leds[i];
        leds[(i != NUM_LEDS-2)?i+1:1] = CRGB::Black;
        FastLED.show();
        delay(30);

    }
    else if (colorCurrent == "oldlace")
    {  
     
        leds[i] = CRGB::OldLace;
        ledsMirror[i] = leds[i];
        leds[(i != NUM_LEDS-2)?i+1:1] = CRGB::Black;
        FastLED.show();
        delay(30);
     
    }
    else if (colorCurrent == "pink")
    {  
      
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
      registerChronometer();    /////// publisher no mosquitto
  } 
  else {
      displayChronometer(colorCurrent);  ///// amostragem no tempo de pemaneência da cor no display do ESP32
  }

  colorBefore = colorCurrent;
 //////////////????????? depuração ?????????
   Serial.print(colorCurrent);
   Serial.println(" - final do processo: registrado como colorBefore");
   Serial.println(".................................................................");
}

/////////////////////////////////////<B>//////////////////////////////////////////
//Função que anuncia no cordão de leds o momento da mudança de cor, fazendo uma passagem (leds 1:48) unicolor na respectiva cor. 
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

/////////////////////////////////<C>/////////////////////////////////////////
//Função encarregada de registrar no broker MQTT o intervalo (tempo de cronômetro) que a respectiva cor permaneceu inalterada.   
void registerChronometer() {
      chronometer = millis() - beginTime;
       //////////////????????? depuração ?????????
      Serial.print("<C> MQTT: ");
      Serial.print(colorBefore);  
      Serial.print(" ======== ");
      Serial.println(chronometer/1000);

      beginTime = millis(); // Dispara o cronômetro
}


/////////////////////////////////<D>/////////////////////////////////////////
//Função encarregada de registrar no dispaly da placa a cor corrente e tempo lisetada na mesma cor.   
void displayChronometer(String colorCurrent) {
    chronometer = millis() - beginTime;
     //////////////????????? depuração ?????????
    Serial.print("<D> Display: ");
    Serial.print(colorCurrent);
    Serial.print(" ++++++++ ");
    Serial.println(chronometer/1000); 

    display.clearDisplay();
    //
    display.setCursor(0, OLED_LINE1);
    display.println("@CheerLights!");
    //
    display.setCursor(0, OLED_LINE2);
    display.println("Saberes&Afazeres");
    //
    display.setCursor(0, OLED_LINE3);
    display.println("----------------");
    //
    display.setCursor(0, OLED_LINE4);
    display.print("cor: ");
    display.println(colorCurrent);
    //
    display.setCursor(0, OLED_LINE5);
    display.print("Tempo: ");
    display.print(chronometer/1000);
    display.println(" seg");
    //       
    display.display();
}
