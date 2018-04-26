/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     ITriggerPseudoJetGetter2.h
// PACKAGE:  Trigger/TrigAlgorithms/TrigHLTJetRec
//
// Description: ITriggerPseudoJetGetter is a dual-use tool interface for 
// retrieving a vector of fastjet pseudojets.
// 
// ********************************************************************

#ifndef TRIGHLTJETREC_ITRIGGERPSEUDOJETGETTER2_H
#define TRIGHLTJETREC_ITRIGGERPSEUDOJETGETTER2_H

#include "JetInterface/IPseudoJetGetter.h"
// #include "JetEDM/PseudoJetVector.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "AsgTools/IAsgTool.h"

class StatusCode;

class ITriggerPseudoJetGetter2: virtual public IPseudoJetGetter {
  ASG_TOOL_INTERFACE(ITriggerPseudoJetGetter2)

public:

  /*  static const InterfaceID& interfaceID() {
    static const InterfaceID
      IID_ITriggerPseudoJetGetter("ITriggerPseudoJetGetter", 1, 0);
    return IID_ITriggerPseudoJetGetter;
  }
  */

  virtual ~ITriggerPseudoJetGetter2();

  // Pseudojet vector setter
  // virtual void prime(const PseudoJetVector*) = 0;
  virtual void prime(const xAOD::CaloClusterContainer*) = 0;
};

#endif
