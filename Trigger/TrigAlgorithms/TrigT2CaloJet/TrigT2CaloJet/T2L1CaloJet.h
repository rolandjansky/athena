/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     T2CaloJet.h
// PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloJet
//
// AUTHOR:   Kyle Cranmer
//
// Description: Level2 Jet algorithm.  Basically copied from L2 EM and
// Tau algs with some cleanup.
// ********************************************************************

#ifndef TRIGT2CALOJET_T2L1CALOJET_H
#define TRIGT2CALOJET_T2L1CALOJET_H

#include <string>
#include "TrigInterfaces/FexAlgo.h"
#include "AthContainers/ConstDataVector.h"
#include "GaudiKernel/ToolHandle.h"

class StoreGateSvc;
class TrigT2Jet;

namespace LVL1BS{
  class ITrigT1CaloDataAccess;
}
namespace LVL1{
  class IL1JetTools;
  class JetElement;
}

class T2L1CaloJet: public HLT::FexAlgo {


 public:
  T2L1CaloJet(const std::string & name, ISvcLocator* pSvcLocator);
  ~T2L1CaloJet();

  HLT::ErrorCode hltExecute(const HLT::TriggerElement* inputTE,
			    HLT::TriggerElement* outputTE);


  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();

 private:

  TrigT2Jet* m_jet;


  // Properties:
  std::string               m_jetOutputKey;

  // Monitored Variables
  double              m_e;     // total energy of the jet after calibration
  double              m_et;
  //double              m_ehad0; // hadronic energy before calibration
  //double              m_eem0;  // electromagnetic energy before calibration
  double              m_eta;
  double              m_phi;
  
  /** For Logging, in all tools we should use (*m_log) to avoid recalling of the MsgStream all the time. */
  MsgStream* m_log;
  
  
  

 protected:

  std::vector<TrigTimer*> m_timer;
  ToolHandle< LVL1BS::ITrigT1CaloDataAccess > m_dataL1;
  ToolHandle< LVL1::IL1JetTools > m_jetL1Tools;
  ConstDataVector<DataVector<LVL1::JetElement> >* m_storedJEs;
  bool m_retrievedJetTool;
  

};

#endif
