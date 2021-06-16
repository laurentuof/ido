/**
 * ------------------------------------------------------------------
 *          OBJECTIF:
 *          
 * Ce programme a pour but d'essayer manuellement des commandes AT du module FONA.
 * 
 *    
 * ------------------------------------------------------------------
 * 
 * liste de commande AT pour GSM :
 * https://www.technologuepro.com/gsm/commande_at.htm
 * 
 * Liste Commande AT pour FONA MiniGSM
 * https://drive.google.com/file/d/1XsEjrCY4kpullxgzbKA4GE_FTWEAmrey/view?usp=sharing
 * ------------------------------------------------------------------
 * 
 *                        PinMapping
 *                        
 *    FireBeetle | Gravity | FONA
 *               | battrie |
 *    3,3V       | +  :  + | Vio
 *    GND        | -  :  - | GND 
 *    IO 16(Din) |         | Key
 *    IO 39(A1)  |         | PS
 *    SDA        | D  | RX | UART1
 *    SCL        | C  | TX | UART1
 *    
 *    IOXX commande l amise a la masse du gravity et FONA
 *    

 *     
 *     Batterie branchée directement sur le connecteur spécifique JST2
 *     ??message(sms) gsm si battrie faible??
 *     
 * ------------------------------------------------------------------
 * 
 *                        FONCTION
 *    etat_gsm
 * renvoie True si FONA "ON" ou fals si off
 * 
 *    gsm
 * commande la mise en marche ou l'arret du FONA
 * 
 *    initialisation_liaison_serie_gsm
 * definition de la redirection du port Serial(1, FONA)
 * 
 *    enleve_CR
 * permet "d'ignorer la CR(carriage return)" pour le traitement des chaine de caractére "STRING"
 * 
 *    attend_reponse_GSM
 * traitement et renvoie des chaine reçu du FONA. Pour une visualisation sur le Port Serial(0,USB)
 * 
 *    gsm_serial_send
 * envoie les commande au module FONA
 * 
 *    commande_get_nvbatt
 * récupére des info battrie [E,%%%,VVVV]; E=état charge (0/1),%%%= pourcentage battrie, VVVV= voltage battrie xxxx mV
 * 
 * ------------------------------------------------------------------
 */
 

//--------------------------------------------
/*Module Gravity I²C -> 2UART*/
#include "iic_to_dual_uart.h"

char tx_buffer[256];//Define the TX array tx_buffer
char rx_buffer[256];//Define the RX array rx_buffer

/**PinMaping**/
#define GSM_KEY 16
#define GSM_PS  39

/**état module GSM**/
#define OK 1
#define KO 0

/**état module GSM**/
#define GSM_ON  1
#define GSM_OFF 0

/**Definition des paramétre GSM**/
#define ERREUR               9999
#define GSM_NOCONNECT           0
#define GSM_CONNECTED           1
#define GSM_HTTPCONNECT         2
#define GSM_TMIN_NOCONNECT    700
#define GSM_TMAX_NOCONNECT    900
#define GSM_TMIN_CONNECTED   2700
#define GSM_TMAX_CONNECTED   3300
#define GSM_TMIN_HTTPCONNECT  200
#define GSM_TMAX_HTTPCONNECT  400

/** definition de la vitsee de communication des port SERIE **/
#define GSM_SERIAL_RATE 115200
#define FONA_UART   1
#define TRILUX_UART 2

/** tableau des opératuer trouver par le module FONA **/
struct operateur {
    byte stat;        // 0 à 3
    String nom;       // nom complet de l'opérateur
    String reseau;    // nom du réseau
    String id;        // identifiant international 
};
String id_operateur_choisi=""; // sera renseigné par la procédure commande_get_COPS

/**adresse Web TOCIO, IP : [ 195.83.247.151 ]**/
const String host = "uboopenfactory.univ-brest.fr/cad/saloum/backoffice";
const String url  = "/cad/saloum/backoffice/mesure/add/FONA_UOF_1";

/** compteur **/
unsigned long debut=0;
String str="";
byte cptr=0; 
unsigned long delais;

/******************************************************************************
 * 
 *          FONCTION
 *
 *****************************************************************************/

bool etat_gsm() {
  // renvoie true si la carte FONA est ON
  // renvoie false si la carte FONA est OFF
  if (digitalRead(GSM_PS) == HIGH) {
    return true;
  } else {
    return false;
  }
}

bool gsm(bool status,unsigned long timeout) {
debut = millis();
  if (status == GSM_ON) {
    if (!etat_gsm()) {
      // le Fona est sur OFF , commande la mise en marche
      Serial.println ("  GSM - démarage en cours ");
      // commande Key sur Low
      digitalWrite(GSM_KEY,LOW);
      while (!etat_gsm()){
        if (millis() - debut > timeout) {
          Serial.println ("  GSM - Timout lors de la mise sous tension");
          digitalWrite(GSM_KEY,HIGH);
          return KO;
        }
      }
      digitalWrite(GSM_KEY,HIGH);
      Serial.println ("  GSM - Module sous tension");
      Serial.println ("");
      return OK;
    } else {
      Serial.println ("Le module GSM est déjà sous tension");
    }
  }
  if (status == GSM_OFF) {
   if (etat_gsm()) {
      // le Fona est sur ON , commande la mise hors tension
      Serial.println ("  GSM - arrêt en cours ");
      // commande Key sur Low
      digitalWrite(GSM_KEY,LOW);
      while (etat_gsm()){
        if (millis() - debut > timeout) {
          Serial.println ("  GSM - Timout lors de la mise hors tension");
          digitalWrite(GSM_KEY,HIGH);
          return KO;
        }
      }
      digitalWrite(GSM_KEY,HIGH);
      Serial.println ("  GSM - Module hors tension");
      Serial.println ("");
      return OK;
    } else {
       Serial.println ("Le module GSM est déjà hors tension");
    }
  }
}

bool initialisation_liaison_serie_gsm(unsigned long timeout) {
  Serial.println("---------------------------------------------------------------------");
  Serial.println("                                 Gravity_SETUP");

//SET FONA.iicUART1
  iicSerialBegin(FONA_UART,USB_SERIAL_RATE);
                   
  //TEST_Gravity_SERIAL(1)
  Serial.print("iicSerial_1 ? : ");
  debut = millis();
    if(!iicSerialAvailable(1)){
      if (millis() - debut > timeout) {
        return KO;
      }
    }
  Serial.println("");
  Serial.println("                                iicSerial_1 OK");

//SET Trilux.iicUART2
  iicSerialBegin(TRILUX_UART,USB_SERIAL_RATE);

  //TEST_Gravity_SERIAL(2)
  Serial.print("iicSerial_2 ? : ");
  debut = millis();
    if(!iicSerialAvailable(2)){
      if (millis() - debut > timeout) {
        return KO;
      }
    }
  Serial.println("");
  Serial.println("                                iicSerial_2 OK");
}

String enleve_CR(String xx) {
  while (true) {
    int l=xx.length();
    if (xx.charAt(l-1)==13){
      xx=xx.substring(0,l-1);
    } else {
      return xx;
    }
  }
}

String attend_reponse_GSM(unsigned long timeout) {
  // renvoie une chaine reçue du module GSM si on n'a pas dépassé le time out
  // renvoie "TimeOut" si time out dépassé
  debut = millis();
  String recu = "";
  
  while (millis() - debut < timeout) {
    if (iicSerialAvailable(1)){
      recu = recu+char(iicSerialRead(1));
    }
    int l=recu.length();
    if (recu.charAt(l-2)==13 and  recu.charAt(l-1)==10  ) {
      recu = recu.substring(0,l-2); // enlève les deux derniers caractères (chr(13) et chr(10))
      recu = enleve_CR(recu);
      Serial.println ("              Reçu : "+recu);
      return recu;
    }
  }
  return "TimeOut";
}

void gsm_serial_send(String commande) {
  Serial.println ("Envoi ["+commande+"]");
  iicSerialFlush(1);  iicSerialFlush(2);
  String *pcommande=&commande;
  iicSerialPrintln(1, pcommande);
}

bool commande_get_nvbatt(bool fct){ // a completer comme "commande_get_COPS" 
 if (fct) {
   gsm_serial_send("AT+CBC"); // return +CBC: 0;PPP,VVVV (0= ;PPP = pourcentage; VVVV = voltage en mV)
   cptr=0;
//   while (true) {
//      str=attend_reponse_GSM(10000);     
//      if (str == "+CBC: ") cptr+=1;
//      if (str == "") cptr+=1;
//      if (str == "???") cptr+=1;
//      if (str == "???") cptr+=1;
//      if (str == "???") cptr+=1;
//      if (str == "???") {
//        Serial.println ("                             Time Out");
//        return KO;
//      }
//      if (cptr == 5){
//        Serial.println ("                             CPIN OK");
//        return OK;
//      }
//    }
  }
}


//---------------------------------------------------------------------
void envoi_donnees_GSM() {

  bool fonctionnement = OK;

  // mise sous tension du module GSM
  if (!gsm(GSM_ON,5000)){
    Serial.println ("ECHEC de la mise sous tension");
    fonctionnement = KO;
    return;
  }
  
  // initialisation de la liaison série
  if (fonctionnement) {
    if (!initialisation_liaison_serie_gsm(5000)){
      Serial.println ("ECHEC de l'initialisation de la lisison série avec le module GSM");
      fonctionnement = KO;
    }
    delay(2000); // pause pour laisser passer des messages parasites
    Serial.println ("init UART GSM OK");
    Serial.println("");
    Serial.println("---------------------------------------------------------------------");
  }
  
  // niveau battrie
  if(!commande_get_nvbatt(fonctionnement)) fonctionnement = KO;

  String recu="";
  while (true) {
    if (Serial.available()){
      size_t n1 = Serial.read(rx_buffer,sizeof(rx_buffer));
        for(size_t i = 0; i < n1; i++){
          rx_buffer[i];
        }
      delay(1);
      gsm_serial_send(rx_buffer);
      Serial.println();
      }
    if (iicSerialAvailable(1)){
      recu = recu+char(iicSerialRead(1));
    }
    int l=recu.length();
    if (recu.charAt(l-2)==13 and  recu.charAt(l-1)==10) {
      recu = recu.substring(0,l-2);
      Serial.println ("Chaine reçue : "+recu);
      recu="";
    }
  }
 Serial.println ("-------------------------------");
  // mise hors tension du module GSM
  if (!gsm(GSM_OFF,5000)){
    Serial.println ("ECHEC de la mise hors tension");
    return;
  }
}

/******************************************************************************
 * 
 * 
 * 
 *****************************************************************************/
void gsm_init() {
  // initialisation des liaisons discrètes avec le GSM hors liaison série
  pinMode(GSM_KEY,OUTPUT);
  pinMode(GSM_PS,INPUT);
  digitalWrite(GSM_KEY,HIGH);
}
