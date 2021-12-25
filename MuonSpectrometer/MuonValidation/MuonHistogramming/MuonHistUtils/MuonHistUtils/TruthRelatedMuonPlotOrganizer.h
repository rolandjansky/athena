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
    TruthRelatedMuonPlotOrganizer(PlotBase* pParent, const std::string& sDir,bool doBinnedResolutionPlots, std::vector<int> *selPlots=0);
    ~TruthRelatedMuonPlotOrganizer();

    
    std::vector<PlotBase*> m_allPlots;
    std::vector<int> m_selPlots;  
    
    void fill(const xAOD::TruthParticle& truthMu, const xAOD::Muon& mu, const xAOD::TrackParticleContainer* MSTracks, float weight=1.0);
    void fill(const xAOD::TruthParticle& truthMu, const xAOD::TrackParticle& mu, float weight=1.0);
    
    // Truth related plots
    Trk::ParamPlots                 *m_oMatchedPlots;
    Trk::ParamPlots                 *m_oMatchedRecoPlots;
    Trk::MSHitDiffPlots             *m_oMSHitDiffPlots;
    Muon::MuonHitDiffSummaryPlots   *m_oMuonHitDiffSummaryPlots;
    Muon::MuonTruthHitPlots         *m_oMuonTruthHitPlots;
    Muon::MuonResolutionPlots       *m_oMuonResolutionPlots;
    Trk::DefParamPullPlots          *m_oDefParamPullPlots;
    Muon::MomentumTruthPullPlots    *m_oMomentumTruthPullPlots_Tail;
    Muon::MomentumTruthPullPlots    *m_oMomentumTruthPullPlots_NoTail;
    Muon::MuonParamElossPlots       *m_oMatchedRecoElossPlots;

  };
 
}

#endif
