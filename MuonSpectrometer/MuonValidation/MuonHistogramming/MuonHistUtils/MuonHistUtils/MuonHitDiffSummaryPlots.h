/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONHISTUTILS_MUONHITDIFFSUMMARYPLOTS_H
#define MUONHISTUTILS_MUONHITDIFFSUMMARYPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "TrkValHistUtils/HitTypePlots.h"
#include "xAODMuon/Muon.h"
#include "xAODTruth/TruthParticle.h"

namespace Muon{

class MuonHitDiffSummaryPlots:public PlotBase {
    public:
      MuonHitDiffSummaryPlots(PlotBase* pParent, const std::string& sDir);
      void fill(const xAOD::Muon& muon, const xAOD::TruthParticle& truthprt, float weight=1.0);
      void fillPlot(Trk::HitTypePlots& hitPlots, xAOD::MuonSummaryType info, const xAOD::Muon& muon, const std::string& sInfo, const xAOD::TruthParticle& truthprt, float weight=1.0);

      Trk::HitTypePlots innerSmallHits;
      Trk::HitTypePlots innerLargeHits;
      Trk::HitTypePlots middleSmallHits;
      Trk::HitTypePlots middleLargeHits;
      Trk::HitTypePlots outerSmallHits;
      Trk::HitTypePlots outerLargeHits;
      Trk::HitTypePlots extendedSmallHits;
      Trk::HitTypePlots extendedLargeHits;

      Trk::HitTypePlots phiLayer1Hits;
      Trk::HitTypePlots phiLayer2Hits;
      Trk::HitTypePlots phiLayer3Hits;
      Trk::HitTypePlots phiLayer4Hits;

      Trk::HitTypePlots etaLayer1Hits;
      Trk::HitTypePlots etaLayer2Hits;
      Trk::HitTypePlots etaLayer3Hits;
      Trk::HitTypePlots etaLayer4Hits;

};
}

#endif
