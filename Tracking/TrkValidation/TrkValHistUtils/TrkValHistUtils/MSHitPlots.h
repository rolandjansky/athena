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
    MSHitPlots(PlotBase *pParent, std::string sDir);
    void fill(const xAOD::TrackParticle& trkprt);
    void fill(const xAOD::TruthParticle& truthprt);
    
    void fillPlot(HitTypePlots& hitPlots, const std::string& sInfo, const xAOD::TruthParticle& truthprt);
    void fillPlot(HitTypePlots& hitPlots, xAOD::SummaryType info, const xAOD::TrackParticle& trkprt);

    Trk::HitTypePlots nprecLayers;
    Trk::HitTypePlots nphiLayers;
    Trk::HitTypePlots ntrigEtaLayers;

};

}

#endif

