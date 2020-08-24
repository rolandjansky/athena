/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// *******************************************************************
//
// NAME:     TrigL2BMuMuFex.h
// PACKAGE:  Trigger/TrigHypothesis/TrigBphysHypo
// AUTHOR:   Sergey Sivoklokov
//
// *******************************************************************

#ifndef TRIG_TrigL2BMuMuCombo_H
#define TRIG_TrigL2BMuMuCombo_H

#include <string>

#include "TrigInterfaces/ComboAlgo.h"

#include "TrigInDetToolInterfaces/ITrigL2VertexFitter.h"

//#include "TrigParticle/TrigL2BphysContainer.h"
#include "xAODTrigMuon/L2CombinedMuon.h"
#include "xAODTrigMuon/L2CombinedMuonContainer.h"



#include "TrigTimeAlgs/TrigTimerSvc.h"

class ITrigL2VertexFitter;
class ITrigVertexingTool;

class TrigBphysHelperUtilsTool;

class TrigL2BMuMuFex: public HLT::ComboAlgo {

  public:

    TrigL2BMuMuFex(const std::string & name, ISvcLocator* pSvcLocator);
    ~TrigL2BMuMuFex();

    HLT::ErrorCode hltInitialize();
    HLT::ErrorCode hltFinalize();
    HLT::ErrorCode hltExecute(HLT::TEConstVec& inputTE, HLT::TriggerElement* outputTE);
    HLT::ErrorCode acceptInputs(HLT::TEConstVec& inputTE, bool& pass );  // TODO: move all to hltExecute ?

  private:
    ToolHandle <TrigBphysHelperUtilsTool> m_bphysHelperTool;

    bool isUnique(const  xAOD::TrackParticle* id1, const  xAOD::TrackParticle* id2) const;
    ElementLink<xAOD::TrackParticleContainer> remap_container(const ElementLink<xAOD::TrackParticleContainer> & oldElink,const ElementLinkVector<xAOD::TrackParticleContainer> &newContainer) const;
    //    ElementLink<xAOD::IParticleContainer> remap_container(const ElementLink<xAOD::IParticleContainer> & oldElink,const ElementLinkVector<xAOD::IParticleContainer> &newContainer) const;
    ElementLink<xAOD::IParticleContainer> remap_container(const ElementLink<xAOD::L2CombinedMuonContainer> & oldElink,
                                                          const ElementLinkVector<xAOD::L2CombinedMuonContainer> &newContainer) const;
        
    std::string m_combinedMuonIDTrackKey,m_combinedMuonKey,m_standaloneMuonKey;
    
    // Variables to keep pre-results from acceptInput to hltExecute
    //const CombinedMuonFeature *m_muon1;
    //const CombinedMuonFeature *m_muon2;
    const xAOD::L2CombinedMuon *m_muon1, *m_muon2;
    
    
    // Configurable properties - mass window cuts
    float m_lowerMassCut;
    float m_upperMassCut;

    // Configurable properties - boolean switches
    bool  m_ApplyupperMassCut;
    bool  m_doVertexFit;
    bool  m_acceptAll;
    bool  m_acceptSameMuon;
    bool  m_oppositeCharge;
    bool  m_sameCharge;
    bool  m_noId;

    // Configurable properties - vertexing tools
    ToolHandle<ITrigL2VertexFitter> m_L2vertFitter;
    ToolHandle<ITrigVertexingTool>  m_vertexingTool;

    // Timers
    TrigTimer* m_BmmHypTot;
    TrigTimer* m_BmmHypVtx;

    // Counters
    int m_lastEvent;
    int m_lastEventPassed;
    unsigned int m_countTotalEvents;
    unsigned int m_countTotalRoI;
    unsigned int m_countPassedEvents;
    unsigned int m_countPassedRoIs;
    unsigned int m_countPassedmumuPairs;
    unsigned int m_countPassedBsMass;
    unsigned int m_countPassedVtxFit;

    // Output collections

    std::vector<xAOD::TrigBphys*> m_resultHolder; /// Hold results between accept and execute
    
    // Monitored variables
    std::vector<int>   m_mon_Errors;
    std::vector<int>   m_mon_Acceptance;
    float              m_mon_TotalRunTime;
    std::vector<float> m_mon_ROIEta;
    std::vector<float> m_mon_ROIPhi;
    std::vector<float> m_mon_Roi1Roi2dEta;
    std::vector<float> m_mon_Roi1Roi2dPhi;
    std::vector<float> m_mon_Roi1Roi2dR;
    // - two combined muons
    std::vector<float> m_mon_MucombROIdR;
    std::vector<float> m_mon_MucombTrkdR;
    std::vector<float> m_mon_MucombPt;
    std::vector<float> m_mon_MutrkPt;
    std::vector<float> m_mon_MutrkPt_wideRange;
    std::vector<float> m_mon_MutrkEta;
    std::vector<float> m_mon_MutrkPhi;
    std::vector<float> m_mon_Mutrk1Mutrk2dEta;
    std::vector<float> m_mon_Mutrk1Mutrk2dPhi;
    std::vector<float> m_mon_Mutrk1Mutrk2dR;
    std::vector<float> m_mon_SumPtMutrk12;
    std::vector<float> m_mon_InvMass_comb;
    std::vector<float> m_mon_InvMass_comb_wideRange;
    std::vector<float> m_mon_FitMass;
    std::vector<float> m_mon_FitMass_wideRange;
    std::vector<float> m_mon_InvMass_comb_okFit;
    std::vector<float> m_mon_Chi2toNDoF;
    std::vector<float> m_mon_Chi2toNDoFProb;
    std::vector<float> m_mon_FitTotalPt;
    std::vector<float> m_mon_SumPtMutrk12_okFit;
    std::vector<float> m_mon_FitVtxR;
    std::vector<float> m_mon_FitVtxZ;
    float              m_mon_VertexingTime;
    // - one combined + one standalone muon
    std::vector<float> m_mon_MustandROIdR;
    std::vector<float> m_mon_MustandPt;
    std::vector<float> m_mon_MustandPt_wideRange;
    std::vector<float> m_mon_MustandEta;
    std::vector<float> m_mon_MustandPhi;
    std::vector<float> m_mon_MutrkMustanddEta;
    std::vector<float> m_mon_MutrkMustanddPhi;
    std::vector<float> m_mon_MutrkMustanddR;
    std::vector<float> m_mon_SumPtMutrkMustand;
    std::vector<float> m_mon_InvMass_stand;
    std::vector<float> m_mon_InvMass_stand_wideRange;

    double m_massMuon;

};

#endif
