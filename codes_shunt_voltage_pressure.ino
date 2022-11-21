#include <LiquidCrystal.h>

//Questo esempio mostra come prendere un trasduttore di pressione a 3 fili standard e leggere il segnale analogico, quindi convertire il segnale in un'uscita leggibile e visualizzalo su uno schermo LCD.
 
//**transducer variables and libraries**//

#include "Wire.h" // consente la comunicazione su dispositivi i2c
#include "HardwareSerial.h"
#include <stdio.h> 
//#include "LiquidCrystal_I2C.h" // consente l'interfacciamento con gli schermi LCD

const int pressureInput1 = A1; // seleziona il pin di ingresso analogico per il trasduttore di pressione 1
const int pressureInput2 = A2; // seleziona il pin di ingresso analogico per il trasduttore di pressione 2
const int pressureInput3 = A3; // seleziona il pin di ingresso analogico per il trasduttore di pressione 3
unsigned long times;

const int pressureZero = 500; // lettura analogica del trasduttore di pressione a 0bar (minima pressione)[la formula e x/5000=0.5V(volt a lettura 0 psi)/5V (max voltage range)]
const int pressureMax = 4500; // lettura analogica del trasduttore di pressione a (massima pressione) [la formula e x/5000=4.5V(volt a lettura max psi)/5V (max voltage range)]
const int pressuretransducermaxPSI = 5000; // valore psi del trasduttore utilizzato
const int baudRate = 9600; // numero intero costante per impostare la velocità di trasmissione per il monitor seriale
const int sensorreadDelay = 1000; // numero intero costante per impostare il ritardo di lettura del sensore in millisecondi


float pressureValue1 =0;
float pressureValue2 =0;
float pressureValue3 =0;
float pressureValue11 = 0;
float pressureValue22 = 0;
float pressureValue33 = 0;
float pressureValueBAR1=0;// variabile per memorizzare il valore proveniente dal trasduttore di pressione
float pressureValueBAR2=0;
float pressureValueBAR3=0;

//**voltage variables**//

const float arduinoVCC = 5.01;//Your Arduino voltage
unsigned long ValueR1 = 4600;
unsigned long ValueR2 = 61100;
double Voltage_Source = 70;
const int alanogPin = A0;//the pin connecting the voltage. 
const int inputResolution =1023;//works with most Arduino boards
const float average_of = 500;//Average of 500 readings

float voltage;


//**current variables and libraries**//

#include <Adafruit_ADS1X15.h>

#include <Wire.h>

Adafruit_ADS1115 ads; 


//LiquidCrystal_I2C lcd (0x3f, 20, 4); // imposta l'indirizzo di comunicazione I2C dell'LCD; formato (indirizzo, colonne, righe)

void setup () // routine di configurazione, viene eseguita una volta quando il sistema viene acceso o ripristinato
{ 
  Serial.begin (9600); 
  ads.setGain(GAIN_SIXTEEN);
  ads.begin();// inizializza la comunicazione seriale alla velocità di trasmissione impostata bit al secondo
//  lcd.begin (); // inizializza lo schermo LCD
  //Serial.print("time\t");
  Serial.print("P1\t");
  Serial.print("P2\t");
  Serial.print("P3\t");
  Serial.print("Vin\t");
  Serial.println("Amps:\t");
 
  
 // Serial.println("LABEL, data e ora, ColonnaA, ColonnaB, ColonnaC");
}

void loop () // la routine loop viene eseguita più e più volte per sempre //ricorda 280 al posto di 100

{ for(int i=0; i<100; i++){
  pressureValue1 =( pressureValue1 * i +analogRead (pressureInput1)*5.0/1023)/(i+1); // legge il valore dal pin di input e assegna alla variabile
  delay (1);}

  for(int i=0; i<100; i++){
  pressureValue2 =( pressureValue2 * i +analogRead (pressureInput2)*5.0/1023)/(i+1); // legge il valore dal pin di input e assegna alla variabile
  delay (1);}

  for(int i=0; i<100; i++){
  pressureValue3 =( pressureValue3 * i +analogRead (pressureInput3)*5.0/1023)/(i+1); // legge il valore dal pin di input e assegna alla variabile
  delay (1);}


  pressureValue11 = ((pressureValue1*1000.0-pressureZero) * (pressuretransducermaxPSI-14.5)) / (pressureMax-pressureZero)+14.5; // equazione di conversione per convertire la lettura analogica in psi
  pressureValueBAR1 = pressureValue11*0.068;
   
  pressureValue22 = ((pressureValue2*1000.0-pressureZero) * (pressuretransducermaxPSI-14.5)) / (pressureMax-pressureZero)+14.5; // equazione di conversione per convertire la lettura analogica in psi
  pressureValueBAR2 = pressureValue22*0.068; 
  
  pressureValue33 = ((pressureValue3*1000.0-pressureZero) * (pressuretransducermaxPSI-14.5)) / (pressureMax-pressureZero)+14.5; // equazione di conversione per convertire la lettura analogica in psi
  pressureValueBAR3 = pressureValue33*0.068; 

  int16_t ads1;

  ads1 = ads.readADC_Differential_0_1();

  float amps = ((float)ads1 * 256.0) / 32768.0;
  amps = amps * 1.333; 

  Serial.println(amps);


  
  //Serial.print(times);
  //Serial.print("\t");
  Serial.print (pressureValueBAR1); // stampa il valore dalla riga precedente in seriale
  Serial.print("\t");
  Serial.print (pressureValueBAR2); 
  Serial.print("\t"); 
  Serial.print (pressureValueBAR3); 
  Serial.println("\t"); 
  Serial.print(getVoltageAverage());
  Serial.println("\t"); 
  Serial.print(amps);
  Serial.println("\t");
  delay(500);
 // Serial.println ("DATA, DATETIME", pressureValueBAR1, pressureValueBAR2, pressureValueBAR3); 
 // delay(1);
  


  
  //lcd.setCursor (0,0); // imposta il cursore sulla colonna 0, riga 0
  //lcd.print ("Pressione:"); // stampa l'etichetta
  //lcd.print (pressureValue, 1); // stampa il valore della pressione sullo schermo LCD, 1 cifra su float
  //lcd.print ("psi"); // stampa l'etichetta dopo il valore
  //lcd.print (""); // per cancellare il display dopo valori grandi o negativi
  //ritardo (sensorreadDelay); // ritardo in millisecondi tra i valori letti
}
void readVoltage(){
    int A0Value = analogRead(alanogPin);
    float voltage_sensed = A0Value * (arduinoVCC / (float)inputResolution); 
    float errorResistor1 = 0.05/sqrt(3);
    float errorResistor2 = 0.05*4/sqrt(3);
    float errorResistances = sqrt((pow(errorResistor1, 2))+pow(errorResistor2, 2));
//  Serial.print("voltage_sensed:");
//  Serial.print(voltage_sensed);       
    voltage = voltage_sensed * ( 1 + ( (float) ValueR2 /  (float) ValueR1) )- errorResistances*((float) ValueR2 /  (float) ValueR1);  
}//readVoltage()


float getVoltageAverage(){ 
    float voltage_temp_average=0;
    for(int i=0; i < average_of; i++)
    {
       readVoltage();
       voltage_temp_average +=voltage;
    }
      
  return voltage_temp_average / average_of;}
