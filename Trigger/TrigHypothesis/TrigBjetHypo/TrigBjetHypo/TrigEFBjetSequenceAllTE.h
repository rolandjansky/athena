/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ************************************************
//
// NAME:     TrigEFBjetSequenceAllTE.h
// PACKAGE:  Trigger/TrigHypothesis/TrigBjetHypo
//
// AUTHOR:   Andrea Coccaro
// EMAIL:    Andrea.Coccaro AT cern.ch
//
// ************************************************

#ifndef TRIGEFBJETSEQUENCEALLTE_H
#define TRIGEFBJETSEQUENCEALLTE_H

#include "TrigInterfaces/AllTEAlgo.h"


class TrigEFBjetSequenceAllTE: public HLT::AllTEAlgo {

 public:
  TrigEFBjetSequenceAllTE(const std::string & name, ISvcLocator* pSvcLocator);
  ~TrigEFBjetSequenceAllTE();
  
  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltExecute(std::vector<std::vector<HLT::TriggerElement*> >&,unsigned int);
  HLT::ErrorCode hltFinalize();

 private:

  std::string m_jetInputKey;
  std::string m_jetOutputKey;
  bool m_useSuperRoi;
  double m_etaHalfWidth;
  double m_phiHalfWidth;
};
 

#endif


