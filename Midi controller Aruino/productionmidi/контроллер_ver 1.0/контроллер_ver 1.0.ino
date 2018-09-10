/*********Multiplexer**************/

#include <MIDI.h>  // Include MIDI library
#include <midi_Defs.h>
#include <midi_Message.h>
#include <midi_Namespace.h>
#include <midi_Settings.h>
#include "Controller.h" // Include Controller library

/*************************************************************
  MIDI CONTROLLER  

**Arduino UNO ONLY!**
 *************************************************************/

MIDI_CREATE_DEFAULT_INSTANCE();

//-----------------------------Матричная клавиатура---------------------
const byte ROWS = 4; //Количество столбцов матричной клавиатуры
const byte COLS = 4; //Количество строк матричной клавиатуры
byte rowPins[ROWS] = {9, 8, 7, 6}; //Пины столбцов
byte colPins[COLS] = {13, 12, 11, 10}; //Пины строк

byte buttonState[COLS][ROWS] = { //Массив состояния кнопок
  {1, 1, 1, 1},
  {1, 1, 1, 1},
  {1, 1, 1, 1},
  {1, 1, 1, 1}
};

byte kpdNote[COLS][ROWS] = { //Массив нот
  {60, 64, 68, 71},
  {61, 65, 69, 72},
  {62, 66, 70, 73},
  {63, 67, 71, 74}
};

//----------------------------------------------------------------------

//byte tmppin = 0;

byte val = 0;
byte dval = 0;

byte PrVal[8] = {0, 0, 0, 0, 0, 0, 0, 0};

long currenttime=0;
boolean b;
byte eval;
long ltime = 0;

//************************************************************
//***SET THE NUMBER OF CONTROLS USED**************************
//************************************************************
//---How many buttons are connected directly to pins?---------
byte NUMBER_BUTTONS = 0;
//---How many potentiometers are connected directly to pins?--
byte NUMBER_POTS = 0;
//---How many buttons are connected to a multiplexer?---------
byte NUMBER_MUX_BUTTONS = 0;
//---How many potentiometers are connected to a multiplexer?--
byte NUMBER_MUX_POTS = 2;
//************************************************************

//***ANY MULTIPLEXERS? (74HC4067)************************************
//MUX address pins must be connected to Arduino UNO pins 2,3,4,5
//A0 = PIN2, A1 = PIN3, A2 = PIN4, A3 = PIN5
//*******************************************************************
//Mux NAME (OUTPUT PIN, , How Many Mux Pins?(8 or 16) , Is It Analog?);


 //Mux M1(10, 8, false); //Digital multiplexer on Arduino pin 10
Mux M2(A0, 16, true); //Analog multiplexer on Arduino analog pin A0
//*******************************************************************


//***DEFINE DIRECTLY CONNECTED POTENTIOMETERS************************
//Pot (Pin Number, Command, CC Control, Channel Number)
//**Command parameter is for future use**

//Pot PO1(A0, 0, 1, 1);
//Pot PO2(A1, 0, 10, 1);
//Pot PO3(A2, 0, 22, 1);
//Pot PO4(A3, 0, 118, 1);
//Pot PO5(A4, 0, 30, 1);
//Pot PO6(A5, 0, 31, 1);
//*******************************************************************
//Add pots used to array below like this->  Pot *POTS[] {&PO1, &PO2, &PO3, &PO4, &PO5, &PO6};
Pot *POTS[] {};
//*******************************************************************


//***DEFINE DIRECTLY CONNECTED BUTTONS*******************************
//Button (Pin Number, Command, Note Number, Channel, Debounce Time)
//** Command parameter 0=NOTE  1=CC  2=Toggle CC **

//Button BU1(4, 4, 60, 1, 5 );
//Button BU2(6, 6, 61, 1, 5 );
//Button BU3(4, 0, 62, 1, 5 );
//Button BU4(5, 0, 63, 1, 5 );
//Button BU5(6, 0, 64, 1, 5 );
//Button BU6(7, 0, 65, 1, 5 );
//Button BU7(8, 1, 64, 1, 5 );
//Button BU8(9, 2, 64, 1, 5 );
//*******************************************************************
//Add buttons used to array below like this->  Button *BUTTONS[] {&BU1, &BU2, &BU3, &BU4, &BU5, &BU6, &BU7, &BU8};
Button *BUTTONS[] {};
//*******************************************************************


//***DEFINE BUTTONS CONNECTED TO MULTIPLEXER*************************
//Button::Button(Mux mux, byte muxpin, byte command, byte value, byte channel, byte debounce)
//** Command parameter 0=NOTE  1=CC  2=Toggle CC **

//Button MBU1(M1, 0, 0, 36, 1, 5);
//Button MBU2(M1, 1, 0, 37, 1, 5);
//Button MBU3(M1, 2, 0, 38, 1, 5);
//Button MBU4(M1, 3, 0, 73, 1, 5);
//Button MBU5(M1, 4, 0, 74, 1, 5);
//Button MBU6(M1, 5, 0, 75, 1, 5);
//Button MBU7(M1, 6, 0, 76, 1, 5);
//Button MBU8(M1, 7, 0, 77, 1, 5);
//Button MBU9(M1, 8, 0, 78, 1, 5);
//Button MBU10(M1, 9, 0, 79, 1, 5);
//Button MBU11(M1, 10, 0, 80, 1, 5);
//Button MBU12(M1, 11, 0, 81, 1, 5);
//Button MBU13(M1, 12, 0, 82, 1, 5);
//Button MBU14(M1, 13, 0, 83, 1, 5);
//Button MBU15(M1, 14, 0, 84, 1, 5);
//Button MBU16(M1, 15, 0, 85, 1, 5);
//*******************************************************************
////Add multiplexed buttons used to array below like this->  Button *MUXBUTTONS[] {&MBU1, &MBU2, &MBU3, &MBU4, &MBU5, &MBU6.....};
Button *MUXBUTTONS[] {};

//*******************************************************************


//***DEFINE POTENTIOMETERS CONNECTED TO MULTIPLEXER*******************
//Pot::Pot(Mux mux, byte muxpin, byte command, byte control, byte channel)
//**Command parameter is for future use**

Pot MPO1(M2, 1, 0, 14, 1);
Pot MPO2(M2, 2, 0, 15, 1);
//Pot MPO3(M2, 2, 0, 50, 1);
//Pot MPO4(M2, 3, 0, 55, 2);
//Pot MPO5(M2, 4, 0, 50, 1);
//Pot MPO6(M2, 5, 0, 55, 2);
//Pot MPO7(M2, 6, 0, 50, 1);
//Pot MPO8(M2, 7, 0, 55, 2);
//Pot MPO9(M2, 8, 0, 50, 1);
//Pot MPO10(M2, 9, 0, 55, 2);
//Pot MPO11(M2, 10, 0, 50, 1);
//Pot MPO12(M2, 11, 0, 55, 2);
//Pot MPO13(M2, 12, 0, 50, 1);
//Pot MPO14(M2, 13, 0, 55, 2);
//Pot MPO15(M2, 14, 0, 50, 1);
//Pot MPO16(M2, 15, 0, 55, 2);
//*******************************************************************
//Add multiplexed pots used to array below like this->  Pot *MUXPOTS[] {&MPO1, &MPO2, &MPO3, &MPO4, &MPO5, &MPO6.....};
Pot *MUXPOTS[] {&MPO1, &MPO2};
//*******************************************************************


void setup() {
  Serial.begin(31250);  
  MIDI.begin();



  // Конфигурируем строки мтрчн клвтр как выходы и подаём на них лог. 1
  for(byte i = 0; i < COLS; i++) {
    pinMode(colPins[i], OUTPUT);
    digitalWrite(colPins[i], HIGH);
  }                                        

  // Конфигурируем столбцы мтрчн клвтр как входы и включаем встроенные в мк подтягивающие резисторы
  for(byte i = 0; i < ROWS; i++) {
    pinMode(rowPins[i], INPUT);
    digitalWrite(rowPins[i], HIGH);
  }
}

void loop() {
  if (NUMBER_BUTTONS != 0) {
    updateButtons();
  }
    
  if (NUMBER_POTS != 0) {
    updatePots();
  }
  
  if (NUMBER_MUX_BUTTONS != 0) {
    updateMuxButtons();
  }
  
  if (NUMBER_MUX_POTS != 0) {
    updateMuxPots();
  }

  // Цикл чтения значений потенциометров Задаём параметры мультиплексора Считываем значение с канала X
  for(byte chn = 0; chn < 4; chn++) {      
    set_mp_chn(chn);
    val=analogRead(0) / 8;

    // Если текущее значение отл. от прошлого больше чем на 5, то посылаем новое значение
    if (abs(val-PrVal[chn]) > 5) {
      MIDI.sendControlChange(chn,val,1);
      PrVal[chn]=val;
    }

    // Считываем значение с канала Y аналогично X
    val=analogRead(1) / 8;
    
    if (abs(val-PrVal[chn+4]) > 5) { 
      MIDI.sendControlChange(chn+4,val,1); 
      PrVal[chn+4]=val;
    }
  }

  // Цикл чтения матричной клавиатуры
  for(byte i = 0; i < COLS; i++) {

    // На считываем столбец выставляем 0
    digitalWrite(colPins[i], LOW);

    // Построчно считываем каждый столбец и при нажатой кнопке передаём ноту
    for(byte j = 0; j < ROWS; j++) {
      dval=digitalRead(rowPins[j]);
      
      if ( dval == LOW && buttonState[i][j] == HIGH ) {
        MIDI.sendNoteOn(kpdNote[j][i],127,1); 
      }
      
      if ( dval == HIGH && buttonState[i][j] == LOW ) {
        MIDI.sendNoteOff(kpdNote[j][i],127,1);
      }
      
      buttonState[i][j] = dval;
    }
    
    digitalWrite(colPins[i], HIGH);  
  }

  
}

//*****************************************************************
void updateButtons() {

  // Cycle through Button array
  for (int i = 0; i < NUMBER_BUTTONS; i = i + 1) {
    byte message = BUTTONS[i]->getValue();

    //  Button is pressed
    if (message == 0) {
      switch (BUTTONS[i]->Bcommand) {
        case 0: //Note
          MIDI.sendNoteOn(BUTTONS[i]->Bvalue, 127, BUTTONS[i]->Bchannel);
          break;
        case 1: //CC
          MIDI.sendControlChange(BUTTONS[i]->Bvalue, 127, BUTTONS[i]->Bchannel);
          break;
        case 2: //Toggle
          if (BUTTONS[i]->Btoggle == 0) {
            MIDI.sendControlChange(BUTTONS[i]->Bvalue, 127, BUTTONS[i]->Bchannel);
            BUTTONS[i]->Btoggle = 1;
          }
          else if (BUTTONS[i]->Btoggle == 1) {
            MIDI.sendControlChange(BUTTONS[i]->Bvalue, 0, BUTTONS[i]->Bchannel);
            BUTTONS[i]->Btoggle = 0;
          }
          break;
      }
    }

    //  Button is not pressed
    if (message == 1) {
      switch (BUTTONS[i]->Bcommand) {
        case 0:
          MIDI.sendNoteOff(BUTTONS[i]->Bvalue, 0, BUTTONS[i]->Bchannel);
          break;
        case 1:
          MIDI.sendControlChange(BUTTONS[i]->Bvalue, 0, BUTTONS[i]->Bchannel);
          break;
      }
    }
  }
}
//*******************************************************************
void updateMuxButtons() {

  // Cycle through Mux Button array
  for (int i = 0; i < NUMBER_MUX_BUTTONS; i = i + 1) {

    MUXBUTTONS[i]->muxUpdate();
    byte message = MUXBUTTONS[i]->getValue();

    //  Button is  pressed
    if (message == 1) {
      switch (MUXBUTTONS[i]->Bcommand) {
        case 0: //Note
          MIDI.sendNoteOn(MUXBUTTONS[i]->Bvalue, 127, MUXBUTTONS[i]->Bchannel);
          break;
        case 1: //CC
          MIDI.sendControlChange(MUXBUTTONS[i]->Bvalue, 127, MUXBUTTONS[i]->Bchannel);
          break;
        case 2: //Toggle
          if (MUXBUTTONS[i]->Btoggle == 0) {
            MIDI.sendControlChange(MUXBUTTONS[i]->Bvalue, 127, MUXBUTTONS[i]->Bchannel);
            MUXBUTTONS[i]->Btoggle = 1;
          }
          else if (MUXBUTTONS[i]->Btoggle == 1) {
            MIDI.sendControlChange(MUXBUTTONS[i]->Bvalue, 0, MUXBUTTONS[i]->Bchannel);
            MUXBUTTONS[i]->Btoggle = 0;
          }
          break;
      }
    }
    //  Button is not pressed
    if (message == 0) {
      switch (MUXBUTTONS[i]->Bcommand) {
        case 0:
          MIDI.sendNoteOff(MUXBUTTONS[i]->Bvalue, 0, MUXBUTTONS[i]->Bchannel);
          break;
        case 1:
          MIDI.sendControlChange(MUXBUTTONS[i]->Bvalue, 0, MUXBUTTONS[i]->Bchannel);
          break;
      }
    }
  }
}
//***********************************************************************
void updatePots() {
  for (int i = 0; i < NUMBER_POTS; i = i + 1) {
    byte potmessage = POTS[i]->getValue();
    if (potmessage != 255) MIDI.sendControlChange(POTS[i]->Pcontrol, potmessage, POTS[i]->Pchannel);
  }
}
//***********************************************************************
void updateMuxPots() {
  for (int i = 0; i < NUMBER_MUX_POTS; i = i + 1) {
    MUXPOTS[i]->muxUpdate();
    byte potmessage = MUXPOTS[i]->getValue();
    if (potmessage != 255) MIDI.sendControlChange(MUXPOTS[i]->Pcontrol, potmessage, MUXPOTS[i]->Pchannel);
  }
}

void set_mp_chn(byte chn1) // Управление мультиплексором
{
  switch (chn1)
  {
    case 0: digitalWrite(18, LOW); digitalWrite(19, LOW); break; 
    case 1: digitalWrite(18, LOW); digitalWrite(19, HIGH); break;
    case 2: digitalWrite(18, HIGH); digitalWrite(19, LOW); break;
    case 3: digitalWrite(18, HIGH); digitalWrite(19, HIGH); break;
  }  
}

void enc() // Обработка энкодера
{
  currenttime=millis();
  if (abs(ltime-currenttime)>50) //  антидребезг
   {
    b=digitalRead(4);
    if (b == HIGH && eval<=122) eval=eval+5;
      else if (b == LOW && eval>=5) eval=eval-5;
    MIDI.sendControlChange(9,eval,1); 
    ltime = millis();
   }
   
}
