/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkValHistUtils/MSHitDiffPlots.h"
#include "xAODTracking/TrackingPrimitives.h"
// #include "TrkValHistUtils/TrkValHistUtilities.h"

namespace Trk {
  MSHitDiffPlots::MSHitDiffPlots(PlotBase *pParent, std::string sDir) :
    PlotBase(pParent, sDir),
    nprecLayers(this, "nprecLayers", "Precision Layers", -5, 5),
    nphiLayers(this, "nphiLayers", "Phi Layers", -5, 5),
    ntrigEtaLayers(this, "ntrigEtaLayers", "Eta Trigger Layers", -5, 5) {
  }

  void
  MSHitDiffPlots::fill(const xAOD::TrackParticle &trkprt, const xAOD::TruthParticle &truthprt) {
    fillPlot(nprecLayers, xAOD::numberOfPrecisionLayers, "nprecLayers", trkprt, truthprt);
    fillPlot(nphiLayers, xAOD::numberOfPhiLayers, "nphiLayers", trkprt, truthprt);
    fillPlot(ntrigEtaLayers, xAOD::numberOfTriggerEtaLayers, "ntrigEtaLayers", trkprt, truthprt);
  }

  void
  MSHitDiffPlots::fillPlot(HitTypePlots &hitPlots, const xAOD::SummaryType &info, const std::string &sInfo,
                           const xAOD::TrackParticle &trkprt, const xAOD::TruthParticle &truthprt) {
    uint8_t hitval = 0;

    if (!trkprt.summaryValue(hitval, info)) {
      return;
    }
    if (!truthprt.isAvailable<uint8_t>(sInfo)) {
      return;
    }
    uint8_t truthhitval = truthprt.auxdata<uint8_t>(sInfo);
    hitPlots.fill(truthhitval - hitval, trkprt.eta(), trkprt.phi());
  }
}
