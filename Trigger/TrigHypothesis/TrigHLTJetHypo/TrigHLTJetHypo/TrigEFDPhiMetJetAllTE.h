/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigEFDPhiMetJetAllTE.h
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
// 
// ********************************************************************

#ifndef TRIGHLTJETHYPO_TRIGEFDPHIMETJETALLTE_H
#define TRIGHLTJETHYPO_TRIGEFDPHIMETJETALLTE_H

#include "TrigInterfaces/AllTEAlgo.h"


class TrigEFDPhiMetJetAllTE: public HLT::AllTEAlgo {


 public:
  TrigEFDPhiMetJetAllTE(const std::string & name, ISvcLocator* pSvcLocator);
  ~TrigEFDPhiMetJetAllTE();

  
  HLT::ErrorCode hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& inputTE,
			    unsigned int outputTE);


  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();

  /* PS 9/2/2015 remove as does not appear to do anything useful
  bool reset();
  */


 private:

  //  bool m_executedEvent_EFDPhiMetJet ;
  
  int m_acceptedEvts;
  int m_rejectedEvts;
  int m_errorEvts;

  double m_MinDPhiCut;
  double m_MaxDPhiJets;
  double m_MinJetEt;
  double m_MinJetEta;
  double m_MaxJetEta;


  double m_jetet         ;
  double m_jeteta        ;
  double m_jetphi        ;


  //Monitored variables


  double m_n_jets        ;
  double m_jet1et         ;
  double m_jet1eta        ;
  double m_jet1phi        ;
  double m_jet_met_phi1  ;  
  double m_jet2et         ;
  double m_jet2eta        ;
  double m_jet2phi        ;
  double m_jet_met_phi2  ;
  
  double m_n_jets_Pass  ; 
  double m_jet1et_Pass   ; 
  double m_jet1eta_Pass  ; 
  double m_jet1phi_Pass  ; 
  double m_jet_met_phi1_Pass ; 
  double m_jet2et_Pass   ; 
  double m_jet2eta_Pass  ; 
  double m_jet2phi_Pass  ; 
  double m_jet_met_phi2_Pass ;
  
};


#endif
