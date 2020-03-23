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

//https://github.com/shurillu/CTBot/blob/master/examples/inlineKeyboard/inlineKeyboard.ino

#include "CTBot.h"
#include <SPI.h>
#include <MFRC522.h>

#define LED_RFID   D0
#define BOTON_PIN  D4
#define LED_BUILTIN D2
#define RST_PIN    D3 
#define RELE       D1   
#define SS_PIN     D8  

CTBot myBot;

MFRC522 mfrc522(SS_PIN, RST_PIN); 

String ssid = "";   
String pass = ""; 
String token = "";
long id = ;  //chat_id

int BanderaInicio=0, BanderaBoton=0;
volatile bool BanderaBotonPresionado = false;


void setup() {

        pinMode(RELE, OUTPUT);
        digitalWrite(RELE,HIGH);
        Serial.begin(9600);
   
        pinMode(BOTON_PIN, INPUT_PULLUP);
        pinMode(LED_BUILTIN, OUTPUT);
        
        attachInterrupt(BOTON_PIN, BotonPresionado, FALLING);
  
        myBot.wifiConnect(ssid, pass);
  
        myBot.setTelegramToken(token);

  
        if (myBot.testConnection()){
              Serial.println("Conectado a la Red Wifi");
              digitalWrite(LED_BUILTIN, HIGH);
              }  else{
                      Serial.println("Error al Conectar a la Red");
                      digitalWrite(LED_BUILTIN, LOW);}

        SPI.begin();      
        mfrc522.PCD_Init();   
        Serial.println("Aproxime La Tarjeta al Lector...");
        Serial.println();
        pinMode(LED_RFID, OUTPUT);
        digitalWrite(LED_RFID,LOW);             

 
} // FIN SETUP

void loop() {
   
    if (mfrc522.PICC_IsNewCardPresent()){if (mfrc522.PICC_ReadCardSerial()) { manejo_tarjeta();  } }
    if (BanderaInicio == 0){ mensaje_inicio ();}
    if ( BanderaBotonPresionado ) {  envio_mensaje ();} 

} //FIN LOOP


void BotonPresionado() {    
                        int button = digitalRead(BOTON_PIN);
                        if(button == LOW && BanderaBoton == 0)
                          {
                          BanderaBotonPresionado = true; 
                          Serial.println("Boton Presionado");
                          digitalWrite(LED_RFID, HIGH);
                          BanderaBoton = 1;
                          }                                                    
                        return;
                       } // FIN  BOTON PRESIONADO 
             
void mensaje_inicio (){
                          Serial.println("Iniciado...");
                          Serial.println("Enviando Mensaje de Inicio ....."); 
                          myBot.sendMessage(id, "Sistema Timbre Iniciado....");
                          Serial.println("Mensaje Enviado"); 
                          Serial.println(" "); 
                          BanderaInicio=1;
                          
  
} // FIN MENSAJE INICIO


void envio_mensaje (){
                        Serial.println("Enviando Mensaje.....");  
                        BanderaBotonPresionado = false;
                        myBot.sendMessage(id, "Timbre");
                        Serial.println("Mensaje Enviado");
                        Serial.println(" "); 
                        BanderaBoton = 0;
                        digitalWrite(LED_RFID, LOW);
                                         
} // FIN ENVIO MENSAJE             


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
                       contenido.substring(1) == "xx xx xx xx" ||
                       contenido.substring(1) == "xx xx xx xx" || 
                       contenido.substring(1) == "xx xx xx xx" || 
                       contenido.substring(1) == "xx xx xx xx" || 
                       contenido.substring(1) == "xx xx xx xx" 
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
                         Serial.println("Enviando Mensaje de intento de ingreso Fallido");
                         Serial.println();
                         myBot.sendMessage(id, "ALERTA *** Intento de Acceso ");
                         
                        }

}   // FIN MANEJO TARJETA
