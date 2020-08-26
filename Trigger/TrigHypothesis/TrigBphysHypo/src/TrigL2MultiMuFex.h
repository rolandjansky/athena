/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// *******************************************************************
//
// NAME:     TrigL2MultiMuFex.h
// PACKAGE:  Trigger/TrigHypothesis/TrigBphysHypo
// AUTHOR:   Sergey Sivoklokov
//
// *******************************************************************

#ifndef TRIG_TrigL2MultiMuCombo_H
#define TRIG_TrigL2MultiMuCombo_H

#include <string>

#include "TrigInterfaces/ComboAlgo.h"

#include "TrigInDetToolInterfaces/ITrigVertexFitter.h"
#include "TrigInDetToolInterfaces/ITrigL2VertexFitter.h"

#include "TrigTimeAlgs/TrigTimerSvc.h"

#include "Constants.h"

#include "xAODTrigBphys/TrigBphysContainer.h"
#include "xAODTrigBphys/TrigBphysAuxContainer.h"
#include "xAODTrigBphys/TrigBphys.h"
#include "xAODTrigMuon/L2CombinedMuon.h"
#include "xAODTrigMuon/L2CombinedMuonContainer.h"


class TriggerElement;
//class CombinedMuonFeature;

class ITrigVertexFitter;
class ITrigL2VertexFitter;
class ITrigVertexingTool;

class TrigBphysHelperUtilsTool;


class TrigL2MultiMuFex: public HLT::ComboAlgo {

  public:

    TrigL2MultiMuFex(const std::string & name, ISvcLocator* pSvcLocator);
    ~TrigL2MultiMuFex();

    HLT::ErrorCode hltInitialize();
    HLT::ErrorCode hltFinalize();
    HLT::ErrorCode hltExecute(HLT::TEConstVec& inputTE, HLT::TriggerElement* outputTE);
    HLT::ErrorCode acceptInputs(HLT::TEConstVec& inputTE, bool& pass );  // TODO: move all to hltExecute ?

  private:
    ToolHandle <TrigBphysHelperUtilsTool> m_bphysHelperTool;

    // Invariant mass helper calculators (TODO: move to InvMass.cxx tool-box)
    //    double invariantMass(const CombinedMuonFeature* mu1, const CombinedMuonFeature* mu2);
    //double invariantMass(const CombinedMuonFeature* mu1, const MuonFeature* mu2);

    void processTriMuon(HLT::TEConstVec& inputTE);
//    double getInvMass2(const CombinedMuonFeature *muon1,const CombinedMuonFeature *muon2);
//    double getInvMass3(const CombinedMuonFeature *muon1,const CombinedMuonFeature *muon2,const CombinedMuonFeature *muon3);
//    TrigL2Bphys* checkInvMass2(const CombinedMuonFeature *muon1,const CombinedMuonFeature *muon2, double Mass);
//    TrigL2Bphys* checkInvMass3(const CombinedMuonFeature *muon1,const CombinedMuonFeature *muon2, const CombinedMuonFeature *muon3, double Mass);

    xAOD::TrigBphys* checkInvMass2(const xAOD::L2CombinedMuon *muon1,const xAOD::L2CombinedMuon *muon2, double Mass);
    xAOD::TrigBphys* checkInvMass3(const xAOD::L2CombinedMuon *muon1,const xAOD::L2CombinedMuon *muon2, const xAOD::L2CombinedMuon *muon3,double Mass);
    
    TrigVertex* fitToVertex(const std::vector<const xAOD::L2CombinedMuon*> & muons);

    // Configurable properties - mass window cuts
    float m_lowerMassCut;
    float m_upperMassCut;

    // Configurable properties - boolean switches
    bool  m_doVertexFit;
    bool  m_acceptAll;
    bool  m_oppositeCharge;
    unsigned int  m_NInputMuon;
    int  m_NMassMuon;
    bool m_checkNinputTE;

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

    bool m_passInvMass;

    // Output collections
    xAOD::TrigBphysContainer* m_trigBphysColl;

    // Monitored variables
    std::vector<int>   m_mon_Errors;
    std::vector<int>   m_mon_Acceptance;
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

    double m_massMuon;
};

#endif
