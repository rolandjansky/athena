/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
// 
// NAME:     muIso.h
// PACKAGE:  Trigger/TrigAlgorithms/TrigmuIso
// 
// AUTHOR:   S. Giagu <stefano.giagu@cern.ch>
// 
// PURPOSE:  LVL2 Muon Isolation FEX Algorithm: V6.0
// ********************************************************************

#ifndef TRIGMUISO_MUISO_H 
#define TRIGMUISO_MUISO_H

#include <string>
#include "GaudiKernel/Algorithm.h"
#include "TrigInterfaces/FexAlgo.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/DataHandle.h"

#include "xAODTrigMuon/L2IsoMuonContainer.h"

class StoreGateSvc;
class TrigTimer;

/** Main LVL2 Algorithm. Sided by a xAOD::L2CombinedMuon, calculate trackID isolation 
    around the muon direction and fill a xAOD::L2IsoMuonContainer. */
class muIso: public HLT::FexAlgo {

 public:
  /** Constructor */
  muIso(const std::string & name, ISvcLocator* pSvcLocator);
  /** Destructor */
  ~muIso();

  /** hltExecute execute the muon isolation FEX. */
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* inputTE, HLT::TriggerElement* outputTE);
  /** hltInitialize. Called by the Steering. */
  HLT::ErrorCode hltInitialize();
  /** hltBeginRun. Called by the Steering. */
  HLT::ErrorCode hltBeginRun();
  /** hltFinalize. Called by the Steering. */
  HLT::ErrorCode hltFinalize();

  /** recordFeature. Record on the event record the output xAOD::L2IsoMuonContainer*) */
  HLT::ErrorCode muIsoSeed(HLT::TriggerElement* , xAOD::L2IsoMuonContainer*);

 private:

  /** Pointer to Storegate. */
  StoreGateSvc*   m_pStoreGate;

  /** Timer service Handle. */
  ITrigTimerSvc*  m_pTimerService;

  /** ID Track collection used for matching */
  StringProperty m_ID_algo_to_use;

  /** Cone radius for ID based isolation. */
  DoubleProperty            m_RID;
  /** Min Pt to select the ID track for matching */
  DoubleProperty            m_PtMinTrk;
  /** Max abs(eta) to select the ID track for matching */
  DoubleProperty            m_EtaMaxTrk;
  /** Max abs(Z_ID - Z_muon) for ID tracks to enter the pt cone sum */
  DoubleProperty            m_DzetaMax;

  /** Isolation cut used for Monitoring (ID) */
  DoubleProperty            m_GlobIsoCut_ID;

  /** Monitored quantities */
  int    m_ErrorFlagMI;
  float  m_ptFL;
  float  m_Sumpt;
  float  m_IDiso;
  float  m_MuPt;
  float  m_MuEta;
  float  m_MuPhi;
  float  m_NTRK;
  float  m_isIsolated;
};
#endif
