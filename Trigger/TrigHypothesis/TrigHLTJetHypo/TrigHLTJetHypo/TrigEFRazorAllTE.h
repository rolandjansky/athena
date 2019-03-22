/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
#ifndef TRIGHLTJETHYPO_TRIGEFRAZORALLTE_H
#define TRIGHLTJETHYPO_TRIGEFRAZORALLTE_H

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

  //Variables to be kept around instead of using TLVs

  double m_jet[2][4];
  double m_met[2];

  double m_jet_m2[2];

  double m_di_hemi_pt[2];
  double m_di_hemi_pz;
  double m_di_hemi_mz;
  double m_di_hemi_m2;
  double m_di_hemi_E;

  double m_Ez2;
  double m_Etot;
  double m_Ptot2;

  double m_gamma;
  double m_beta[2];
  double m_pdotbeta[2];

  double m_p[2];
  double m_dot;

  // Timing:

  ITrigTimerSvc*            m_timersvc;
  std::vector<TrigTimer*>   m_timers;

};
#endif

