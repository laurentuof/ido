/**
 * ---------------------------------------------------------------------
 * 
 *                        09_testGravity_I2C_2UART
 * 
 * - Ce programe a pour objectif d'interfacer un Firebeetle ESP-32 avec 
 *  un Module FONA Mini GSM via un module I²C vers 2UART (Gravity).
 *    
 * - By Laurent Marchal & Arthur Pignalet
 * 
 * - DATE : juin 2021
 * 
 * - licence d'utilisation : CC-BY-SA 
 * ---------------------------------------------------------------------
 *          PINMAPPING
 * FireBeetle    <->    Gravity    <->    FONA
 *  IO 22 (SCL)      C : T (Uart2)        RX
 *  IO 21 (SDA)      D : C (Uart2)        TX
 *  GND              - : -                GND
 *  3.3 V            + : +                Vio
 *  IO 39 (A1)                            PS
 *  IO 17 (LRCK)                          Key
 * ---------------------------------------------------------------------
 *          FONCTION
 * 
 * - SETUP
 * - LOOP
 * ---------------------------------------------------------------------
 */

/**********************************************************************/

#include <DFRobot_IICSerial.h>

/**********************************************************************/

//*build.UART1*/DFRobot_IICSerial I2C_Serial1(Wire, /*subUartChannel =*/SUBUART_CHANNEL_1,/*IA1 = */1,/*IA0 = */1);
/*build.UART1*/DFRobot_IICSerial I2C_Serial1(Wire, SUBUART_CHANNEL_1,1,1);
/*build.UART2*/DFRobot_IICSerial I2C_Serial2(Wire, SUBUART_CHANNEL_2,1,1);
char tx_buffer[256];//Define the TX array tx_buffer
char rx_buffer[256];//Define the RX array rx_buffer

/**********************************************************************/

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


/*SET.iicUART1*/I2C_Serial1.begin(/*baud = */115200, /*format = */IICSerial_8N1);
               delay(100);
/*TEST_Gravity_SERIAL(1)*/
Serial.print("iicSerial_1 ? : ");
while(!I2C_Serial1){
  Serial.print(".");
  delay(200);
  }
Serial.println("");
Serial.println("                                iicSerial_1 OK");


/*SET.iicUART2*/I2C_Serial2.begin(/*baud = */115200, /*format = */IICSerial_8N1);
               delay(100);
/*TEST_Gravity_SERIAL(2)*/
Serial.print("iicSerial_2 ? : ");
while(!I2C_Serial2){
  Serial.print(".");
  delay(200);
  }
Serial.println("");
Serial.println("                                iicSerial_2 OK");

/**********************************************************************/
Serial.println("");
Serial.println("                             END_SETUP");
Serial.println("---------------------------------------------------------------------");
}

void loop() {
  while(Serial.available()){
/*USB_00*/
    Serial.write(tx_buffer, sizeof(tx_buffer)-1);//determine les donnée du buffer USB a envoyée
    Serial.flush();
    size_t n0 = Serial.read(rx_buffer, sizeof(rx_buffer));//détermine la quantiter de donnée reçus dans le buffer
    Serial.print("RX_USB    (0): ");
      for(size_t i = 0; i < n0; i++){ //met dans le buffer rx_UART_1 les donnée lue sur le port USB
        rx_buffer[i];
      }
    delay(1);
    Serial.print(rx_buffer);//vérification via moniteur serie des donnée du buffer de l'UART1
    I2C_Serial1.prints(rx_buffer);// écrit les donnée du buffer USB a destination de l'UART1

/*UART_01*/
    I2C_Serial1.write(tx_buffer, sizeof(tx_buffer)-1);
    I2C_Serial1.flush();
    size_t n1 = I2C_Serial1.read(rx_buffer, sizeof(rx_buffer));
    Serial.print("RX_buffer (1): ");
      for(size_t i = 0; i < n1; i++){
        rx_buffer[i];
      }
    delay(1);
    Serial.print(rx_buffer);//vérification via moniteur serie des donnée du buffer de l'UART1
    I2C_Serial2.print(rx_buffer);// écrit les donnée du buffer UART1 a destination de l'UART2

/*UART_02*/
    I2C_Serial2.write(tx_buffer, sizeof(tx_buffer)-1);
    I2C_Serial2.flush();
    size_t n2 = I2C_Serial2.read(rx_buffer, sizeof(rx_buffer));
    Serial.print("RX_buffer (2): ");
      for(size_t i = 0; i < n2; i++){
        rx_buffer[i];
      }
    delay(1);
    Serial.print(rx_buffer);// écrit les donnée du buffer UART2 a destination de l'USB
  }
}
