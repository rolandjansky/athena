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

  double     m_EtCut{0.};
  double     m_etaMinCut{0.};
  double     m_etaMaxCut{0.};
  unsigned int m_multiplicitycut{0};

  /// Et thresholds for assymmetric jet thresholds
  /// NB: for backwards compatability, retain the m_EtCut, 
  ///     so if multiple thresholds are not set, it will 
  ///     use the old, m_EtCut for each jet,
  ///     if however multiple thresholds are set, the 
  ///     old m_EtCut will be ignored.
  ///     use an extra vector for the actual property, 
  ///     so it can be modified, without changing what 
  ///     is actually passed in from the configurable 
  std::vector<double> m_EtThresholds{};
  std::vector<double> m_EtThresholdsInput{};


  int m_accepted{0};
  int m_rejected{0};
  int m_errors{0};

  // Switch to accept all the events.

  bool m_acceptAll{false};

  // Switch on Monitoring:
  
  bool m_doMonitoring{false};

  int m_cutCounter{0};
  double m_njet{0.};
  double m_et{0.};
  double m_eta{0.};
  double m_phi{0.};
  
  
  // switch on cleaning
  
  bool m_doBasicCleaning{false};
  bool m_triggerOnBad{false};
  
  float m_n90Threshold{0.};
  float m_presamplerThreshold{0.};
  float m_hecFThreshold{0.};
  float m_hecQThreshold{0.};
  float m_qmeanThreshold{0.};
  float m_negativeEThreshold{0.};
  float m_emFThreshold{0.};
  float m_larQThreshold{0.};
  

  // path for the histograms:

  //std::string m_path;

  // Histogram Service:
  //IHistogramSvc* m_histsvc;

  // Timing:

  ITrigTimerSvc*            m_timersvc{nullptr};
  std::vector<TrigTimer*>   m_timers;

};
#endif

