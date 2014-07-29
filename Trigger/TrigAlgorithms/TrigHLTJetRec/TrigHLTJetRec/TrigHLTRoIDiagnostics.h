/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigHLTRoIDiagnostics.h
// PACKAGE:  Trigger/TrigAlgorithms/TrigHLTJetRec
//
// AUTHOR:   Valentinos Christodoulou
//
// Description: Hypo for printing the RoI
//
// ********************************************************************

#ifndef TRIGHLTJETREC_TRIGHLTROIDIAGNOSTICS_H
#define TRIGHLTJETREC_TRIGHLTROIDIAGNOSTICS_H

#include "TrigInterfaces/HypoAlgo.h"
	
class TrigHLTRoIDiagnostics: public HLT::HypoAlgo {
	
public:
  TrigHLTRoIDiagnostics(const std::string& name, ISvcLocator* pSvcLocator);
  ~TrigHLTRoIDiagnostics();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* inputTE, bool& pass);

}; 

#endif
