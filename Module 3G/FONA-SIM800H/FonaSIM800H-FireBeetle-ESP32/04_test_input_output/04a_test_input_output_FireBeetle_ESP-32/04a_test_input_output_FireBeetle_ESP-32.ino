/**
 * --------------------------------------------------
 *                 04_test_input_output
 *                 
 * - Ce programe est un test intermédiaire des port 
 * Serie, A noter que le loop back ne foncttione pas avec les:
 *        - SoftwareSerial (Emulation de port serie)
 *        - HardwareSerial (Relocalisation de port serie)
 * D'ou lutilisation d'une arsuino Uno pour recevoir et
 * renvoyer les information qu passe sur les port serie.
 * Ce dernier a également été utiliser 
 * pour tester les entré sortie du FireBeetle.
 * 
 * - By Laurent Marchal & Arthur Pignalet
 * 
 * - DATE : juin 2021
 * 
 * - licence d'utilisation : CC-BY-SA
 * --------------------------------------------------   
 *                   
 *                   Test laison Serie : FireBeetle <-> Fona
 *                    
 * ------------------------------------------------------------------         
 * NOTE : !TEST EFFECTUER SANS CARTE SIM DANS LE MODULE Fona!
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
 * Ce programe permet de vérifier en mettant dans le champs de saisie 
 * du moniteur serie les commande AT tel que AT qui renvoie ok ou
 * d'autre commande permetant de récupérer des information du Fona
 * 
 * liste de commande AT: LIEN : 
 * https://www.technologuepro.com/gsm/commande_at.htm
 * 
 * ------------------------------------------------------------------
 * 
 *                    UART_Mapping FireBeetle
 * 
 * ATTENTION! n'utiliser que les UART dit "libre"
 * 
 * #define UART0   0 // (rx) serial 0  utiliser pour la programation
 * #define UART0   1 // (tx) serial 0  utiliser pour la programation
 * #define UART1   5 // (rx) serial 1  utiliser pour le flash
 * #define UART1   6 // (tx) serial 1  utiliser pour le flash
 * #define UART2  16 // (rx) serial 2  libre
 * #define UART2  17 // (tx) serial 2  libre
 * 
 * ------------------------------------------------------------------ 
 * 
 *                    PinMapping
 *        
 *    FONA     FireBeetle
 *     
 *     VIO     5v
 *     BAT     + d'une batterie extérieure (3.7V par exemple)
 *     Tx      A0 (rx)
 *     Rx      A1 (tx)
 *     Key     GND
 *     
 * ------------------------------------------------------------------
 * 
 *                    Fonction
 * 
 * initialisation : boche utiliser
 *                  Liaison UART0 pour l'USB
 *                  Liaison Uart2 pour le module Fona
 *                  
 * Boucle : on envoie sur la liaison Uart2 ce que l'on li sur l'Uart0
 *          puis,
 *          on envoie sur la liaison Uart0 ce que l'on li sur l'Uart2
 *          
 * ------------------------------------------------------------------ 
 * 
 */         
#include <HardwareSerial.h>

#define GSM_RX 39 //A1
#define GSM_TX 15 //A4
#define Serial_baud_rate 115200
#define Ax 15 //a0 = 36 ; a2 =34, a3=35

//HardwareSerial FonaSerial(1);

void setup() {
  
  //USB
  Serial.begin(Serial_baud_rate);
  while (!Serial) {;/*attend que le port série "USB" se connecte*/}
  Serial.println("");
  Serial.println("----------------------------------");
  Serial.println("               SETUP              ");
  Serial.println("");
  Serial.println("Serial 1 (USB)  : OK");
      
  //FONA
//  FonaSerial.begin(Serial_baud_rate,SERIAL_8N1,GSM_RX,GSM_TX);
//  while (!FonaSerial) {;Serial.println("FONA = non available");/*attend que le port série "USB" se connecte*/}
 // Serial.println("SoftSerial (Fona) : OK");
  
  //test des sortie analogique output
  pinMode(Ax,OUTPUT);
  
  Serial.println("");
  Serial.println("----------------------------------");
}

void loop() {
/*
   while (Serial.available()) {
     FonaSerial.print(char(Serial.read()));
   }
   while (FonaSerial.available()) {
     Serial.println(FonaSerial.read());
   }
   */
  digitalWrite(Ax,HIGH);
  delay(250);
  digitalWrite(Ax,LOW);
  delay(250);
}
