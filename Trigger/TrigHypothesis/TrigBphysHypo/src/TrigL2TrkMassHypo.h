/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRIGL2TRKMASSHYPO_H
#define TRIGL2TRKMASSHYPO_H

#include "TrigInterfaces/HypoAlgo.h"
#include "TrigT1Interfaces/RecMuonRoI.h" 
#include "GaudiKernel/IIncidentListener.h"

#include "TrigBphysHypo/Constants.h"

class TriggerElement;
class TrigBphysHelperUtilsTool;

class TrigL2TrkMassHypo: public HLT::HypoAlgo
{

public:
    TrigL2TrkMassHypo(const std::string& name, ISvcLocator* pSvcLocator);
    ~TrigL2TrkMassHypo();

    HLT::ErrorCode hltInitialize();
    HLT::ErrorCode hltFinalize();
    HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);
	
    bool execHLTAlgorithm(TriggerElement* );

private:
    ToolHandle <TrigBphysHelperUtilsTool> m_bphysHelperTool;

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
    std::vector<float> m_mon_Mass;
    //std::vector<float> m_mon_FitMass;
    //std::vector<float> m_mon_Chi2;
    //std::vector<float> m_mon_Chi2Prob;

};

#endif 
