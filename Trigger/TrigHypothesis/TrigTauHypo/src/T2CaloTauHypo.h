/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
// 
// NAME:     T2CaloTauHypo.h
// PACKAGE:  Trigger/TrigHypothesis/TrigTauHypo
// 
// AUTHOR:   M.P. Casado
// 
// ********************************************************************

#ifndef TRIG_T2CALOTAUHYPO_H 
#define TRIG_T2CALOTAUHYPO_H

#include <string>
#include "TrigInterfaces/HypoAlgo.h"
#include "TrigSteeringEvent/Enums.h"


class T2CaloTauHypo: public HLT::HypoAlgo {
  enum { MaxNumberTools = 20 };  
public:
/** Constructor */
  T2CaloTauHypo(const std::string & name, ISvcLocator* pSvcLocator);
/** Destructor */
  ~T2CaloTauHypo();
/** HLT method to initialize */
  HLT::ErrorCode hltInitialize();
/** HLT method to finalize */
  HLT::ErrorCode hltFinalize();     
/** HLT method to execute */
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass); 
  
private:
  
  // Other members:   
  /** RawEt cut */
  double m_etRaw_Cut;
  /** EMRadius3S cut */
  double m_emRadius3S_Cut;
  /** Core fraction cut */
  double m_coreFraction_Cut;

/** Cut counter */
  int  m_cutCounter;


};

#endif
