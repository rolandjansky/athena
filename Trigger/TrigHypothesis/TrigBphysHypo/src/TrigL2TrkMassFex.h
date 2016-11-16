/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// *******************************************************************
//
// NAME:     TrigL2TrkMassFex.h
// PACKAGE:  Trigger/TrigHypothesis/TrigBphysHypo
// AUTHOR:   Julie Kirk
//
// *******************************************************************

#ifndef TRIG_TrigL2TrkMassFex_H
#define TRIG_TrigL2TrkMassFex_H

#include <string>

#include "GaudiKernel/ToolHandle.h"

#include "TrigInterfaces/FexAlgo.h"

#include "TrigInDetEvent/TrigVertexCollection.h"
#include "TrigInDetToolInterfaces/ITrigVertexFitter.h"
#include "TrigInDetToolInterfaces/ITrigL2VertexFitter.h"

#include "xAODTrigBphys/TrigBphysContainer.h"
#include "xAODTrigBphys/TrigBphysAuxContainer.h"
#include "xAODTrigBphys/TrigBphys.h"

#include "TrigTimeAlgs/TrigTimerSvc.h"

class TriggerElement;

class ITrigVertexFitter;
class ITrigL2VertexFitter;
class ITrigVertexingTool;

class TrigBphysHelperUtilsTool;


class TrigL2TrkMassFex: public HLT::FexAlgo {

  public:

    TrigL2TrkMassFex(const std::string & name, ISvcLocator* pSvcLocator);
    ~TrigL2TrkMassFex();

    HLT::ErrorCode hltInitialize();
    HLT::ErrorCode hltFinalize();
    HLT::ErrorCode hltExecute(const HLT::TriggerElement* inputTE, HLT::TriggerElement* outputTE);
    // HLT::ErrorCode acceptInput(const HLT::TriggerElement* inputTE, bool& pass );

  private:
    ToolHandle <TrigBphysHelperUtilsTool> m_bphysHelperTool;

    // container inputs names
    std::string m_input_trackCollectionKey; //! list of tracks to search for second leg
    
    // Configurable properties - cuts
    float m_matchL1;
    float m_trackPtthr;
    float m_muonPtthr;
    float m_dEta_cut;
    float m_dPhi_cut;
    float m_daughterMass;
    float m_mass_low_cut;
    float m_mass_high_cut;

    // Configurable properties - boolean switches
    bool  m_doVertexFit;
    bool  m_acceptAll;
    bool  m_oppositeCharge;

    // Configurable properties - vertexing tools
    ToolHandle<ITrigVertexFitter>   m_vertFitter;
    ToolHandle<ITrigL2VertexFitter> m_L2vertFitter;
    ToolHandle<ITrigVertexingTool>  m_vertexingTool;

    // Timers
    TrigTimer* m_TotTimer;
    TrigTimer* m_VtxFitTimer;

    // Counters
    unsigned int m_lastEvent;
    unsigned int m_lastEventPassed;
    unsigned int m_countTotalEvents;
    unsigned int m_countTotalRoI;
    unsigned int m_countPassedEvents;
    unsigned int m_countPassedRoIs;
    unsigned int m_countPassedRoIMatch;
    unsigned int m_countPassedMass;

    // Output collections
    xAOD::TrigBphysContainer* m_trigBphysColl;
    TrigVertexCollection* m_VertexColl;

    // Monitored variables
    std::vector<int>   m_mon_Errors;
    std::vector<int>   m_mon_Acceptance;
    std::vector<float> m_mon_ROIEta;
    std::vector<float> m_mon_ROIPhi;
    int                m_mon_nTracks;
    std::vector<float> m_mon_TrkPt;
    std::vector<float> m_mon_TrkPt_wideRange;
    std::vector<float> m_mon_TrkEta;
    std::vector<float> m_mon_TrkPhi;
    std::vector<float> m_mon_TrkROIdEta;
    std::vector<float> m_mon_TrkROIdPhi;
    std::vector<float> m_mon_TrkROIdR;
    std::vector<float> m_mon_InvMassNoTrkPtCut;
    std::vector<float> m_mon_InvMassNoTrkPtCut_wideRange;
    std::vector<float> m_mon_InvMass;
    std::vector<float> m_mon_InvMass_wideRange;
    std::vector<float> m_mon_Trk1Pt;
    std::vector<float> m_mon_Trk2Pt;
    std::vector<float> m_mon_Trk1Eta;
    std::vector<float> m_mon_Trk2Eta;
    std::vector<float> m_mon_Trk1Phi;
    std::vector<float> m_mon_Trk2Phi;
    std::vector<float> m_mon_Trk1Trk2dEta;
    std::vector<float> m_mon_Trk1Trk2dPhi;
    std::vector<float> m_mon_Trk1Trk2dR;
    std::vector<float> m_mon_SumPtTrk12;
    std::vector<float> m_mon_FitMass;
    std::vector<float> m_mon_FitMass_wideRange;
    std::vector<float> m_mon_InvMass_okFit;
    std::vector<float> m_mon_Chi2toNDoF;
    std::vector<float> m_mon_Chi2toNDoFProb;
    std::vector<float> m_mon_FitTotalPt;
    std::vector<float> m_mon_SumPtTrk12_okFit;
    std::vector<float> m_mon_FitVtxR;
    std::vector<float> m_mon_FitVtxZ;
    int                m_mon_nBphys;
    float              m_mon_TotalRunTime;
    float              m_mon_VertexingTime;

};

#endif
