/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     ITriggerPseudoJetGetter.h
// PACKAGE:  Trigger/TrigAlgorithms/TrigHLTJetRec
//
// AUTHOR:   Valentinos Christodoulou
//
// Description: ITriggerPseudoJetGetter is a dual-use tool interface for 
// retrieving a vector of fastjet pseudojets.
// 
// ********************************************************************

#ifndef TRIGHLTJETREC_ITRIGGERPSEUDOJETGETTER_H
#define TRIGHLTJETREC_ITRIGGERPSEUDOJETGETTER_H

#include "JetInterface/IPseudoJetGetter.h"
#include "JetEDM/PseudoJetVector.h"
#include "AsgTools/IAsgTool.h"

class StatusCode;

class ITriggerPseudoJetGetter: virtual public IPseudoJetGetter {
  ASG_TOOL_INTERFACE(ITriggerPseudoJetGetter)

public:

  /*  static const InterfaceID& interfaceID() {
    static const InterfaceID
      IID_ITriggerPseudoJetGetter("ITriggerPseudoJetGetter", 1, 0);
    return IID_ITriggerPseudoJetGetter;
  }
  */

  virtual ~ITriggerPseudoJetGetter();

  // Pseudojet vector setter
  virtual void prime(const PseudoJetVector*) = 0;

};

#endif
