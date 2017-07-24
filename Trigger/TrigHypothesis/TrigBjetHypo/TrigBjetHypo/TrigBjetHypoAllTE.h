/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ************************************************
//
// NAME:     TrigBjetHypoAllTE.h
// PACKAGE:  Trigger/TrigHypothesis/TrigBjetHypo
//
// AUTHORS:   John Alison   johnda@uchicago.edu
// 
// ************************************************

#ifndef TRIGBJETHYPOALLTE_H
#define TRIGBJETHYPOALLTE_H

#include "TrigInterfaces/AllTEAlgo.h"

class TriggerElement;

class TrigBjetHypoAllTE: public HLT::AllTEAlgo {

 public:

  TrigBjetHypoAllTE(const std::string& name, ISvcLocator* pSvcLocator);
  ~TrigBjetHypoAllTE();
  
  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute(std::vector<std::vector<HLT::TriggerElement*> >&,unsigned int);
  HLT::ErrorCode hltEndEvent();

 private:

  const xAOD::BTagging* getBTaggingPtr(HLT::TriggerElement* btagTE);
  const xAOD::Jet* getJetPtr(const xAOD::BTagging* btagInfo);
  bool  eventPassesTrigger();
  void  clearCounters();

 private:

  struct triggerRequirement{
    float          m_etThreshold;
    float          m_btagCut;
    unsigned int   m_multiplicity;

    unsigned int   m_count;
    triggerRequirement(float etThreshold, float btagCut, unsigned multiplicity)  : m_etThreshold(etThreshold), m_btagCut(btagCut), m_multiplicity(multiplicity),  m_count(0) 
    { }
    
  };
  std::vector<triggerRequirement> m_triggerReqs;

  /** @brief string corresponding to the trigger level in which the algorithm is running. */
  //  std::string m_instance;

  /** @brief Name of the input btagging objects. */
  std::string m_btaggingKey;

  /** @brief Name of the btagging variable. */
  std::string m_tagger;

  /** @brief to check the beam spot flag status. */
  bool m_useBeamSpotFlag;

  /** @brief Overide the requirement that the BS is valid. */
  /** @brief Used to not apply the correction to the GSC chains */
  bool m_overRideBeamSpotValid;

  float m_cutCounter;

};

#endif


