/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 *
 * NAME:     TrigEFHTHypo.h
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
#ifndef TRIGEFHTHYPO_H
#define TRIGEFHTHYPO_H

#include <string>
#include <vector>

#include "TrigInterfaces/HypoAlgo.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"

class TriggerElement;

class TrigEFHTHypo : public HLT::HypoAlgo {

 public:

  TrigEFHTHypo(const std::string& name, ISvcLocator* pSvcLocator);
  ~TrigEFHTHypo();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);

 private:

  // define the properties:
  //----------------------------

  // Cuts to be applied:

  double     m_EtCut;
  double     m_HTCut;

  double     m_etaMinCut;
  double     m_etaMaxCut;

  int m_accepted;
  int m_rejected;
  int m_errors;

  // Switch to accept all the events.

  // Switch on Monitoring:
  
  bool m_doMonitoring;

  int m_cutCounter;

  int m_nJets            ;
  int m_nJets_passedCuts ;
  int m_nJets_passedHT   ;
  int m_nJets_failedHT   ;
  double m_HT            ;
  double m_HT_passedHT   ;

  std::vector<double> m_jetet     ; 
  std::vector<double> m_jetphi    ;
  std::vector<double> m_jeteta    ;

  std::vector<double> m_passedJetCuts_jetet     ; 
  std::vector<double> m_passedJetCuts_jetphi    ;
  std::vector<double> m_passedJetCuts_jeteta    ;

  double m_jetet1        ; 
  double m_jetphi1       ;
  double m_jeteta1       ;

  double m_jetet2        ; 
  double m_jetphi2       ;
  double m_jeteta2       ;
  
  // Timing:

  ITrigTimerSvc*            m_timersvc;
  std::vector<TrigTimer*>   m_timers;

};
#endif

