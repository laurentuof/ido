/**
 *
 * --------------------------------------------------
 *                 03_test_liaison_serie
 *                 
 * - Ce programe est un test intermédiaire du port 
 * Serial2. Ce dernier a également été utiliser 
 * pour tester les commande AT via le moniteur serie.
 * 
 * - By Laurent Marchal & Arthur Pignalet
 * 
 * - DATE : juin 2021
 * 
 * - licence d'utilisation : CC-BY-SA
 * --------------------------------------------------        
 * NOTE : !TEST EFFECTUER SANS CARTE SIM DANS LE MODULE Fona!
 * - - - - - - -  - - - - - - - - - - - - - - - - - - 
 * Ce programe permet de vérifier en mettant dans le champs de saisie 
 * du moniteur serie les commande AT tel que AT qui renvoie ok ou
 * d'autre commande permetant de récupérer des information du Fona
 * 
 * liste de commande AT: LIEN : 
 * https://www.technologuepro.com/gsm/commande_at.htm
 * 
 * --------------------------------------------------
 * 
 *                    UART_Mapping FireBeetle
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
 * -------------------------------------------------- 
 * 
 *                    PinMapping
 *        
 *    FONA     FireBeetle
 *     
 *     VIO     5v
 *     BAT     + d'une batterie extérieure (3.7V par exemple)
 *     Tx      D16(rx)
 *     Rx      D17(tx)
 *     Key     GND
 *     
 * --------------------------------------------------
 * 
 *                    Fonction
 * 
 * initialisation : boche utiliser
 *                  Liaison UART0 pour l'USB
 *                  Liaison Uart2 pour le module Fona
 *                  
 * Boucle : on envoie sur la liaison Uart2 ce que l'on li sur l'Uart0
 *          puis,
 *          on envoie sur la liaison Uart0 ce que l'on li sur l'Uart2
 *          
 * -------------------------------------------------- 
 */         

#define GSM_RX 16
#define GSM_TX 17
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
  
  Serial.println("");
  Serial.println("----------------------------------");
}

void loop() {


   while (Serial.available()) {
     Serial2.print(char(Serial.read()));
   }

   while (Serial2.available()) {
     Serial.print(char(Serial2.read()));
   }


}
