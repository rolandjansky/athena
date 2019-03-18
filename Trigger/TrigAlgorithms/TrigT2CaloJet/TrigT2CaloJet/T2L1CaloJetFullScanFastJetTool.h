/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT2CALOJET_T2L1CALOJETFULLSCANFASTJETTOOL_H
#define TRIGT2CALOJET_T2L1CALOJETFULLSCANFASTJETTOOL_H

/********************************************************************

NAME:     T2L1CaloJetFullScanFastJetTool.h
PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloJet

AUTHORS:  Matthew Tamsett
CREATED:  July, 2011

PURPOSE:  FastJet Tool for all T2L1CaloJet tools that require
          a vector of T2CaloJet objects as its input.  The derived
          tool must implement an execute(vector<T2CaloJet*>) method.


********************************************************************/

class TrigT2Jet;
class ITrigTimerSvc;

namespace LVL1BS{
  class ITrigT1CaloDataAccess;
}

namespace LVL1{
  class IL1JetTools;
  class JetElement;
  class TriggerTower;
}

namespace fastjet{
    class PseudoJet;
    class JetDefinition;
    class ClusterSequence;
}

// Extra tools for the RoI IDs
namespace T2L1{
    enum roi_types_enums {
        NONE      = 0x01,  
        L1SW8x8   = 0x02,  
        A4TT      = 0x04,
        A10TT     = 0x08,
        A4JE      = 0x10,
        A10JE     = 0x20,
        UNKNOWN   = 0x40,
        INPUT     = 0x10000,
        OUTPUT    = 0x100,
        BLANKWORD = 0x70000000
    };
}

#include "TrigT2CaloJet/T2L1CaloJetFullScanBaseTool.h"
#include "GaudiKernel/AlgTool.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"

#include "fastjet/JetDefinition.hh"

#include <vector>

class T2L1CaloJetFullScanFastJetTool : public T2L1CaloJetFullScanBaseTool
{
 public:

  T2L1CaloJetFullScanFastJetTool(const std::string& type, const std::string& name,
		    const IInterface* parent);

  ~T2L1CaloJetFullScanFastJetTool();

  virtual StatusCode initialize();
  using T2L1CaloJetFullScanBaseTool::execute;
  virtual StatusCode execute();
  virtual StatusCode finalize();

  virtual StatusCode execute(DataVector<TrigT2Jet>*, int&, float&, float&, float&, float&);
 
 protected:
  ToolHandle< LVL1BS::ITrigT1CaloDataAccess > m_dataL1;
  
  double m_coneRadius;
  double m_pTmin;
  bool m_retrievedJetTool;
  int m_inputType;
  
  TrigT2Jet* m_jet;
  std::vector<float> m_l1_tower_information;
  
  double m_etaMin;
  double m_etaMax;
  double m_phiMin;
  double m_phiMax;
  bool m_fullScan;
  
  
  ITrigTimerSvc *m_pTimerService;   // pointer to Timer Service
  TrigTimer *m_total_timer;
  TrigTimer *m_unpacking_timer;
  TrigTimer *m_load_collection_timer;
  TrigTimer *m_jet_finding_timer;
  TrigTimer *m_fastjet_timer;
  
  /** For Logging, in all tools we should use (*m_log) to avoid recalling of the MsgStream all the time. */
  MsgStream* m_log;
  
  bool m_doTriggerTowers; // bool to switch to trigger towers mode, default mode is Jet Elements
  // fast jet
  std::vector<fastjet::PseudoJet> m_particles;
  fastjet::JetDefinition * m_jet_def;
  //fastjet::ClusterSequence cs;
  std::vector<fastjet::PseudoJet> m_jets;
  std::vector<fastjet::PseudoJet> m_constituents;
  
  // cleaning:
  float m_leadingCellFraction;
  std::vector<double> m_cellenergies;
  
  // debug
  bool m_doCleaning;
  bool m_doJetFinding;

};

#endif // TRIGT2CALOJET_T2L1CaloJetFullScanFastJetTool
