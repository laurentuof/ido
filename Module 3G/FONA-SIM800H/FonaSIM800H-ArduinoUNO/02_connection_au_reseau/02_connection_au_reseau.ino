/** 
 *  --------------------------------------------------
 *                 02_connection_au_reseau
 *                 
 * - Ce programe ajoute au programe precedent une commande
 * manuel de connection au reseau GSM du module FONA MniGSM via 
 * Arduino Micro.
 * 
 * - By Laurent Marchal & Arthur Pignalet
 * 
 * - DATE : juin 2021
 * 
 * - licence d'utilisation : CC-BY-SA
 * --------------------------------------------------
 *     Cablage
 *             
 *        FONA     Micro          
 *        VIO     5v          
 *        BAT     + d'une batterie extérieure (3.7V par exemple)          
 *        GND     GND          
 *        Key     D5          
 *        Rx      D2          
 *        Tx      D3          
 *        RES     D4          
 *        PS      D6          
 *        NS      D7          
 *         
 * ----------------------------------------------------      
 *                  Fonctionalités 
 *                  
 * Scrutation du port série (console) en attente d'une commande 
 * ne pas oublier de définir "pas de fin de ligne" dans la console Arduino ainsi que la vitesse de transmission : 115200          
 * Commandes :                  
 *                a ou A : Arrêt                  
 *                m ou M : Marche                  
 *                e ou E : vérification de l'état de connection au réseau                  
 *                c ou C : connection au réseau (envoi du PIN)
 *                    
 *  Vérification de l'état (ON ou OFF) de la carte FONA    
 *  si la demande est cohérente (demande de mise en marche si la carte est à l'arrêt)        
 *  envoi de la commande correspondante avec prise en compte du Power status pour vérifier l'état de la carte.  
 *  La commande "e" permet de connaitre l'état de la connection avec le réseau            
 *  0 : pas de connection au reseau
 *  
 */

#include <SoftwareSerial.h>

#define MAXMILLIS 4294967295

#define ON  1
#define OFF 0

#define GSM_KEY 5
#define GSM_RX 2
#define GSM_TX 3
#define GSM_PS 6
#define GSM_RES 4
#define GSM_NS  7

#define GSM_ERREUR             -1
#define GSM_NOTCONNECTED        0
#define GSM_CONNECTED           1
#define GSM_HTTPCONNECT         2
#define GSM_OFF                 3

#define GSM_TMIN_NOCONNECT    700
#define GSM_TMAX_NOCONNECT    900
#define GSM_TMIN_CONNECTED   2700
#define GSM_TMAX_CONNECTED   3300
#define GSM_TMIN_HTTPCONNECT  200
#define GSM_TMAX_HTTPCONNECT  400


String gsm_pin = "xxxx";

  int incomingByte = 0;


// instanciation de la liaison série avec la carte FONA
SoftwareSerial GSM_Serial(GSM_TX, GSM_RX); // RX, TX


// *********************************************************************
//     SETUP
// *********************************************************************


void setup() {
  

  Serial.begin(115200);
  delay(200);
  Serial.println ("------------------------------------");
  Serial.println ("Prêt !");
  Serial.println ("");

   
   // initialisation de la liaison série avec la carte FONA
   GSM_Serial.begin(9600);
   
   // initialisation de la broche de télécommande ON/OFF pour le module FONA
   digitalWrite(GSM_KEY,HIGH);
   pinMode (GSM_KEY,OUTPUT);
   // initialisation de la lecture du Power Status
   pinMode (GSM_PS, INPUT);
    // initialisation de la lecture du "network status"
   pinMode(GSM_NS, INPUT);

   
}



// *********************************************************************
//     LOOP
// *********************************************************************

void loop() {



  
  // vérifie si l'utilisateur à tapé une commande ("A" ou "M")
  if (Serial.available()) {
    char x = Serial.read();
    if (x ==65 or x == 97) { // A ou a arrêt 
      gsm(OFF);
    }
    if (x ==77 or x== 109) { // M ou m mise en marche
      gsm(ON);
    }
    if (x ==67 or x== 99) { // C ou C : connection au réseau
      gsm_connect_to_network();
    }
    if (x ==69 or x== 101) { // E ou e état de la connection réseau
      byte res = is_gsm_connected();
      if (res == GSM_ERREUR){
        Serial.println ("ERREUR DE CONNECTION");
      } else if (res == GSM_OFF) {
        Serial.println ("Carte hors tension");
      } else if (res == GSM_NOTCONNECTED) {
        Serial.println ("NON CONNECTE");
      } else if (res == GSM_CONNECTED) {
        Serial.println ("CONNECTE ====> Super !!!");
      } else if (res == GSM_HTTPCONNECT) {
        Serial.println ("Connecté en HTTP");
      } else {
        Serial.println ("ERREUR inconnue.");
        Serial.println (" Causes possibles : absence de carte SIM");
      }
    }
  }
}


// *********************************************************************
//     FONCTIONS
// *********************************************************************

//-----------------------------------------------
//
//



byte is_gsm_connected() {
  // retourne true si on est connecté au réseau
  //  
  // retourne false si on n'est pas connecté au réseau

  // vérifie si le module est sous tension. Sinon renvoie une erreur
  if (!etat_gsm()) {
    return GSM_OFF;
  }
  Serial.println ("Vérification de la connection au réseau ...");
  //attend que le niveau passe à 1
  while (digitalRead(GSM_NS)==LOW) {
    delay(1);
  }
  //attend qu'il repasse à 0
  while (digitalRead(GSM_NS)==HIGH) {
    delay(1);
  }
  //declenche le compteur
  unsigned long debut = millis();
  
  // on attend que le signal repasse à 1
  while (digitalRead(GSM_NS)==LOW) {
    delay(1);
  }
  //arrête le compteur
  unsigned long fin = millis();
  unsigned long duree = 0;
  if (fin < debut) {
    duree = (MAXMILLIS-debut)+fin;
  } else {
    duree = fin-debut;
  }
  //*
  Serial.print ("Debut : ");
  Serial.println (debut);
  Serial.print ("Fin   : ");
  Serial.println (fin);
  Serial.print ("Durée : ");
  Serial.println (duree);
  

  if (duree >= GSM_TMIN_NOCONNECT && duree <= GSM_TMAX_NOCONNECT) {
    return GSM_NOTCONNECTED;
  }
   if (duree >= GSM_TMIN_CONNECTED && duree <= GSM_TMAX_CONNECTED) {
    return GSM_CONNECTED;
  }
  if (duree >= GSM_TMIN_HTTPCONNECT && duree <= GSM_TMAX_HTTPCONNECT) {
    return GSM_HTTPCONNECT;
  }
  return GSM_ERREUR;
}


//-----------------------------------------------
//
//
void gsm_connect_to_network() {
  Serial.print ("Envoi code PIN : ");
  GSM_Serial.println("AT+CPIN="+gsm_pin);
  delay(500);
  Serial.println ("OK");
}




//-----------------------------------------------
//
//
bool gsm(int status) {
  if (status == ON) {
    if (!etat_gsm()) {
      // Fona sur OFF , commande la mise en marche
      Serial.println ("   application commande marche ...");
      // commande Key sur Low
      digitalWrite(GSM_KEY,LOW);
      while (!etat_gsm()){
        delay(1);
      }
      digitalWrite(GSM_KEY,HIGH);
      Serial.println ("     Module sous tension");
    }
    
  } else {
    if (etat_gsm()) {
      // Fona sur ON , commande l'arrêt
      Serial.println ("   application commande arrêt ...");
      digitalWrite(GSM_KEY,LOW);
      while (etat_gsm()){
        delay(1);
      }
      digitalWrite(GSM_KEY,HIGH);
      Serial.println ("     Module hors tension");
    }
  }  
}




//-----------------------------------------------
//
//
bool etat_gsm() {
  // renvoie true si la carte FONA est ON
  // renvoie false si la carte FONA est OFF
  if (digitalRead(GSM_PS) == HIGH) {
    return true;
  } else {
    return false;
  }
}
