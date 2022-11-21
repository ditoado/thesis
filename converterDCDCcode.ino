#include <ArduinoRS485.h>
#include <Adafruit_ADS1X15.h>

#include <Wire.h>

Adafruit_ADS1115 ads; 
#if defined(ARDUINO_PORTENTA_H7_M7)
  #include <Arduino_MachineControl.h> 
#endif
#include <ArduinoModbus.h>
#include "Strutture_Dati.h"

#define COLLAUDO true                         //è possibile richiedere il collaudo senza ricaricare lo sketch cambiando il valore logico al pin (false se non lo voglio nel serale)
/*--IMPOSTAZIONI RS485 MODBUS RTU--*/
#define BAUDRATE_MODBUS 38400                 // alternativa del dc converter e' 57600
#define PREAMBLE 10                           //parametri necessari per la classe rs485, valori classici sono preamble = 10 e postamble = 500 (microsecondi)
#define POSTAMBLE 500                         //modificare con cautela: possibile corruzione del messaggio!
#define ID_CONVERTER 0x06
/*se non si usa la machine control*/
#define TXPIN 18
#define DEPIN 13
#define REPIN 12
/*--PERIODI DI TEMPO DELLE VARIE OPERAZIONI--*/
#define CONVERTER_RATE 100                    //tempo di iterazione per convertitore cc in millisecondi
#define CONVERTER_UPDATE_RATE 10000           //tempo di aggiornamento dei riferimenti in millisecondi
#define AGGIORNA_SERIALE 5000
#define TIMEOUT_COMM_CONV 5000                //ogni quanto si vuole forzare un aggiornamento al convertitore cc il dc converter va in blocco se non riceve setpoint per 10 secondi
#define PERIODO_IN_SERIALE 500                //ogni quanto si vuole che il controllore verifichi la presenza di input sul canale seriale per la modifica dei riferimenti
/*--PARAMETRI PER LIMITARE LA PENDENZA DELLA CORRENTE--*/
#define METODO 1                              //definisce il metodo da utilizzare: 0 1o ordine, 1 rampa, -1 nessun smussamento del segnale
#define TAU_CONVERTER_V 0.5                   //costanti di tempo usate nella funzione compute per smorzare i riferimenti dati al convertitore
#define TAU_CONVERTER_P 1.0
#define MAX_SLOPE_V 5                         //calcolata come massima derivata per il tempo di giro del convertitore cc
#define MAX_SLOPE_P 5
/*-- MISCELLANEOUS --*/
#define MESSAGGIO_SOLO_SE_DIVERSO false       //il controllore comunica i riferimenti al convertitore soltanto se sono cambiati
#define PIN_COLLAUDO 1
#define MAPPA_BIT_ERRORE 0001111100011111     //bit a zero vengono ignorati

#if defined(ARDUINO_PORTENTA_H7_M7)
  using namespace machinecontrol;  
#endif         
//definizione stati della macchina a stati
enum Status {ini, lettura_in, lettura_hold, scrittura, errore};
enum Errore {errore_ini, errore_lett_in, errore_lett_hold, errore_scrit, no_errore, errore_conv};

//variabili per definire la frequenza delle operazioni
unsigned long  prev_conv=0, prev_conv_update=0, prev_conv_update_collaudo=0, prev_collaudo=0, prev_aggiorna_seriale=0, prev_comm_conv=0;

union Un_Uscita_Modbus Dati_Dal_Regolatore;     //Questi sono i dati letti dal regolatore
union Un_Ingresso_Modbus Set_Letti_Dal_Regolatore, Set_Da_Inviare_Al_Regolatore; //dati letti in fase di inizializzazione e dati da mandare al regolatore
Status stato = ini;
Errore stato_errore = no_errore;

unsigned short set_tensione, set_corrente, set_potenza;   //riferimenti richiesti dal pms
#if METODO
  unsigned short x_V, x_P;                                       
#else
  float x_V, x_P;                           
#endif

bool collaudo = COLLAUDO;
unsigned long current_millis;
bool usa_pms[3];

RS485Class* rs485;
/*in fase di compilazione arduino ide crea un header .h che definisce la scheda su cui il programma viene caricato.
Siccome l'unica parte del codice che dipende dal tipo di scheda è l'inizializzazione della classe rs485, il metodo degli if define
permette di utilizzare il codice su diverse schede senza modifiche (a costo di aggiungere righe per ogni eccezione)  */
#if defined(ARDUINO_PORTENTA_H7_M7)
  COMMClass MC_comm;
#elif defined(ARDUINO_AVR_MEGA) || defined(ARDUINO_AVR_MEGA2560)
  RS485Class rs485connection = RS485Class(Serial1, TXPIN, DEPIN, REPIN);
#else
  #error "Unknown board"
#endif

ModbusRTUClientClass  modbus_comm;
//voltage sensor 
const float arduinoVCC = 5.01;//Your Arduino voltage
unsigned long ValueR1 = 4600;
unsigned long ValueR2 = 61100;
double Voltage_Source = 70;
const int alanogPin = A0;//the pin connecting the voltage. 
const int inputResolution =1023;//works with most Arduino boards
const float average_of = 500;//Average of 500 readings

float voltage;

void setup() {

  Serial.begin(9600);
  ads.setGain(GAIN_SIXTEEN); // 16x gain +/- 0.256V 1 bit = 0.125mV 0.0046875mV
  ads.begin();
  Serial.print("Amps: ");
  Serial.println("Vin\n");
  
  //pinMode(PIN_COLLAUDO, INPUT);

  if(collaudo)
    Serial.println("Modbus RTU on RS485 initialization");
  
  #if defined(ARDUINO_PORTENTA_H7_M7)
    MC_comm.init();
    MC_comm.rs485Enable(true);
    rs485 = &MC_comm.rs485;
  #else
    rs485 = &rs485connection;
  #endif
  rs485->setDelays(PREAMBLE,POSTAMBLE);  //Scolpito nella pietra RICORDARSI IL TEMPO DI TRANSIZIONE DEL TX/RX Hw DEFAULT 10, 500
    
  modbus_comm = ModbusRTUClientClass(*rs485);

 
  // start the Modbus RTU client
   if (!modbus_comm.begin(BAUDRATE_MODBUS)) {
     if(collaudo)     
      Serial.println("Failed to start Modbus RTU Client!");
    while (1);
  }

  for(int i=0; i<3; i++)
    usa_pms[i] = true;

  if(collaudo)
    Serial.println("Initialization done!");

}

void loop() {
  if((millis()-prev_conv)>500){
    readVoltage();

  //Serial.print(voltage);
  //Serial.print("V Avg: ");
    Serial.print(getVoltageAverage());
    Serial.println("V"); 
    int16_t ads1;

    ads1 = ads.readADC_Differential_0_1();

    Serial.println(ads1);

    float amps = ((float)ads1 * 256.0) / 32768.0;
    amps = amps * 1.333; 

    Serial.println(amps);
    prev_conv = millis();
  }
    
  
  //macchina a stati per convertitore cc
  if(millis()-prev_conv > CONVERTER_RATE){
    
    prev_conv = millis();
    switch(stato){
    case ini:
      enable();    
      break;

    case lettura_hold:
      readHoldingRegisterValues();
      break;

    case lettura_in:
      readInputRegisterValues();
      checkError();
      if(!collaudo || usa_pms[0] == true){
        set_tensione = Dati_Dal_Regolatore.Dati_Sh[0] + (unsigned short)20;  //tensione ingresso 2 volt sopra tensione uscita
        if(set_tensione > 66)
          set_tensione = 66;
        else if(set_tensione < 40)
          set_tensione = 40;
      }
      break;

    case scrittura:
      if(stato_errore == no_errore)     //aggiorna il riferimento soltanto se ha comunicato correttamente
        compute();
      if(!MESSAGGIO_SOLO_SE_DIVERSO || x_V !=  Set_Da_Inviare_Al_Regolatore.St.Set_Tensione_V10 || x_P != Set_Da_Inviare_Al_Regolatore.St.Set_Potenza_W || 
          stato_errore != no_errore || millis() - prev_comm_conv > TIMEOUT_COMM_CONV){ //se c'è stato un errore di comunicazione deve mandare lo stesso il messaggio

        Set_Da_Inviare_Al_Regolatore.St.Reset_Allarmi = 0;
        Set_Da_Inviare_Al_Regolatore.St.Abilita_Potenza = 1;
        Set_Da_Inviare_Al_Regolatore.St.Set_Tensione_V10 = x_V;
        Set_Da_Inviare_Al_Regolatore.St.Set_Corrente_A10 = set_corrente;
        Set_Da_Inviare_Al_Regolatore.St.Set_Potenza_W = x_P;          
        
        writeHoldingRegisterValues();
        if(MESSAGGIO_SOLO_SE_DIVERSO)
          prev_comm_conv = millis();
      }

      break;

    case errore:
      manageErrors();
      break;

    default:
      if(collaudo)
        Serial.println("Sono finito dove non dovevo finire");
        stato = ini;
        stato_errore = no_errore;
      break;
    }
  }

  //nel caso si volesse dare dei riferimenti tramite connessione seriale
  else if(collaudo && (millis()-prev_conv_update_collaudo > PERIODO_IN_SERIALE)){
    prev_conv_update_collaudo = millis();
    float rif_ricevuti[3];
    if(Serial.available() > 0 ){
      rif_ricevuti[0] = Serial.parseFloat(); 
      rif_ricevuti[1] = Serial.parseFloat(); 
      rif_ricevuti[2] = Serial.parseFloat(); 
      if(Serial.read() == '\n'){
        if(rif_ricevuti[0] == -1.0){
          usa_pms[0] = true;
          set_tensione = Dati_Dal_Regolatore.Dati_Sh[0];
          Serial.println("Uso la tensione automatica.");
        }
        else if(rif_ricevuti[0] > 56.0){
          usa_pms[0] = false;
          Serial.println("La tensione richiesta è superiore a quella limite.");
          set_tensione = 560;
        }
        else if(rif_ricevuti[0] < 40.0){
          usa_pms[0] = false;
          Serial.println("La tensione richiesta è inferiore a quella limite.");
          set_tensione = 400;          
        }
        else{
          usa_pms[0] = false;
          set_tensione = rif_ricevuti[0]*10;
        }
        if(rif_ricevuti[1] == -1.0){
          usa_pms[1] = true;
          Serial.println("Imposto la corrente massima");
          set_corrente = 500;
        }
        else if(rif_ricevuti[1] > 50.0){
          usa_pms[1] = false;
          Serial.println("La corrente richiesta è superiore a quella limite.");
          set_corrente = 500;
          }
        else if(rif_ricevuti[1] < 0.0){
          usa_pms[1] = false;
          Serial.println("La corrente richiesta è inferiore a zero.");
          set_corrente = 0;
          }
        else{
          usa_pms[1] = false;
          set_corrente = rif_ricevuti[1]*10;
        }
        if(rif_ricevuti[2] == -1.0){
          usa_pms[2] = true;
          Serial.println("Uso la potenza automatica");
        }
        else if(rif_ricevuti[2] > 2500.0){
          usa_pms[2] = false;
          Serial.println("La potenza richiesta è superiore a quella limite.");
          set_potenza = 2500;
          }
        else if(rif_ricevuti[2] < 0.0){
          usa_pms[2] = false;
          Serial.println("La potenza richiesta è inferiore a zero.");
          set_potenza = 0;
          }
        else{
          usa_pms[2] =false;
          set_potenza = rif_ricevuti[2]; 
        }
        
        Serial.print("Riferimenti impostati: V= "); 
        Serial.print(set_tensione/10, DEC);
        Serial.print(" I= "); 
        Serial.print(set_corrente/10, DEC);
        Serial.print(" P =");
        Serial.println(set_potenza, DEC);
      }
      else{                                                      //pulisci il buffer da possibili schifezze
        while(Serial.available()){
          char getData = Serial.read();     
        }  
        Serial.println("Comando formulato erroneamente");
      }
    }
  }

  else if(collaudo && (millis()-prev_aggiorna_seriale > AGGIORNA_SERIALE)){
    prev_aggiorna_seriale = millis();

    if(stato_errore == no_errore){
      Serial.println();
      Serial.print("Riferimenti correnti: V ");
      Serial.print(float(set_tensione)/10, 2);
      Serial.print(" I ");
      Serial.print(float(set_corrente)/10, 2);
      Serial.print(" P ");
      Serial.println(set_potenza, 2);

      Serial.print("riferimenti correnti con transitori: V= ");
      Serial.print(float(x_V)/10, 2);
      Serial.print(" I= ");
      Serial.print(float(set_corrente)/10, 2);
      Serial.print(" P= ");
      Serial.println(x_P, 2);   
      Serial.print("tensione ingresso: ");
      Serial.println(float(Dati_Dal_Regolatore.St.V_Ingresso) / 10.0, 2);
      Serial.print("tensione uscita: ");
      Serial.println(float(Dati_Dal_Regolatore.St.V_Uscita) / 10.0, 2);
      Serial.print("corrente uscita: ");
      Serial.println(float(Dati_Dal_Regolatore.St.I_Uscita) / 10.0, 2);
      Serial.print("potenza uscita: ");
      Serial.println(Dati_Dal_Regolatore.St.P_Uscita, 2);
      Serial.print("errori: ");
      Serial.println(Dati_Dal_Regolatore.Dati_Sh[7], BIN);        //verificare se è corretto
    }
    else{
      switch (stato_errore){
        case errore_ini:
        Serial.println("errore in abilitazione convertitore cc");
      break;

      case errore_lett_hold:
        Serial.println("errore in lettura holding registers");
      break;

      case errore_lett_in:
        Serial.println("errore in lettura input registers");
      break;

      case errore_scrit:
        Serial.println("errore in scrittura holding registers");
      break;

      case errore_conv:
        Serial.println("errore del convertitore");
      break;
      }
    } 
  }

  //verifica ogni secondo se il pin ha cambiato stato 
  /*
  else if(!COLLAUDO && millis()-prev_collaudo > 1000){
    prev_collaudo = millis();
    if(collaudo ^ digitalread(PIN_COLLAUDO)){
      
      set_corrente = 500;
      collaudo = digitalread(PIN_COLLAUDO);
      }
  }
  */
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

void compute(){

  //riferimenti calcolati come risposte del primo ordine con i float
  #if METODO == 0
  x_V = x_V + CONVERTER_RATE / TAU_CONVERTER_V * (set_tensione - x_V); 
  x_P = x_P + CONVERTER_RATE / TAU_CONVERTER_P * (set_potenza - x_P);

  //riferimenti come rampa
  #elif METODO == 1
  if(set_tensione > x_V && set_tensione - x_V > MAX_SLOPE_V)     //attenzione agli overflow
    x_V += MAX_SLOPE_V;
  else if(set_tensione < x_V && x_V - set_tensione > MAX_SLOPE_V)
    x_V -= MAX_SLOPE_V;
  else
    x_V = set_tensione;

  if(set_potenza > x_P && set_potenza - x_P > MAX_SLOPE_P)
    x_P += MAX_SLOPE_P;
  else if(set_potenza < x_P && x_P - set_potenza > MAX_SLOPE_P )
    x_P -= MAX_SLOPE_P;
  else 
    x_P = set_potenza; 
  
  #elif METODO == -1
    x_V = set_tensione;
    x_P = set_potenza;
  #else
    #error "metodo invalido"
  #endif  

}

void writeHoldingRegisterValues() {
    
  modbus_comm.beginTransmission(ID_CONVERTER, HOLDING_REGISTERS, 0x00, DIM_ST_IN_MODBUS_SH);
  for (int i = 0; i < DIM_ST_IN_MODBUS_SH; i++) {
    modbus_comm.write(Set_Da_Inviare_Al_Regolatore.Dati_Sh[i]);
    //Serial.print(Set_Da_Inviare_Al_Regolatore.Dati_Sh[i], DEC);
  }
  //Serial.println();

  if (!modbus_comm.endTransmission()) {
    
    stato = errore;
    stato_errore = errore_scrit;

  } else {
    
    stato = lettura_in;
    stato_errore = no_errore;
  }
}

void readHoldingRegisterValues() {
 
  if (!modbus_comm.requestFrom(ID_CONVERTER, HOLDING_REGISTERS, 0x00, DIM_ST_IN_MODBUS_SH)) {
  
    stato = errore;
    stato_errore = errore_lett_hold;
  } 
  else {
    while (modbus_comm.available()) {
      for(int i=0; i<DIM_ST_IN_MODBUS_SH; i++){
        Set_Letti_Dal_Regolatore.Dati_Sh[i] = modbus_comm.read();
      }
      if(collaudo){
        Serial.println("successo");
        Serial.print("dati letti: ");
      
        Serial.print("Enable Logico: ");
        Serial.print(Set_Letti_Dal_Regolatore.St.Abilita_Potenza, BIN);
        Serial.print(", Reset Allarmi: ");
        Serial.print(Set_Letti_Dal_Regolatore.St.Reset_Allarmi, BIN);
        Serial.println();
        Serial.print("Riferimenti precedenti letti: ");
      
        Serial.print("V: ");
        Serial.print(Set_Letti_Dal_Regolatore.St.Set_Tensione_V10/10, DEC);
        Serial.print(" I: ");
        Serial.print(Set_Letti_Dal_Regolatore.St.Set_Corrente_A10/10, DEC);
        Serial.print(" P: ");
        Serial.print(Set_Letti_Dal_Regolatore.St.Set_Potenza_W, DEC);
        Serial.println();

      }
      
    #if METODO == 0
      x_V = float(Set_Letti_Dal_Regolatore.St.Set_Tensione_V10);
      x_P = float(Set_Letti_Dal_Regolatore.St.Set_Potenza_W);
    #else
      x_V = Set_Letti_Dal_Regolatore.St.Set_Tensione_V10;
      x_P = Set_Letti_Dal_Regolatore.St.Set_Potenza_W;
    #endif    
    set_tensione = Set_Letti_Dal_Regolatore.St.Set_Tensione_V10;
    set_corrente = Set_Letti_Dal_Regolatore.St.Set_Corrente_A10;
    set_potenza = Set_Letti_Dal_Regolatore.St.Set_Potenza_W;
    stato = lettura_in;
    stato_errore = no_errore;
    }
  }
}  

void readInputRegisterValues() {
  
  if (!modbus_comm.requestFrom(ID_CONVERTER, INPUT_REGISTERS, 0x00, DIM_ST_OUT_MODBUS_SH)) {
  
    stato = errore;
    stato_errore = errore_lett_in;

  } else {
    
    while(modbus_comm.available()){
      for (int i=0; i<DIM_ST_OUT_MODBUS_SH; i++) {
        Dati_Dal_Regolatore.Dati_Sh[i] = modbus_comm.read();
      }
    }

    stato = scrittura;
    stato_errore = no_errore;

  }

}

void enable() {

  // write the bitmap values to (slave) id ID, address 0x00
  Set_Da_Inviare_Al_Regolatore.Dati_Sh[0] = 0;
  Set_Da_Inviare_Al_Regolatore.St.Reset_Allarmi  = 1;

  
  if (!modbus_comm.holdingRegisterWrite(ID_CONVERTER, 0, Set_Da_Inviare_Al_Regolatore.Dati_Sh[0])) {
    
    stato_errore = errore_ini;
    stato = errore;      
    } 

    else {
      if(collaudo)
        Serial.println("abilitazione avvenuta con successo");
      stato = lettura_hold;   
      stato_errore = no_errore;     
  }

}

// funzione da modificare per avere il comportamento desiderato in caso di errori della macchina a stati
void manageErrors(){
  switch (stato_errore){
    case errore_ini:
      stato = ini;
      //stato_errore = no_errore;
      break;

    case errore_lett_hold:
      stato = lettura_hold;
      //stato_errore = no_errore;
      break;

    case errore_lett_in:
      stato = lettura_in;
      //stato_errore = no_errore;
      break;

    case errore_scrit:
      stato = scrittura;
      //stato_errore = no_errore;
      break;

    case errore_conv:
      stato = ini;
      //stato_errore = no_errore;
      break;

    default:
      if(collaudo)
        Serial.print("Sono capitato dove non dovevo capitare negli errori");
      stato = ini;
      stato_errore = no_errore;
      break;
  }


}

//funzione da modificare nel caso si voglia un determinato comportamento in caso di un errore specifico del convertitore
void checkError(){

  if((Dati_Dal_Regolatore.Dati_Sh[7] & MAPPA_BIT_ERRORE) !=0){  
    stato = errore;
    stato_errore = errore_conv;
  }
}
