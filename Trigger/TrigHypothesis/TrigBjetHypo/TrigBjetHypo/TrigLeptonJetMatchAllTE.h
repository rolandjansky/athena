/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ************************************************
//
// NAME:     TrigLeptonJetMatchAllTE.h
// PACKAGE:  Trigger/TrigHypothesis/TrigBjetHypo
//
// AUTHOR:   Andrea Coccaro
// EMAIL:    Andrea.Coccaro AT cern.ch
// 
// ************************************************

#ifndef TRIGEFLEPTONJETMATCH_H
#define TRIGEFLEPTONJETMATCH_H

#include "TrigInterfaces/AllTEAlgo.h"


class TriggerElement;



class TrigLeptonJetMatchAllTE: public HLT::AllTEAlgo {

 public:

  TrigLeptonJetMatchAllTE(const std::string& name, ISvcLocator* pSvcLocator);
  ~TrigLeptonJetMatchAllTE();
  
  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute(std::vector<std::vector<HLT::TriggerElement*> >&,unsigned int);

  bool reset();

 private:

  float phiCorr(float);

  int m_workingMode;
  float m_deltaRCut;
  float m_deltaZCut;

  float m_cutCounter;
  float m_deltaEtaPass;
  float m_deltaPhiPass;
  float m_muonEFEta;
  float m_muonEFPhi;
};

#endif


