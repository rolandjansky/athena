/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETPHYSVALMONITORING_INDETPERFPLOT_EXTENDEDFAKES
#define INDETPHYSVALMONITORING_INDETPERFPLOT_EXTENDEDFAKES
/**
 * @file InDetPerfPlot_ExtendedFakes.h
 * @author shaun roe
 **/
// local includes
#include "InDetPerfPlot_fakes.h"

// std includes
#include <string>
#include <map>
#include <vector>

class TProfile;


///class holding fake plots for Inner Detector RTT Validation and implementing fill methods
class InDetPerfPlot_ExtendedFakes: public InDetPerfPlot_fakes {
public:
  InDetPerfPlot_ExtendedFakes (InDetPlotBase* pParent, const std::string& dirName);
  void fill(const xAOD::TrackParticle& trkprt, const bool isFake, const InDetPerfPlot_fakes::Category& f = ALL);
private:
  // fake rates as TProfiles
  std::map< std::string, TProfile* > m_HitInfoFakerates;
  std::map< std::string, TProfile* > m_KinematicFakerates;

  // Variables (for easy access).
  std::vector< std::string > m_variables = {
    "nBLayerHits",
    "nBLayerOutliers",
    "nBLayerSplitHits",
    "nBLayerSharedHits",

    "nPixHits",
    "nPixHoles",
    "nPixSharedHits",
    "nPixOutliers",
    "nPixContribLayers",
    "nPixSplitHits",
    "nPixGangedHits",

    "nSCTHits",
    "nSCTHoles",
    "nSCTDoubleHoles",
    "nSCTSharedHits",
    "nSCTOutliers",

    "nSiHits",

    "nTRTHits",
    "nTRTHighThresholdHits",
    "nTRTOutliers",
    "nTRTHighThresholdOutliers",
  };


  // plot base has nop default implementation of this; we use it to book the histos
  void initializePlots();
};

#endif
