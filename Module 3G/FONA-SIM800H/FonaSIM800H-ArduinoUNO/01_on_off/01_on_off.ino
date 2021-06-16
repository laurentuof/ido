/**
 * --------------------------------------------------
 * 
 *                 01_ON_OFF
 *                 
 * - Ce programe a pour objectif de tester une commande
 *   manuel de marche/arret du module FONA MniGSM via 
 *   Arduino Micro.
 * 
 * - By Laurent Marchal & Arthur Pignalet
 * 
 * - DATE : juin 2021
 * 
 * - licence d'utilisation : CC-BY-SA
 * --------------------------------------------------
 *     Cablage
 *             
 *             FONA     Micro          
 *             VIO     5v          
 *             BAT     + d'une batterie extérieure (3.7V par exemple)          
 *             GND     GND          
 *             Key     D5          
 *             Rx      D2
 *             Tx      D3 
 *             RES     D4
 *             PS      D6
 * --------------------------------------------------
 * 
 */

//
//  --------------------------------------------------
//
          
// ----------------------------------------------------          
//      Fonctionalités
//
// Scrutation du port série (console) en attente d'une commande d'arrêt ("A" ou "a") ou marche ("M" ou "m")
// ne pas oublier de définir "pas de fin de ligne" dans la console Arduino ainsi que la vitesse de transmission : 115200
//    Vérification de l'état (ON ou OFF) de la carte FONA
//    si la demande est cohérente (demande de mise en marche si la carte est à l'arrêt)
//        envoi de la commande correspondante avec prise en compte du Power status pour vérifier l'état de la carte.
//
//
//
//


#include <SoftwareSerial.h>
#define ON  1
#define OFF 0

#define GSM_KEY 5
#define GSM_RX 2
#define GSM_TX 3
#define GSM_PS 6
#define GSM_RES 4



// instanciation de la liaison série avec la carte FONA
SoftwareSerial GSM_Serial(GSM_TX, GSM_RX); // RX, TX


// *********************************************************************
//     SETUP
// *********************************************************************


void setup() {
  

  Serial.begin(115200);
  delay(200);
 
   
   // initialisation de la liaison série avec la carte FONA
   GSM_Serial.begin(9600);
   
   // initialisation de la broche de télécommande ON/OFF pour le module FONA
   digitalWrite(GSM_KEY,HIGH);
   pinMode (GSM_KEY,OUTPUT);
   // initialisation de la lecture du Power Status
   pinMode (GSM_PS, INPUT);

   
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
  }
}


// *********************************************************************
//     FONCTIONS
// *********************************************************************




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
void gsm_impulsion_on_off () {
  // génère une impulsion LOW de 2 secondes pour activer ou desactiver
  //  la carte FONA
  digitalWrite(GSM_KEY,LOW);
  delay(2000);
  digitalWrite(GSM_KEY,HIGH);
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
