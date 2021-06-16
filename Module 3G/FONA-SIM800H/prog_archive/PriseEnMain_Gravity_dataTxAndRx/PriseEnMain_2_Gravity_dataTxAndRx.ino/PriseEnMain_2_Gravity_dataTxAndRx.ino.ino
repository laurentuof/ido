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


/*SET.IIUART1*/I2C_Serial1.begin(/*baud = */115200, /*format = */IICSerial_8N1);
               delay(100);
/*TEST_Gravity_SERIAL(1)*/
Serial.print("iicSerial_1 ? : ");
while(!I2C_Serial1){
  Serial.print(".");
  delay(500);
  }
Serial.println("");
Serial.println("                                iicSerial_1 OK");


/*SET.IIUART2*/I2C_Serial2.begin(/*baud = */115200, /*format = */IICSerial_8N1);
               delay(100);
/*TEST_Gravity_SERIAL(2)*/
Serial.print("iicSerial_2 ? : ");
while(!I2C_Serial2){
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

void loop() {

  while(Serial.available()){
    
/*UART_01*/
    if(I2C_Serial1.available()){
      I2C_Serial1.write(tx_buffer, sizeof(tx_buffer)-1);
      I2C_Serial1.flush();//Wait for the data to be transmited completely 
      size_t n1 = Serial.read(/*pBuf = */rx_buffer, /*size = */sizeof(rx_buffer));
      Serial.print("rx1_buffer cache's data: ");
        for(size_t i = 0; i < n1; i++){
          rx_buffer[i];
        }
      delay(1);
      Serial.print(rx_buffer);
      I2C_Serial2.print(rx_buffer);
      Serial.println();
  }

/*UART_02*/
    if(I2C_Serial2.available()){
      I2C_Serial2.write(/*pBuf = */tx_buffer, /*size = */sizeof(tx_buffer)-1);//Transmit the data of array tx_buffer via Sub UART, 256 bytes at most every time
      I2C_Serial2.flush();//Wait for the data to be transmited completely 
      size_t n2 = I2C_Serial2.read(/*pBuf = */rx_buffer, /*size = */sizeof(rx_buffer));//Return the readings and store in array a, 256 bytes at most every time 
      Serial.print("rx2_buffer cache's data: ");
        for(size_t i = 0; i < n2; i++){
          rx_buffer[i];
        }
      delay(1);
      Serial.print(rx_buffer);
      Serial.println();
    }
    
  }
}
