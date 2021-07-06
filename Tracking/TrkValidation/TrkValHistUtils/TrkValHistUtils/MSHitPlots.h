/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKVALHISTUTILS_MSHITPLOTS_H
#define TRKVALHISTUTILS_MSHITPLOTS_H

#include "PlotBase.h"
#include "HitTypePlots.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTruth/TruthParticle.h"

namespace Trk{

class MSHitPlots: public PlotBase {
  public:
    MSHitPlots(PlotBase *pParent, const std::string& sDir);
    void fill(const xAOD::TrackParticle& trkprt, float weight=1.0);
    void fill(const xAOD::TruthParticle& truthprt, float weight=1.0);
    
    void fillPlot(HitTypePlots& hitPlots, const std::string& sInfo, const xAOD::TruthParticle& truthprt, float weight=1.0);
    void fillPlot(HitTypePlots& hitPlots, xAOD::SummaryType info, const xAOD::TrackParticle& trkprt, float weight=1.0);

    Trk::HitTypePlots nprecLayers;
    Trk::HitTypePlots nphiLayers;
    Trk::HitTypePlots ntrigEtaLayers;

};

}

#endif

