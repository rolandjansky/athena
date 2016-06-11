/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGBHREMOVAL_H
#define  TRIGBHREMOVAL_H

#include <iostream>

#include "TrigInterfaces/AllTEAlgo.h"


class TrigBHremoval : public HLT::AllTEAlgo {

 public:
  TrigBHremoval(const std::string & name, ISvcLocator* pSvcLocator);
  ~TrigBHremoval();
  
  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltExecute(std::vector<std::vector<HLT::TriggerElement*> >&,unsigned int);
  HLT::ErrorCode hltFinalize();

 private:

  std::string m_jetInputKey;
  std::string m_jetOutputKey;

  double m_etaHalfWidth;
  double m_phiHalfWidth;
  double m_zHalfWidth;

  double m_minJetEt;
  double m_maxJetEta;
  double m_minLogRatio;
  
};
 

#endif


