/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************************
                        PosteriorWeightsCalculator.h  -  description
                        ---------------------------------------------
created              : Wednesday 22nd December 2004
author               : atkinson
email                : Tom.Atkinson@cern.ch
description          : Class to calculate the weighting of state components
                        after an update from a measurement. The information for
                        the measurement is provided through the RIO_OnTrack
                        object.
*********************************************************************************/

#ifndef PosteriorWeightsCalculator_H
#define PosteriorWeightsCalculator_H

#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkEventPrimitives/ProjectionMatricesSet.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkMultiComponentStateOnSurface/MultiComponentState.h"
#include "TrkParameters/TrackParameters.h"

namespace Trk {

class PosteriorWeightsCalculator
{
public:
  std::vector<Trk::ComponentParameters> weights(MultiComponentState&&,
                                                const MeasurementBase&) const;

private:
  /** Function to calculate the determinant and  chi2 of a measurement for a 1D
   * hit */
  std::pair<double, double> calculateWeight_1D(
    const TrackParameters* componentTrackParameters,
    const AmgSymMatrix(5) * predictedCov,
    const double measPar,
    const double measCov,
    int paramKey) const;

  /** Function to calculate the determinant and  chi2 of a measurement for a 2D
     hit specially when the measurements are when local x and local y  */
  std::pair<double, double> calculateWeight_2D_3(
    const TrackParameters* componentTrackParameters,
    const AmgSymMatrix(5) * predictedCov,
    const AmgVector(2) & measPar,
    const AmgSymMatrix(2) & measCov) const;

  /** Tempplated Function to calculate the determinant and  chi2 of a
   * measurement for a N'D hit */
  template<int DIM>
  std::pair<double, double> calculateWeight_T(
    const TrackParameters* componentTrackParameters,
    const AmgSymMatrix(5) * predictedCov,
    const AmgVector(DIM) & measPar,
    const AmgSymMatrix(DIM) & measCov,
    int paramKey) const;
};

template<int DIM>
std::pair<double, double>
PosteriorWeightsCalculator::calculateWeight_T(
  const TrackParameters* componentTrackParameters,
  const AmgSymMatrix(5) * predictedCov,
  const AmgVector(DIM) & measPar,
  const AmgSymMatrix(DIM) & measCov,
  int paramKey) const
{

  static const ProjectionMatricesSet reMatrices(
    5); //!< expansion and reduction matrices set
  // Define the expansion matrix
  const AmgMatrix(DIM, 5) H =
    reMatrices.expansionMatrix(paramKey).block<DIM, 5>(0, 0);

  // Calculate the residual
  AmgVector(DIM) r = measPar - H * componentTrackParameters->parameters();

  // Residual covariance. Posterior weights is calculated used predicted state
  // and measurement. Therefore add covariances
  AmgSymMatrix(DIM) R(measCov + H * (*predictedCov) * H.transpose());

  // compute determinant of residual
  const double det = R.determinant();

  if (det == 0) {
    // ATH_MSG_WARNING( "Determinant is 0, cannot invert matrix... Ignoring
    // component" );
    return std::pair<double, double>(0, 0);
  }
  // Compute Chi2
  return std::pair<double, double>(
    det, (1. / (double)DIM) * ((r.transpose() * R.inverse() * r)(0, 0)));
}

} // end Trk namespace

#endif
