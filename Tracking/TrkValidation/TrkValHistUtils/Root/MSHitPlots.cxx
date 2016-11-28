/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkValHistUtils/MSHitPlots.h"
#include "xAODTracking/TrackingPrimitives.h"

namespace Trk {
  MSHitPlots::MSHitPlots(PlotBase *pParent, std::string sDir) :
    PlotBase(pParent, sDir),
    nprecLayers(this, "nprecLayers", "Precision Layers", 0, 10),
    nphiLayers(this, "nphiLayers", "Phi Layers", 0, 10),
    ntrigEtaLayers(this, "ntrigEtaLayers", "Eta Trigger Layers", 0, 10) {
  }

  void
  MSHitPlots::fill(const xAOD::TrackParticle &trkprt) {
    fillPlot(nprecLayers, xAOD::numberOfPrecisionLayers, trkprt);
    fillPlot(nphiLayers, xAOD::numberOfPhiLayers, trkprt);
    fillPlot(ntrigEtaLayers, xAOD::numberOfTriggerEtaLayers, trkprt);
  }

  void
  MSHitPlots::fill(const xAOD::TruthParticle &truthprt) {
    fillPlot(nprecLayers, "nprecLayers", truthprt);
    fillPlot(nphiLayers, "nphiLayers", truthprt);
    fillPlot(ntrigEtaLayers, "ntrigEtaLayers", truthprt);
  }

  void
  MSHitPlots::fillPlot(HitTypePlots &hitPlots, xAOD::SummaryType info, const xAOD::TrackParticle &trkprt) {
    uint8_t hitval = 0;

    if (!trkprt.summaryValue(hitval, info)) {
      return;
    }
    hitPlots.fill(hitval, trkprt.eta(), trkprt.phi());
  }

  void
  MSHitPlots::fillPlot(HitTypePlots &hitPlots, const std::string &sInfo, const xAOD::TruthParticle &truthprt) {
    if (!truthprt.isAvailable<uint8_t>(sInfo)) {
      return;
    }
    hitPlots.fill(truthprt.auxdata<uint8_t>(sInfo), truthprt.eta(), truthprt.phi());
  }
}
