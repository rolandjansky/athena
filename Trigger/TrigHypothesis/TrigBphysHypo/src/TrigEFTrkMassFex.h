/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
// 
// NAME:     TrigEFTrkMassFex.h
// PACKAGE:  Trigger/TrigHypothesis/TrigBphysHypo
// 
// AUTHOR:   Julie Kirk
// 
// ********************************************************************

#ifndef TRIG_TrigEFTrkMassFex_H 
#define TRIG_TrigEFTrkMassFex_H

#include "TrigInterfaces/FexAlgo.h"

#include "xAODMuon/Muon.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTrigBphys/TrigBphys.h"
#include "xAODTrigBphys/TrigBphysContainer.h"



//class TriggerElement;

// forward includes
class TrigBphysHelperUtilsTool;
class TrigTimer;



class TrigEFTrkMassFex: public HLT::FexAlgo {
  
  public:
    TrigEFTrkMassFex(const std::string & name, ISvcLocator* pSvcLocator);
    ~TrigEFTrkMassFex();
    HLT::ErrorCode hltInitialize();
    HLT::ErrorCode hltFinalize();     
    HLT::ErrorCode hltExecute(const HLT::TriggerElement* inputTE, HLT::TriggerElement* outputTE );
    //    HLT::ErrorCode acceptInput(const HLT::TriggerElement* inputTE, bool& pass );

  private:
    
    ToolHandle <TrigBphysHelperUtilsTool> m_bphysHelperTool;
    TrigTimer* m_TotTimer;
    TrigTimer* m_VtxFitTimer;

    void buildMuTrkPairs(const TrigRoiDescriptor * roi,
                         const std::vector<ElementLink<xAOD::MuonContainer> > & muons,
                       const std::vector<ElementLink< xAOD::TrackParticleContainer> > & tracks,
                       xAOD::TrigBphysContainer & physcontainer
                       );
    
    // Properties:
    
    // container inputs names
    std::string m_input_trackCollectionKey; //! list of tracks to search for second leg

    float        m_trackPtthr;
    float        m_muonPtthr;
    float        m_dEta_cut;
    float        m_dPhi_cut;
    float        m_mass_low_cut;
    float        m_mass_high_cut;
    float        m_daughterMass;

    bool         m_doVertexFit;
    bool         m_oppositeCharge;
    bool         m_acceptAll;



  uint32_t m_lastEvent;
  uint32_t m_lastEventPassed;
  unsigned int m_countTotalEvents;
  unsigned int m_countTotalRoI;
  unsigned int m_countPassedEvents;
  unsigned int m_countPassedRoIs;
  unsigned int m_countPassedRoIMatch;
  unsigned int m_countPassedMass;

    std::vector<bool> m_flag_stages;

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
