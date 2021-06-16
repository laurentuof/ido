/**
 * ------------------------------------------------------------------
 *          OBJECTIF:
 *          
 * Ce programme a pour but d'intégrer au montage 2 un module Gravity (I²C vers 2 UART) 
 * permettant ainsi d’obtenir un montage électronique proche de sa version finale
 * 
 *  il ajoute donc au programe 07 Connection reseau GSM automatisée :
 *    * integration du Module Gravity I²C 2UART
 *    * gestion de port UART via I²C 
 *    
 * ------------------------------------------------------------------
 *          NOTE : ADAPTATION DU PIN MAPPING A FAIRE
 * 
 *  Ce programme n'utilise pas le mapping définitif du Data logger:
 *    * il manque encore la GESTION de l'ALIMENTATION par commande de
 *      la masse pour lalimentation du module FONA et Gravity par 
 *      transitor, via le FireBeetle ESP-32.
 *  
 *  Les autres broches utilisées (Key, PS ,NS et RX/TX) du module FONA correspondent au 
 *  brochage définitif du Data logger.
 * 
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
 * 
 * liste de commande AT pour GSM :
 * https://www.technologuepro.com/gsm/commande_at.htm
 * 
 * Le module FONA utilisé ne les reconnait pas toutes
 * 
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
 * récupére des info battrie [E,PPP,VVVV]; E=état charge (0/1),PPP= pourcentage battrie, VVVV= voltage battrie xxxx mV
 * 
 *    commande_set_CPIN
 * init du CODE PIN a renseigner dans "code_PIN.h". Return ok + quelque paramettre, sinon ERREUR
 * 
 *    commande_get_CREG
 * Status du reseau GSM. return 2paramettre si ok sinon ERREUR
 * 
 *    commande_get_COPS
 * Liste des opérateur disponnible Vue par le FONA. return opérateur + paramettre si ok, sinon ERREUR
 * 
 *    commande_GPRS
 * Paramettrage + activation GPRS "3G"
 * 
 *    commande_??_???
 * 
 * ------------------------------------------------------------------
 *          Si besoin liste de commande AT :
 *  
 *  gsm_serial_send("AT+HTTPINIT"); //return ok
 *  gsm_serial_send("AT+HTTPSTATUS?");//return GET,0,0,0
 *  gsm_serial_send("AT+HTTPPARA=\"URL\",\"http://marchytronic.fr/Saloum/"); // pas encore tester
 *  gsm_serial_send("AT+SAPBR=2,1");
 *  AT+HTTPACTION=<Method> peut-étre besoin
 *  AT+HTTPSTATUS? return : +HTTPSTATUS: GET,0,0,0 [methode,status,finish,remain]
 *                                                  GET,0=idel,0=transmited,0=sent/received
 *  AT+CGDCONT? return : +CGDCONT: <cid>,<PDP_type>,<APN>,<PDP_addr>,<data_comp>,<head_comp> 
 *  [<CR><LF>+CGDCONT: <cid>,<PDP_type>,<APN>,<PDP_addr>,<data_comp>,<head_comp> […]]
 *  
 * AT+COPN : liste des opérateurs enregistrés dans la mémoire (EEPROM) du module GSM
 * AT+CREG? Network Registration status
 * AT+CGATT=? +CGATT :  repond (0,1)= aux deux que peut retourner la commande
 * AT+CGATT?  +CGATT :  repond 0 si non connecter, repond 1 si connecter
 * 
 * pour le ssl actualiser le firmware du FONA pour enagle la commande d'activation du HTTPS
 * 
 * AT+SAPBR=3,1,”CONTYPE”,”GPRS” OK  Configure le profil 1, type de connexion internet : GPRS
 * AT+SAPBR=3,1,”APN”,”free” OK  Configure le réseau APN
 * AT+SAPBR=1,1  OK  Tentative de connexion GPRS
 * AT+HTTPINIT OK  Initialisation
 * AT+HTTPPARA=”CID”,1 OK  Configuration http
 * AT+HTTPPARA=”URL”,””  OK  Configure l’adresse URL
 * AT+HTTPACTION=0 OK  Lance la session
 * AT+HTTPREAD +HTTPREAD :<date_len>,  Lis les données DATA du serveur http
 * AT+HTTPTERM OK  Fin
 * AT+CIFSR    Retourne l’adresse IP
 * 
 * 
 * //////////////////////////////////////////
 *        MAJ firmware
 * AT+CGMR returne la version du firmware
 * //////////////////////////////////////////
 * Fichier en FTP a test si le principal est fait(envoie sur TOCIO)
 * 
 *        **** NV_Procedure ****
 * AT+CIPSHUT
 * AT+cgatt
 * AT+SAPBR 3.1
 * AT+SAPBR 3.1
 * AT+CSTT = opérateur
 * AT+SAPBR 1,1
 * 
 * 
 * 
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
  iicSerialFlush(1);
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

bool commande_set_CPIN(bool fct){
 if (fct) {
    gsm_serial_send("AT+CPIN="+GSM_PIN);
    cptr=0;
    while (true) {
      str=attend_reponse_GSM(10000);     
      if (str == "AT+CPIN="+GSM_PIN)cptr+=1;
      if (str == "OK") cptr+=1;
      if (str == "+CPIN: READY") cptr+=1;
      if (str == "Call Ready") cptr+=1;
      if (str == "SMS Ready") cptr+=1;
      if (str == "TimeOut") {
        Serial.println ("                             CPIN Time Out");
        return KO;
      }
      if (cptr == 5){
        Serial.println ("                             CPIN OK");
        return OK;
      }
    }    
  }
}

bool commande_get_CREG(bool fct){
 if (fct) {
    gsm_serial_send("AT+CREG?");  
    cptr=0;
    while (true) {
      str=attend_reponse_GSM(10000);
      if (str.substring(0,7) == "+CREG: "){
        // ici on peut récupérer 2 paramètre
        Serial.println ("                           1er paramètre  : "+str.substring(7,8));
        Serial.println ("                           2éme paramètre : "+str.substring(9,10));
        cptr+=1;
      }
      if (str == "OK") cptr+=1;
      if (str == "TimeOut") {
        Serial.println ("                             CREG Time Out");
        return KO;
      }
      if (cptr == 2){
        Serial.println ("                             CREG OK");
        return OK;
      }
      
    }    
  }
}

bool commande_get_COPS(bool fct){
 if (fct) {
    gsm_serial_send("AT+COPS=?");  
    cptr=0;
    while (true) {
      str=attend_reponse_GSM(20000);
      if (str.substring(0,9) == "AT+COPS=?")cptr+=1;

      if (str.substring(0,7)=="+COPS: ") {

        // cherche une chaine comprise entre ()
        // si ok
        //    identifie les 4 champs (separateur = ","
        //    si ok
        //          Si le premier champ = 2
        //              on mémorise le 4éme dans id_operateur_choisi
        //              on sort avec OK
        //          sinon on recommence
        //    sinon 
        //          on sort avec KO : opérateur non trouvé
        // sinon
        //    on sort avec KO
        bool quitter=false;
        while(!quitter) {
          Serial.println (str);
          int deb=str.indexOf('(');
          int fin=str.indexOf(')');
          Serial.println (deb);
          String ch = str.substring(deb+1,fin); //
          str = str.substring(fin+1); //
          Serial.println ("Bloc operateur trouvé : "+ch);
          // on cherche 3 virgules
          
          int n=0;
          int pos[4]={0,0,0,0};
         
          while(true) {
            int p = 0;
            if (n==0) {
              p = ch.indexOf(",",pos[n]);
            }else {
              p = ch.indexOf(",",pos[n]+1);
            }
            if (p!=-1) {
              n+=1;
              pos[n] = p;
            } else {
              // il n'y a plus de virgules
              // si n=3 => ok
              // sinon ko
              if (n==3) {
                struct operateur operateur_trouve;
                operateur_trouve.stat = ch.substring(0,pos[1]).toInt();
                operateur_trouve.nom = ch.substring(pos[1]+2,pos[2]-1);
                operateur_trouve.reseau = ch.substring(pos[2]+2,pos[3]-1);
                operateur_trouve.id = ch.substring(pos[3]+2,ch.length()-1);

                Serial.print("  stat_GSM          ==>  ");
                Serial.println (operateur_trouve.stat);
                Serial.println ("  nom_operateur     ==>  "+operateur_trouve.nom);
                Serial.println ("  reseau_operateur  ==>  "+operateur_trouve.reseau);
                Serial.println ("  ID_operateur      ==>  "+operateur_trouve.id);
                Serial.println ("");
                if(operateur_trouve.stat == 2){
                  id_operateur_choisi = operateur_trouve.id;
                  quitter=true;
                  break;
                  }
              } else {
                Serial.print("opérateur courant non trouvé");
                break;
              }
            }
          }
         
          Serial.println ("Chaine restante : "+ch);
          if (deb == -1 | fin == -1) break;
          if (fin) break; 
        }
        cptr+=1;
      }
      if (str == "OK") cptr+=1;
      if (str == "TimeOut") {
        Serial.println ("                             COPS Time Out");
        return KO;
      }
      if (cptr == 3){
        Serial.println ("        id_operateur_choisi ==> "+id_operateur_choisi);
        Serial.println ("                             COPS OK");
        return OK;
      }
    }
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
  //if(!commande_get_nvbatt(fonctionnement)) fonctionnement = KO;

  // Envoi du code PIN pour dévérouiiler la SIM et se connecter au réseau GSM
  if (!commande_set_CPIN(fonctionnement)) fonctionnement = KO;

  //    Etat de connection au réseau
  if (!commande_get_CREG(fonctionnement)) fonctionnement = KO;
 
  //    liste des opérateurs disponibles vu par le module
  if (!commande_get_COPS(fonctionnement)) fonctionnement = KO;

  // espionnage de l'UART
  // pour savoir si des choses se passent sur le bus
  Serial.println ("");
  Serial.println ("");
  Serial.println ("");
  Serial.println ("");
  Serial.println ("-------------------------------");
  Serial.println (" Espionnage (time out 10s)");
  delais=10000;
  debut = millis();

  String recu="";
  while (millis() - debut < delais) {
    if (iicSerialAvailable(1)){
      recu = recu+char(iicSerialRead(1));
    }
    int l=recu.length();
    if (    recu.charAt(l-2)==13 and  recu.charAt(l-1)==10  ) {
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
