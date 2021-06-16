/**
 * ------------------------------------------------------------------
 * 
 *                        07_Envoie_HTTP_TOCIO
 *                    
 * ------------------------------------------------------------------         
 * NOTE : ADAPTATION DU PIN MAPPING A FAIRE
 * 
 *  Ce programme n'utilise pas le mapping définitif du Data logger pour
 *  a liaison série avec le module GSM. Elle utilise un port série matériel (HardwareSerail)
 *  Dans le Data logger cette liaison sera fournie par un module I²C vers 2xUART.
 *   
 *   Les autres broches utilisées (Key, PS et NS)  correspondent au brochage définitif du Data logger
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
  * 
 *                        PinMapping
 *        
 *    FONA     FireBeetle
 *     
 *     VIO    3v3
 *     GND    GND
 *     NS     IO16 (Di)
 *     Key    IO17 (Do)
 *     PS     IO39 (A1)
 *     Rx     Tx IO15  (A4)
 *     Tx     Rx IO35  (A3)
 *     
 *     Batterie branchée directement sur le connecteur spécifique JST2
 *     
 * ------------------------------------------------------------------
 * 
 *                        Fonction
 * 
  *          
 * ------------------------------------------------------------------ 
 *                Supposition procedure d'envoie HTTPS
 *                
 * Lien aide pour envoie HTTP via PHONA :
 * http://www.raviyp.com/sim900-gprs-http-at-commands/
 * 
 * AT+SAPBR=3,1,”Contype”,”GPRS” // paramettre le type de connection a GPRS
 * V
 * AT+SAPBR=3,1,”APN”,”www” //Réglez l'APN sur «www» puisqu'il utilise une carte SIM Vodafone, cela peut être différent pour nous et dépend du réseau
 * 
 * AT+SAPBR =1,1 // acrive le GPRS
 * 
 * AT+SAPBR=2,1 // verifie les paramettres de connection, 
 * retourne une adresse IP
 * 
 * AT+HTTPINIT //activation du mode HTTP  
 * 
 * AT+HTTPSSL=1 //active l'option HTTPS ne peut pas fonctionner si la commande precedant n'est pas envoyer, 
 * permet l'envoie au URL en HTTPS mais il faut retirer "HTTP://" dans l'URL utiliser 
 *  
 * AT+HTTPPARA=”CID”,1 //configurer l'identifiant du profil du support HTTP
 * 
 * AT+HTTPPARA=“uboopenfactory.univ-brest.fr/cad/saloum/backoffice”,”/cad/saloum/backoffice/mesure/add/FONA_UOF_1.txt” //URL cible 
 * 
 * AT+HTTPDATA=192,10000 //192bytes, 10000=10s timeout 
 * 
 * AT+HTTPACTION=1 //ENVOIE les data du buffer vers le server "POST"
 * 
 * AT+HTTPREAD // lire les donnée reçus par le serveur
 * ------------------------------------------------------------------
 *                      commande AT utile
 * AT+CPIN="code pin= : Contrôle du mot de passe
 * AT+Cpin=? : Vérification du code PIN
 * AT+CBC : Etat de la batterie
 * AT+CMGS=? : verification de la posibiliter d'envoie de SMS
 * 
 * ------------------------------------------------------------------ 
 * 
 */
 
/**
 * ------------------------------------------------------------------
 *          Fona
 */


/**état Fona**/
#define GSM_ON  1
#define GSM_OFF 0

/**PinMaping**/
#define GSM_NS  25
#define GSM_KEY 26
#define GSM_PS  27

#define GSM_RX  16
#define GSM_TX  17
//#define GSM_RES 21

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

/**definition de la vitsee de communication des port SERIE**/
#define SERIAL_RATE 115200

/**Code PIN**/
String GSM_PIN = "4352";
/**
 *          /Fona
 * ------------------------------------------------------------------
 */

/**adresse Web TOCIO, IP : [ 195.83.247.151 ]**/
const String host = "uboopenfactory.univ-brest.fr/cad/saloum/backoffice";
const String url  = "/cad/saloum/backoffice/mesure/add/FONA_UOF_1";

/**
 * ------------------------------------------------------------------
 *      SETUP_FONA
 * ------------------------------------------------------------------
 */
void SETUP_FONA(){
  //USB
  Serial.begin(SERIAL_RATE);
  while (!Serial) {
    delay(1); //attend que le port série "USB" se connecte
    }
  Serial.println("");
  Serial.println("----------------------------------");
  Serial.println("               SETUP              ");
  Serial.println("");
  Serial.println("Serial 1 (USB)  : OK");
      
  //FONA
  Serial2.begin(SERIAL_RATE, SERIAL_8N1, GSM_RX, GSM_TX);
  while (!Serial2) {
    delay(1); //attend que le port série "USB" se connecte
    }
  Serial.println("Serial 2 (Fona) : OK");

  // initialisation de la broche de télécommande ON/OFF pour le module FONA
  pinMode (GSM_KEY,OUTPUT);
  digitalWrite(GSM_KEY,HIGH);
  
  // initialisation de la lecture du Power Status
  pinMode (GSM_PS, INPUT);
  
  //initialisation de la lecture du Net Status
  pinMode(GSM_NS, INPUT);

  Serial.println("----------------------------------");  
  }

/**
 * ------------------------------------------------------------------
 *      FONCTION
 * ------------------------------------------------------------------
 */

/**
 * ------------------------------------------------------------------
 * état_GSM
 */
bool etat_gsm() {
  // renvoie true si la carte FONA est ON
  // renvoie false si la carte FONA est OFF
  if (digitalRead(GSM_PS) == HIGH) {
    return true;
  } else {
    return false;
  }
}

/**
 * ------------------------------------------------------------------
 * statue GSM
 */
bool gsm(int status) {
  if (status == ON) {
    if (!etat_gsm()) {
      // le Fona est sur OFF , commande la mise en marche
      Serial.println ("  - démarage en cours ");
      // commande Key sur Low
      digitalWrite(GSM_KEY,LOW);
      while (!etat_gsm()){
        delay(1);
      }
      digitalWrite(GSM_KEY,HIGH);
      Serial.println ("  - Module sous tension");
      Serial.println ("");
      
    }
  } else {
    if (etat_gsm()) {
      // le Fona est sur ON , commande l'arrêt
      Serial.println ("  - Arrêt en cours ");
      digitalWrite(GSM_KEY,LOW);
      while (etat_gsm()){
        delay(1);
      }
      digitalWrite(GSM_KEY,HIGH);
      Serial.println ("-- Module hors tension --");
    }
  }
}

/**
 * ------------------------------------------------------------------
 * implustion ON/OFF
 */
void gsm_impulsion_on_off () {
  // génère une impulsion LOW de 2 secondes pour activer ou desactiver
  //  la carte FONA
  digitalWrite(GSM_KEY,LOW);
  delay(2000);
  digitalWrite(GSM_KEY,HIGH);
}

/**
 * état battrie
 */
void etat_battrie (){
  Serial2.print("AT+CBC");
  Serial.print("niveau battrie : [ ");
  Serial.print(Serial2.read());
  Serial.println("% ]");
  }

/**
 * ------------------------------------------------------------------
 * Procedure de connection
 */
byte gsm_connected() {
  // retourne true si on est connecté au réseau
  // retourne false si on n'est pas connecté au réseau
  //attend que le niveau passe à 1
  
  Serial.print("  - Connection au réseau :");
  while (digitalRead(GSM_NS)==LOW) {
    //Serial.println("GSM_NS == LOW");
    delay(100);
  }
  
  while (digitalRead(GSM_NS)==HIGH) {
    //Serial.println("GSM_NS == HIGH");
    delay(100);
  }
  
  //declenche le compteur
  unsigned long debut = millis();
  while (digitalRead(GSM_NS)==LOW) {
    //Serial.println("GSM_NS == LOW");
    //Serial.print(".");
    delay(1);
  }

  //arrête le compteur
  unsigned long fin = millis();
  unsigned long duree = fin-debut;
  Serial.println("");
  Serial.print("[ duree = ");
  Serial.print(duree);
  Serial.print(" | ");
  
  if (duree >= GSM_TMIN_NOCONNECT && duree <= GSM_TMAX_NOCONNECT) {
    Serial.println("GSM_NOCONNECT = 0 ]");        
    return GSM_NOCONNECT;
  }
   if (duree >= GSM_TMIN_CONNECTED && duree <= GSM_TMAX_CONNECTED) {
    Serial.println("GSM_CONNECTED = 1 ] ");
    return GSM_CONNECTED;
  }
  if (duree >= GSM_TMIN_HTTPCONNECT && duree <= GSM_TMAX_HTTPCONNECT) {
    Serial.println("GSM_HTTPCONNECT = 2 ] ");
    return GSM_HTTPCONNECT;
  }
  Serial.println("GSM_ERREUR ]");
  return ERREUR;
}

/**
 * ------------------------------------------------------------------
 * Demande de code pin
 */
void gsm_SIM_unlock() {
  Serial.println("  - deverouillage de la carte SIM ");
  Serial.println ("  - Envoi code PIN");
  delay(2000);
  Serial2.println("AT+CPIN="+GSM_PIN);

    Serial.print(Serial.read());

  /**
  char(R1);
  char(R2);
  char(R3);
  Serial.print(=Serial.read(10));
  Serial.print(R2=Serial.read(10));
  Serial.print(R3=Serial.read(10));
  delay(10000);
  */
}

 /**
  * ------------------------------------------------------------------
  *           Procedure d'envoie HTTP
  * non tester et non completer
  * commante AT GSM : https://www.technologuepro.com/gsm/commande_at.htm
  * Commande AT FONA: https://www.elecrow.com/wiki/images/2/20/SIM800_Series_AT_Command_Manual_V1.09.pdf
  */

void envoie_http(){
  
  
  /**
  //AT&V verifier la configuration du ME
  /*allowed/notallowed*//*COMMAND*//*commantaire*/
  
  /******int GSM/GPRS******/
  /*ok AT           //Returne ok si il voie le FONA
  /*ok AT+CMEE=2    //Returne ok si il a pris en comte la commande
  /*ok AT+CCID      //Returne n°identifiant SIM (similaire @mac)
  /*ok AT+CPIN=4352 //Returne ok,+CPIN : READY, Call Ready, SMS READY
  /*ok AT+COPS?     //Returne operator configuration
  /*ok AT+COPS=?    //Returne une liste des operateur
  
  /*not-allowed AT+SAPBR = 3,1,”APN”,”www” //Réglez l'APN sur «www» puisqu'il utilise une carte SIM Vodafone, cela peut être différent pour nous et dépend du réseau, paramettre le type de connection a GPRS
  /*?? besoi reponce* AT+SAPBR = 1,1 // acrive le GPRS //peut retourner non autoriser
  /*ok AT+CREG? /**doit étre égale a 0,X. X= 0: not register
                  *                         X= 1: Regiter =, home network
                  *                         X= 2: not register, but try to connect GPRS anable not allowed
                  *                         X= 3: registration denied. GPRS disable
                  *                         X= 4: unknow
                  *                         X= 5: Registration, roaming
                  * exemple : attendre que AT+CREG = +CREG : 0,1 ou 0,5                       
                  *
  /******init TCP******
  /**AT+CIPSTART /*Start Up TCP or UDP Connection*/
  /**AT+CIPSTART=? /*aide: affiche commant formater la commande et les paramettre a rensegnier*/
  /**AT+CIPSTART? /**
  /**/ /**/

  /******int HTTP******
  /*not-allowed* AT+SAPBR = 2,1 // verifie les paramettres de connection, retourne une adresse IP
  /*ok* AT+HTTPINIT //activation du mode HTTP
  AT+CIPSSL //active l'option HTTPS ne peut pas fonctionner si la commande precedant n'est pas envoyer, permet l'envoie au URL en HTTPS mais il faut retirer "HTTP://" dans l'URL utiliser 
  AT+SSLSETCERT//selectionner le certificat SSL sur un support de stoquage
  AT+HTTPPARA = ”CID”,1 //configurer l'identifiant du profil du support HTTP
  AT+HTTPPARA? = afficher les paramettre de configuration HTTP
  AT+HTTPPARA = “uboopenfactory.univ-brest.fr/cad/saloum/backoffice”,”/cad/saloum/backoffice/mesure/add/FONA_UOF_1/[data]” //URL cible 
  AT+HTTPDATA = 192-5000,10000-100000 //192 à 5000 bytes, 10000=10s à 100s timeout (ESTIMATION)
  AT+HTTPACTION = 1 //ENVOIE les data du buffer vers le server (POST)
  AT+HTTPREAD // lire les donnée reçus par le serveur
  */
  }

/**
 * ------------------------------------------------------------------
 * HTTPSTATUS
 * 
 */
void httpstatus(){
  Serial2.print("AT+HTTPSTATUS=?");
  Serial.print("HTTPstatus : [ ");
  Serial.print(Serial2.read());
  Serial.println(" ]");
  Serial.println("");
  
  Serial2.print("AT+CIPPING = uboopenfactory.univ-brest.fr");
  Serial.print("reponse serveur UBOF : [ ");
  Serial.print(Serial2.read());
  Serial.println(" ]");
  Serial.println("");
  }

/**
 * ------------------------------------------------------------------
 * qualitee du signale
 * 
 */
void qualitee_du_signale(){
  Serial.println("");
  Serial2.print("AT+CSQ"); // documentation https://support.industry.siemens.com/cs/document/24094060/comment-%C3%A9valuer-la-puissance-du-signal-en-cours-sur-un-modem-gsm-?dti=0&lc=fr-WW#:~:text=Envoyez%20la%20commande%20%22AT%2BCSQ,signal%20entre%200%20et%2031.
  Serial.print("qualitee du signale : ");
  int QS = Serial2.read();
  Serial.print("[ -");
  Serial.print(QS);
  Serial.print("db");
  Serial.print(" | ");
  if (QS > 87){
    Serial.print("MAUVIAS ]");
    }
  if (QS > 75 && QS < 87){
    Serial.print("INSTABLE ]");
    }
  if (QS < 75){
    Serial.print("STABLE ]");
    }
  Serial.println("");
  }

/**
 * ------------------------------------------------------------------
 * ETAT ME
 * 
 */
void TEST(){
  int i =0;
// TEST FONA
  Serial2.print("ATI");
  Serial.print("  - FONA :          [ ");
  while(Serial.available() > 0){
    for (int i = 0; i <= 10; i++){
      char (Serial.print(Serial2.read()));
      }
    }
  Serial.println(" ]");
  Serial.print("");
  // ERROR ON
  Serial2.print("AT+CMEE=2");
  Serial.print("  - error repport : [ ");
  Serial.print(Serial2.read());
  Serial.println(" ]");
  Serial.print("");
  // test SIM
  Serial2.print("AT+CCID");
  Serial.print("  - n°SIM :         [ ");
  Serial.print(Serial2.read());
  Serial.println(" ]");
  Serial.print("");
  // test operateur
  Serial2.print("AT+COPS?");
  Serial.print("  - operatteur :    [ ");
  Serial.print(Serial2.read());
  Serial.println(" ]");
  Serial.print("");
  }
  
/**
 * ------------------------------------------------------------------
 *          LOOP_FONA
 * ------------------------------------------------------------------
 */
void ENVOI_FONA (){
/**
 *          Module FONA
 * Note : systhéme "bloquer" tanque la procedure n'est pa fini voir a utiliser la fonction millis
 * if nowmilis = GSM_eteint allors fiare le code qui suit
 * else mesure;
 */
    Serial.println("----------------------------------");
    Serial.println("           MODULE FONA            ");
    Serial.println("");
    Serial2.print("AT+CMEE = 2");// active le retour si erreur sur les commande AT
    // marche Fona
    Serial.println("-- alimentations du module FONA --");
    gsm(ON);
    delay(1000);//laisse le temps au systéme d'etre correctement alimenter
    

    //TEST();
    gsm_SIM_unlock();      // demande de code PIN
    gsm_connected();       // Connection au reseau
    //qualitee_du_signale(); // qualiter du reseau
    //etat_battrie();        // viveau de la battrie
    //httpstatus();          // état connection HTTP
    
    /**
     * // Envoie HTTP
     * //Serial.println("  - envoie des donnée / du fichier vers "site WEB" ");
     * //procedure_denvoie_http();
     * delay(100);
     * Serial.println("");
    */
    // arrêt Fona
    gsm(OFF);
    Serial.println("----------------------------------");
    
  //paramettre la periode pendant la quelle le module est eteint
    delay(5*1000);
  }
/**
 * ------------------------------------------------------------------
 * ------------------------------------------------------------------
 */
