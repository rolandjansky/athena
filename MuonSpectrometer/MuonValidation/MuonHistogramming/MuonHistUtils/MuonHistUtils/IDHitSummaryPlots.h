/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONHISTUTILS_IDHITSUMMARYPLOTS_H
#define MUONHISTUTILS_IDHITSUMMARYPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "TrkValHistUtils/HitTypePlots.h"
#include "xAODTracking/TrackParticle.h"

namespace Muon{

class HitFracTypePlots:public PlotBase {
 public:
  HitFracTypePlots(PlotBase* pParent, std::string sHitType, std::string sHitLabel);
  void fill(float hitval, float trketa, float weight=1.0);

  TH1* fracHits{nullptr};
  TH1* fracHitsVsEta{nullptr};
 private:
  void initializePlots();
  std::string m_sHitType;
  std::string m_sHitLabel;
};
    
class IDHitSummaryPlots:public PlotBase {
 public:
  IDHitSummaryPlots(PlotBase* pParent, const std::string& sDir);
  void fill(const xAOD::TrackParticle& trk,float weight=1.0);

  Trk::HitTypePlots nBLayerHitsIfExpected;
  Trk::HitTypePlots nPixelHitsPlusDead;
  Trk::HitTypePlots nSCTHitsPlusDead;
  Trk::HitTypePlots nTRTHitsPlusDead;
  Trk::HitTypePlots nTRTHitsPlusOutliers;
  Trk::HitTypePlots nPixSCTHoles;
  
  HitFracTypePlots fPixelOutliers;
  HitFracTypePlots fSCTOutliers;
  HitFracTypePlots fTRTOutliers;

};
 
}

#endif
