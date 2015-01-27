/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

 /**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigBphysHypo/TrigL2DiMuXFex.h
 **
 **   Description: L2 hypothesis algorithms for B+ -> K+ mu+ mu-  
 **                Create TrigL2Bphys
 **
 **   Author: C. Adorisio (Cristina.Adorisio@cern.ch)
 **
 **   Created:   08.10.2007
 **   Modified:  25.02.2008(new TrigDiMuon class)
 **              07.03.2008(monitoring histograms added)
 **
 **************************************************************************/ 

#ifndef TRIG_TrigL2DiMuXFex_H 
#define TRIG_TrigL2DiMuXFex_H

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrigInterfaces/FexAlgo.h"

#include "TrigInDetEvent/TrigInDetTrackFitPar.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"

#include "TrigParticle/TrigL2Bphys.h"
#include "TrigParticle/TrigL2BphysContainer.h"
#include "TrigInDetEvent/TrigVertexCollection.h"

#include "TrigInDetToolInterfaces/ITrigL2VertexFitter.h"
#include "TrigInDetToolInterfaces/ITrigVertexingTool.h"
#include "TrigInDetToolInterfaces/ITrigVertexFitter.h"

#include "TrigBphysHypo/Constants.h"
#include "xAODTrigBphys/TrigBphysContainer.h"

class TrigBphysHelperUtilsTool;

class TrigL2DiMuXFex: public HLT::FexAlgo  
{
  public:
    TrigL2DiMuXFex(const std::string & name, ISvcLocator* pSvcLocator);
    ~TrigL2DiMuXFex();
    HLT::ErrorCode hltInitialize();
    HLT::ErrorCode hltFinalize();     
    HLT::ErrorCode hltExecute(const HLT::TriggerElement* inputTE, HLT::TriggerElement* outputTE);
            
 private:
    ToolHandle <TrigBphysHelperUtilsTool> m_bphysHelperTool;

    // to set Accept-All mode: should be done with force-accept when possible
    BooleanProperty m_acceptAll;
    BooleanProperty m_doVertexFit;

    // Mass window cuts
    float m_lowerMuMuMassCut;
    float m_upperMuMuMassCut;
    float m_lowerKplusMuMuMassCut;
    float m_upperKplusMuMuMassCut;

    //Counters
    int m_lastEvent;
    int m_lastEvent_DiMu;
    unsigned int m_lastEventPassed;
    unsigned int m_lastEventPassedMuMu;
    unsigned int m_lastEventPassedKMuMu;

    unsigned int m_countTotalEvents;
    unsigned int m_countTotalRoI;
    unsigned int m_countTotalEvents_DiMu;
    unsigned int m_countTotalRoI_DiMu;

    unsigned int m_countPassedEvents;
    unsigned int m_countPassedRoIs;
    unsigned int m_countPassedEventsMuMu;
    unsigned int m_countPassedEventsKMuMu;

    unsigned int m_countPassedMuMuMassCut;
    unsigned int m_countPassedKMuMuMassCut;
    unsigned int m_countPassedKMuMuVertexCut;

    xAOD::TrigBphysContainer* m_trigBphysColl;
    //TrigVertexCollection* m_VertexColl;

    ToolHandle<ITrigL2VertexFitter> m_L2vertFitter;
    ToolHandle<ITrigVertexingTool> m_vertexingTool;

    TrigTimer* m_TotTimer;
    TrigTimer* m_VtxFitTimer;

    /* monitored variables*/
    std::vector<float> mon_dimumass;
    std::vector<float> mon_kdimumass;
};

#endif
