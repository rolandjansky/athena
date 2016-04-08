/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MeanTrackParsAtFloor.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef INDET__MEANTRACKPARSATFLOOR_H
#define INDET__MEANTRACKPARSATFLOOR_H

#include "TrkParameters/TrackParameters.h"
#include "TrkMaterialOnTrack/EstimatedBremOnTrack.h"
#include "TrkFitterUtils/DNA_MaterialEffects.h"

namespace Trk {

  /** @brief Helper class to manage averaging of several track and brem parameters
      into floors, which are defined such that differences between consecutive
      MeanTrackParametersAtFloor are sensitive to brem.

      Designed for giving structure to internally used set of variables,
      hence this class is part of a component lib and all members are public.

      @author VK, WL
  */

  class MeanTrackParsAtFloor {
  public:
    MeanTrackParsAtFloor();
    void addTrackParsToFloor(const Trk::TrackParameters&, 
                             const Trk::DNA_MaterialEffects*,
                             const Trk::EstimatedBremOnTrack*);
    int    nen;
    //        int    numberMeasPerFloor() {return nen;};
    double ras;
    //        double meanRadiusAtFloor() {return ras;};
    double phs;
    //        double measuredPhiAtFloor() {return phs;};
    double ths;
    //        double measuredThetaAtFloor() {return ths;};
    double pts;
    //        double measuredPtSmoothedAtFloor() {return pts;};
    double dps;
    //        double errorPtSmoothedAtFloor() {return dps;};
    int    nnf;
    double dnf;
    int    nnb;
    double dnb;
/*  double ich,bin,rat,los ;*/
  };
}
#endif

