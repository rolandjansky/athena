/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   KalmanParameterUpdator.h
 * @date   31/07/2020
 * @author Anthony Morley, Christos Anastopoulos
 *
 * Implementation of the gain formalist for Kalman filters
 * allowing updating of parameters.
 *
 * Started from/based on KalmanUpdatorAmg
 */

#ifndef TRK_KALMANPARAMETERUPDATOR_H
#define TRK_KALMANPARAMETERUPDATOR_H

#include "EventPrimitives/EventPrimitives.h"
#include "TrkEventPrimitives/FitQualityOnSurface.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkEventPrimitives/ProjectionMatricesSet.h"
#include "TrkParameters/TrackParameters.h"

namespace Trk {

namespace KalmanParameterUpdator {
/// calling the appropriate implementation for this rank of the measurement
/// perform a filter step.
/// Updates (in place) the current TrackParameters and FitQualityOnSurface
/// to represent the predicted state containing
/// the measurement LocalParameters and covariance.
/// For prediction sign should be 1 for smoothing -1.
bool
filterStep(TrackParameters& trackParameters,
           FitQualityOnSurface& fitQos,
           const LocalParameters& measurement,
           const Amg::MatrixX& measCovariance,
           const int sign);

bool
stateFitQuality(FitQualityOnSurface& updatedFitQoS,
                const TrackParameters& trkPar,
                const LocalParameters& position,
                const Amg::MatrixX& covariance,
                int predFull);
} // end of namespace KalmanParameterUpdator

} // end of namespace Trk

#endif // TRK_KALMANPARAMETERUPDATOR_H
