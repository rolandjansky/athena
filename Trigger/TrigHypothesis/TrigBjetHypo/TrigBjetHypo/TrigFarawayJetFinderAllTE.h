/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ************************************************
//
// NAME:     TrigFarawayJetFinderAllTE.h
// PACKAGE:  Trigger/TrigHypothesis/TrigBjetHypo
//
// AUTHOR:   Katharine Leney
// EMAIL:    Katharine.Leney AT cern.ch
// 
// ************************************************

#ifndef TRIGFARAWAYJETFINDER_H
#define TRIGFARAWAYJETFINDER_H

#include "TrigInterfaces/AllTEAlgo.h"


//class TriggerElement;


class TrigFarawayJetFinderAllTE: public HLT::AllTEAlgo {

 public:

  TrigFarawayJetFinderAllTE(const std::string& name, ISvcLocator* pSvcLocator);
  ~TrigFarawayJetFinderAllTE();
  
  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute(std::vector<std::vector<HLT::TriggerElement*> >&,unsigned int);
  HLT::ErrorCode hltEndEvent();

 private:

  float phiCorr(float);

  float m_minDeltaRCut;
  std::string m_jetInputKey;
  std::string m_jetOutputKey;
  double m_etaHalfWidth;
  double m_phiHalfWidth;

};

#endif


