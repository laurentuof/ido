/**
 * ------------------------------------------------------------------
 *          OBJECTIF:
 *          
 * Ce programe a pour but d'etablire une connection HTTPS
 *  il ajoute donc au programe 1.06 toutes :
 *    *une procedure de commande AT
 *    *traitmeent des donnée retouner par le fona  
 *    *ajout de la posibiliter de verifier le niveau de la battrie
 *    
 * ------------------------------------------------------------------
 *          NOTE : ADAPTATION DU PIN MAPPING A FAIRE
 * 
 *  Ce programme n'utilise pas le mapping définitif du Data logger pour 
 *  la liaison série avec le module GSM. 
 *  Elle utilise un port série matériel (HardwareSerail). 
 *  Dans le Data logger cette liaison sera fournie par un module:
 *    *I²C vers 2xUART.
 *  
 *  Les autres broches utilisées (Key, PS et NS)  correspondent au 
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
 *    FONA     FireBeetle
 *     
 *     VIO    3,3V
 *     BAT    + d'une batterie extérieure (3.7V par exemple)
 *     GND    GND
 *     GND    IOxx commande masse
 *     Key    IO16 (Do)
 *     PS     IO39 (A1)
 *     Rx     Tx IO15  (A4)
 *     Tx     Rx IO35  (A3)
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
 */
 

//--------------------------------------------

/**état module GSM**/
#define OK 1
#define KO 0


/**état module GSM**/
#define GSM_ON  1
#define GSM_OFF 0

/**PinMaping**/
#define GSM_NS  17
#define GSM_KEY 16 
#define GSM_PS  39

#define GSM_RX  35  // cette broche est la broche Rx du FireBeetle qui est reliée à la broche Tx du module GSM
#define GSM_TX  15  // cette broche est la broche Tx du FireBeetle qui est reliée à la broche Rx du module GSM

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

/** réatribution du port Seria(1,FONA) sur les broche choisie **/
  HardwareSerial gsm_serial(1);
  //HardwareSerial *pgsm_serial = &gsm_serial;

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

bool gsm(bool status,unsigned long delay) {
  debut = millis();
  if (status == GSM_ON) {
    if (!etat_gsm()) {
      // le Fona est sur OFF , commande la mise en marche
      Serial.println ("  GSM - démarage en cours ");
      // commande Key sur Low
      digitalWrite(GSM_KEY,LOW);
      while (!etat_gsm()){
        if (millis() - debut > delay) {
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
        if (millis() - debut > delay) {
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

bool initialisation_liaison_serie_gsm(unsigned long delay) {
  gsm_serial.begin(GSM_SERIAL_RATE, SERIAL_8N1,GSM_RX, GSM_TX);
  debut = millis();
  if (!gsm_serial) {
    if (millis() - debut > delay) {
      return KO;
    }
  }
  return OK;
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
    if (gsm_serial.available()){
      recu = recu+char(gsm_serial.read());
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
  gsm_serial.flush();
  gsm_serial.println(commande);
}

bool commande_get_nvbatt(bool fct){ // a completer comme "commande_get_COPS"  
 if (fct) {
   gsm_serial_send("AT+CBC"); // return +CBC: 0;PPP,VVVV (0= ;PPP = pourcentage; VVVV = voltage en mV)
   cptr=0;
/*   while (true) {
      str=attend_reponse_GSM(10000);     
      if (str == "+CBC: ") cptr+=1;
      if (str == "") cptr+=1;
      if (str == "???") cptr+=1;
      if (str == "???") cptr+=1;
      if (str == "???") cptr+=1;
      if (str == "???") {
        Serial.println ("                             Time Out");
        return KO;
      }
      if (cptr == 5){
        Serial.println ("                             CPIN OK");
        return OK;
      }
    }*/
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
 
 
 String CID="";
bool commande_GPRS(bool fct,String *pCID){ 
  bool flag_GPRS = false;
  if (fct) {
    gsm_serial_send("AT+CIPSHUT");//AT+CIPSHUT; Deactivate GPRS PDP Context
    cptr=0;
    while (true) {
      str=attend_reponse_GSM(10000);     
      if (str == "AT+CIPSHUT"){ cptr+=1;}
      if (str == "SHUT OK")   { cptr+=1;}
      if (str == "TimeOut") {
        Serial.println ("                             CIPSHUT Time Out");
        Serial.println("");
        return KO;
      }
      //Serial.print("                                                                    cptr : ");
      //Serial.println(cptr);
      if (cptr == 2){
        flag_GPRS = true;
        Serial.println ("                             parametre CIPSHUT OK");
        Serial.println("");
        break;
      }
    }

    if (bool flag_GPRS = true) {//AT+CGATT; Attach or Detach from GPRS Service
      flag_GPRS = false;
      gsm_serial_send("AT+CGATT?");
      cptr=0;
      while (true) {
        str=attend_reponse_GSM(10000);
        if (str == "+CGATT: 1")cptr+=1;
        if (str == "OK")cptr+=1;
        if (str == "TimeOut") {
          Serial.println ("                                CGATT Time Out");
          Serial.println ("");
          return KO;
        }
        //Serial.print("                                                                    cptr : ");
        //Serial.println(cptr);
        if (cptr == 2){
          flag_GPRS = false;
          Serial.println ("                                parametre CGATT OK");
          Serial.println ("");
          break;
        }
      }
    }

    if (flag_GPRS == true){//AT+SAPBR=3,1,”CONTYPE”,”GPRS”; OK  Configure le profil 1, type de connexion internet : GPRS
      flag_GPRS = false;
      gsm_serial_send("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"");
      cptr=0;
      while (true) {
        str=attend_reponse_GSM(10000);     
        if (str == "AT+SAPBR=3,1,”CONTYPE”,”GPRS”");
        if (str == "OK")cptr+=1;
        if (str == "TimeOut") {
          Serial.println ("                             SAPBR [3,1] Time Out");
          Serial.println ("");
          return KO;
        }
        //Serial.print("                                                                    cptr : ");
        //Serial.println(cptr);
        if (cptr == 1){
          flag_GPRS = true;
          Serial.println ("                             parametre SAPBR [3,1] OK");
          Serial.println ("");
          break;
        }
      }
    }

    if (flag_GPRS == true) {//AT+SAPBR=3,1,”APN”,”free”; OK  Configure le réseau APN(nom du point d'accé)
      flag_GPRS == false;
      gsm_serial_send("AT+SAPBR=3,1,\"APN\",\"sl2sfr\"");
      cptr=0;
      while (true) {
        str=attend_reponse_GSM(10000);     
        if (str == "AT+SAPBR=");
        if (str == "OK")cptr+=1;
        if (str == "TimeOut") {
          Serial.println ("                             SAPBR [3,1]² Time Out");
          Serial.println ("");
          return KO;
        }
        //Serial.print("                                                                    cptr : ");
        //Serial.println(cptr);
        if (cptr == 1){
          flag_GPRS = true;
          Serial.println ("                             parametre SAPBR [3,1]² OK");
          Serial.println ("");
        break;
        }
      }
    }

    if (flag_GPRS == true) {// AT+CSTT = opératueur; Start Task and Set APN, USER NAME, PASSWORD
        flag_GPRS == false;
        gsm_serial_send("AT+CSTT="+id_operateur_choisi);
        cptr=0;
        while (true) {
          str=attend_reponse_GSM(10000);     
          if (str == "OK")cptr+=1;
          if (str == "TimeOut") {
            Serial.println ("                             CSTT Time Out");
            Serial.println ("");
            return KO;
          }
          //Serial.print("                                                                    cptr : ");
          //Serial.println(cptr);
          if (cptr == 1){
            flag_GPRS=true;
            Serial.println ("                             parametre CSTT OK");
            Serial.println ("");
          break;
          }
        }
      }

    if (flag_GPRS == true) {//AT+SAPBR=1,1;
        flag_GPRS == false;
        gsm_serial_send("AT+SAPBR=1,1");
        cptr=0;
        while (true) {
          str=attend_reponse_GSM(10000);
          if (str == "OK")cptr+=1;
          if (str == "TimeOut") {
            Serial.println ("                             SAPBR Time Out");
            Serial.println ("");
            return KO;
          }
          //Serial.print("                                                                    cptr : ");
          //Serial.println(cptr);
          if (cptr == 1){
            flag_GPRS = true;
            Serial.println ("                             parametre SAPBR [1,1] OK");
            Serial.println ("");
           break; 
          }
        }
      }

    if (flag_GPRS == true) {//AT+SAPBR=2,1; //
        flag_GPRS == false;
        gsm_serial_send("AT+SAPBR=2,1");
        cptr=0;
     while (true) {
          str=attend_reponse_GSM(10000);
          if (str.substring(0,7) == "+SAPBR:")cptr+=1;          
          if (str == "OK")cptr+=1;
          if (str == "TimeOut") {
            Serial.println ("                             SAPBR Time Out");
            Serial.println ("");
            return KO;
          }
          //Serial.print("                                                                    cptr : ");
          //Serial.println(cptr);
          if (cptr == 2){
            flag_GPRS = true;
            Serial.println ("                             parametre SAPBR [2,1] OK");            
            Serial.println ("");
          break;
          }
        }
      }
      
    if (flag_GPRS == true) {//AT+HTTPINIT; Initialize HTTP Service
        flag_GPRS == false;
        gsm_serial_send("AT+HTTPINIT");
        cptr=0;
     while (true) {
          str=attend_reponse_GSM(10000);
          if (str == "OK")cptr+=1;
          if (str == "TimeOut") {
            Serial.println ("                             HTTPINIT Time Out");
            Serial.println ("");
            return KO;
          }
          if (cptr == 1){
            flag_GPRS = true;
            Serial.println ("                             parametre HTTPINIT OK");
            Serial.println ("");
            break;
          }
        }
      }
  
    if (flag_GPRS == true) {//AT+FTPCID?; FTP Bearer Profile Identifier = CID;
        flag_GPRS == false;
        gsm_serial_send("AT+FTPCID?");
        cptr=0;
     while (true) {
          str=attend_reponse_GSM(10000);
          if (str.substring(0,9) == "+FTPCID: "){
            // ici on recupere le paramettre CID
            *pCID = str.substring(9);
            Serial.print("                  CID = ");
            Serial.println (*pCID);
          cptr+=1;
          }
          if (str == "OK")cptr+=1;
          if (str == "TimeOut") {
            Serial.println ("                             FTPCID Time Out");
            Serial.println ("");
            return KO;
          }
          if (cptr == 2){
            flag_GPRS = true;
            Serial.println ("                             parametre FTPCID OK");
            Serial.println ("");
            break;
          }
        }
      }

/*    if (flag_GPRS == true) {//AT+HTTPSSL?; return état SLL "0" ou "1"
        flag_GPRS == false;
        gsm_serial_send("AT+HTTPSSL?");
        cptr=0;
      while (true) {
          str=attend_reponse_GSM(10000);
          if (str == "OK")cptr+=1;
          if (str == "TimeOut") {
            Serial.println ("                             HTTPSSL Time Out");
            Serial.println ("");
            return KO;
          }
          //Serial.print("                                    cptr : ");
          //Serial.println(cptr);
          if (cptr == 1){
            flag_GPRS = true;
            Serial.println ("                             parametre HTTPSSL OK");
            Serial.println ("");
            break;
          }
        }
    }
*/

/*    if (flag_GPRS == true) {//AT+HTTPSSL=1; activation HTTPS; AT+HTTPSSL=<n> ; n=0 Disable, n=1 Enable
 *     
        flag_GPRS == false;
        gsm_serial_send("AT+HTTPSSL=1");
        cptr=0;
      while (true) {
          str=attend_reponse_GSM(10000);
          if (str == "OK")cptr+=1;
          if (str == "TimeOut") {
            Serial.println ("                             HTTPSSL Time Out");
            Serial.println ("");
            return KO;
          }
          //Serial.print("                                    cptr : ");
          //Serial.println(cptr);
          if (cptr == 1){
            flag_GPRS = true;
            Serial.println ("                             parametre HTTPSSL OK");
            Serial.println ("");
            break;
          }
        }
    }
*/

    if (flag_GPRS == true) {//"AT+HTTPPARA=\""+*pCID+"\",1"
        flag_GPRS == false;
        gsm_serial_send("AT+HTTPPARA=CID,"+*pCID);
        cptr=0;
     while (true) {
          str=attend_reponse_GSM(10000);
          if (str == "OK")cptr+=1;
          if (str == "TimeOut") {
            Serial.println ("                             HTTPPARA_CID Time Out");
            Serial.println ("");
            return KO;
          }
          if (cptr == 1){
            flag_GPRS = true;
            Serial.println ("                             parametre HTTPPARA_CID OK");
            Serial.println ("");
            break;
          }
        }
      }

    if (flag_GPRS == true) {//AT+HTTPPARA=”URL”,””  OK  Configure l’adresse URL
      flag_GPRS == false;
      gsm_serial_send("AT+HTTPPARA=\"URL\",\" http://marchytronic.fr/Saloum\"");
      cptr=0;
      while (true) {
        str=attend_reponse_GSM(10000);
        if (str == "OK")cptr+=1;
        if (str == "TimeOut") {
          Serial.println ("                             HTTPPARA_URL Time Out");
          Serial.println ("");
          return KO;
        }
        if (cptr == 1){
          flag_GPRS = true;
          Serial.println ("                             parametre HTTPPARA_URL OK");
          Serial.println ("");
          break;
        }
      }
    }

    if (flag_GPRS == true) {// AT+HTTPACTION=0 OK  Lance la session
      flag_GPRS == false;
      gsm_serial_send("AT+HTTPACTION=0");
      cptr=0;
        while (true) {
        str=attend_reponse_GSM(10000);
        if (str == "OK")cptr+=1;
        if (str.substring(0,13) == "+HTTPACTION: "){
          String res_httppaction=str.substring(12);
          //Serial.print("res_httppaction : ");
          //Serial.println(res_httppaction);
          cptr+=1;
        }
        //Serial.print("                                    cptr : ");
        //Serial.println(cptr);
        if (str == "TimeOut") {
          Serial.println ("                             HTTPACTION Time Out");
          Serial.println ("");
          return KO;
        }
        if (cptr == 2){
          flag_GPRS = true;
          Serial.println ("                             parametre HTTPACTION OK");
          Serial.println ("");
          break;
        }
      }
    }

/*    if (flag_GPRS == true) {//AT+HTTPREAD ;Read the HTTP Server Response; +HTTPREAD :<date_len>,<data>  Lis les données DATA du serveur http
      flag_GPRS == false;
      gsm_serial_send("AT+HTTPREAD");
      cptr=0;
      while (true) {
        str=attend_reponse_GSM(10000);
        if (str == "OK")cptr+=1;
        if (str == "TimeOut") {
          Serial.println ("                             HTTPREAD Time Out");
          Serial.println ("");
          return KO;
        }
        //Serial.print("                                    cptr : ");
        //Serial.println(cptr);
        if (cptr == 1){
          flag_GPRS = true;
          Serial.println ("                             parametre HTTPREAD OK");
          Serial.println ("");
          break;
        }
      }
    }
*/

/*    if (flag_GPRS == true) {//AT+CIFSR    get local IP address
      flag_GPRS == false;
      gsm_serial_send("AT+CIFSR");
      cptr=0;
      while (true) {
        str=attend_reponse_GSM(10000);
        if (str == "OK")cptr+=1;
        if (str == "TimeOut") {
          Serial.println ("                             CIFSR Time Out");
          Serial.println ("");
          return KO;
        }
        if (cptr == 1){
          flag_GPRS = true;
          Serial.println ("                             parametre CIFSR [1,1] OK");
          Serial.println ("");
          break;
        }
      }
    }
*/

/*    if (flag_GPRS == true) {//AT+HTTPTERM; Terminate HTTP Service; return ok si connection HTTP close
      flag_GPRS == false;
      gsm_serial_send("AT+HTTPTERM");
      cptr=0;
      while (true) {
        str=attend_reponse_GSM(10000);
        if (str == "OK")cptr+=1;
        if (str == "TimeOut") {
          Serial.println ("                             HTTPTERM Time Out");
          Serial.println ("");
          return KO;
        }
        if (cptr == 1){
          flag_GPRS = true;
          Serial.println ("                             parametre HTTPTERM [1,1] OK");
          Serial.println ("");
          break;
        }
      }
    }
*/

/**       info / a programer 
 *   voir pour fiare en FTP due un un pob de maj du firmware du FONA
 * AT+CDNSGIP = "marchytronic.fr" Query the IP Address of Given Domain Name
 * AT+CDNSCFG ="xxx.xxx.xxx.xxx","xxx.xxx.xxx.xxx" Configure Domain Name Server
 * 2/3 info commande httppara (dans l'exemple) https://www.microchip.com/forums/m1109499.aspx
 * 
 * 
 * 
 * //AT+CIPSSL=<n> ;TCP SSL
 * 
 * 
 */
  }
}

/** Si besoin liste de commande AT :
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

/******************************************************************************
 * 
 * 
 * 
 *****************************************************************************/
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
  }
  
  // niveau battrie
  //if(!commande_get_nvbatt(fonctionnement)) fonctionnement = KO;

  // Envoi du code PIN pour dévérouiiler la SIM et se connecter au réseau GSM
  if (!commande_set_CPIN(fonctionnement)) fonctionnement = KO;

  //    Etat de connection au réseau
  if (!commande_get_CREG(fonctionnement)) fonctionnement = KO;
 
  //    liste des opérateurs disponibles vu par le module
  if (!commande_get_COPS(fonctionnement)) fonctionnement = KO;
 
  //  Configuration du module pour utiliser le GPRS
  if(!commande_GPRS(fonctionnement,&CID)) fonctionnement = KO;

  // espionnage de l'UART
  // pour savoir si des choses se passent sur le bus
  Serial.println ("");
  Serial.println ("");
  Serial.println ("");
  Serial.println ("");
  Serial.println ("-------------------------------");
  Serial.println (" Espionnage (time out 10s)");
  unsigned long delais=10000;
  debut = millis();

  String recu="";
  while (millis() - debut < delais) {
    if (gsm_serial.available()){
      recu = recu+char(gsm_serial.read());
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
