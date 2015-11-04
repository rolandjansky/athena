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
  HLT::ErrorCode hltEndEvent();

 private:

  /** @brief string corresponding to the trigger level in which the algorithm is running. */
  //  std::string m_instance;


  float phiCorr(float);

  int m_workingMode;
  float m_deltaRCut;
  float m_deltaZCut;
  std::string m_jetKey;
  std::string m_priVtxKey;

  float m_etCut;

  float m_cutCounter;
  float m_deltaEtaPass;
  float m_deltaPhiPass;
  float m_muonEFEta;
  float m_muonEFPhi;
  float m_deltaRPass;
  float m_deltaRAll;
  float m_deltaZPass;
  float m_deltaZAll;
 


  HLT::ErrorCode getPrmVtxCollection(const xAOD::VertexContainer*& pointerToEFPrmVtxCollections, const HLT::TriggerElement* outputTE, std::string priVtxKey);
  HLT::ErrorCode getTrackCollection(const xAOD::TrackParticleContainer*& pointerToEFTrackCollections, const HLT::TriggerElement* outputTE);
};

#endif


