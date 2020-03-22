/*//////////////////////// CONEXION///////////////////////////////////
  
      Nodemcu       //     RFID-RC522 
       D3 [1*]      GPIO-0      //     RST/Reset RST  //     Marron
       D8 [2*]      GPIO-15     //     SPI SS  SDA[3] //     Amarillo 
       D7           GPIO-13     //     SPI MOSI MOSI  //     Verde
       D6           GPIO-12     //     SPI MISO MISO  //     Violeta
       D5           GPIO-14     //     SPI SCK SCK    //     Gris
       -----------------------------------------------
       D4           Pulsador                         //   Celeste-Naranja
       D0           Led_Rfid                         //   Blanco
       D2           Led_Wifi
       D1           Rele
       3v                                            //  Rojo
       GND                                           // Azul


       --------------------------------------------

              PINES DE LA PLAQUETA TIMBRE                     //               PINES DE BOT TELEGRAM                   
 
       D3                               MARRON                //               D1     LED WIFI
       D5                               GRIS                  //               D2     PUERTA 2    
       D6                               VIOLETA               //               D5     PUERTA 1 
       D7                               VERDE                 //               D7     PORTON
       D8                               AMARILLO
       3V                               ROJO
       LED                              BLANCO
       PULSADOR _ RETORNO               NARANJA
       NEGATIVO Y PULSADOR              CELESTE Y AZUL
/////////////////////////////////////////////////////////////////////*/



#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <SPI.h>
#include <MFRC522.h>

#define LED_RFID   D0
#define BOTON_PIN  D4
#define LED_BUILTIN D2
#define RST_PIN    D3 
#define RELE       D1   
#define SS_PIN     D8  
 

MFRC522 mfrc522(SS_PIN, RST_PIN); 
// Inicializamos la conexion WIFI con el Router
char ssid[] = "";     // el nombre de tu Red
char password[] = ""; // la clave de tu Red
const char  id_chat[]="";
const char respuesta[] = "Timbre" ,alerta[] = "ALERTA *** Intento de Acceso " , iniciado[]="Sistema Iniciado....";
int BanderaInicio=0, BanderaBoton=0;
volatile bool BanderaBotonPresionado = false;

// Initialize Telegram BOT
#define BOTtoken ""  // el token de tu BOT, lo obtenemos de BotFather


WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

void setup() {
        pinMode(RELE, OUTPUT);
        digitalWrite(RELE,HIGH);
        Serial.begin(9600);
        // Establecer el modo WiFi y desconectarse de un AP si fue Anteriormente conectada
        WiFi.mode(WIFI_STA);
        WiFi.disconnect();
        delay(100);
        // Intentar conectarse a la red
        Serial.print("Conectando a la Red Wifi: ");
        Serial.println(ssid);
        WiFi.begin(ssid, password);        
        while (WiFi.status() != WL_CONNECTED) {Serial.print("."); delay(500);}        
        Serial.println("");
        Serial.println("WiFi conectada");   
                Serial.print("IP address: ");
        Serial.println(WiFi.localIP());    
        pinMode(BOTON_PIN, INPUT_PULLUP);
        pinMode(LED_BUILTIN, OUTPUT);
        
        attachInterrupt(BOTON_PIN, BotonPresionado, FALLING);
        SPI.begin();      
        mfrc522.PCD_Init();   
        Serial.println("Aproxime La Tarjeta al Lector...");
        Serial.println();
        pinMode(LED_RFID, OUTPUT);
        digitalWrite(LED_RFID,LOW);
        
        
      } // Fin Setup


                      
void loop() { 
              if (mfrc522.PICC_IsNewCardPresent()){if (mfrc522.PICC_ReadCardSerial()) { manejo_tarjeta();  } }
              if (BanderaInicio == 0){ mensaje_inicio ();}
              if ( BanderaBotonPresionado ) {  envio_mensaje ();} 
              if (WiFi.status() == WL_CONNECTED) {
                            digitalWrite(LED_BUILTIN, HIGH);
                                        } else {
                                                 digitalWrite(LED_BUILTIN, LOW);
                                                  WiFi.disconnect();
                                                   delay(100);
                                                   // Intentar conectarse a la red
                                                  Serial.print("Conectando a la Red Wifi: ");
                                                  Serial.println(ssid);
												  WiFi.begin(ssid, password);        
												  while (WiFi.status() != WL_CONNECTED) {Serial.print("."); delay(500);}        
												  Serial.println("");
												  Serial.println("WiFi conectada");   
												  		Serial.print("IP address: ");
												  Serial.println(WiFi.localIP());    
        
                                                 }
              }// FIN LOOP

void BotonPresionado() {    
                        int button = digitalRead(BOTON_PIN);
                        if(button == LOW && BanderaBoton == 0)
                          {
                          BanderaBotonPresionado = true; 
                          Serial.println("Boton Presionado");
                          BanderaBoton = 1;
                          }                                                    
                        return;
                       } // FIN  Boton Presionado        
        
void mensaje_inicio (){
  
                          Serial.println("Iniciado...");
                          Serial.println("Enviando Mensaje de Inicio ....."); 
                          bot.sendMessage(id_chat, iniciado);
                          Serial.println("Mensaje Enviado"); 
                          Serial.println(" "); 
                          BanderaInicio=1;
                          
  
} // FIN Mensaje de inicio
        
void envio_mensaje (){
                        digitalWrite(LED_RFID, HIGH);
                        Serial.println("Enviando Mensaje.....");  
                        BanderaBotonPresionado = false;
                        bot.sendMessage(id_chat, respuesta);
                        Serial.println("Mensaje Enviado");
                        Serial.println(" "); 
                        BanderaBoton = 0;
                        digitalWrite(LED_RFID, LOW);
                                         
} // FIN Envio de Mensaje    

void manejo_tarjeta(){
                      String contenido= "";
                      Serial.print("Codigo de Tarjeta :");
                      for (byte i = 0; i < mfrc522.uid.size; i++) {
                                                                    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
                                                                    Serial.print(mfrc522.uid.uidByte[i], HEX);
                                                                    contenido.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
                                                                    contenido.concat(String(mfrc522.uid.uidByte[i], HEX));
                                                                  }
                   Serial.println();
                   Serial.print("Mensaje : ");
                   contenido.toUpperCase();
                   if (
                       contenido.substring(1) == "XX XX XX XX" ||
                       contenido.substring(1) == "XX XX XX XX" || 
                       contenido.substring(1) == "XX XX XX XX" || 
                       contenido.substring(1) == "XX XX XX XX" || 
                       contenido.substring(1) == "XX XX XX XX" 
                       ) {
                                                  Serial.println("Abiertoo--------------- Bienvenido !!!!");
                                                  Serial.println();
                                                  digitalWrite(RELE,LOW);
                                                  digitalWrite(LED_RFID, HIGH);    
                                                  delay(1000);             
                                                  digitalWrite(LED_RFID, LOW); 
                                                  digitalWrite(RELE,HIGH);
                                                                } 
                   else{
                         Serial.println("Juiira Gatooo");
                         Serial.println();             
                         digitalWrite(LED_RFID, HIGH);
                         delay(100);           
                         digitalWrite(LED_RFID, LOW); 
                         delay(100);
                         digitalWrite(LED_RFID, HIGH);
                         delay(100);           
                         digitalWrite(LED_RFID, LOW); 
                         delay(100);
                         digitalWrite(LED_RFID, HIGH);
                         delay(100);           
                         digitalWrite(LED_RFID, LOW); 
                         Serial.println("Mensaje de intento de ingreso Fallido");
                         Serial.println();
                         bot.sendMessage(id_chat, alerta);
                        }

}   // FIN MANEJO TARJETA