
const int OFILLTANK = 3;
const int OSTEAM = 4;
const int OFILLBOTTLE = 5;
const int OCONVEYOR = 6;
const int ISETBOTTLE = 7;
 
/*===== INPUTS =====*/
const int ILOWLEVEL = 8;
const int IHIGHLEVEL = 9;
const int ILOWTEMP = 10;
const int IHIGHTEMP = 11;
const int IBOTTLELEVEL = 12;
const int IBOTTLEPOSITION = 13;

const bool ON = true;
const bool OFF = false;
 
/*===== OUTPUTS =====*/
bool iLowLevel;
bool iHighLevel;
bool iLowTemp;
bool iHighTemp;
bool iBottleLevel;
bool iBottlePosition;
 
/*===== INPUTS =====*/
bool oFillTank;
bool oSteam;
bool oFillBottle;
bool oConveyor;

bool iSetBottle;
bool Prev = iSetBottle;

float TankLevel; 
float TankTemp;
float BottleCoord;
float BottleLevel;
long curTime;

const float INFLOW_RATE = 2.0;
const float OUTFLOW_RATE = 1.0;
const float TANK_LOW_LEVEL = 10.0;
const float TANK_HIGH_LEVEL = 100.0;
const float TANK_MAX_LEVEL = 120.0;

const float HEATING_RATE = 0.1;
const float COOLING_RATE = 0.05;
const float ROOM_TEMP = 20.0;
const float STEAM_TEMP = 150.0;
const float LOW_TEMP = 95.0;
const float HIGH_TEMP = 110.0;

const float FILLING_RATE = 1.0;
const float BOTTLE_FULL_LEVEL = 20.0;
const float BOTTLE_MAX_LEVEL = 25.0;
const float MIN_UNDER_NOZZLE = 145.0;
const float MAX_UNDER_NOZZLE = 150.0;

const float CONVEYOR_RATE = 1.0;
const float CONVEYOR_LENGTH = 160.0;


uint32_t curtime;
uint32_t tasktime;
uint32_t starttime;
uint32_t deltaT;
uint32_t deltaT_1;

const uint32_t interval = 100;


void setup()
{
  
pinMode(OFILLTANK, INPUT);
pinMode(OSTEAM, INPUT);
pinMode(OFILLBOTTLE, INPUT);
pinMode(OCONVEYOR, INPUT);
pinMode(ISETBOTTLE, INPUT);
 
/*===== INPUTS =====*/
pinMode(ILOWLEVEL, OUTPUT);
pinMode(IHIGHLEVEL, OUTPUT);
pinMode(ILOWTEMP, OUTPUT);
pinMode(IHIGHTEMP, OUTPUT);
pinMode(IBOTTLELEVEL, OUTPUT);
pinMode(IBOTTLEPOSITION, OUTPUT);
  
bool Prev = iSetBottle;
TankTemp = ROOM_TEMP;
TankLevel = TANK_HIGH_LEVEL;


  Serial.begin( 9600 );
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  Serial.print( "Hello!" );
  curtime = millis();
  tasktime = curtime;
}

void loop() {
//  Serial.print( "S" );
//  delay(1000);
  
  curtime =  millis();
  if ((long)(curtime - tasktime) >= 0) { // run control cycle
    tasktime += interval; // calc
    starttime = curtime;
  
    oFillTank = digitalRead(OFILLTANK);
    oSteam = digitalRead(OSTEAM);
    oFillBottle = digitalRead(OFILLBOTTLE);
    oConveyor = digitalRead(OCONVEYOR);
    iSetBottle = digitalRead(ISETBOTTLE);
  
// ============= TankSim =====================
    if (oFillTank == ON) TankLevel += INFLOW_RATE;
    
    if (oFillBottle == ON) TankLevel -= OUTFLOW_RATE;
    
    if (TankLevel >= TANK_HIGH_LEVEL) { 
      iHighLevel = ON;
      if (TankLevel >= TANK_MAX_LEVEL) 
          TankLevel = TANK_MAX_LEVEL;  
    } else { 
      iHighLevel = OFF;
    }
    
    if (TankLevel >= TANK_LOW_LEVEL) { 
      iLowLevel = ON;
    } else { 
      iLowLevel = OFF;
      if (TankLevel < 0.0) TankLevel = 0.0;  
    } 
    
        // debug
//    if ((oFillTank == ON) || (oFillBottle == ON)) {
  //    Serial.print("TankLevel = ");
  //    Serial.println(TankLevel);
//    }
  
//=============== TempSim ==================    
    if (oFillTank == ON) 
      TankTemp = (TankLevel * TankTemp + 
                  INFLOW_RATE * ROOM_TEMP) /
                  (TankLevel + INFLOW_RATE);
 
    if (oSteam == ON) 
      TankTemp = TankTemp + 
                (STEAM_TEMP - TankTemp) * 
                HEATING_RATE;
    else 
      TankTemp = TankTemp - 
                 (TankTemp - ROOM_TEMP) * 
                 COOLING_RATE;     
    
    if (TankTemp >= HIGH_TEMP) { 
      iHighTemp = ON;
    } else { 
      iHighTemp = OFF; 
    } 
    
    if (TankTemp >= LOW_TEMP) { 
      iLowTemp = ON;
    } else { 
      iLowTemp = OFF;
    } 
    // debug
//    Serial.print("TankTemp = ");
//    Serial.println(TankTemp);


  
  
//================== process BottleFillingSim {  
  
    iBottleLevel = OFF;    
    if ((BottleCoord >= MIN_UNDER_NOZZLE) && 
        (BottleCoord < MAX_UNDER_NOZZLE)) {
      if (oFillBottle == ON) {  
        BottleLevel += FILLING_RATE;  
     // debug
//        Serial.print("BottleLevel = ");
//        Serial.println(BottleLevel);
      }
      
      if (BottleLevel > BOTTLE_MAX_LEVEL) 
        BottleLevel = BOTTLE_MAX_LEVEL;  
        
      if (BottleLevel >= BOTTLE_FULL_LEVEL)
        iBottleLevel = ON;            
    }    
    
//========================= Conveyor  
    if (oConveyor == ON) { 
      iBottlePosition = OFF; 
      if (BottleCoord > 0.0) { 
        BottleCoord += CONVEYOR_RATE;
        if ((BottleCoord >= MIN_UNDER_NOZZLE) 
             && (BottleCoord < MAX_UNDER_NOZZLE)) {
          iBottlePosition = ON;
        } 
        if (BottleCoord >= CONVEYOR_LENGTH) {
          BottleCoord = 0.0;      
          BottleLevel = 0.0;
        }
        // if (BottleCoord >0) 
//        Serial.print("Coord = ");
//        Serial.println(BottleCoord);
      }        
    }    

//================= Set bottle
    if ((iSetBottle == ON) && (Prev == OFF)) {
      if (oConveyor) {
        if (BottleCoord < 0.01) {
          BottleCoord = MIN_UNDER_NOZZLE;  
          BottleLevel = 0.0;  
        }
      }
    }             
    Prev = iSetBottle;

  //    Serial.print(BottleCoord);
  //    Serial.print(',');
  //    Serial.print(BottleLevel);
  //    Serial.print(',');
  //    Serial.print(TankLevel);
  //    Serial.print(',');
  //    Serial.println(TankTemp);  
    // output
    digitalWrite(ILOWLEVEL, iLowLevel);
    digitalWrite(IHIGHLEVEL, iHighLevel);
    digitalWrite(ILOWTEMP, iLowTemp);
    digitalWrite(IHIGHTEMP, iHighTemp);
    digitalWrite(IBOTTLELEVEL, iBottleLevel);
    digitalWrite(IBOTTLEPOSITION, iBottlePosition);
  
    /* check calctime: */

    curtime =  millis();
    deltaT = curtime - starttime;
    if ((long)(curtime - starttime) > interval) { // abnormal calctime
      Serial.print("RUN-TIME ERROR: Computation time exceeds specified interval (");
      Serial.print(curtime - starttime);
      Serial.print(" ms > ");
      Serial.print(interval);
      Serial.println(" ms)");
  // errmsg("Computation time exceeds specified interval!");
      tasktime = curtime;
    } else {
      if (deltaT_1 != deltaT) {
        Serial.print("-");
        Serial.println(deltaT);     
        deltaT_1 = deltaT;
      }
    }    

  } /* end of control cycle*/
} /* end of loop */




