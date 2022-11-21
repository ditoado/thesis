/********************************************************************************/
/* Strutture dati per comunicazione seriale con la scheda                       */
/* in questo caso modbus                                                        */
/********************************************************************************/
#ifndef __STRUTTURE_H
#define __STRUTTURE_H   1

//Qui introduco anche la versione del sw pc
#define VERSIONE_H              1
#define VERSIONE_L              2

//Minimi e massimi set accettabili da modbus
#define MIN_SE_TV_OUT_V10                       400         //40V
#define MAX_SET_V_OUT_V10                       560         //56V
#define MIN_SET_I_OUT_I10            0
#define MAX_SET_I_OUT_I10           610         //61A
#define MIN_SET_P_OUT_W                         0
#define MAX_SET_P_OUT_W                         3000        //W

//---------------------------------------------------------------------------------------------------
//Struttura dati di uscita sul bus 485 in comunicazione modbus
#define DIM_ST_OUT_MODBUS_CH  sizeof(struct St_Uscita_Modbus)
#define DIM_ST_OUT_MODBUS_SH  (sizeof(struct St_Uscita_Modbus)/2)


//Struttura definita per ACE
struct St_Uscita_Modbus
{

  unsigned short V_Uscita;            //V*10      //1
  unsigned short I_Uscita;            //I*10      //2
  unsigned short P_Uscita;            //W         //3
  unsigned short V_Ingresso;          //V*10      //4
  signed short T_Dissipatore_1;       //C*10      //5

  unsigned short In_Enable:1;         //Stato ingresso enable (Fisico)  //6
  unsigned short In_Manuale:1;        //Stato ingresso fisico di carica manuale
  unsigned short Dip_1:1;             //Stato dip 1
  unsigned short Dip_2:1;             //Stato dip 2
  unsigned short Dip_3:1;             //Stato dip 3
  unsigned short Dip_4:1;             //Stato dip 4
  unsigned short Out_Fault:1;         //Stato uscita fault
  unsigned short Out_Teleruttore:1;   //Rel di abilitazione teleruttore
  unsigned short Ventola_V1:1;        //Stato ventola Velocit 1
  unsigned short Ventola_V2:1;        //Stato ventola Velocit 2
  unsigned short Simulazione:1;       //Comunico sistema in simulazione
  unsigned short Unused_Bit_2:5;      //Libero


  unsigned short Uscita_Attiva:1;       //Stato di uscita attiva      //7
  unsigned short Avvio_Set_Interni:1;     //Si avvier in automatico con i set interni
  unsigned short Avvio_Manuale:1;       //Avvio in manuale
  unsigned short Ctr_Corrente:1;        //Controllo corrente
  unsigned short Ctr_Tensione:1;        //Controllo tensione
  unsigned short Ctr_Potenza:1;       //Controllo Potenza
  unsigned short Fg_Ctr_Liberi:10;


  unsigned short Allarme_In_Corso:1;          //Riassuntivo dell' allarme in corso //8
  unsigned short All_Termico:1;               //Allarme termico realtime
  unsigned short SottoTens_Ingresso:1;        //Sottotensione di ingresso realtime
  unsigned short All_Low_VAux:1;              //Bassa tensione di alimentazione realtime
  unsigned short All_Over_Tens_Out:1;         //Overtensione di uscita realtime
  unsigned short Err_Set_V_Modbus:1;          //Set di tensione fuori del range
  unsigned short Err_Set_I_Modbus:1;          //Si  immesso un set di Corrente fuori dai limiti
  unsigned short Err_Set_P_Modbus:1;          //Set di potenza fuori dai range
  unsigned short Lth_All_Low_Tens_Vin:1;      //Latch per l' allarme di bassa tensione di ingresso
  unsigned short Lth_All_Low_Tens_Ali:1;      //Lacth per l' allarme di bassa tensione di alimentazione +15V
  unsigned short Lth_All_Over_Tens_Out:1;     //Latch per l' allarme di sovratensione di uscita
  unsigned short Timeout_Set_Modbus:1;        //Timeout di aggiornamento set modbus
  unsigned short Timeout_Com_Modbus:1;        //Timeout in comunicazione modbus generica
  unsigned short Seg_Over_V_In:1;             //Segnalazione di massima tensione in ingresso
  unsigned short Liberi_all:2;


  unsigned short Fd_Set_Tensione_V10;         //Feed Set di Tensione          //9
  unsigned short Fd_Set_Corrente_A10;         //Feed Set di Corrente      //10
  unsigned short Fd_Set_Potenza_W;            //Feed Set di Potenza           //11
  unsigned short Alive_1s;                    //Contatore di Alive ad 1 secondo //12

  /*
  unsigned short Contaminuti;         //Conteggio minuti                  //13
  unsigned short Stato_Controllo;                 //14
  unsigned short Ver_Fw_Lw:8;         //Versione fw Basso     //15
  unsigned short Ver_Fw_Hg:8;         //versione fw Alto
  unsigned short Alimentazione_V_Aux_V10;     //16            Tensione di alimentazione V aux in volt*10
  //Dati per collaudo
  unsigned short Camp_V_Out;                   //17        //Quella su cui si aggancia il controllo
  unsigned short Camp_I_Out;                   //18        //Corrente di uscita
  unsigned short Camp_Potenza_Out;             //19        //Potenza di uscita
  unsigned short Camp_Potenza_In;              //20        //Potenza di ingresso
  unsigned short Camp_V_Aux_Monitor;           //21        //tensione ausiliaria di alimentazione +15V
  unsigned short Camp_V_In_Monitor;            //22        //Tensione di ingresso
  unsigned short Camp_V_Out_Prot;              //23        //Questa  quella letta su cui  agganciata la protezione veloce di tensione sotto interrupt ad
  unsigned short Camp_Selezione;               //24        //Questa  quella su cui si decidono i dip
  unsigned short Camp_Temperatura_1;           //25        //temperatura 1 in campioni
  unsigned short Out_Pwm_Ctr;                  //26        //Pwm effetttivamente in uscita
  unsigned short Set_Max_Tens_Camp;            //27        //Soglia assoluta di uscita per la protezione hw
  unsigned short Saturazione_S_PID_Potenza:1; //Saturazione Superiore pid di potenza      //28
  unsigned short Saturazione_I_PID_Potenza:1; //Saturazione Inferiore pid di potenza
  unsigned short Saturazione_S_PID_Corrente:1;  //Saturazione Superiore pid di potenza
  unsigned short Saturazione_I_PID_Corrente:1;  //Saturazione Inferiore pid di potenza
  unsigned short Saturazione_S_PID_Tensione:1;  //Saturazione Superiore pid di potenza
  unsigned short Saturazione_I_PID_Tensione:1;  //Saturazione Inferiore pid di potenza
  unsigned short unused_saturazioni:10;
    */
};
union Un_Uscita_Modbus
{
  struct St_Uscita_Modbus St;
  unsigned char Dati[DIM_ST_OUT_MODBUS_CH];
  unsigned short Dati_Sh[DIM_ST_OUT_MODBUS_SH];
};

//Dati ingresso modbus
//Per ora solo in versione Alta Tensione, ma prevedo struttura dati per entrambi
#define DIM_ST_IN_MODBUS_CH sizeof(struct St_In_Modbus)
#define DIM_ST_IN_MODBUS_SH DIM_ST_IN_MODBUS_CH/2
struct St_In_Modbus
{
  unsigned short Abilita_Potenza:1;   //Abilitazione della potenza
  unsigned short Reset_Allarmi:1;     //Avvia reset allarmi latch
  unsigned short unused:14;

  unsigned short Set_Tensione_V10;    //Poi non li usiamo ma tornano comodi
  unsigned short Set_Corrente_A10;    //Set di corrente
  unsigned short Set_Potenza_W;       //Set di potenza in W
    /*
  unsigned short Unused;        //Sempre meglio
  unsigned short Led_R:1;       //Led rosso
  unsigned short Led_V:1;       //Led verde
  unsigned short Led_G:1;       //Led Giallo
  unsigned short Out_Fault:1;     //Out Fault
  unsigned short Out_Teleruttore:1; //Rel di abilitazione teleruttore
  unsigned short Enable_Potenza:1;  //Abilitazione uscita di potenza
  unsigned short Enable_CB:1;     //Abilitazione CB
  unsigned short un_bit:9;
  unsigned short Pwm_Controllo; //Pwm per il controllo, se in simulazione altimenti sotto interrupt
  unsigned short Pwm_Ventola;   //Pwm per la ventola
     */
};
union Un_Ingresso_Modbus
{
  struct St_In_Modbus St;
  unsigned char Dati[DIM_ST_IN_MODBUS_CH];
  unsigned short Dati_Sh[DIM_ST_IN_MODBUS_SH];
};








#endif      //Strutture dati.h



/********************************************************************************/
/* Strutture dati per comunicazione seriale con la scheda                       */
/* in questo caso modbus                                                        */
/********************************************************************************/
/*#ifndef __STRUTTURE_H
#define __STRUTTURE_H   1
//Qui introduco anche la versione del sw pc
#define VERSIONE_H              1
#define VERSIONE_L              2
//Minimi e massimi set accettabili da modbus
#define MIN_SET_V_OUT_V10                       400         //40V
#define MAX_SET_V_OUT_V10                       560         //56V
#define MIN_SET_I_OUT_I10           0
#define MAX_SET_I_OUT_I10           610         //61A
#define MIN_SET_P_OUT_W                         0
#define MAX_SET_P_OUT_W                         3000        //W
//---------------------------------------------------------------------------------------------------
//Struttura dati di uscita sul bus 485 in comunicazione modbus
#define DIM_ST_OUT_MODBUS_CH  sizeof(struct St_Uscita_Modbus)
#define DIM_ST_OUT_MODBUS_SH  (sizeof(struct St_Uscita_Modbus)/2)
//Struttura definita per ACE
struct St_Uscita_Modbus
{
  unsigned short V_Uscita;          //V*10      //1
  unsigned short I_Uscita;          //I*10      //2
  unsigned short P_Uscita;          //W       //3
  unsigned short V_Ingresso;          //V*10      //4
  signed short T_Dissipatore_1;       //C*10      //5
  unsigned short In_Enable:1;         //Stato ingresso enable (Fisico)  //6
  unsigned short In_Manuale:1;        //Stato ingresso fisico di carica manuale
  unsigned short Dip_1:1;           //Stato dip 1
  unsigned short Dip_2:1;           //Stato dip 2
  unsigned short Dip_3:1;           //Stato dip 3
  unsigned short Dip_4:1;           //Stato dip 4
  unsigned short Out_Fault:1;         //Stato uscita fault
  unsigned short Out_Teleruttore:1;     //Rel di abilitazione teleruttore
  unsigned short Ventola_V1:1;        //Stato ventola Velocit 1
  unsigned short Ventola_V2:1;        //Stato ventola Velocit 2
  unsigned short Simulazione:1;       //Comunico sistema in simulazione
  unsigned short Unused_Bit_2:5;        //Libero
  unsigned short Uscita_Attiva:1;       //Stato di uscita attiva      //7
  unsigned short Avvio_Set_Interni:1;     //Si avvier in automatico con i set interni
  unsigned short Avvio_Manuale:1;       //Avvio in manuale
  unsigned short Ctr_Corrente:1;        //Controllo corrente
  unsigned short Ctr_Tensione:1;        //Controllo tensione
  unsigned short Ctr_Potenza:1;       //Controllo Potenza
  unsigned short Fg_Ctr_Liberi:10;
    unsigned short Allarme_In_Corso:1;          //Riassuntivo dell' allarme in corso //8
    unsigned short All_Termico:1;       //Allarme termico realtime
  unsigned short SottoTens_Ingresso:1;    //Sottotensione di ingresso realtime
  unsigned short All_Low_VAux:1;        //Bassa tensione di alimentazione realtime
  unsigned short All_Over_Tens_Out:1;     //Overtensione di uscita realtime
    unsigned short Err_Set_V_Modbus:1;          //Set di tensione fuori del range
  unsigned short Err_Set_I_Modbus:1;      //Si  immesso un set di Corrente fuori dai limiti
    unsigned short Err_Set_P_Modbus:1;          //Set di potenza fuori dai range
    unsigned short Lth_All_Low_Tens_Vin:1;      //Latch per l' allarme di bassa tensione di ingresso
    unsigned short Lth_All_Low_Tens_Ali:1;      //Lacth per l' allarme di bassa tensione di alimentazione +15V
    unsigned short Lth_All_Over_Tens_Out:1;     //Latch per l' allarme di sovratensione di uscita
  unsigned short Timeout_Set_Modbus:1;        //Timeout di aggiornamento set modbus
  unsigned short Timeout_Com_Modbus:1;        //Timeout in comunicazione modbus generica
    unsigned short Seg_Over_V_In:1;             //Segnalazione di massima tensione in ingresso
  unsigned short Liberi_all:2;
  unsigned short Fd_Set_Tensione_V10;         //Feed Set di Tensione          //9
  unsigned short Fd_Set_Corrente_A10;     //Feed Set di Corrente      //10
    unsigned short Fd_Set_Potenza_W;            //Feed Set di Potenza           //11
    unsigned short Alive_1s;                    //Contatore di Alive ad 1 secondo //12
    
  unsigned short Contaminuti;         //Conteggio minuti                  //13
  unsigned short Stato_Controllo;                 //14
  unsigned short Ver_Fw_Lw:8;         //Versione fw Basso     //15
  unsigned short Ver_Fw_Hg:8;         //versione fw Alto
  unsigned short Alimentazione_V_Aux_V10;     //16            Tensione di alimentazione V aux in volt*10
  //Dati per collaudo
  unsigned short Camp_V_Out;                   //17        //Quella su cui si aggancia il controllo
  unsigned short Camp_I_Out;                   //18        //Corrente di uscita
  unsigned short Camp_Potenza_Out;             //19        //Potenza di uscita
  unsigned short Camp_Potenza_In;              //20        //Potenza di ingresso
  unsigned short Camp_V_Aux_Monitor;           //21        //tensione ausiliaria di alimentazione +15V
    unsigned short Camp_V_In_Monitor;            //22        //Tensione di ingresso
    unsigned short Camp_V_Out_Prot;              //23        //Questa  quella letta su cui  agganciata la protezione veloce di tensione sotto interrupt ad
    unsigned short Camp_Selezione;               //24        //Questa  quella su cui si decidono i dip
    unsigned short Camp_Temperatura_1;           //25        //temperatura 1 in campioni
    unsigned short Out_Pwm_Ctr;                  //26        //Pwm effetttivamente in uscita
    unsigned short Set_Max_Tens_Camp;            //27        //Soglia assoluta di uscita per la protezione hw
    unsigned short Saturazione_S_PID_Potenza:1; //Saturazione Superiore pid di potenza      //28
  unsigned short Saturazione_I_PID_Potenza:1; //Saturazione Inferiore pid di potenza
  unsigned short Saturazione_S_PID_Corrente:1;  //Saturazione Superiore pid di potenza
  unsigned short Saturazione_I_PID_Corrente:1;  //Saturazione Inferiore pid di potenza
  unsigned short Saturazione_S_PID_Tensione:1;  //Saturazione Superiore pid di potenza
  unsigned short Saturazione_I_PID_Tensione:1;  //Saturazione Inferiore pid di potenza
  unsigned short unused_saturazioni:10;
     
};
*/
/*
union Un_Uscita_Modbus
{
  struct St_Uscita_Modbus St;
  unsigned char Dati[DIM_ST_OUT_MODBUS_CH];
  unsigned short Dati_Sh[DIM_ST_OUT_MODBUS_SH];
};
//Dati ingresso modbus
//Per ora solo in versione Alta Tensione, ma prevedo struttura dati per entrambi
#define DIM_ST_IN_MODBUS_CH sizeof(struct St_In_Modbus)
#define DIM_ST_IN_MODBUS_SH DIM_ST_IN_MODBUS_CH/2
struct St_In_Modbus
{
  unsigned short Abilita_Potenza:1;   //Abilitazione della potenza
    unsigned short Reset_Allarmi:1;     //Avvia reset allarmi latch
    unsigned short unused:14;
    unsigned short Set_Tensione_V10;    //Poi non li usiamo ma tornano comodi
  unsigned short Set_Corrente_A10;    //Set di corrente
  unsigned short Set_Potenza_W;       //Set di potenza in W
    /*
    unsigned short Unused;        //Sempre meglio
  unsigned short Led_R:1;       //Led rosso
  unsigned short Led_V:1;       //Led verde
  unsigned short Led_G:1;       //Led Giallo
  unsigned short Out_Fault:1;     //Out Fault
  unsigned short Out_Teleruttore:1; //Rel di abilitazione teleruttore
  unsigned short Enable_Potenza:1;  //Abilitazione uscita di potenza
  unsigned short Enable_CB:1;     //Abilitazione CB
  unsigned short un_bit:9;
  unsigned short Pwm_Controllo; //Pwm per il controllo, se in simulazione altimenti sotto interrupt
  unsigned short Pwm_Ventola;   //Pwm per la ventola
     
};
*/
/*
union Un_Ingresso_Modbus
{
  struct St_In_Modbus St;
  unsigned char Dati[DIM_ST_IN_MODBUS_CH];
  unsigned short Dati_Sh[DIM_ST_IN_MODBUS_SH];
};
#endif      //Strutture dati.hz
*/
