/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonHistUtils/MuonHitSummaryPlots.h"
#include "xAODTracking/TrackingPrimitives.h"

namespace Muon{

MuonHitSummaryPlots::MuonHitSummaryPlots(PlotBase* pParent, std::string sDir):
PlotBase(pParent, sDir), 
innerSmallHits(this, "innersmallhits", "inner small hits", 0, 40),
innerLargeHits(this, "innerlargehits", "inner large hits", 0, 40),
middleSmallHits(this, "middlesmallhits", "middle small hits", 0, 40),
middleLargeHits(this, "middlelargehits", "middle large hits", 0, 40),
outerSmallHits(this, "outersmallhits", "outer small hits", 0, 40),
outerLargeHits(this, "outerlargehits", "outer large hits", 0, 40),
extendedSmallHits(this, "extendedsmallhits", "extended small hits", 0, 40),
extendedLargeHits(this, "extendedlargehits", "extended large hits", 0, 40),

phiLayer1Hits(this, "phiLayer1hits", "phi Layer 1 hits", 0, 20),
phiLayer2Hits(this, "phiLayer2hits", "phi Layer 2 hits", 0, 20),
phiLayer3Hits(this, "phiLayer3hits", "phi Layer 3 hits", 0, 20),
phiLayer4Hits(this, "phiLayer4hits", "phi Layer 4 hits", 0, 20),

etaLayer1Hits(this, "etaLayer1hits", "eta Layer 1 hits", 0, 20),
etaLayer2Hits(this, "etaLayer2hits", "eta Layer 2 hits", 0, 20),
etaLayer3Hits(this, "etaLayer3hits", "eta Layer 3 hits", 0, 20),
etaLayer4Hits(this, "etaLayer4hits", "eta Layer 4 hits", 0, 20)
{  
}	

void MuonHitSummaryPlots::fill(const xAOD::Muon& muon){
  fillPlot(innerSmallHits, xAOD::innerSmallHits, muon);
  fillPlot(innerLargeHits, xAOD::innerLargeHits, muon);
  fillPlot(middleSmallHits, xAOD::middleSmallHits, muon);
  fillPlot(middleLargeHits, xAOD::middleLargeHits, muon);
  fillPlot(outerSmallHits, xAOD::outerSmallHits, muon);
  fillPlot(outerLargeHits, xAOD::outerLargeHits, muon);
  fillPlot(extendedSmallHits, xAOD::extendedSmallHits, muon);
  fillPlot(extendedLargeHits, xAOD::extendedLargeHits, muon);
  fillPlot(phiLayer1Hits, xAOD::phiLayer1Hits, muon);
  fillPlot(phiLayer2Hits, xAOD::phiLayer2Hits, muon);
  fillPlot(phiLayer3Hits, xAOD::phiLayer3Hits, muon);
  fillPlot(phiLayer4Hits, xAOD::phiLayer4Hits, muon);
  fillPlot(etaLayer1Hits, xAOD::etaLayer1Hits, muon);
  fillPlot(etaLayer2Hits, xAOD::etaLayer2Hits, muon);
  fillPlot(etaLayer3Hits, xAOD::etaLayer3Hits, muon);
  fillPlot(etaLayer4Hits, xAOD::etaLayer4Hits, muon);
}

void MuonHitSummaryPlots::fill(const xAOD::TruthParticle& truthprt){
  fillPlot(innerSmallHits, "innerSmallHits", truthprt);
  fillPlot(innerLargeHits, "innerLargeHits", truthprt);
  fillPlot(middleSmallHits, "middleSmallHits", truthprt);
  fillPlot(middleLargeHits, "middleLargeHits", truthprt);
  fillPlot(outerSmallHits, "outerSmallHits", truthprt);
  fillPlot(outerLargeHits, "outerLargeHits", truthprt);
  fillPlot(extendedSmallHits, "extendedSmallHits", truthprt);
  fillPlot(extendedLargeHits, "extendedLargeHits", truthprt);
  fillPlot(phiLayer1Hits, "phiLayer1Hits", truthprt);
  fillPlot(phiLayer2Hits, "phiLayer2Hits", truthprt);
  fillPlot(phiLayer3Hits, "phiLayer3Hits", truthprt);
  fillPlot(phiLayer4Hits, "phiLayer4Hits", truthprt);
  fillPlot(etaLayer1Hits, "etaLayer1Hits", truthprt);
  fillPlot(etaLayer2Hits, "etaLayer2Hits", truthprt);
  fillPlot(etaLayer3Hits, "etaLayer3Hits", truthprt);
  fillPlot(etaLayer4Hits, "etaLayer4Hits", truthprt);
}

void MuonHitSummaryPlots::fillPlot(Trk::HitTypePlots& hitPlots, xAOD::MuonSummaryType info, const xAOD::Muon& muon){
  uint8_t hitval = 0;
  if (!muon.summaryValue(hitval,info)) return;
  hitPlots.fill(hitval, muon.eta(), muon.phi());
}

void MuonHitSummaryPlots::fillPlot(Trk::HitTypePlots& hitPlots, const std::string& sInfo, const xAOD::TruthParticle& truthprt){
  if (!truthprt.isAvailable<uint8_t>(sInfo)) return;
  hitPlots.fill(truthprt.auxdata<uint8_t>(sInfo), truthprt.eta(), truthprt.phi());
}


}
