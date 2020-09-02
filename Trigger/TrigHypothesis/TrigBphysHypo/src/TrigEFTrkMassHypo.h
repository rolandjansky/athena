/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRIGEFTRKMASSHYPO_H
#define TRIGEFTRKMASSHYPO_H

#include "TrigInterfaces/HypoAlgo.h"

class TriggerElement;

class TrigEFTrkMassHypo: public HLT::HypoAlgo
{

public:
    TrigEFTrkMassHypo(const std::string& name, ISvcLocator* pSvcLocator);
    ~TrigEFTrkMassHypo();

    HLT::ErrorCode hltInitialize();
    HLT::ErrorCode hltFinalize();
    HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);
	
    bool execHLTAlgorithm(TriggerElement* );

private:
    
    BooleanProperty m_acceptAll;

    int m_lastEvent;
    int m_lastEventPassed;
    unsigned int m_countTotalEvents;
    unsigned int m_countTotalRoI;
    unsigned int m_countPassedEvents;
    unsigned int m_countPassedRoIs;
    void handle(const Incident &); 

    /* monitored variables */
    int m_mon_cutCounter;
    int m_mon_NBphys;
    //std::vector<float> m_mon_Mass;
    //std::vector<float> m_mon_FitMass;
    //std::vector<float> m_mon_Chi2;
    //std::vector<float> m_mon_Chi2Prob;
    
};

#endif 
