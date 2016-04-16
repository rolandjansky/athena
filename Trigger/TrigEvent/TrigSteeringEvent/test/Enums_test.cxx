/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifdef XAOD_STANDALONE
int main(){return 0;}
#endif
#ifndef XAOD_STANDALONE

#include <iostream>
#include "TrigSteeringEvent/Enums.h"
using namespace HLT;
using namespace std;
int main () {
  cout << "Start testing " << endl;
 //  ErrorCode c1 = ErrorCode(1,5); // this should fail to compile
  ErrorCode c2(Action::CONTINUE); // this should go
  cout << " code" << hex << c2.code << " " << HLT::strErrorCode(c2)<< endl;

  ErrorCode c3(Action::ABORT_CHAIN, Reason::MISSING_FEATURE);
  cout << " code" << hex << c3.code << " " << HLT::strErrorCode(c3)<< endl;

  c3 = ErrorCode(Action::ABORT_CHAIN);
  cout << " code" << hex << c3.code << " " << HLT::strErrorCode(c3)<< endl;

  c3 = ErrorCode(Action::ABORT_EVENT);
  cout << " code" << hex << c3.code << " " << HLT::strErrorCode(c3)<< endl;

  c3 = ErrorCode(Action::ABORT_JOB, Reason::BAD_JOB_SETUP );
  cout << " code" << hex << c3.code << " " << HLT::strErrorCode(c3)<< endl;


  if ( c2 != HLT::OK ) {
    cout << "ERROR c2 is found to be not OK while it is " << endl; 
    return -1;
  }

  if (c3 == HLT::OK ) {
    cout << "ERROR c3 should not be seen as OK" << endl;
    return -1;
  }

  c3 = ErrorCode(c3, SteeringInternalReason::ALGO_ERROR);
  cout << " code" << hex << c3.code << " " << HLT::strErrorCode(c3)<< endl;

  ErrorCode c4 = (c3>c2) ? c3 : c2;
  
  cout << " code" << hex << c4.code << " " << HLT::strErrorCode(c4)<< endl;

  cout << "predefined error codes" << endl;
  cout << HLT::strErrorCode(OK) << endl << HLT::strErrorCode(MISSING_FEATURE) << endl 
       << HLT::strErrorCode(ERROR) << endl << HLT::strErrorCode(INVALID_TE) << endl
       << HLT::strErrorCode(TOOL_FAILURE) << endl << HLT::strErrorCode(NUM_ERROR) << endl 
       << HLT::strErrorCode(SG_ERROR) << endl <<HLT::strErrorCode(NAV_ERROR) << endl
       << HLT::strErrorCode(GAUDI_EXCEPTION) << endl <<HLT::strErrorCode(EFORMAT_EXCEPTION) << endl 
       << HLT::strErrorCode(STD_EXCEPTION) << endl << HLT::strErrorCode(UNKNOWN_EXCEPTION) << endl
       << HLT::strErrorCode(TIMEOUT) << endl <<HLT::strErrorCode(NO_LVL1_ITEMS) << endl 
       << HLT::strErrorCode(NO_LVL2_CHAINS) << endl << HLT::strErrorCode(NO_LVL1_RESULT) << endl 
       << HLT::strErrorCode(WRONG_HLT_RESULT) << endl <<HLT::strErrorCode(NO_HLT_RESULT) << endl 
       << HLT::strErrorCode(BAD_JOB_SETUP) << endl << HLT::strErrorCode(BAD_ALGO_CONFIG) << endl 
       << HLT::strErrorCode(FATAL) << endl;


  cout << "way back --- from ec name to number (hex)" << endl;
  cout << HLT::getErrorCodePosFromStr("CONTINUE") << " truth: " <<  ErrorCode(Action::CONTINUE) << endl; 
  cout << HLT::getErrorCodePosFromStr("CONTINUE MISSING_FEATURE") << " truth: " <<  ErrorCode(Action::CONTINUE, Reason::MISSING_FEATURE) << endl; 
  cout << HLT::getErrorCodePosFromStr("CONTINUE TIMEOUT TIMEOUT") << " truth: " <<  ErrorCode(Action::CONTINUE, Reason::TIMEOUT, SteeringInternalReason::TIMEOUT) << endl; 
  cout << HLT::getErrorCodePosFromStr("ABORT_EVENT TIMEOUT TIMEOUT") << " truth: " <<  ErrorCode(Action::ABORT_EVENT, Reason::TIMEOUT, SteeringInternalReason::TIMEOUT) << endl; 
  cout << HLT::getErrorCodePosFromStr("ABORT_EVENT TIMEOUT") << " truth: " <<  ErrorCode(Action::ABORT_EVENT, Reason::TIMEOUT) << endl; 
  cout << HLT::getErrorCodePosFromStr("ABORT_JOB BAD_JOB_SETUP") << " truth: " <<  ErrorCode(Action::ABORT_JOB, Reason::BAD_JOB_SETUP) << endl; 
  cout << HLT::getErrorCodePosFromStr("ABORT_THISJOB BAD_JOB_SETUP") << " truth: " <<  -1 << endl; 

  for ( int act = HLT::Action::CONTINUE; act <= HLT::Action::ABORT_JOB; act++) {
    for (int res = HLT::Reason::UNKNOWN; res < HLT::Reason::LAST_; ++res ) {
      for ( int ster = HLT::SteeringInternalReason::UNKNOWN; ster < HLT::SteeringInternalReason::LAST_; ++ster) {
	// make string
	HLT::ErrorCode tmpec = HLT::ErrorCode(HLT::Action::Code(act), HLT::Reason::Code(res), HLT::SteeringInternalReason::Code(ster));
	std::string str = HLT::strErrorCode(tmpec);
	cerr << "Trying : " << str << endl;
	HLT::ErrorCode newec = HLT::ErrorCode(getErrorCodePosFromStr(str));
	if (tmpec != newec ) {
	  cout << "ERROR failed conversion for : " << str << " != " << HLT::strErrorCode(newec) << endl;
	  return -1;
	}
      }
    }
  }

  cout << "tesint member function for printing " << ErrorCode(Action::CONTINUE, Reason::TIMEOUT, SteeringInternalReason::TIMEOUT).str() << endl;
  return 0;
}
#endif
