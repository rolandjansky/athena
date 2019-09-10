/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#include "TrkGaussianSumFilter/IPosteriorWeightsCalculator.h"

#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkEventPrimitives/ProjectionMatricesSet.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkMultiComponentStateOnSurface/MultiComponentState.h"
#include "TrkParameters/TrackParameters.h"

#include "AthenaBaseComps/AthAlgTool.h"

namespace Trk {

class MeasurementBase;

class PosteriorWeightsCalculator
  : public AthAlgTool
  , virtual public IPosteriorWeightsCalculator
{
public:
  /** Constructor with AlgTool parameters */
  PosteriorWeightsCalculator(const std::string&, const std::string&, const IInterface*);

  /** Virtual destructor */
  virtual ~PosteriorWeightsCalculator(){};

  /** AlgTool initialise method */
  StatusCode initialize();

  /** AlgTool finalise method */
  StatusCode finalize();

  /** Method to compute the state weights after measurement update */
  virtual std::unique_ptr<MultiComponentState> weights(const MultiComponentState&, const MeasurementBase&) const;

private:
  const ProjectionMatricesSet m_reMatrices; //!< expansion and reduction matrices set

  /** Function to calculate the determinant and  chi2 of a measurement for a 1D hit */
  std::pair<double, double> calculateWeight_1D(const TrackParameters* componentTrackParameters,
                                               const AmgSymMatrix(5) * predictedCov,
                                               const double measPar,
                                               const double measCov,
                                               int paramKey) const;

  /** Function to calculate the determinant and  chi2 of a measurement for a 2D hit
      specially when the measurements are when local x and local y  */
  std::pair<double, double> calculateWeight_2D_3(const TrackParameters* componentTrackParameters,
                                                 const AmgSymMatrix(5) * predictedCov,
                                                 const AmgVector(2) & measPar,
                                                 const AmgSymMatrix(2) & measCov) const;

  /** Tempplated Function to calculate the determinant and  chi2 of a measurement for a N'D hit */
  template<int DIM>
  std::pair<double, double> calculateWeight_T(const TrackParameters* componentTrackParameters,
                                              const AmgSymMatrix(5) * predictedCov,
                                              const AmgVector(DIM) & measPar,
                                              const AmgSymMatrix(DIM) & measCov,
                                              int paramKey) const;
};

template<int DIM>
std::pair<double, double>
PosteriorWeightsCalculator::calculateWeight_T(const TrackParameters* componentTrackParameters,
                                              const AmgSymMatrix(5) * predictedCov,
                                              const AmgVector(DIM) & measPar,
                                              const AmgSymMatrix(DIM) & measCov,
                                              int paramKey) const
{
  std::pair<double, double> result(0, 0);
  // Extract predicted state track parameters
  const AmgVector(5)& trackParameters = componentTrackParameters->parameters();

  // Define the expansion matrix
  const AmgMatrix(DIM, 5) H = m_reMatrices.expansionMatrix(paramKey).block<DIM, 5>(0, 0);

  // Calculate the residual
  AmgVector(DIM) r = measPar - H * trackParameters;

  // Residual covariance. Posterior weights is calculated used predicted state and measurement. Therefore add
  // covariances
  AmgSymMatrix(DIM) R(measCov + H * (*predictedCov) * H.transpose());

  // compute determinant of residual
  result.first = R.determinant();

  if (result.first == 0) {
    // ATH_MSG_WARNING( "Determinant is 0, cannot invert matrix... Ignoring component" );
    return result;
  }
  // Compute Chi2
  result.second = (1. / (double)DIM) * ((r.transpose() * R.inverse() * r)(0, 0));
  return result;
}

} // end Trk namespace

#endif
