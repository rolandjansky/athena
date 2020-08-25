/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigBphysHypo/TrigEFMultiMuHypo.h
 **
 **   Description: EF hypothesis algorithms for 3 and 4 mu
 **                currently a dummy algorithm
 **
 **   Author: J.Kirk
 **
 **   Created:   12.09.07
 **   Modified:     
 **
 **************************************************************************/ 

#ifndef TRIG_TrigEFMultiMuHypo_H 
#define TRIG_TrigEFMultiMuHypo_H

// standard stuff
#include <string>
// general athena stuff
#include "TrigInterfaces/HypoAlgo.h"


class TrigEFMultiMuHypo: public HLT::HypoAlgo  {
  
  public:
    TrigEFMultiMuHypo(const std::string & name, ISvcLocator* pSvcLocator);
    ~TrigEFMultiMuHypo();
    HLT::ErrorCode hltInitialize();
    //StatusCode execute();
    HLT::ErrorCode hltFinalize();     
    HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);
    
  private:


    // Mass window cuts
    bool m_oppositeCharge;
    float m_lowerMassCut;
    float m_upperMassCut;
    bool m_ApplyupperMassCut;
    bool m_ApplyChi2Cut;
    float m_Chi2VtxCut;

    // to set Accept-All mode: should be done with force-accept when possible
    bool m_acceptAll;
    std::string m_bphysCollectionKey;


  //Counters
  int m_lastEvent;
  int m_lastEventPassed;
  unsigned int m_countTotalEvents;
  unsigned int m_countTotalRoI;
  unsigned int m_countPassedEvents;
  unsigned int m_countPassedRoIs;
  unsigned int m_countPassedBsMass;
  unsigned int m_countPassedChi2Cut;


  int m_mon_cutCounter;
  double m_mon_MuMumass;
  double m_mon_FitChi2;
  
};

#endif
