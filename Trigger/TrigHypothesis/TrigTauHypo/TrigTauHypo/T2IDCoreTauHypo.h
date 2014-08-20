/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGIDCORETAUHYPO_H
#define TRIGIDCORETAUHYPO_H
//----------------------------------------
// Name    : T2IDCoreTauHypo.h
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

class T2IDCoreTauHypo : public HLT::HypoAlgo {

 public:
 
  T2IDCoreTauHypo(const std::string& name, ISvcLocator* pSvcLocator);
  ~T2IDCoreTauHypo();

  // HLT method to execute
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* inputTE, bool& pass);
  // HLT method to initialize
  HLT::ErrorCode hltInitialize();
  // HLT method to finalize 
  HLT::ErrorCode hltFinalize();


 private:

  // define the properties:

  /** Cut on minimum number of tracks Ntrk >= NtrkMinCut */
  int   m_nTrkMin_Cut;
  /** Cut on maximu number of tracks Ntrk <= NtrkMaxCut */
  int   m_nTrkMax_Cut;
  /** cut on leading track pt */
  float m_ldTrkPt_Cut;



  //Monitoring: Cut counter
  int   m_cutCounter;
  //Monitoring : Ntracks
  int   m_nTracks;
  //Monitoring : leading Pt
  float m_ldTrkPt ;
  

};
#endif
