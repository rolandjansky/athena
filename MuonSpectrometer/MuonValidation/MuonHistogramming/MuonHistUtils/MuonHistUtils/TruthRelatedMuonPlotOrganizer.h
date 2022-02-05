/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONHISTUTILS_TRUTHRELATEDMUONPLOTORGANIZER_H
#define MUONHISTUTILS_TRUTHRELATEDMUONPLOTORGANIZER_H

#include "TrkValHistUtils/PlotBase.h"
#include "TrkValHistUtils/ParamPlots.h"
#include "TrkValHistUtils/MSHitDiffPlots.h"
#include "MuonHistUtils/MuonHitDiffSummaryPlots.h"
#include "MuonHistUtils/MuonResolutionPlots.h"
#include "MuonHistUtils/MomentumTruthPullPlots.h"
#include "MuonHistUtils/MuonParamElossPlots.h"
#include "MuonHistUtils/MuonTree.h"
#include "MuonHistUtils/MuonTruthHitPlots.h"
#include "TrkValHistUtils/DefParamPullPlots.h"

#include "xAODMuon/Muon.h"
#include "xAODTruth/TruthParticle.h"
#include <vector>

namespace Muon
{
  enum TRUTHRELATEDPLOTCLASS {TRK_MATCHEDTRUE,TRK_MATCHEDRECO,TRK_MSHITDIFF,MUON_HITDIFF,MUON_TRUTHHIT,MUON_RESOL,TRK_DEFPARAMPULLS,MUON_PULLSTAIL,MUON_PULLSNOTAIL,MUON_PARAMELOSS,MAX_TRUTHRELATEDPLOTCLASS};
  
  class TruthRelatedMuonPlotOrganizer:public PlotBase {
  public:
    TruthRelatedMuonPlotOrganizer(PlotBase* pParent, const std::string& sDir,bool doBinnedResolutionPlots, std::vector<int> selPlots = {});
    ~TruthRelatedMuonPlotOrganizer();

    

    std::vector<int> m_selPlots;  
    
    void fill(const xAOD::TruthParticle& truthMu, const xAOD::Muon& mu, const xAOD::TrackParticleContainer* MSTracks, float weight=1.0);
    void fill(const xAOD::TruthParticle& truthMu, const xAOD::TrackParticle& mu, float weight=1.0);
    
    // Truth related plots
    std::unique_ptr<Trk::ParamPlots>                 m_oMatchedPlots{};
    std::unique_ptr<Trk::ParamPlots>                 m_oMatchedRecoPlots{};
    std::unique_ptr<Trk::MSHitDiffPlots>             m_oMSHitDiffPlots{};
    std::unique_ptr<Muon::MuonHitDiffSummaryPlots>   m_oMuonHitDiffSummaryPlots{};
    std::unique_ptr<Muon::MuonTruthHitPlots>         m_oMuonTruthHitPlots{};
    std::unique_ptr<Muon::MuonResolutionPlots>       m_oMuonResolutionPlots{};
    std::unique_ptr<Trk::DefParamPullPlots>          m_oDefParamPullPlots{};
    std::unique_ptr<Muon::MomentumTruthPullPlots>    m_oMomentumTruthPullPlots_Tail{};
    std::unique_ptr<Muon::MomentumTruthPullPlots>    m_oMomentumTruthPullPlots_NoTail{};
    std::unique_ptr<Muon::MuonParamElossPlots>       m_oMatchedRecoElossPlots{};

  };
 
}

#endif
