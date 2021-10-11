/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkValHistUtils/MSHitPlots.h"
#include "xAODTracking/TrackingPrimitives.h"

namespace Trk {
  MSHitPlots::MSHitPlots(PlotBase *pParent, const std::string& sDir) :
    PlotBase(pParent, sDir),
    nprecLayers(this, "nprecLayers", "Precision Layers", 0, 10),
    nphiLayers(this, "nphiLayers", "Phi Layers", 0, 10),
    ntrigEtaLayers(this, "ntrigEtaLayers", "Eta Trigger Layers", 0, 10) {
  }

  void
  MSHitPlots::fill(const xAOD::TrackParticle &trkprt, float weight) {
    fillPlot(nprecLayers, xAOD::numberOfPrecisionLayers, trkprt, weight);
    fillPlot(nphiLayers, xAOD::numberOfPhiLayers, trkprt, weight);
    fillPlot(ntrigEtaLayers, xAOD::numberOfTriggerEtaLayers, trkprt, weight);
  }

  void
  MSHitPlots::fill(const xAOD::TruthParticle &truthprt,float weight) {
    fillPlot(nprecLayers, "nprecLayers", truthprt, weight);
    fillPlot(nphiLayers, "nphiLayers", truthprt, weight);
    fillPlot(ntrigEtaLayers, "ntrigEtaLayers", truthprt, weight);
  }

  void
  MSHitPlots::fillPlot(HitTypePlots &hitPlots, xAOD::SummaryType info, const xAOD::TrackParticle &trkprt, float weight) {
    uint8_t hitval = 0;

    if (!trkprt.summaryValue(hitval, info)) {
      return;
    }
    hitPlots.fill(hitval, trkprt.eta(), trkprt.phi(), weight);
  }

  void
  MSHitPlots::fillPlot(HitTypePlots &hitPlots, const std::string &sInfo, const xAOD::TruthParticle &truthprt, float weight) {
    if (!truthprt.isAvailable<uint8_t>(sInfo)) {
      return;
    }
    hitPlots.fill(truthprt.auxdata<uint8_t>(sInfo), truthprt.eta(), truthprt.phi(), weight);
  }
}
