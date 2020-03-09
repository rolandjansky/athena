/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// LocalParameters.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkFitterUtils/SensorBoundsCheck.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkSurfaces/Surface.h"
#include "TrkSurfaces/TrapezoidBounds.h"


bool Trk::SensorBoundsCheck::areParamsInside
(const Trk::MeasurementBase& sensor,
 const AmgVector(5)&         positionPars,
 const AmgSymMatrix(5)&      positionCov,
 const double&               tolerance1,
 const double&               tolerance2) {

  // get the tolerances from error of the current track parameters
  // if (positionPars.covariance() == NULL) return true;
  double err1 = sqrt(positionCov(loc1,loc1));
  double err2 = sqrt(positionCov(loc2,loc2));
  double sig1 = err1 * tolerance1;
  double sig2 = (tolerance2>=0 ? err2 * tolerance2 : err2 * tolerance1);

  const Surface& surf  = sensor.associatedSurface();
  const Amg::Vector2D localTrack(positionPars(Trk::loc1),positionPars(Trk::loc2) );
  return surf.insideBounds(localTrack,sig1,sig2);
}

