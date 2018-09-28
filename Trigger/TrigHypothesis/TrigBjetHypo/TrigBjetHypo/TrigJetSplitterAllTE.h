// emacs: this is -*- c++ -*-
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//
//   @file    TrigJetSplitterAllTE.h        
//
//                   
//  
//
//   $Id: TrigJetSplitterAllTE.h, v0.0   Tue 17 Jun 2014 03:26:44 CEST sutt $


#ifndef  TRIGJETSPLITTERALLTE_H
#define  TRIGJETSPLITTERALLTE_H

#include <iostream>

#include "TrigInterfaces/AllTEAlgo.h"


class TrigJetSplitterAllTE : public HLT::AllTEAlgo {

 public:
  TrigJetSplitterAllTE(const std::string & name, ISvcLocator* pSvcLocator);
  ~TrigJetSplitterAllTE();
  
  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltExecute(std::vector<std::vector<HLT::TriggerElement*> >&,unsigned int);
  HLT::ErrorCode hltFinalize();

 private:

  HLT::ErrorCode      getAndCheckHistoPrmVtx(HLT::TriggerElement* vtxTriggerElement , const xAOD::VertexContainer* vertices);
  const xAOD::Vertex* getPrimaryVertex(const xAOD::VertexContainer* vertexContainer);

  std::string m_jetInputKey;
  std::string m_jetOutputKey;
  std::string m_priVtxKey;
  bool        m_usePriVtxKeyBackup;
  std::string m_priVtxKeyBackup;

  double m_etaHalfWidth;
  double m_phiHalfWidth;
  double m_zHalfWidth;

  double m_minJetEt; // is this needed still? 
                     // (at least allows 1-to-1 tests with TrigEFBjetSequenceAllTE while we keep it at 15)
                     // ((not really a 1-1 comparison though, because of differences in the way the chains are configured))

  double m_maxJetEta;

  std::string m_dynamicEtaPhiHalfWidth;

};
 

#endif


