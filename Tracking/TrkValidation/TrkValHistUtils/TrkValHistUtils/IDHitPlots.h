/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKVALHISTUTILS_IDHITPLOTS_H
#define TRKVALHISTUTILS_IDHITPLOTS_H

#include "PlotBase.h"
#include "xAODTracking/TrackParticleFwd.h"

namespace Trk{

class IDHitPlots: public PlotBase {
  public:
    IDHitPlots(PlotBase *pParent, std::string sDir):PlotBase(pParent, sDir){ init();}
    void fill(const xAOD::TrackParticle& trk);

    ///@name Basic hit content of xAOD::TrackParticle
    ///@{
    TH1* nBLayerHits;
    TH1* nPixelHits;
    TH1* nPixelHoles;
    TH1* nSCTHits;
    TH1* nSCTHoles;
    TH1* nTRTHits;
    TH1* nTRTHighThresholdHits;
    ///@}

    ///@name Expert hit content of xAOD::TrackParticle
    ///@{
    TH1* nBLayerOutliers;
    TH1* nBLayerSharedHits;
    TH1* nBLayerSplitHits;
    TH1* nPixelOutliers;
    TH1* nPixelContribLayers;
    TH1* nPixelSharedHits;
    TH1* nPixelSplitHits;
    TH1* nPixelGangedHits;
    TH1* nSCTOutliers;
    TH1* nSCTDoubleHoles;
    TH1* nSCTSharedHits;
    TH1* nTRTOutliers;
    TH1* nTRTHighThresholdOutliers;
    TH1* nPixelGangedHitsFlaggedFakes;
    ///@}

  private:
    void init();
    void initializePlots();
};

}

#endif

