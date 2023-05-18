/*================================================*/
/*===== Process-Oriented Topology-Free ===========*/
/*===== Distributed Control System ===============*/
/*===== for Bottle-Filling System ================*/
/*================================================*/
/*===== Created by V. Zyubin 01/2023 =============*/
/*===== Cyber-Physical lab =======================*/
/*===== Institute of Automation ==================*/
/*===== and Electrometry SB RAS ==================*/
/*===== www.iae.nsk.su ===========================*/
/*================================================*/
#include <SPI.h>
#include "mcp_can.h"

#define Controller1 1
//#define Controller2 1
//#define Controller3 1
//#define Controller4 1

#define DEBUG_FLAG true


// messages
enum Msg {
  startForcedSterilization,
  startKeepSterilization,
  stopKeepSterilization,
  startBottleFilling,
  startNextBottle,
  activeForcedSterilization,
  passiveForcedSterilization,
  activeKeepSterilization,
  passiveKeepSterilization,
  activeBottleFilling,
  passiveBottleFilling,
  activeNextBottle,
  passiveNextBottle
};

//controllers CAN ID
enum Controller {all, controller1, controller2, controller3, controller4};

const int SPI_CS_PIN = 10;
MCP_CAN CAN(SPI_CS_PIN);

// [slice: "PROCESSOR 1"]
const uint8_t START = 0;
const uint8_t CONFIRMED = 253;
const uint8_t ERROR = 254;
const uint8_t STOP = 255;

uint8_t _p_Initialization_state;
uint8_t _p_MainLoop_state;
uint8_t _p_TankFilling_state;
uint8_t _p_ForcedSterilization_state;
uint8_t _p_KeepSterilization_state;
uint8_t _p_BottleFilling_state;
uint8_t _p_NextBottle_state;

uint8_t _p_Initialization_state_1;
uint8_t _p_MainLoop_state_1;
uint8_t _p_TankFilling_state_1;
uint8_t _p_ForcedSterilization_state_1;
uint8_t _p_KeepSterilization_state_1;
uint8_t _p_BottleFilling_state_1;
uint8_t _p_NextBottle_state_1;

//_p_Initialization_time
//_p_MainLoop_time
//_p_TankFilling_time
uint32_t _p_ForcedSterilization_time;
//_p_KeepSterilization_time
//_p_BottleFilling_time
//_p_NextBottle_time

void setup_CAN() {
  CAN.init_CS(10);

#ifdef Controller1
  Serial.println("C1 ON!");
#endif
#ifdef Controller2
  Serial.println("C2 ON!");
#endif
#ifdef Controller3
  Serial.println("C3 ON!");
#endif
#ifdef Controller4
  Serial.println("C4 ON!");
#endif  
  Serial.println("Init");
  START_INIT:
    if (CAN_OK == CAN.begin(CAN_500KBPS, MCP_16MHz))  // init can bus : baudrate = 500k
    {
#if DEBUG_FLAG
  //      Serial.println("CAN BUS ok");
      ;
#endif
    } else {
#if DEBUG_FLAG
      Serial.println("[CAN] Init Error. New attempt");
//      Serial.println("Init CAN BUS Shield again");
#endif      
      delay(1000);
      goto START_INIT;
    }
  Serial.println("[CAN] OK!");
}


void message(int to, int cmd) {
  byte buf[1];
  buf[0] = cmd;
  CAN.sendMsgBuf((unsigned long)to, 0, sizeof(buf), buf, true);
}

//void sim_message(int type, bool val) { // 
//  byte buf[2];
//  buf[0] = type;
//  buf[1] = (byte)val;
//  int to = sim;
//  CAN.sendMsgBuf((unsigned long)to, 0, sizeof(buf), buf, true);
//}


int receive_messages() {
  byte buf[8];
  byte len;
  int ret;

  ret = false;  
  
  while (CAN_MSGAVAIL == CAN.checkReceive()) { // check if data is coming
    ret = true; 
#if DEBUG_FLAG
//      Serial.print("CAN! ");
#endif
    CAN.readMsgBuf(&len, buf);            

    unsigned short canId = CAN.getCanId();

#if DEBUG_FLAG
    Serial.print("[can]ID = ");
    Serial.println(canId);          
#endif    
    //Plant simulation messages
    //if (canId == sim && len > 1) {
    //  if (buf[0] == SimVars::vLowLevel) {
    //    iLowLevel = (buf[1] > 0);
    //    Serial.print("[CAN sim msg] LowLevel is ");
    //    Serial.println(iLowLevel);          
    //  }
    //  if (buf[0] == SimVars::vHighLevel) {
    //    iHighLevel = (buf[1] > 0);
    //    Serial.print("[CAN sim msg] iHighLevel is ");
    //    Serial.println(iHighLevel);          
    //  }
    //}

    //Handling messages to start/stop something..
#ifdef Controller1
    if (canId == Controller::all) {
      if (buf[0] == Msg::activeForcedSterilization) {
#if DEBUG_FLAG
        Serial.println("[can] ForcedSterilization is active");          
#endif        
        _p_ForcedSterilization_state = CONFIRMED;
      }
      if (buf[0] == Msg::passiveForcedSterilization) {
#if DEBUG_FLAG
        Serial.println("[can] ForcedSterilization is passive");          
#endif        
        _p_ForcedSterilization_state = STOP;
      }
      if (buf[0] == Msg::activeKeepSterilization) {
#if DEBUG_FLAG
        Serial.println("[can] KeepSterilization is active");          
#endif        
        _p_KeepSterilization_state = CONFIRMED;
      }
      if (buf[0] == Msg::passiveKeepSterilization) {
#if DEBUG_FLAG
        Serial.println("[can] KeepSterilization is passive");          
#endif        
        _p_KeepSterilization_state = STOP;
      }
      
      if (buf[0] == Msg::activeBottleFilling) {
#if DEBUG_FLAG
        Serial.println("[can] BottleFilling is active");          
#endif        
        _p_BottleFilling_state = CONFIRMED;
      }
      if (buf[0] == Msg::passiveBottleFilling) {
#if DEBUG_FLAG
        Serial.println("[can] BottleFilling is passive");          
#endif        
        _p_BottleFilling_state = STOP;
      }
      
      if (buf[0] == Msg::activeNextBottle) {
#if DEBUG_FLAG
        Serial.println("[can] NextBottle is active");          
#endif        
        _p_NextBottle_state = CONFIRMED;
      }
      if (buf[0] == Msg::passiveNextBottle) {
#if DEBUG_FLAG
        Serial.println("[can] NextBottle is passive");          
#endif        
        _p_NextBottle_state = STOP;
      }
    }
#endif

#ifdef Controller2
    if (canId == Controller::all) {
      if (buf[0] == Msg::startForcedSterilization) {
#if DEBUG_FLAG
        Serial.println("[can] START ForcedSterilization");          
#endif        
        _p_ForcedSterilization_state = START;
//        message(Controller::all, Msg::activeForcedSterilization); // !!! CAN send 
      }
      if (buf[0] == Msg::startKeepSterilization) {
#if DEBUG_FLAG
        Serial.println("[can] START KeepSterilization");          
#endif        
        _p_KeepSterilization_state = START;
//        message(Controller::all, Msg::activeKeepSterilization); // !!! CAN send 
        
      }
      if (buf[0] == Msg::stopKeepSterilization) {
#if DEBUG_FLAG
        Serial.println("[can] STOP KeepSterilization");          
#endif        
        _p_KeepSterilization_state = STOP;
//        message(Controller::all, Msg::passiveKeepSterilization); // !!! CAN send 
      }
    }
#endif  


#ifdef Controller3
    if (canId == Controller::all) {
      if (buf[0] == Msg::startBottleFilling) {
#if DEBUG_FLAG
        Serial.println("[can] START BottleFilling");          
#endif        
        _p_BottleFilling_state = START;
//        message(Controller::all, Msg::activeBottleFilling); // !!! CAN send 
      }
    }
#endif  


#ifdef Controller4
    if (canId == Controller::all) {
      if (buf[0] == Msg::startNextBottle) {
#if DEBUG_FLAG
        Serial.println("[can] START NextBottle");          
#endif        
        _p_NextBottle_state = START;
//        message(Controller::all, Msg::activeNextBottle); // !!! CAN send 
      }
    }
#endif  
  }  // end of while
  return (ret);
}

const bool ON = true;
const bool OFF = false;

/*===== OUTPUTS =====*/
#ifdef Controller1
const int OFILLTANK = 8;
#endif
#ifdef Controller2
const int OSTEAM = 8;
#endif
#ifdef Controller3
const int OFILLBOTTLE = 8;
#endif
#ifdef Controller4
const int OCONVEYOR = 8;
#endif
 
/*===== INPUTS =====*/
#ifdef Controller1
const int ILOWLEVEL = 6;
const int IHIGHLEVEL = 7;
#endif
#ifdef Controller2
const int ILOWTEMP = 6;
const int IHIGHTEMP = 7;
#endif
#ifdef Controller3
const int IBOTTLELEVEL = 7;
#endif
#ifdef Controller4
const int IBOTTLEPOSITION = 7;
#endif
 
/*===== OUTPUTS =====*/
#ifdef Controller1
bool iLowLevel;
bool iHighLevel;
#endif
#ifdef Controller2
bool iLowTemp;
bool iHighTemp;
#endif
#ifdef Controller3
bool iBottleLevel;
#endif
#ifdef Controller4
bool iBottlePosition;
#endif
 
/*===== INPUTS =====*/
#ifdef Controller1
bool oFillTank;
#endif
#ifdef Controller2
bool oSteam;
#endif
#ifdef Controller3
bool oFillBottle;
#endif
#ifdef Controller4
bool oConveyor;
#endif


uint32_t curtime;
uint32_t tasktime;
uint32_t starttime;
uint32_t deltaT;
uint32_t deltaT_1;
//long curtime;
//long tasktime;
const uint32_t interval = 100;

void setup()
{
  
#ifdef Controller1
  pinMode(ILOWLEVEL, INPUT);
  pinMode(IHIGHLEVEL, INPUT);
  pinMode(OFILLTANK, OUTPUT);
#endif
#ifdef Controller2
  pinMode(ILOWTEMP, INPUT);
  pinMode(IHIGHTEMP, INPUT);
  pinMode(OSTEAM, OUTPUT);
#endif
#ifdef Controller3
  pinMode(IBOTTLELEVEL, INPUT);
  pinMode(OFILLBOTTLE, OUTPUT);
#endif
#ifdef Controller4
  pinMode(IBOTTLEPOSITION, INPUT);
  pinMode(OCONVEYOR, OUTPUT);
#endif
   
  Serial.begin( 9600 );
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  Serial.print( "Hello!" );
  setup_CAN();


  _p_Initialization_state = START;
  _p_MainLoop_state = STOP;
  _p_TankFilling_state = STOP;
  _p_ForcedSterilization_state = STOP;
  _p_KeepSterilization_state = STOP;
  _p_BottleFilling_state = STOP;
  _p_NextBottle_state = STOP;

  curtime = millis();
  tasktime = curtime;

  _p_ForcedSterilization_time = curtime;
}

void loop()
{
//  Serial.print( "S" );
  
curtime =  millis();

if ((long)(curtime - tasktime) >= 0) { // run control cycle
  tasktime += interval; // calc
  starttime = curtime;
#ifdef Controller1
  iLowLevel = digitalRead(ILOWLEVEL);
  iHighLevel = digitalRead(IHIGHLEVEL);
#endif
#ifdef Controller2
  iLowTemp = digitalRead(ILOWTEMP);
  iHighTemp = digitalRead(IHIGHTEMP);
#endif
#ifdef Controller3
  iBottleLevel = digitalRead(IBOTTLELEVEL);
#endif
#ifdef Controller4
  iBottlePosition = digitalRead(IBOTTLEPOSITION);
#endif

 enum _p_Initialization_states {
	_p_Initialization_state_Begin = START,
	_p_Initialization_state_WaitForFilling,
	_p_Initialization_state_WaitForSterilization
 };	
 enum _p_MainLoop_states {
	_p_MainLoop_state_Begin = START,
	_p_MainLoop_state_WaitForNextBottle,
	_p_MainLoop_state_WaitForFilling
 };	
 enum _p_TankFilling_states {
	_p_TankFilling_state_Begin = START,
	_p_TankFilling_state_TankFilling
 };	
 enum _p_ForcedSterilization_states {
	_p_ForcedSterilization_state_HeatUp = START,
	_p_ForcedSterilization_state_SterilizationFor1min
 };	
 enum _p_KeepSterilization_states {
	_p_KeepSterilization_state_WaitLowTemp = START,
	_p_KeepSterilization_state_WaitHighTemp
 };
 enum _p_BottleFilling_states {
	_p_BottleFilling_state_Begin = START
 };
 enum _p_NextBottle_states {
	_p_NextBottle_state_Begin = START,
	_p_NextBottle_state_WaitBottlePosition
 };


 receive_messages(); // CAN read and process
 
#ifdef Controller1
switch (_p_Initialization_state) { // process Initialization {
  case _p_Initialization_state_Begin:	// state Begin { 
    _p_TankFilling_state = START;		// start process TankFilling;
    _p_Initialization_state = _p_Initialization_state_WaitForFilling;		// set next state;     
   	break; 				// }
  case _p_Initialization_state_WaitForFilling:		// state WaitForFilling { 
    if (_p_TankFilling_state >= ERROR) {		// if (process TankFilling in state inactive) {
      _p_ForcedSterilization_state = START;	// start process ForcedSterilization;
      message(Controller::all, Msg::startForcedSterilization); // !!! CAN send 
      _p_Initialization_state = _p_Initialization_state_WaitForSterilization; // set next state;     
	  }						//    } 
   	break; 				// }
  case _p_Initialization_state_WaitForSterilization:	//  state WaitForSterilization { 
    if (_p_ForcedSterilization_state >= ERROR) {	// if (process ForcedSterilization in state inactive) {
      _p_KeepSterilization_state = START;	//      start process KeepSterilization;
      message(Controller::all, Msg::startKeepSterilization); // !!! CAN send 
      _p_MainLoop_state = START;		//      start process MainLoop;
      _p_Initialization_state = STOP;		//      stop; 
    }						//    } 
   	break; 				// }
 } /* end of switch */
 
 switch (_p_MainLoop_state) { ///* process MainLoop {  
  case _p_MainLoop_state_Begin:		//  state Begin { 
    _p_NextBottle_state = START;	//    start process NextBottle;
    message(Controller::all, Msg::startNextBottle); // !!! CAN send 
    _p_MainLoop_state = _p_MainLoop_state_WaitForNextBottle; //    set next state; 
   	break; 				// }
  case _p_MainLoop_state_WaitForNextBottle:	//  state WaitForNextBottle {
    if (_p_NextBottle_state >= ERROR) {	//    if (process NextBottle in state inactive) 
      _p_BottleFilling_state = START;	//      start process BottleFilling;
      message(Controller::all, Msg::startBottleFilling); // !!! CAN send 
      _p_MainLoop_state = _p_MainLoop_state_WaitForFilling; //      set next state; 
    }					//    }
    break; 					//  }
  case _p_MainLoop_state_WaitForFilling:	//  state WaitForFilling {
    if (_p_BottleFilling_state >= ERROR) {	//    if (process BottleFilling in state inactive) { 
      if (iLowLevel == OFF) { // no liquid
        _p_KeepSterilization_state = STOP;//        stop process KeepSterilization;
        message(Controller::all, Msg::stopKeepSterilization); // !!! CAN send 
        _p_Initialization_state = START; //        start process Initialization;
        _p_MainLoop_state = STOP;  	//        stop;
      } else {
        _p_MainLoop_state = START;  	//        restart;
      }
   	}  
   	break; 				// }
 }  /* end of switch */ 
 
 switch (_p_TankFilling_state) { // process TankFilling {  
  case _p_TankFilling_state_Begin: //  state Begin { 
   	if (iHighLevel != ON) { // is not filled
   		oFillTank = ON;
  	}  
	  _p_TankFilling_state = _p_TankFilling_state_TankFilling; //    set next state;     
   	break; 				// }
  case _p_TankFilling_state_TankFilling: 	//  state TankFilling { 
   	if (iHighLevel == ON) {
   		oFillTank = OFF;
	  	_p_TankFilling_state = STOP; //      stop; 
    } 
   	break; 				// }
 }  /* end of switch */
#endif

/* === Controller 2 ================ */
// [slice: "PROCESSOR2"]
#ifdef Controller2
 switch (_p_ForcedSterilization_state) { // process ForcedSterilization { 
  case _p_ForcedSterilization_state_HeatUp: //  state HeatUp { 
   	oSteam = ON;
   	if (iHighTemp == ON) {
  		_p_ForcedSterilization_state = _p_ForcedSterilization_state_SterilizationFor1min; //      set next state;
	  	_p_ForcedSterilization_time = curtime;
  	} 
   	break; 				// }
  case _p_ForcedSterilization_state_SterilizationFor1min: //  state SterilizationFor1min { 
	  if ((curtime - _p_ForcedSterilization_time) > 60000L) {	//    timeout (0t1m) {
	   	oSteam = OFF;
		  _p_ForcedSterilization_state = STOP; 	//      stop; 
      message(Controller::all, Msg::passiveForcedSterilization); // !!! CAN send 
  	}						//    } 
   	break; 				// }
 } /* end of process */
 
 
 switch (_p_KeepSterilization_state) { // /* process KeepSterilization { 
  case _p_KeepSterilization_state_WaitLowTemp: //  state WaitLowTemp { 
    if (iLowTemp != ON) {
      oSteam = ON;
      _p_KeepSterilization_state = _p_KeepSterilization_state_WaitHighTemp; //      set next state;
    }  
   	break; 				// }
  case _p_KeepSterilization_state_WaitHighTemp: //  state WaitHighTemp looped { 
    if (iHighTemp == ON) {
    	oSteam = OFF;
		  _p_KeepSterilization_state = START; //      restart; 
    } 
   	break; 				// }
 } /* end of process */
#endif

#ifdef Controller3
/* === Controller 3 ====== */
// [slice: "PROCESSOR3"]
 switch (_p_BottleFilling_state) { // /* process BottleFilling { 
  case _p_BottleFilling_state_Begin:	//  state Begin { 
    oFillBottle = ON;
	  if (iBottleLevel == ON) {
    	oFillBottle = OFF;
		  _p_BottleFilling_state = STOP; //      stop; 
      message(Controller::all, Msg::passiveBottleFilling); // !!! CAN send 
   	} 
   	break; 				// }
 } /* end of process */
#endif

#ifdef Controller4
/* === Controller 4 ================ */
//[slice: "P4"]
//[ID: "d d d d"]
 switch (_p_NextBottle_state) {	// /* process NextBottle { 
  case _p_NextBottle_state_Begin: //  state Begin { 
   	oConveyor = ON;
   	if (iBottlePosition != ON) // push current bottle
  		_p_NextBottle_state = _p_NextBottle_state_WaitBottlePosition; //      set next state;
   	break; 				// }
  case _p_NextBottle_state_WaitBottlePosition:	// state WaitBottlePosition { 
   	if (iBottlePosition == ON) {
   		oConveyor = OFF;
	  	_p_NextBottle_state = STOP; 	//      stop;
      message(Controller::all, Msg::passiveNextBottle); // !!! CAN send 
   	}
   	break; 				// }
 } /* end of process */
// } /* END OF PROGRAM */
#endif

//    Serial.print("TankLevel = ");
//    Serial.println(TankLevel);
//    Serial.print(BottleCoord);
//    Serial.print(',');
//    Serial.print(BottleLevel);
//    Serial.print(',');
//    Serial.print(TankLevel);
//    Serial.print(',');
//    Serial.println(TankTemp);  
  // output
#ifdef Controller1
  digitalWrite(OFILLTANK, oFillTank);
#endif
#ifdef Controller2
  digitalWrite(OSTEAM, oSteam);
#endif
#ifdef Controller3
  digitalWrite(OFILLBOTTLE, oFillBottle);
#endif
#ifdef Controller4
  digitalWrite(OCONVEYOR, oConveyor);
#endif


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
#ifdef Controller1
    if ((_p_Initialization_state_1 != _p_Initialization_state) ||
        (_p_MainLoop_state_1 != _p_MainLoop_state) ||
        (_p_TankFilling_state_1 != _p_TankFilling_state)) {
      Serial.print("-P1S");     
      Serial.print(_p_Initialization_state);     
      Serial.print("-P2S");     
      Serial.print(_p_MainLoop_state);     
      Serial.print("-P3S");     
      Serial.println(_p_TankFilling_state);     
      _p_Initialization_state_1 = _p_Initialization_state;
      _p_MainLoop_state_1 = _p_MainLoop_state;
      _p_TankFilling_state_1 = _p_TankFilling_state;
      deltaT_1 = deltaT + 1;
    }
#endif
#ifdef Controller2
    if ((_p_ForcedSterilization_state_1 != _p_ForcedSterilization_state) ||
        (_p_KeepSterilization_state_1 != _p_KeepSterilization_state)) {
      Serial.print("-P4S");     
      Serial.print(_p_ForcedSterilization_state);     
      Serial.print("-P5S");     
      Serial.println(_p_KeepSterilization_state);     
      _p_ForcedSterilization_state_1 = _p_ForcedSterilization_state;
      _p_KeepSterilization_state_1 = _p_KeepSterilization_state;
      deltaT_1 = deltaT + 1;
    }
#endif
#ifdef Controller3
    if (_p_BottleFilling_state_1 != _p_BottleFilling_state) {
      Serial.print("-P6S");     
      Serial.println(_p_BottleFilling_state);     
      _p_BottleFilling_state_1 = _p_BottleFilling_state;
      deltaT_1 = deltaT + 1;
    }
#endif
#ifdef Controller4
    if (_p_NextBottle_state_1 != _p_NextBottle_state) {
      Serial.print("-P6S");     
      Serial.println(_p_NextBottle_state);     
      _p_NextBottle_state_1 = _p_NextBottle_state;
      deltaT_1 = deltaT + 1;
    }
#endif
  }
} /* end of control cycle*/
} /* end of loop */
