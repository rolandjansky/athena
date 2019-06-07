/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGL2JETHYPO_H
#define TRIGL2JETHYPO_H
/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : TrigL2JetHypo.h
/// Package : Trigger/TrigHypothesis/TrigCaloHypo/
/// Author  : Ester Segura (segura@ifae.es)
/// Created : June 2006
///
/// DESCRIPTION: simple hypo algorithms for Lvl2 Jet, cut on energy
///
///////////////////////////////////////////////////////////////////////////////////////////////////////
#include <vector>

#include "TrigInterfaces/HypoAlgo.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"

class StoreGateSvc;
class TriggerElement;

class TrigL2JetHypo : public HLT::HypoAlgo {

 public:

  TrigL2JetHypo(const std::string& name, ISvcLocator* pSvcLocator);
  ~TrigL2JetHypo();

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
  bool m_doHECfnLeadingCellsCleaning;
  bool m_doEMfJetQualityCleaning;
  bool m_doHECfJetQualityCleaning;
  bool m_doTimeQualityCleaning;
  bool m_doEMfCleaningHighEta;
  bool m_doEMfCleaningLowEta;
  bool m_applyCleaningToHighEtJets;
  bool m_applyCleaningToLowEtJets;
  // depending on the flags m_applyCleaningToHighEtJets and m_applyCleaningToLowEtJets, the cleaning
  // is only applied to jets with Et > cleaningLowEtThreshold and/or Et < cleaningHighEtThreshold.
  float m_cleaningLowEtThreshold;        // in MeV
  float m_cleaningHighEtThreshold;       // in MeV      
  // thresholds for cleaning cuts:  
  int m_leadingCellsThr;         // threshold for number of leading cells in hecf > m_leadingCellsThr &&  nLeadingCells <= m_leadingCellsThr cut
  float m_hecfThrN;              // hecf threshold for cut in combination with nLeadingCells (i.e. hecf > m_leadingCellsThr &&  nLeadingCells <= m_leadingCellsThr)
  float m_hecfThrQ;              // hecf threshold for cut in combination with jet Quality (i.e. hecf > m_hecfThrQ && |quality| > m_jetQualityThrH)
  float m_jetQualityThrH;        // quality threshold for cut in combination with hecf (i.e. hecf > m_hecfThrQ && |quality| > m_jetQualityThrH)
  float m_jetQualityThrE;        // quality threshold for cut in combination with emf (i.e. emf > m_emfThrQ && |quality| > m_jetQualityThrE && |eta| < m_etaThrEmfQ)
  float m_emfThrQ;               // emf threshold for cut in combination with m_jetQualityThrE (i.e. emf > m_emfThrQ && |quality| > m_jetQualityThrE && |eta| < m_etaThrEmfQ)
  float m_etaThrEmfQ;            // eta threshold for cut in combination with m_jetQualityThrE (i.e. emf > m_emfThrQ && |quality| > m_jetQualityThrE && |eta| < m_etaThrEmfQ)
  float m_emfThrEmfHighEta;      // emf threshold for cut at high eta (i.e. emf < m_emfThrEmfHighEta && |eta| >= m_etaThrEmfHighEta)
  float m_etaThrEmfHighEta;      // eta threshold for cut at high eta (i.e. emf < m_emfThrEmfHighEta && |eta| >= m_etaThrEmfHighEta)
  float m_emfThrEmfLowEta;       // emf threshold for cut at low eta (i.e. emf < m_emfThrEmfLowEta && |eta| < m_etaThrEmfLowEta)
  float m_etaThrEmfLowEta;       // eta threshold for cut at low eta (i.e. emf < m_emfThrEmfLowEta && |eta| < m_etaThrEmfLowEta)
  float m_timeThrTimeQuality;    // time threshold for identification of out-of-time jets (i.e. |time| > m_timeThrTimeQuality && |quality| > m_qualityThrTimeQuality)
  float m_qualityThrTimeQuality; // quality threshold for identification of out-of-time jets (i.e. |time| > m_timeThrTimeQuality && |quality| > m_qualityThrTimeQuality)
  
  // basic cleaning default for every chain
  bool m_doBasicCleaning;
  float m_n90Threshold;
  float m_presamplerThreshold;
  float m_negativeEThreshold;

  int m_cutCounter;
  double              m_e;     // total energy of the jet after calibration
  double              m_et;
  double              m_eta;
  double              m_phi;
  // cleaning:
  float m_hecf;
  int m_nLeadingCells;
  float m_emf;
  float m_jetQuality;
  float m_jetTimeCells;

  // Histogram Service:
  //IHistogramSvc* m_histsvc;

  //StoreGateSvc*                           m_storeGate;

  // Timing:

  ITrigTimerSvc*            m_timersvc;
  std::vector<TrigTimer*>   m_timers;
};
#endif
