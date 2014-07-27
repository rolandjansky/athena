/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 *
 * NAME:     TrigEFCaloHypo.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigEgammaHypo
 *
 * AUTHOR:   P. Conde Muino
 * CREATED:  Nov 25, 2004
 *           Jan 29, 2007 Migrated to the new Steering (C. Santamarina)
 *
 *********************************************************************/
#ifndef TRIGEFJETHYPO_H
#define TRIGEFJETHYPO_H


#include "TrigInterfaces/HypoAlgo.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"

class StoreGateSvc;
class TriggerElement;

class TrigEFJetHypo : public HLT::HypoAlgo {

 public:

  TrigEFJetHypo(const std::string& name, ISvcLocator* pSvcLocator);
  ~TrigEFJetHypo();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);

 private:

  bool doBasicCleaning(const xAOD::Jet*, bool&);
  bool getJetAttribute(const std::string&, float&, const xAOD::Jet*);

  // Cuts to be applied:

  double     m_EtCut;
  double     m_etaMinCut;
  double     m_etaMaxCut;
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


  int m_accepted;
  int m_rejected;
  int m_errors;

  // Switch to accept all the events.

  bool m_acceptAll;

  // Switch on Monitoring:
  
  bool m_doMonitoring;

  int m_cutCounter;
  double m_njet;
  double m_et;
  double m_eta;
  double m_phi;
  
  
  // switch on cleaning
  
  bool m_doBasicCleaning;
  bool m_triggerOnBad;
  
  float m_n90Threshold;
  float m_presamplerThreshold;
  float m_hecFThreshold;
  float m_hecQThreshold;
  float m_qmeanThreshold;
  float m_negativeEThreshold;
  float m_emFThreshold;
  float m_larQThreshold;
  

  // path for the histograms:

  //std::string m_path;

  // Histogram Service:
  //IHistogramSvc* m_histsvc;

  StoreGateSvc*                           m_storeGate;

  // Timing:

  ITrigTimerSvc*            m_timersvc;
  std::vector<TrigTimer*>   m_timers;

};
#endif

