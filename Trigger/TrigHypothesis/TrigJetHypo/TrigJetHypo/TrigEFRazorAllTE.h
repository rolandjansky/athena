/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 *
 * NAME:     TrigEFRazorAllTE.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigJetHypo
 *
 * AUTHOR:   Lawrence Lee, based on DPhiMetJet hypo from others
 * CREATED:  23 Oct 2014
 *
 *                     
 *********************************************************************/
#ifndef TRIGEFRAZORALLTE_H
#define TRIGEFRAZORALLTE_H

#include <string>
#include <vector>

#include "TrigInterfaces/AllTEAlgo.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"

class StoreGateSvc;
class TriggerElement;

class TrigEFRazorAllTE : public HLT::AllTEAlgo {

 public:

  TrigEFRazorAllTE(const std::string& name, ISvcLocator* pSvcLocator);
  ~TrigEFRazorAllTE();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  // HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);
  HLT::ErrorCode hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& inputTE,
          unsigned int outputTE);


 private:

  // define the properties:
  //----------------------------

  // Cuts to be applied:

  double     m_RazorCut;

  int m_accepted;
  int m_rejected;
  int m_errors;

  // Switch to accept all the events.

  // Switch on Monitoring:
  
  bool m_doMonitoring;

  int m_cutCounter;

  int m_nJets            ;
  double m_Razor            ;
  double m_Razor_passedRazor   ;

  double m_passedJetCuts_jetet_0     ; 
  double m_passedJetCuts_jetphi_0    ;
  double m_passedJetCuts_jeteta_0    ;

  double m_passedJetCuts_jetet_1     ; 
  double m_passedJetCuts_jetphi_1    ;
  double m_passedJetCuts_jeteta_1    ;

  double m_gaminvRp1;
  double m_shatR;
  double m_mdeltaR;
  double m_cosptR;

  double m_Minv2;
  double m_Einv;

  //IHistogramSvc* m_histsvc;

  StoreGateSvc*                           m_storeGate;

  // Timing:

  ITrigTimerSvc*            m_timersvc;
  std::vector<TrigTimer*>   m_timers;

};
#endif

