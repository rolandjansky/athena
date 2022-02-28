/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONHISTUTILS_MUONHITSUMMARYPLOTS_H
#define MUONHISTUTILS_MUONHITSUMMARYPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "TrkValHistUtils/HitTypePlots.h"
#include "TrkValHistUtils/MSHitPlots.h"
#include "MuonHistUtils/IDHitSummaryPlots.h"
#include "xAODMuon/Muon.h"
#include "xAODTruth/TruthParticle.h"

namespace Muon{
  
class MuonHitSummaryPlots:public PlotBase {
 public:
  MuonHitSummaryPlots(PlotBase* pParent, const std::string& sDir);  
  void fill(const xAOD::Muon& muon, float weight=1.0);
  void fill(const xAOD::TruthParticle& truthprt, float weight=1.0);
  void fillPlot(Trk::HitTypePlots& hitPlots, xAOD::MuonSummaryType info, const xAOD::Muon& muon, float weight=1.0);
  void fillPlot(Trk::HitTypePlots& hitPlots, const std::string& sInfo, const xAOD::TruthParticle& truthprt, float weight=1.0);

  Trk::MSHitPlots m_oMSHitPlots;
  
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

  Trk::HitTypePlots innerSmallHoles;
  Trk::HitTypePlots innerLargeHoles;
  Trk::HitTypePlots middleSmallHoles;
  Trk::HitTypePlots middleLargeHoles;
  Trk::HitTypePlots outerSmallHoles;
  Trk::HitTypePlots outerLargeHoles;
  Trk::HitTypePlots extendedSmallHoles;
  Trk::HitTypePlots extendedLargeHoles;

  Trk::HitTypePlots phiHoles;
  Trk::HitTypePlots etaHoles;
  Trk::HitTypePlots etaphiHoles;
  
  /* Trk::HitTypePlots phiLayer1Holes; */
  /* Trk::HitTypePlots phiLayer2Holes; */
  /* Trk::HitTypePlots phiLayer3Holes; */
  /* Trk::HitTypePlots phiLayer4Holes; */

  /* Trk::HitTypePlots etaLayer1Holes; */
  /* Trk::HitTypePlots etaLayer2Holes; */
  /* Trk::HitTypePlots etaLayer3Holes; */
  /* Trk::HitTypePlots etaLayer4Holes; */

};
}

#endif
