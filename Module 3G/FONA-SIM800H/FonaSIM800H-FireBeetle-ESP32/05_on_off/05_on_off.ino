/**
 * ------------------------------------------------------------------
 * 
 *                        05_ON_OFF
 *                        
  * - Ce programe a pour objectif de tester une commande
 *    manuel de marche/arret du module FONA MniGSM via 
 *    FireBeetle ESP-32.
 * 
 * - By Laurent Marchal & Arthur Pignalet
 * 
 * - DATE : juin 2021
 * 
 * - licence d'utilisation : CC-BY-SA 
 * ------------------------------------------------------------------         
 * NOTE : !TEST EFFECTUER SANS CARTE SIM DANS LE MODULE Fona!
 *        !Le PinMapping ne corespond pas a ça version final!
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
 * 
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
  digitalWrite(GSM_KEY,HIGH);
  pinMode (GSM_KEY,OUTPUT);
  
  // initialisation de la lecture du Power Status
  pinMode (GSM_PS, INPUT);
  
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
 */
