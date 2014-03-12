/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SensorBoundsCheck.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKFUTTERITILS_SENSORBOUNDSCHECK_H
#define TRKFUTTERITILS_SENSORBOUNDSCHECK_H

#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkParameters/TrackParameters.h"

namespace Trk {

  class MeasurementBase;

  /** @class SensorBoundsCheck
      @brief Simple class to check if a track fit or prediction is compatible with the
             measurement's active sensor area.

      code is externalised to this class purely for avoiding code duplication.

      @author Wolfgang.Liebig -at- cern.ch
  */
  class SensorBoundsCheck {

  public:

    /** @brief the code snipplet to perform the sensor compatibility check.

    @param[in] MeasurementBase Measurement with a bounded surface (sensor active area)
    @param[in] AmgVector(5) parameters for the position test
    @param[in] AmgSymMatrix(5) covariance for the tolerance to be used in position test
    @param[in] tolerance1 tolerance level in sigma for declaring coordinate 1 'within bounds'
    @param[in] tolerance2 tolerance level in sigma for declaring coordinate 2 'within bounds'
    @param[out] bool true if the sensor test is passed

    If one coordinate is not to be checked, set the tolerance to -1.0
        
    */
    static bool areParamsInside(const Trk::MeasurementBase&,
                                const AmgVector(5)&,
                                const AmgSymMatrix(5)&,
                                const double&,
                                const double& = -1.0);

  private: 
    static  Amg::Vector2D  s_localTrack;
  };
}

#endif
