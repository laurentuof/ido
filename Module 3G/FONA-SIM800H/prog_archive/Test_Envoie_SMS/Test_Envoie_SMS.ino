/**
 * ------------------------------------------------------------------
 * 
 *                        COMMANDE_ON_OFF
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
 *                        UART_Mapping FireBeetle
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
 *                        PinMapping
 *        
 *    FONA     FireBeetle
 *     
 *     VIO    5v
 *     BAT    + d'une batterie extérieure (3.7V par exemple)
 *     GND    GND
 *     NS     D2
 *     Key    D3
 *     PS     D4
 *     RES    D5
 *     Tx     D16(rx)
 *     Rx     D17(tx)
 *     
 * ------------------------------------------------------------------
 * 
 *                        Fonction
 * 
 * Scrutation du port série (console) en attente d'une commande d'arrêt ("A" ou "a") ou marche ("M" ou "m")
 * ne pas oublier de définir "pas de fin de ligne" dans la console Arduino ainsi que la vitesse de transmission : 115200
 * Vérification de l'état (ON ou OFF) de la carte FONA
 * si la demande est cohérente (demande de mise en marche si la carte est à l'arrêt)
 * envoi de la commande correspondante avec prise en compte du Power status pour vérifier l'état de la carte.
 *          
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
 * état Fona
 * ------------------------------------------------------------------
 */
#define ON  1
#define OFF 0

/**
 * ------------------------------------------------------------------
 * PinMaping
 * ------------------------------------------------------------------ 
 */
#define GSM_NS 21 //connecter non utiliser
#define GSM_KEY 14
#define GSM_PS  12
#define GSM_RES 22 //connecter non utiliser
#define GSM_RX  16
#define GSM_TX  17

/**
 * Definition des paramétre GSM
 */
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
/**
 * Code PIN
 */
String GSM_PIN = "4352";

/**
 * definition de la vitesse de communication des port Série
 */
#define Serial_baud_rate 115200

void setup() {
  //USB
  Serial.begin(Serial_baud_rate);
  while (!Serial) {
    delay(1); //attend que le port série "USB" se connecte
    }
  Serial.println("");
  Serial.println("----------------------------------");
  Serial.println("               SETUP              ");
  Serial.println("");
  Serial.println("Serial 1 (USB)  : OK");
      
  //FONA
  Serial2.begin(Serial_baud_rate, SERIAL_8N1, GSM_RX, GSM_TX);
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
    
  Serial.println("");
  Serial.println("----------------------------------");
}

void loop() {

  if(Serial.available()!= 0) {
    char x = Serial.read();
    
      if (x == 65 or x == 97) { // A ou a : arrêt Fona
        Serial.print("commande envoyé : ");
        Serial.println(x);
        gsm(OFF);
        } 
    
      if (x == 77 or x == 109) { // M ou m : marche Fona
        Serial.print("commande envoyé : ");
        Serial.println(x);
        gsm(ON);
        }
      if (x == 69 or x == 101) { // E ou e état de la connection réseau
        Serial.print("commande envoyé : ");
        Serial.println(x);
        gsm_SIM_unlock();
        Serial.println (gsm_connected());
      }
      if (x == 83 or x == 115) { // S ou s envoi un SMS
        if (etat_gsm()) {
            gsm_envoi_sms("0622149016","E.T. TELEPHONE MAISON"); // remplacer 0000000000 par le numéro du téléphone visé
        }
      }
    }
  }

/**
 * ------------------------------------------------------------------
 *      FONCTION
 * ------------------------------------------------------------------
 */

/**
 * ------------------------------------------------------------------
 * statue GSM
 */
bool gsm(int status) {
  if (status == ON) {
    if (!etat_gsm()) {
      // le Fona est sur OFF , commande la mise en marche
      Serial.println ("   démarage en cours ...");
      // commande Key sur Low
      digitalWrite(GSM_KEY,LOW);
      while (!etat_gsm()){
        delay(1);
      }
      digitalWrite(GSM_KEY,HIGH);
      Serial.println ("     Module sous tension");
      Serial.println ("");
      
    }
  } else {
    if (etat_gsm()) {
      // le Fona est sur ON , commande l'arrêt
      Serial.println ("   arrêt en cours ...");
      digitalWrite(GSM_KEY,LOW);
      while (etat_gsm()){
        delay(1);
      }
      digitalWrite(GSM_KEY,HIGH);
      Serial.println ("     Module hors tension");
      Serial.println ("");
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
 * Procedure de connection
 */
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

/**
 * ------------------------------------------------------------------
 * Demande de code pin
 */
void gsm_SIM_unlock() {
  Serial.println ("Envoi code PIN");
  delay(2000);
  Serial2.println("AT+CPIN="+GSM_PIN);
  delay(500);
  
}

/**
 * ------------------------------------------------------------------
 * Envoie SMS
 */
void gsm_envoi_sms(String tel, String msg) {
          // passage en mode "TEXTE"
        Serial.println ("Envoi message : "+msg);
        Serial2.println("AT+CMGF=1");
        delay(100);
        // indication du numéro à appeler
        Serial2.println("AT+CMGS=\""+tel+"\"");      // remplacer par le numéro du téléphone auquel envoyer le SMS
        delay(100);
        // Envoi du texte
        Serial2.println(msg);
        delay(100);
        // fin de transmission
        Serial2.println((char)26);

}
