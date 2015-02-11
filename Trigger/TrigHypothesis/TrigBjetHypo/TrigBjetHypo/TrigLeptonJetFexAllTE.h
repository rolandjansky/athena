/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ************************************************
//
// NAME:     TrigLeptonJetFexAllTE.h
// PACKAGE:  Trigger/TrigHypothesis/TrigBjetHypo
//
// AUTHOR:   Andrea Coccaro
// EMAIL:    Andrea.Coccaro AT cern.ch
// 
// ************************************************

#ifndef TRIGEFLEPTONJETFEX_H
#define TRIGEFLEPTONJETFEX_H

#include "TrigInterfaces/FexAlgo.h"
#include "TrigInterfaces/AllTEAlgo.h"


class TriggerElement;



class TrigLeptonJetFexAllTE: public HLT::AllTEAlgo {

 public:

  TrigLeptonJetFexAllTE(const std::string& name, ISvcLocator* pSvcLocator);
  ~TrigLeptonJetFexAllTE();
  
  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute(std::vector<std::vector<HLT::TriggerElement*> >&,unsigned int);
  HLT::ErrorCode hltEndEvent();

 private:

  float phiCorr(float);

  int m_workingMode;
  float m_deltaRCut;
  float m_etThreshold;

  float m_cutCounter;
  float m_deltaEtaPass;
  float m_deltaPhiPass;
  float m_muonEFEta;
  float m_muonEFPhi;
};

#endif


