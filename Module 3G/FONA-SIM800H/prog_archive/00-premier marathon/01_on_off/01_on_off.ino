//
//  Arduino Micro / FONA miniGSM
//
//  Utilisation d'une carte GSM FONA avec un arduino Micro
//
//  --------------------------------------------------
//
//    Cablage
//
//        FONA     Micro
//          VIO     5v
//          BAT     + d'une batterie extérieure (3.7V par exemple)
//          GND     GND
//          Key     D10
//          Rx      D8
//          Tx      D9
//          PS      D11
//          
// ----------------------------------------------------          
//      Fonctionalités
//
// Scrutation du port série (console) en attente d'une commande d'arrêt ("A" ou "a") ou marche ("m" ou "m")
//    Vérification de l'état (ON ou OFF) de la carte FONA
//    si la demande est cohérente (demande de mise en marche si la carte est à l'arrêt)
//        envoi de la commande correspondante.
//
//
//
//


#include <SoftwareSerial.h>


#define GSM_KEY 10
#define GSM_RX 8
#define GSM_TX 9
#define GSM_PS 11



// instanciation de la liaison série avec la carte FONA
SoftwareSerial GSM_Serial(GSM_TX, GSM_RX); // RX, TX




void setup() {
  
  delay(1000);
   Serial.begin(115200);
   while (!Serial) {
    delay(1); //attend que le port série "USB" se connecte
   }

   
   // initialisation de la liaison série avec la carte FONA
   GSM_Serial.begin(9600);
   // initialisation de la broche de télécommande ON/OFF pour le module FONA
   digitalWrite(GSM_KEY,HIGH);
   pinMode (GSM_KEY,OUTPUT);
   pinMode (GSM_PS, INPUT);

   
}
  
void loop() {
  // vérifie si l'utilisateur à tapé une commande ("A" ou "M")
  if (Serial.available()) {
    char a = Serial.read();
    if (a ==65 or a == 97) { // A ou a arrêt 
      gsm_off();
    }
    if (a ==77 or a== 109) { // M ou m mise en marche
      gsm_on();
    }
  }
}


// ---------------------------------------------------------------------
//     FONCTIONS



bool gsm_on() {
  if (!etat_gsm()) {
    // Fona sur OFF , commande l'arrêt
    Serial.println ("   application commande marche ...");
    gsm_impulsion_on_off();
  }
}

bool gsm_off() {
  if (etat_gsm()) {
    // Fona sur OFF , commande l'arrêt
    Serial.println ("   application commande arrêt ...");
    gsm_impulsion_on_off();
  }
}


void gsm_impulsion_on_off () {
  // génère une impulsion LOW de 2 secondes pour activer ou desactiver
  //  la carte FONA
  digitalWrite(GSM_KEY,LOW);
  delay(2000);
  digitalWrite(GSM_KEY,HIGH);
}

bool etat_gsm() {
  // renvoie true si la carte FONA est ON
  // renvoie false si la carte FONA est OFF
  if (digitalRead(GSM_PS) == HIGH) {
    return true;
  } else {
    return false;
  }
}
