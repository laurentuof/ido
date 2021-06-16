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
//  LA commande "e" permet de connaitre l'état de la connection avec le réseau
//            0 : pas de connection au reseau
//


#include <SoftwareSerial.h>


#define GSM_KEY 10
#define GSM_RX 8
#define GSM_TX 9
#define GSM_PS 11
#define GSM_NS 12
#define ERREUR 9999
#define GSM_NOCONNECT       0
#define GSM_CONNECTED       1
#define GSM_HTTPCONNECT     2
#define GSM_TMIN_NOCONNECT  700
#define GSM_TMAX_NOCONNECT  900
#define GSM_TMIN_CONNECTED 2700
#define GSM_TMAX_CONNECTED  3300
#define GSM_TMIN_HTTPCONNECT  200
#define GSM_TMAX_HTTPCONNECT  400


String gsm_pin = "xxxx";

// instanciation de la liaison série avec la carte FONA
SoftwareSerial GSM_Serial(GSM_TX, GSM_RX); // RX, TX


void setup() {
  
  delay(1000);
   Serial.begin(115200);
   while (!Serial) {
    delay(1); //attend que le port série "USB" se connecte
   }
   Serial.println ("------------------------------------");
   Serial.println ("Prêt !");
   Serial.println ("");

   
   // initialisation de la liaison série avec la carte FONA
   GSM_Serial.begin(9600);
   // initialisation de la broche de télécommande ON/OFF pour le module FONA
   digitalWrite(GSM_KEY,HIGH);
   pinMode (GSM_KEY,OUTPUT);
   pinMode (GSM_PS, INPUT);
   pinMode(GSM_NS, INPUT);

   
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
    if (a ==69 or a== 101) { // E ou e état de la connection réseau
      Serial.println (gsm_connected());
    }
  }
}


// ---------------------------------------------------------------------
//     FONCTIONS



byte gsm_connected() {
  // retourne true si on est connecté au réseau
  // retourne false si on n'est pas connecté au réseau
  Serial.println ("Vérification de la connection au réseau ...");
  //attend que le niveau passe à 1
  while (digitalRead(GSM_NS)==LOW) {
    delay(100);
  }
  while (digitalRead(GSM_NS)==HIGH) {
    delay(1);
  }
  //declenche le compteur
  unsigned long debut = millis();
  while (digitalRead(GSM_NS)==LOW) {
    delay(1);
  }
  //arrête le compteur
  unsigned long fin = millis();
  unsigned long duree = fin-debut;
  //

  if (duree >= GSM_TMIN_NOCONNECT && duree <= GSM_TMAX_NOCONNECT) {
    return GSM_NOCONNECT;
  }
   if (duree >= GSM_TMIN_CONNECTED && duree <= GSM_TMAX_CONNECTED) {
    return GSM_CONNECTED;
  }
  if (duree >= GSM_TMIN_HTTPCONNECT && duree <= GSM_TMAX_HTTPCONNECT) {
    return GSM_HTTPCONNECT;
  }
  return ERREUR;
}


void gsm_connect_to_network() {
  Serial.println ("Envoi code PIN");
  delay(2000);
  GSM_Serial.println("AT+CPIN="+gsm_pin);
  delay(500);
}


bool gsm_on() {
  if (!etat_gsm()) {
    // Fona sur OFF , commande l'arrêt
    Serial.println ("   application commande marche ...");
    gsm_impulsion_on_off();
    while (!etat_gsm()) {
      delay(10);    // mettre un time out éventuel
    }
    gsm_connect_to_network();
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
