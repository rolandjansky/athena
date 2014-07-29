/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigHLTRoIDiagnostics.cxx
// PACKAGE:  Trigger/TrigAlgorithms/TrigHLTJetRec
//
// AUTHOR:   Valentinos Christodoulou
//
// Description: Hypo for printing the RoI
//
// ********************************************************************

#include "TrigHLTJetRec/TrigHLTRoIDiagnostics.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"


TrigHLTRoIDiagnostics::TrigHLTRoIDiagnostics(const std::string& name, ISvcLocator* pSvcLocator):
  HLT::HypoAlgo(name, pSvcLocator) {
}


TrigHLTRoIDiagnostics::~TrigHLTRoIDiagnostics() { }


HLT::ErrorCode TrigHLTRoIDiagnostics::hltInitialize() {
  ATH_MSG_INFO("Initializing " << name() << "...");  
  return HLT::OK; 
}


HLT::ErrorCode TrigHLTRoIDiagnostics::hltFinalize(){
  ATH_MSG_INFO ("Finalizing " << name() << "...");
  return HLT::OK;
}


HLT::ErrorCode TrigHLTRoIDiagnostics::hltExecute(const HLT::TriggerElement* inputTE, bool& pass) {
  ATH_MSG_DEBUG("Executing " << name() << "...");
  pass = true;

  // Get RoI from input trigger element
  const TrigRoiDescriptor* RoI = 0;
  HLT::ErrorCode hltStatus = getFeature(inputTE, RoI);  

  if (hltStatus == HLT::OK) {
    ATH_MSG_DEBUG("Retrieved RoI, address" << RoI);
  } else {
    ATH_MSG_ERROR("Failed to retrieve RoI.");
    return hltStatus;
  }

  if (RoI == 0){
    ATH_MSG_INFO("RoI address = 0, giving up");
    return HLT::OK;
  }

  ATH_MSG_INFO(*RoI << "\tfullscan " << RoI->isFullscan());

  return HLT::OK;
}
