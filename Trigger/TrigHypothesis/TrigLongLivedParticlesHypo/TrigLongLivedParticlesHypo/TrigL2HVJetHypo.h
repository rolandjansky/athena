/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGL2HVJETHYPO_H
#define TRIGL2HVJETHYPO_H
/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : TrigL2HVJetHypo.h
/// Package : Trigger/TrigHypothesis/TrigJetHypo/
/// Authors  : Stefano Giagu <stefano.giagu@cern.ch>, Antonio Policicchio <antonio.policicchio@cern.ch>
/// Created : Sept. 2008
///
/// DESCRIPTION: simple hypo algorithms for Lvl2 Hidden Valley Jet, cut on energy and log10 of 
///              had/em energy ratio; 
///
///////////////////////////////////////////////////////////////////////////////////////////////////////
#include <vector>

#include "TrigInterfaces/HypoAlgo.h"



class StoreGateSvc;
class TriggerElement;

class TrigL2HVJetHypo : public HLT::HypoAlgo {

 public:

  TrigL2HVJetHypo(const std::string& name, ISvcLocator* pSvcLocator);
  ~TrigL2HVJetHypo();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);


 private:

  // define the properties:
  //----------------------------

  // Cuts to be applied for Jets
  /** cut on jet Et */
  double     m_EtCut_L2;
  /** cut on Log(H/E) */ 
  double     m_LRatioCut_L2;
  /** cut on jet eta */
  double     m_JetEta;


  // cleaning:
  // ---------
  // flags (do specific jet cleaning or don't):
  bool m_doHECfnLeadingCellsCleaning;
  bool m_doEMfJetQualityCleaning;
  bool m_doHECfJetQualityCleaning;
  bool m_doJetTimeCleaning;
  bool m_doLowEMfCleaning;
  // thresholds for cleaning cuts: 
  int m_leadingCellsThr;        // threshold for number of leading cells in (hecf > m_leadingCellsThr &&  nLeadingCells <= m_leadingCellsThr) cut
  float m_hecfThrN;             // hecf threshold for cut in combination with nLeadingCells (i.e. hecf > m_leadingCellsThr &&  nLeadingCells <= m_leadingCellsThr)
  float m_hecfThrQ;             // hecf threshold for cut in combination with jet Quality (i.e. hecf > m_hecfThrQ && |quality| > m_jetQualityThrH)
  float m_jetQualityThrH;       // quality threshold for cut in combination with hecf (i.e. hecf > m_hecfThrQ && |quality| > m_jetQualityThrH)
  float m_jetQualityThrE;       // quality threshold for cut in combination with emf (i.e. emf > m_emfThrQ && |quality| > m_jetQualityThrE)
  float m_emfThrQ;              // emf threshold for cut in combination with m_jetQualityThrE (i.e. emf > m_emfThrQ && quality > m_jetQualityThrE)
  float m_emfThrE;              // emf threshold for Exclusive emf cut (i.e. emf < m_emfThrE)
  float m_jetTimeCellsThr;      // threshold for identification of out-of-time jets

  float m_hecf;
  int m_nLeadingCells;
  float m_emf;
  float m_fabsjetQuality;
  float m_fabsjetTimeCells;


  //Events accepted, rejected and with some error
  int m_accepted_L2;
  int m_rejected_L2;
  int m_errors_L2;

  // Switch to accept all the events.
  bool m_acceptAll;

  // Switch on Monitoring:
  bool m_doMonitoring;

  //Monitored Variables
  int m_cutCounter;
  double m_JetEt;
  double m_logRatio;
};
#endif
