/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGL2JETHYPONOISE_H
#define TRIGL2JETHYPONOISE_H
/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : TrigL2JetHypoNoise.h
/// Package : Trigger/TrigHypothesis/TrigJetHypo/
/// Author  : Denis Damazio (Denis.Oliveira.Damazio@cern.ch)
/// Created : January 2012
///
/// DESCRIPTION: simple hypo algorithms for Lvl2 Jet, cut on energy
///
///////////////////////////////////////////////////////////////////////////////////////////////////////
#include <vector>

#include "TrigInterfaces/HypoAlgo.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"

class StoreGateSvc;
class TriggerElement;

class TrigL2JetHypoNoise : public HLT::HypoAlgo {

 public:

  TrigL2JetHypoNoise(const std::string& name, ISvcLocator* pSvcLocator);
  ~TrigL2JetHypoNoise();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);


 private:

  // define the properties:
  //----------------------------

  // Cuts to be applied:

  double     m_EtCut_L2;

  int m_accepted_L2;
  int m_rejected_L2;
  int m_errors_L2;

  // Switch to accept all the events.

  bool m_acceptAll;

  // Switch on Monitoring:

  bool m_doMonitoring;
  
  // cleaning:
  // ---------
  // flags (do specific jet cleaning or don't):
  bool m_doNLeadingCellsUncleaning;
  bool m_doJetQualityUncleaning;
  int m_leadingCellsThr;         // threshold for number of leading cells in jet
  float m_jetQualityThr;        // quality threshold 

  int m_cutCounter;
  double              m_e;     // total energy of the jet after calibration
  double              m_et;
  double              m_eta;
  double              m_phi;
  // cleaning:
  int m_nLeadingCells;
  float m_jetQuality;

  //StoreGateSvc*                           m_storeGate;

  // Timing:

  ITrigTimerSvc*            m_timersvc;
  std::vector<TrigTimer*>   m_timers;
};
#endif
