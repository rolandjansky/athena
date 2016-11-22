/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 *
 * NAME:     TrigEFJetDphiHypo.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigEgammaHypo
 *
 * AUTHOR:   Josh McFayden
 * CREATED:  2 Feb 2011
 *
 * Taken from:
 *  TrigEFCaloHypo.h
 *  Trigger/TrigHypothesis/TrigEgammaHypo
 *  P. Conde Muino, (C. Santamarina)
 *                     
 *********************************************************************/
#ifndef TRIGEFJETLEADINGDPHIHYPO_H
#define TRIGEFJETLEADINGDPHIHYPO_H


#include "TrigInterfaces/HypoAlgo.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"

class StoreGateSvc;
class TriggerElement;

class TrigEFJetDphiHypo : public HLT::HypoAlgo {

 public:

  TrigEFJetDphiHypo(const std::string& name, ISvcLocator* pSvcLocator);
  ~TrigEFJetDphiHypo();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);

 private:

  // define the properties:
  //----------------------------

  // Cuts to be applied:

  double     m_DphiCut;
  double     m_minDphiCut;
  double     m_EtCut1;
  double     m_EtCut2;
  double     m_etaMinCut;
  double     m_etaMaxCut;
  //int m_multiplicitycut;

  int m_accepted;
  int m_rejected;
  int m_errors;

  // Switch to accept all the events.

  bool m_acceptAll;

  // Switch on Monitoring:
  
  bool m_doMonitoring;

  int m_cutCounter;


  double m_passedJetCuts_minDphi;

  double m_nComb;  
  double m_nJet;

  std::vector<double> m_et1;
  std::vector<double> m_et2;
  std::vector<double> m_eta1;
  std::vector<double> m_eta2;
  std::vector<double> m_phi1;
  std::vector<double> m_phi2;

  std::vector<double> m_passedJetCuts_dphi;
  std::vector<double> m_passedJetCuts_et1;
  std::vector<double> m_passedJetCuts_et2;
  std::vector<double> m_passedJetCuts_eta1;
  std::vector<double> m_passedJetCuts_eta2;
  std::vector<double> m_passedJetCuts_phi1;
  std::vector<double> m_passedJetCuts_phi2;

  std::vector<double> m_passedDphiCut_dphi;
  std::vector<double> m_passedDphiCut_et1;
  std::vector<double> m_passedDphiCut_et2;
  std::vector<double> m_passedDphiCut_eta1;
  std::vector<double> m_passedDphiCut_eta2;
  std::vector<double> m_passedDphiCut_phi1;
  std::vector<double> m_passedDphiCut_phi2;

  double m_passedDphiCut_minDphi;
  double m_passednComb;

  // path for the histograms:

  //std::string m_path;

  // Histogram Service:
  //IHistogramSvc* m_histsvc;

  //StoreGateSvc*                           m_storeGate;

  // Timing:

  ITrigTimerSvc*            m_timersvc;
  std::vector<TrigTimer*>   m_timers;

};
#endif

