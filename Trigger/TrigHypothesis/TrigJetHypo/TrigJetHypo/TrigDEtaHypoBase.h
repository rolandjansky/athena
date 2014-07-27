/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGDETAHYPOBASE_H
#define TRIGDETAHYPOBASE_H

#include "TrigInterfaces/HypoAlgo.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"

//#include "TrigTimeAlgs/TrigTimerSvc.h"

class TriggerElement;
class ISvcLocator;

class INavigable4Momentum;

class TrigDEtaHypoBase : public HLT::HypoAlgo {
  
public:
  
  TrigDEtaHypoBase(const std::string &name, ISvcLocator *pSvcLocator);
  ~TrigDEtaHypoBase();
  
  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);
  
  
private:
  
  virtual HLT::ErrorCode getJets(const HLT::TriggerElement *outputTE, std::vector<const INavigable4Momentum*> &jets) = 0;
  
  int m_nGaps;
  int m_nJets;
  
  // Used to monitor the number of gaps
  int m_gapCounter;
  // Used to monitor the gap size
  double m_gapSize;
  
};


#endif
