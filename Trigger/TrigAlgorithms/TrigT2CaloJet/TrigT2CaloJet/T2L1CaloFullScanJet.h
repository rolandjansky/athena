/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     T2L1CaloFullScanJet.h
// PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloJet
//
// AUTHOR:   Matthew Tamsett
//
// Description: Level2 Jet Full scan algorithm.  Based on L2 MET
// ********************************************************************

#ifndef TRIGT2CALOJET_T2L1CaloFullScanJet_H
#define TRIGT2CALOJET_T2L1CaloFullScanJet_H

#include "TrigInterfaces/AllTEAlgo.h"
#include <vector>

#include <string>
#include "GaudiKernel/ToolHandle.h"
#include "TrigT2CaloCommon/T2CaloBase.h"
#include "TrigT2CaloJet/T2L1CaloJetFullScanBaseTool.h"

class StoreGateSvc;
class TrigT2Jet;
class ITrigTimerSvc;

namespace LVL1BS{
  class ITrigT1CaloDataAccess;
}

namespace LVL1{
  class IL1JetTools;
}

class T2L1CaloFullScanJet: public HLT::AllTEAlgo {


 public:
  T2L1CaloFullScanJet(const std::string & name, ISvcLocator* pSvcLocator);
  ~T2L1CaloFullScanJet();

  HLT::ErrorCode hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& input,
                            unsigned int output);


  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();

 private:

  const TrigT2Jet* m_jet;
  DataVector<TrigT2Jet>* m_jets;
  ToolHandleArray< T2L1CaloJetFullScanBaseTool > m_tools;
  
  // Properties:
  std::string               m_jetOutputKey;
  bool                      m_recordOutput;

  // Monitored Variables
  int                 m_nJets;          // The number of jets found
  int                 m_nTowers;        // The number of trigger towers
  float               m_UnpckTime;      // The unpacking time, includes L1 unpack + tower storage time
  float               m_L1_UnpckTime;   // The L1 unpacking time
  float               m_FastJetTime;    // The FastJet time
  float               m_JetFindingTime; // The complete jet finding time
  float               m_TotalTime;      // The total time
  float               m_RoITime;        // The RoI making time
  std::vector<double> m_e;              // energy
  std::vector<double> m_et;             // transverse energy
  std::vector<double> m_ehad;           // hadronic energy 
  std::vector<double> m_eem;            // electromagnetic energy
  std::vector<double> m_ethad;          // hadronic transverse energy 
  std::vector<double> m_etem;           // electromagnetic transverse energy
  std::vector<double> m_emfrac;         // electromagnetic fraction
  std::vector<double> m_eta;            // eta
  std::vector<double> m_phi;            // phi
  std::vector<int>    m_nLeadingTowers; // number of leading towers
  
  
  /** For Logging, in all tools we should use (*m_log) to avoid recalling of the MsgStream all the time. */
  MsgStream* m_log;
  
  

 protected:
 
  std::vector<TrigTimer*> m_timer;
  

};

#endif
