/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigBphysHypo/TrigL2DiMuXHypo.h
 **
 **   Description: L2 hypothesis algorithms for B+ -> K+ mu+ mu-  
 **                Create TrigL2Bphys
 **
 **   Author: C. Adorisio
 **
 **   Created:   05.10.2007
 **   Modified:  07.03.2008(monitoring histograms added)     
 **
 **************************************************************************/ 

#ifndef TRIG_TrigL2DiMuXHypo_H 
#define TRIG_TrigL2DiMuXHypo_H

#include "GaudiKernel/StatusCode.h"
#include "TrigInterfaces/HypoAlgo.h"

#include "TrigBphysHypo/Constants.h"

class TrigL2DiMuXHypo: public HLT::HypoAlgo  {
  
  public:
    TrigL2DiMuXHypo(const std::string & name, ISvcLocator* pSvcLocator);
    ~TrigL2DiMuXHypo();
    HLT::ErrorCode hltInitialize();
    HLT::ErrorCode hltFinalize();     
    HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);
    
    // private:

    bool m_acceptAll; 
    float m_lowerBplusMassCut;
    float m_upperBplusMassCut;

    unsigned int m_lastEvent;
    unsigned int m_lastEventPassed;
    unsigned int m_countTotalEvents;
    unsigned int m_countTotalRoI;
    unsigned int m_countPassedEvents;
    unsigned int m_countPassedRoIs;
    unsigned int m_countPassedBplusMass;

    TrigTimer* m_TotTimer;

    /* monitored variables*/
    std::vector<float> mon_bmass;
};

#endif
