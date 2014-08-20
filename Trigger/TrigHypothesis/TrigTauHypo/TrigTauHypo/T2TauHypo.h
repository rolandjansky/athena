/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGTAUHYPO_H
#define TRIGTAUHYPO_H

// ********************************************************************
//
// Name    : T2TauHypo.h
// Package : Trigger/TrigHypothesis/TrigTauHypo/
//
// ********************************************************************

#include <vector>

#include "TrigInterfaces/HypoAlgo.h"

namespace HLT {
  class TriggerElement;
}

class T2TauHypo : public HLT::HypoAlgo {

 public:

  /** Constructor */
  T2TauHypo(const std::string& name, ISvcLocator* pSvcLocator);
  /** Destructor */
  ~T2TauHypo();

  /** HLT method to execute */
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* inputTE, bool& pass);
  /** HLT method to initialize */
  HLT::ErrorCode hltInitialize();
  /** HLT method to finalize */
  HLT::ErrorCode hltFinalize();



 private:

  // define the properties:
  //----------------------------
  float m_emRadius3S1p_Cut;
  float m_etOverPtLeadTrk_Cut;
  float m_etOverPtLeadTrk_1PCut;
  float m_etOverPtLeadTrk_MPCut;
  float m_trkAvgDist_Cut;
  float m_trkAvgDist_1PCut;
  float m_trkAvgDist_MPCut;
  
  /** Cut counter */
  int   m_cutCounter;
  int   m_nTrks2011;
  int   m_nTrks2012;
  float m_etOverPtLeadTrk;
  float m_trkAvgDist;

};
#endif
