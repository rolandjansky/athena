/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONHISTUTILS_MUONTREE_H
#define MUONHISTUTILS_MUONTREE_H

#include "TrkValHistUtils/PlotBase.h"

#include "xAODEventInfo/EventInfo.h"
#include "xAODMuon/Muon.h"
#include "xAODTruth/TruthParticle.h"

#include "TTree.h"

#include <vector>

namespace Muon{

  class MuonTree:public PlotBase {
  public:
    MuonTree(PlotBase* pParent, const std::string& sDir, bool mcFlag);
    MuonTree(PlotBase* pParent, const std::string& sDir);
    ~MuonTree();

    void initializePlots();
    void fillEventBranches(const xAOD::EventInfo* eventInfo, bool isData);
    void fillRecoMuonBranches(const xAOD::Muon& mu);
    void fillTruthMuonBranches(const xAOD::TruthParticle &truthMu, bool isGoodTruthTrack);
    void fillTruthMuonBranches(const xAOD::TruthParticle &truthMu, const xAOD::Muon& mu, const xAOD::TrackParticleContainer* MSTracks, bool isGoodTruthTrack);
    void postFillTreeActions();
    TTree* getTree();

  private:

    TTree* m_tree;
    
    //steering flag 
    bool m_isMC;

    // Event info variables
    uint32_t m_runNumber;
    uint32_t m_lumiBlock;
    unsigned long long m_eventNumber;
    uint32_t m_mcChannelNumber;
    Float_t m_mcBeamSpotWeight;
    //
    //Reco variables
    //

    //general properties 
    std::vector<float > m_pt;
    std::vector<float > m_eta;
    std::vector<float > m_phi;
    std::vector<float > m_e;
    std::vector<float > m_rapidity;
    
    //authors and types
    std::vector<uint16_t > m_allAuthors;
    std::vector<int > m_muonType;
   
    //summary values
    std::vector<int8_t > m_innerSmallHits; 	
    std::vector<int8_t > m_innerLargeHits; 	
    std::vector<int8_t > m_innerSmallHoles; 	
    std::vector<int8_t > m_innerLargeHoles; 	

    //parameters
    std::vector<float > m_msInnerMatchChi2;
    std::vector<int >   m_msInnerMatchDOF;
    std::vector<float > m_msOuterMatchChi2;
    std::vector<int >   m_msOuterMatchDOF;
    std::vector<float > m_CaloLRLikelihood;
    std::vector<int >   m_CaloMuonIDTag;
    std::vector<float > m_EnergyLoss;
    
    //quality
    std::vector<int > m_Quality;
    
    //isolation
    std::vector<float > m_iso_etcone20;
    std::vector<float > m_iso_ptcone20;
    std::vector<float > m_iso_topoetcon20;
    std::vector<float > m_iso_ptvarcon20;
    std::vector<float > m_iso_neflowisol20;
    
    //associated TrackParticle
    std::vector<float > m_trkp_d0;
    std::vector<float > m_trkp_z0;
    std::vector<float > m_trkp_qOverP;
    std::vector<float > m_trkp_chiSquared;
    std::vector<float > m_trkp_numberDoF;
    
    //used MuonSegments
    std::vector<size_t > m_nMuonSegments;
    std::vector<std::vector<float> > m_museg_x;
    std::vector<std::vector<float> > m_museg_y;
    std::vector<std::vector<float> > m_museg_z;
    std::vector<std::vector<int> >   m_museg_sector;
    std::vector<std::vector<unsigned int> > m_museg_chamberIndex;
    std::vector<std::vector<unsigned int> > m_museg_technology;
    std::vector<std::vector<int> > m_museg_nPrecisionHits;
    std::vector<std::vector<int> > m_museg_nPhiLayers;
    std::vector<std::vector<int> > m_museg_nTrigEtaLayers;
    
    // 	
    //Truth variables
    //

    //Trk::Param
    // all TruthTrack
    std::vector<bool > m_th_isGoodTruthTrack;
    
    std::vector<double > m_th_pt;
    std::vector<double > m_th_eta;
    std::vector<double > m_th_phi;
    
    //Trk::TruthInfo
    // all TruthTrack
    std::vector<int > m_th_truthType;
    std::vector<int > m_th_truthOrigin;
    
    //Trk::TruthTrkExtrapolation
    // all TruthTrack
    std::vector<float > m_th_CaloEntry_p;
    std::vector<float > m_th_MuonEntry_p;
    std::vector<float > m_th_MuonExit_p;

    //Trk::MSHit
    // all TruthTrack
    std::vector<int8_t > m_th_nprecLayers;
    std::vector<int8_t > m_th_nphiLayers;
    std::vector<int8_t > m_th_ntrigEtaLayers;
  };

}

#endif
