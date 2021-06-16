/*
*   Connection FireBeetle / FONA miniGSM
*
*     Vcc                       Vio
*     GND                       GND  
*     GND                       Key
*     IO25                      TX
*     IO26                      RX
*
*
*           Envoi d'un simple SMS
*           en utilisant les commandes AT
*
*       Sans utiliser la bibliothèque Adafruit FONA
*
*
*
*
*/


// bibliothèque pour la création d'un deuxième port série.
// le premier étant utilisé par la liaison USB il est nécessaire de créer un
// deuxième port série pour y connecter la carte FONA
#include <HardwareSerial.h>

// définition des broches Rx et Tx de ce nouveau port série
#define RXD2 25
#define TXD2 26
 
// ---------------------------------------------------------------
// Initialisation
// ---------------------------------------------------------------
void setup() {
  // iniatialisation de la liaison avec le PC (console de l'IDE arduino)
  Serial.begin(115200);
  // initialisation de la liaison avec la carte FONA
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
}

// ---------------------------------------------------------------
//Programme principal
// ---------------------------------------------------------------
void loop() { //Choose Serial1 or Serial2 as required
  les deux boucles suivantes pour lire les données entrant sur un port série et les écrire sur l'autre port
  while (Serial.available()) {
    Serial2.print(char(Serial.read()));
  }
  while (Serial2.available()) {
    Serial.print(char(Serial2.read()));
  }
  
  // envoi des commandes au module FONA
  // configuration du module en mode "Texte"
  Serial2.println("AT+CMGF=1");
  // 
  delay(100);
  Serial2.println("AT+CMGS=\"0000000000\"");      // remplacer par le numéro du téléphone auquel envoyer le SMS
  delay(100);
  Serial2.println(" Bonjour le monde ! ");
  delay(100);
  Serial2.println((char)26);
  delay(5000);
  
}
