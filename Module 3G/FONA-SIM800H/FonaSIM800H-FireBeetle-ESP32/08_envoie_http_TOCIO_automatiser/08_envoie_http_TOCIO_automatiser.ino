/** 
 *  ------------------------------------------------------------------
 *  
 *                  08_envoie_http_TOCIO_automatiser
 *                        
 * - Ce programe a pour objectif d'ajouter et d'automatiser la procedure 
 * d'envoie HTTP(GPRS) au programme precedente de connection au reseau GSM.
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
#include <HardwareSerial.h>

unsigned long intervale = 30000;  // intervale de temps entre 2 envois HTTP

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
void initialisation_port_USB() {
  Serial.begin(USB_SERIAL_RATE);
  while (!Serial) {
    ;
  }
  for(byte i=0; i<20;i++) Serial.println ("");
  Serial.println ("----------------------------------------");
  Serial.println ("Port USB initialisé");
  Serial.println ("");
}
