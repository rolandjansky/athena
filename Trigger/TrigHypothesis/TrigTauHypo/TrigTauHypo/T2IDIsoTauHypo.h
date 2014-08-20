/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGIDISOTAUHYPO_H
#define TRIGIDISOTAUHYPO_H
//----------------------------------------
// Name    : T2IDIsoTauHypo.h
// Package : Trigger/TrigHypothesis/TrigTauHypo/
//
// Author  : Tofolet, 2012
//
//----------------------------------------
#include <vector>

#include "TrigInterfaces/HypoAlgo.h"

namespace HLT {
  class TriggerElement;
}

class T2IDIsoTauHypo : public HLT::HypoAlgo {

 public:
 
  T2IDIsoTauHypo(const std::string& name, ISvcLocator* pSvcLocator);
  ~T2IDIsoTauHypo();

  //HLT method to execute
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* inputTE, bool& pass);
  //HLT method to initialize
  HLT::ErrorCode hltInitialize();
  //HLT method to finalize
  HLT::ErrorCode hltFinalize();


 private:

  // define the properties:
  /** cut on ratio of sumPt in isolation to core, any prongs */
  float m_sumPtRatio_Cut ;
  /** cut on ratio of sumPt in isolation to core, 1P */
  float m_sumPtRatio_1PCut ;
  /** cut on ratio of sumPt in isolation to core, MP */
  float m_sumPtRatio_MPCut ;



  //Monitoring: Cut counter
  int   m_cutCounter;
  //Monitoring : sumPT_core/sumPt_isol
  float m_sumPtRatio;
  /**Monitoring : Ntracks */
  int   m_nTracks;

};
#endif
