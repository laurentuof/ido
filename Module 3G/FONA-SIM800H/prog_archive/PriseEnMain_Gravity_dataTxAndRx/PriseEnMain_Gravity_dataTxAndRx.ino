/**
 * ---------------------------------------------------------------------
 *  * - Ce programe a pour objectif d'interfacer un Firebeetle ESP-32 avec 
 *  un Module FONA Mini GSM via un module I²C vers 2UART (Gravity).
 *    
 * - By Laurent Marchal & Arthur Pignalet
 * 
 * - DATE : juin 2021
 * 
 * - licence d'utilisation : CC-BY-SA 
 * ---------------------------------------------------------------------
 * 
 */
#include <DFRobot_IICSerial.h>


uint8_t flag = 0;//A flag bit, judge whether to print the prompt information of UART1 and UART2.
//if it is 0, print "UART1 receive data: " or "UART2 receive data: "

/*build.UART1*/DFRobot_IICSerial iicSerial1(Wire, /*subUartChannel =*/SUBUART_CHANNEL_1,/*IA1 = */1,/*IA0 = */1);
/*build.UART2*/DFRobot_IICSerial iicSerial2(Wire, /*subUartChannel =*/SUBUART_CHANNEL_2, /*IA1 = */1,/*IA0 = */1);

/**---------------------------------------------------------------------
 *                        SETUP
 ---------------------------------------------------------------------*/
void setup() {

/*SET.USB*/  Serial.begin(115200);
             delay(100);
Serial.println("---------------------------------------------------------------------");
Serial.println("                                 SETUP");


/*TEST_USB_SERIAL*/
Serial.print("USB_Serial1 ? : ");
while(!Serial){
  Serial.print(".");
  delay(500);
  }
Serial.println("");
Serial.println("                                USB_Serial  OK");


/*SET.IIUART1*/iicSerial1.begin(/*baud = */115200, /*format = */IICSerial_8N1);
               delay(100);
/*TEST_Gravity_SERIAL(1)*/
Serial.print("iicSerial_1 ? : ");
while(!iicSerial1){
  Serial.print(".");
  delay(500);
  }
Serial.println("");
Serial.println("                                iicSerial_1 OK");


/*SET.IIUART2*/iicSerial2.begin(/*baud = */115200, /*format = */IICSerial_8N1);
               delay(100);
/*TEST_Gravity_SERIAL(2)*/
Serial.print("iicSerial_2 ? : ");
while(!iicSerial2){
  Serial.print(".");
  delay(500);
  }
Serial.println("");
Serial.println("                                iicSerial_2 OK");


/**********************************************************************/
Serial.println("");
Serial.println("                             END_SETUP");
Serial.println("---------------------------------------------------------------------");
}

/**---------------------------------------------------------------------
 *                        SETUP
 ---------------------------------------------------------------------*/
void loop() {
   while (Serial.available()){
    iicSerial1.println(Serial.read());
    Serial.println("envoie_a_Gravity(1) : ");
    Serial.print(char(Serial.read()));
    iicSerial2.println(Serial.read());
    Serial.println("envoie_a_Gravity(2) : ");
    Serial.print(char(Serial.read()));
    }
   
   while (iicSerial1.available()) {
     Serial.print(char(iicSerial1.read()));
     Serial.println("Gravity(1) : ");
     
     delay(500);
   }

   while (iicSerial2.available()) {
     Serial.println("Gravity(2) : ");
     Serial.print(char(iicSerial2.read()));
     
     delay(500);
   }

}
