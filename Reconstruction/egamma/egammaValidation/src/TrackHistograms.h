/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAVALIDATION_TRACKHISTOGRAMS_H
#define EGAMMAVALIDATION_TRACKHISTOGRAMS_H

#include <utility>

#include "xAODTracking/TrackParticle.h"
#include "TProfile.h"
#include "IHistograms.h"

namespace egammaMonitoring{

  class TrackHistograms : public IHistograms {
  public:

    using IHistograms::IHistograms;
    
//    std::map<std::string, TH1D* > histoMap;
    std::map<std::string, TProfile* > profileMap;

    StatusCode initializePlots();
    void fill(const xAOD::IParticle& track, float mu);
    void fill(const xAOD::IParticle& track);

  private:
    int getNumberOfHits(const xAOD::TrackParticle*, xAOD::SummaryType);

  };

}

#endif
