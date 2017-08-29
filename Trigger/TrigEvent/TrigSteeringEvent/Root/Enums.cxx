/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigSteeringEvent/Enums.h"
#include <string>
#include <sstream>
#include <iostream>

using namespace HLT;
const std::string ActionNames [4] = {"CONTINUE", "ABORT_CHAIN", "ABORT_EVENT", "ABORT_JOB"};

const std::string& strAction(const Action::Code code) {
  return ActionNames[code & 0x3];
}

Action::Code getActionFromString(const std::string& str) {
  if (str == "CONTINUE")         return Action::CONTINUE;
  else if (str == "ABORT_CHAIN") return Action::ABORT_CHAIN;
  else if (str == "ABORT_EVENT") return Action::ABORT_EVENT;
  else if (str == "ABORT_JOB")   return Action::ABORT_JOB;
  return Action::UNSPECIFIED_;
}


namespace HLTEnums {
  const std::string ReasonNames [16] = {"UNKNOWN", "MISSING_FEATURE", 
					"GAUDI_EXCEPTION", "EFORMAT_EXCEPTION", 
					"STD_EXCEPTION", "UNKNOWN_EXCEPTION",
					"NAV_ERROR", 
					"MISSING_ROD", "CORRUPTED_ROD", "TIMEOUT", 
					"BAD_JOB_SETUP",
					"USERDEF_1", "USERDEF_2", "USERDEF_3", "USERDEF_4",
					"LAST_" };
}

const std::string strReason(const Reason::Code code) {
  return HLTEnums::ReasonNames[code & 0xf];
}

Reason::Code getReasonFromString(const std::string& str) {
  bool found = false;
  int i=0;
  for ( i = 0; i < HLT::Reason::LAST_; ++i ) {
    if (str == HLTEnums::ReasonNames[i]) {
      found = true;
      break;
    }
  }
  if ( ! found )
    return Reason::UNSPECIFIED_;
  return Reason::Code(i);
}


namespace HLTEnums { 
  const std::string SteeringInternalReasonNames [] = { "UNKNOWN",
						       "NO_LVL1_ITEMS", 
						       "NO_LVL2_CHAINS",
						       "NO_LVL1_RESULT", 
						       "WRONG_HLT_RESULT", 
						       "NO_HLT_RESULT",
						       "ALGO_ERROR",
						       "TIMEOUT",
						       "BUSY",
						       "BAD_JOB_SETUP",
                               "MISSING_CALO_ROI",
                               "MISSING_MUON_ROI",
                               "MISSING_OTHER_ROI",
						       "LAST_"};
}


SteeringInternalReason::Code getSteeringInternalReasonFromString(const std::string& str) {
  bool found = false;
  unsigned int i=0;

  for ( i = 0; i < HLT::SteeringInternalReason::LAST_; ++i ) {
    if (str == HLTEnums::SteeringInternalReasonNames[i]) {
      found = true;
      break;
    }
  }
  if ( ! found )
    return SteeringInternalReason::UNSPECIFIED_;
  return SteeringInternalReason::Code(i);
}




const std::string strSteeringInternalReason (const SteeringInternalReason::Code code ){
  if ( unsigned(code) <  HLT::SteeringInternalReason::LAST_ )
    return HLTEnums::SteeringInternalReasonNames[code];
  return "UNSPECIFIED_";
}


std::string HLT::strErrorCode(const ErrorCode code) {
  return ""+strAction(code.action())
    + " " + strSteeringInternalReason(code.steeringInternalReason())
    + " " + strReason(code.reason()) ;
}

int HLT::getErrorCodePosFromStr(const std::string& ec) {
  std::istringstream ss(ec);

  std::string actionStr;
  std::string reasonStr;
  std::string steeringStr;

  // parse input
  ss >> actionStr  >> steeringStr >> reasonStr;


  Action::Code actionCode = getActionFromString(actionStr);
  
  if ( actionCode == Action::UNSPECIFIED_ ) 
    return -1;
  
  Reason::Code reasonCode = getReasonFromString(reasonStr);
  if ( reasonCode == Reason::UNSPECIFIED_ )
    return -1;
  

  SteeringInternalReason::Code steeringCode = getSteeringInternalReasonFromString(steeringStr);
  if ( steeringCode == SteeringInternalReason::UNSPECIFIED_ )
    return -1;
  
  return ErrorCode(actionCode, reasonCode, steeringCode );
}


std::string HLT::ErrorCode::str() {
  return strErrorCode(*this);
}
