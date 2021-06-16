/*
 * --------------------------------------------------
 *                 04_test_input_output_Arduino_Uno
 *                 
 * - Ce programe est la 2éme partie du test a implanter dans l'arduino Uno
 *   permetant de lire et renvoyer les donnée present sur le port serie.
 * 
 * - By Laurent Marchal & Arthur Pignalet
 * 
 * - DATE : juin 2021
 * 
 * - licence d'utilisation : CC-BY-SA
 * -------------------------------------------------- 
  Plastron de test pour FireBeetle afin de tester la liaison
  UART logicielle sur les ports analogiques.

  L'arduino UNO est configuré comme suit :
    Rx : D4
    Tx : D3


   Les liaisons (Serial et SoftwareSerial sont configurées à 115200 bits par seconde.
   Un cordon torsadé de 50 cm relie les 2 circuits

   Une fois les liaisons séries (USB et ESP32 établies, le programme attend des données sur les deux liaisosns et les écrit sur l'autre liaison


    ATTENTION : le fait de courtcircuiter Tx et Rx por réaliser un loopBack sur la liaison SoftwareSerial ne fonctionne pas.
    Ceci est inhérent au fonctionnement de la liaison série logicielle


 */
#include <SoftwareSerial.h>

SoftwareSerial mySerial(4, 3); // RX, TX

void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Native USB only
  }


  Serial.println("Goodnight moon!");

  // set the data rate for the SoftwareSerial port
  mySerial.begin(115200);
  mySerial.println("Hello, world?");
}

void loop() // run over and over
{
  if (mySerial.available())
    //Serial.println ("ok");
    Serial.print(mySerial.read());
    
  if (Serial.available())
    mySerial.print(Serial.read());
}
