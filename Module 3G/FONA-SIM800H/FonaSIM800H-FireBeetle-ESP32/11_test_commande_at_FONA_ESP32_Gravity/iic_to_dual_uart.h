/*
* ------------------------------------------------------------------
 * 
 *     iic_to_dual_uart.h
 * 
 * - Bibliothèque pour utiliser le module I2C vers 2 UART 
 *  
 *      Fonctions disponibles :
 *            void iicSerialBegin(byte port, long int vitesse, byte format=IICSerial_8N1)
 *            void iicSerialEnd(byte port)
 *            bool iicSerialAvailable(byte port)
 *            byte iicSerialRead(byte port)
 *            void iicSerialWrite(byte port, byte n)
 *            void iicSerialPrintln(byte port, String *texte)
 *            void iicSerialFlush(byte port);
 *  
 *  
 *    
 * - By Laurent Marchal
 * 
 * - DATE : juin 2021
 * 
 * - licence d'utilisation : CC-BY-SA
 */


// paramètrage de l'adresse IIC du module.
// configurer les microswitch en conséquence
#define IICtodualUARTadresseA0 1
#define IICtodualUARTadresseA1 1


// bibliothèque initiale
#include <DFRobot_IICSerial.h>


// Construction de l'UART 1 (liaison vers ...)
DFRobot_IICSerial iicSerial1(Wire,SUBUART_CHANNEL_1,IICtodualUARTadresseA1,IICtodualUARTadresseA0);//Construct UART1

// Construction de l'UART 1 (liaison vers ...)
DFRobot_IICSerial iicSerial2(Wire, SUBUART_CHANNEL_2,IICtodualUARTadresseA1,IICtodualUARTadresseA0);//Construct UART2





//---------------------------------------------------------------------------
void iicSerialBegin(byte port, long int vitesse, byte format=IICSerial_8N1) {
  //
  //  port : 1 ou 2
  //
  //  vitesse :     2400
  //                4800
  //                7200
  //               14400
  //               19200
  //               28800
  //               38400
  //               57600
  //               76800
  //              115200
  //              153600
  //              230400
  //              460800
  //              307200
  //              921600
  //
  //  format :  IICSerial_8N1
  //                IICSerial_8N2
  //                IICSerial_8Z1
  //                IICSerial_8Z2
  //                IICSerial_8O1
  //                IICSerial_8O2
  //                IICSerial_8E1
  //                IICSerial_8E2
  //                IICSerial_8F1
  //                IICSerial_8F2
  //                8 represents the number of data bit,
  //                N for no parity,
  //                Z for 0 parity
  //                O for Odd parity
  //                E for Even parity,
  //                F for 1 parity
  //                1 or 2 for the number of stop bit
  //                Default IICSerial_8N1
  //
  //
  //
  if (port ==1) {
    iicSerial1.begin(vitesse,format);
  }
  if (port==2) {
    iicSerial2.begin(vitesse,format);
  }
}

//---------------------------------------------------------------------------
void iicSerialEnd(byte port) {
  //
  //  Termine une liaison UART
  //
  if (port == 1) {
    iicSerial1.end();
  }
  if (port == 2) {
    iicSerial2.end();
  }

  
}


//---------------------------------------------------------------------------
bool iicSerialAvailable(byte port) {
  //
  //  Y a t il des données disponibles en lecture sur un des ports
  //     Renvoie true si oui, false si non
  //
  if (port == 1) {
    if (iicSerial1.available()) return true; else return false;
  }
  if (port == 2) {
    if (iicSerial2.available()) return true; else return false;
  }
}

//---------------------------------------------------------------------------
byte iicSerialRead(byte port) {
  if (port ==1) {
    return(iicSerial1.read());
  }
  if (port==2) {
    return(iicSerial2.read());
  }
}


//---------------------------------------------------------------------------
void iicSerialWrite(byte port, byte n) {
  //
  //  Envoie octet sur l'un des ports
  //
  if (port == 1) iicSerial1.write(n);
  if (port == 2) iicSerial2.write(n);

}

//---------------------------------------------------------------------------
void iicSerialPrintln(byte port, String *texte) {
  //
  //  Envoie une chaine de caractères sur l'un des ports
  //
  if (port == 1) iicSerial1.println(*texte);
  if (port == 2) iicSerial2.println(*texte);
}
  
//---------------------------------------------------------------------------
void iicSerialFlush(byte port) {
  //
  // Attend que le buffer émission d'un port soit vide
  //
  if (port == 1) iicSerial1.flush();
  if (port == 2) iicSerial2.flush();
  
}
