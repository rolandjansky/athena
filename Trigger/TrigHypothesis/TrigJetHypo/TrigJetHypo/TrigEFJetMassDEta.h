/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 *
 * NAME:     TrigEFJetMassDEta.cxx
 * PACKAGE:  Trigger/TrigHypothesis/TrigJetHypo
 *
 * AUTHOR:   Justin Griffiths
 *    clone of TrigEFJetMassY
 *    cut on DEta rather than YStar
 *    Consider all Jet pairs with at least XX Pt
 *
 *********************************************************************/
#ifndef TRIGEFJETMASSDETA_H
#define TRIGEFJETMASSDETA_H


#include "TrigInterfaces/HypoAlgo.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"

#include <vector>

class StoreGateSvc;
class TriggerElement;

class TrigEFJetMassDEta : public HLT::HypoAlgo {

 public:

  TrigEFJetMassDEta(const std::string& name, ISvcLocator* pSvcLocator);
  ~TrigEFJetMassDEta();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);

 private:

  // define the properties:
  //----------------------------

  // Cuts to be applied:

  double     m_MjjCut;
  double     m_dEtaCut;

  double     m_EtCut;
  double     m_etaMaxCut;

  int m_accepted;
  int m_rejected;
  int m_errors;

  // Switch to accept all the events.

  bool m_acceptAll;

  // Switch on Monitoring:
  
  bool m_doMonitoring;

  int m_cutCounter;
  int m_jetnr;
  int m_complexity;
  double m_deta;
  double m_mjj;
  double m_deta_cuted;
  double m_mjj_cuted;

  std::vector<double> m_pt;
  std::vector<double> m_pt_cuted;
  std::vector<double> m_eta;  
  std::vector<double> m_eta_cuted;
  

  //StoreGateSvc*                           m_storeGate;

  // Timing:

  ITrigTimerSvc*            m_timersvc;
  std::vector<TrigTimer*>   m_timers;

};
#endif

