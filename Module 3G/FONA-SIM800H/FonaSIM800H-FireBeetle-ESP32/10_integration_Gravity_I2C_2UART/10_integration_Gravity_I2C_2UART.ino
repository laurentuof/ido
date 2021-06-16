/** 
 *  ------------------------------------------------------------------
 *  
 *                  10_interagtion_Gravity_I²C_2UART
 *                        
 *  Ce programme a pour but d'intégrer au montage 2 un module Gravity (I²C vers 2 UART) 
 *  permettant ainsi d’obtenir un montage électronique proche de sa version finale.
 *    
 * - By Laurent Marchal & Arthur Pignalet
 * 
 * - DATE : juin 2021
 * 
 * - licence d'utilisation : CC-BY-SA
 * ------------------------------------------------------------------
 */

/**
 * ------------------------------------------------------------------
 * Procedure_envoie_HTTP
 */
#include "configuration.h"
#include "code_PIN.h"
#include "GSM.h"

/*intervale de temps entre 2 envois HTTP*/
unsigned long intervale = 30000;
/**
 * ------------------------------------------------------------------
 *          SETUP
 * ------------------------------------------------------------------
 */
void setup() {
  initialisation_port_USB();
  gsm_init();

  envoi_donnees_GSM();
  }

/**
 * ------------------------------------------------------------------
 *          LOOP
 * ------------------------------------------------------------------
 */
void loop() {

}

/**
 * ------------------------------------------------------------------
 *          INIT_USB_PORT
 * ------------------------------------------------------------------
 */
bool initialisation_port_USB() {
/*SET.USB*/  Serial.begin(USB_SERIAL_RATE);
  while (!Serial) {
    ;
  }
  for(byte i=0; i<20;i++) Serial.println ("");
  delay(100);
Serial.println("---------------------------------------------------------------------");
Serial.println("                                 USB_SETUP");

/*TEST_USB_SERIAL*/
Serial.print("USB_Serial1 ? : ");
debut = millis();
  if(!Serial){
    Serial.print(".");
    delay(100);
    if (millis() - debut > delais) {
      return KO;
    }
  }
Serial.println("");
Serial.println("                                USB_Serial  OK");
Serial.println("");
Serial.println("---------------------------------------------------------------------");
return OK;
}
