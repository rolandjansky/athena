/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 *
 * NAME:     TrigEFJetMctHypo.h
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
#ifndef TRIGEFJETLEADINGMCTHYPO_H
#define TRIGEFJETLEADINGMCTHYPO_H


#include "TrigInterfaces/HypoAlgo.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"

class StoreGateSvc;
class TriggerElement;

class TrigEFJetMctHypo : public HLT::HypoAlgo {

 public:

  TrigEFJetMctHypo(const std::string& name, ISvcLocator* pSvcLocator);
  ~TrigEFJetMctHypo();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);

 private:

  // define the properties:
  //----------------------------

  // Cuts to be applied:

  double     m_MctCut;
  double     m_EtCut1;
  double     m_EtCut2;
  double     m_etaMinCut;
  double     m_etaMaxCut;
  int m_multiplicitycut;

  int m_accepted;
  int m_rejected;
  int m_errors;

  // Switch to accept all the events.

  bool m_acceptAll;

  // Switch on Monitoring:
  
  bool m_doMonitoring;

  int m_cutCounter;


  double m_passedJetCuts_maxmct;
  double m_passedMctCut_maxmct;

  double m_nJet;
  double m_nComb;
  double m_passednComb;
  
  std::vector<double> m_et1;
  std::vector<double> m_et2;
  std::vector<double> m_eta1;
  std::vector<double> m_eta2;
  std::vector<double> m_phi1;
  std::vector<double> m_phi2;

  std::vector<double> m_passedJetCuts_mct;
  std::vector<double> m_passedJetCuts_dphi;
  std::vector<double> m_passedJetCuts_et1;
  std::vector<double> m_passedJetCuts_et2;
  std::vector<double> m_passedJetCuts_eta1;
  std::vector<double> m_passedJetCuts_eta2;
  std::vector<double> m_passedJetCuts_phi1;
  std::vector<double> m_passedJetCuts_phi2;

  std::vector<double> m_passedMctCut_mct;
  std::vector<double> m_passedMctCut_dphi;
  std::vector<double> m_passedMctCut_et1;
  std::vector<double> m_passedMctCut_et2;
  std::vector<double> m_passedMctCut_eta1;
  std::vector<double> m_passedMctCut_eta2;
  std::vector<double> m_passedMctCut_phi1;
  std::vector<double> m_passedMctCut_phi2;

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

