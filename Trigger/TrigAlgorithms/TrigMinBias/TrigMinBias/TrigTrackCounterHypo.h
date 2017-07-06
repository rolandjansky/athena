/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMINBIAS_TRIGTRACKCOUNTERHYPO_H 
#define TRIGMINBIAS_TRIGTRACKCOUNTERHYPO_H 

#include "TrigInterfaces/HypoAlgo.h"
#include <xAODTrigMinBias/TrigTrackCounts.h>

/** @class TrigTrackCounter

@author Regina Kwee <Regina.Kwee@cern.ch>

A hypothesis algorithm to test a trigger condition based on online low
pt reconstructed track distributions: z0 vs pt and eta vs phi.

*/
class TrigTrackCounterHypo: public HLT::HypoAlgo {
  
 public:
  TrigTrackCounterHypo(const std::string & name, ISvcLocator* pSvcLocator);
  ~TrigTrackCounterHypo();
  
  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);
  
 private:
  /** Hypo selection criteria */
  float m_max_z0;
  float m_min_pt;
  float m_min_eta;
  float m_max_eta;
  unsigned int m_required_ntrks;
  int m_max_required_ntrks; // to select events with only few tracks
  bool m_cutMinAndMax; // Flag to apply both lower and uper boundary requirements

  /** Flag used to bypass hypo selection criteria. */
  bool m_acceptAll;

  /** For monitoring */
  float m_ntrksHypo;
  float m_ntrksSelected;
  
  /** For accessing retrieved data */
  const xAOD::TrigTrackCounts* m_trigTrackCounts;
  
};

#endif
