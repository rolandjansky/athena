/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGL2MULTIJETHYPO_H
#define TRIGL2MULTIJETHYPO_H
/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : TrigL2MultiJetHypo.h
/// Package : Trigger/TrigHypothesis/TrigMultiJetHypo/
/// Author  : Matthew Tamsett (tamsett@cern.ch)
/// Created : July 2011
///
/// DESCRIPTION: simple hypo algorithms for Lvl2 Jet, cut on energy and multiplicity
/// This is based on the TrigL2JetHypo and the TrigEFJetHypo which includes multiplicity dependent cuts
///
///////////////////////////////////////////////////////////////////////////////////////////////////////
#include <vector>

#include "TrigInterfaces/HypoAlgo.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"

class StoreGateSvc;
class TriggerElement;

class TrigL2MultiJetHypo : public HLT::HypoAlgo {

 public:

  TrigL2MultiJetHypo(const std::string& name, ISvcLocator* pSvcLocator);
  ~TrigL2MultiJetHypo();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);


 private:

  // define the properties:
  //----------------------------
  // input jet collection
  std::string  m_jetInputKey;

  // Cuts to be applied:

  double     m_EtCut_L2;
  unsigned int m_multiplicitycut;
  /// Et thresholds for assymmetric jet thresholds
  /// NB: for backwards compatability, retain the m_EtCut, 
  ///     so if multiple thresholds are not set, it will 
  ///     use the old, m_EtCut for each jet,
  ///     if however multiple thresholds are set, the 
  ///     old m_EtCut will be ignored.
  ///     use an extra vector for the actual property, 
  ///     so it can be modified, without changing what 
  ///     is actually passed in from the configurable 
  std::vector<double> m_EtThresholds;
  std::vector<double> m_EtThresholdsInput;
  double m_etaMinCut;
  double m_etaMaxCut;

  int m_accepted_L2;
  int m_rejected_L2;
  int m_errors_L2;

  // Switch to accept all the events.
  bool m_acceptAll;

  // Switch on Monitoring:

  bool m_doMonitoring;
  
  // Monitored Variables
  double              m_nJets;          // The number of jets accepted  
  int                 m_cutCounter;     // The number of events which pass
  std::vector<double> m_e;              // energy
  std::vector<double> m_et;             // transverse energy
  std::vector<double> m_eta;            // eta
  std::vector<double> m_phi;            // phi
  std::vector<int>    m_nLeadingTowers; // number of leading towers
  
  std::vector<int>    m_accepted_jets;    // a vector of the accepted jets
  
  // Histogram Service:
  //IHistogramSvc* m_histsvc;

  //StoreGateSvc*                           m_storeGate;

  // Timing:

  ITrigTimerSvc*            m_timersvc;
  std::vector<TrigTimer*>   m_timers;
  
  /** For Logging, in all tools we should use (*m_log) to avoid recalling of the MsgStream all the time. */
  MsgStream* m_log;
};
#endif
