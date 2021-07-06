/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKVALHISTUTILS_MSHITDIFFPLOTS_H
#define TRKVALHISTUTILS_MSHITDIFFPLOTS_H

#include "PlotBase.h"
#include "HitTypePlots.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTruth/TruthParticle.h"

namespace Trk{

class MSHitDiffPlots: public PlotBase {
  public:
    MSHitDiffPlots(PlotBase *pParent, const std::string& sDir);
    void fill(const xAOD::TrackParticle& trkprt, const xAOD::TruthParticle& truthprt, float weight=1.0);
    void fillPlot(HitTypePlots& hitPlots, const xAOD::SummaryType& info, const std::string& sInfo, const xAOD::TrackParticle& trkprt, const xAOD::TruthParticle& truthprt, float weight=1.0);

    Trk::HitTypePlots nprecLayers;
    Trk::HitTypePlots nphiLayers;
    Trk::HitTypePlots ntrigEtaLayers;

};

}

#endif

