/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************************
      MultiStateMaterialEffectsAdapter.cxx  -  description
      ----------------------------------------------------
begin                : Tuesday 22nd February 2005
author               : atkinson
email                : Tom.Atkinson@cern.ch
decription           : Implementation code for MultiStateMaterialEffectsAdapter class
**************************************************************************************/

#include "TrkGaussianSumFilter/MultiStateMaterialEffectsAdapter.h"
#include "TrkExInterfaces/IMaterialEffectsUpdator.h"

void
Trk::MultiStateMaterialEffectsAdapter::compute(Trk::IMultiStateMaterialEffects::Cache& cache,
                                               const ToolHandle<IMaterialEffectsUpdator>& tool,
                                               const ComponentParameters& componentParameters,
                                               const MaterialProperties& materialProperties,
                                               double pathLength,
                                               PropDirection direction,
                                               ParticleHypothesis particleHypothesis)
{
  // Reset the cache
  cache.reset();

  // Request track parameters from component parameters
  const Trk::TrackParameters* originalTrackParameters = componentParameters.first;

  // Update this track parameters object
  const Trk::TrackParameters* updatedTrackParameters =
    tool->update(*originalTrackParameters, materialProperties, pathLength, direction, particleHypothesis);

  /* Extract the required values for the Gsf Material Effects Updator
     - weight = 1 for single state
     - deltaP
     - deltaSigma
  */

  double deltaP =
    Trk::MultiStateMaterialEffectsAdapter::extractDeltaP(*updatedTrackParameters, *originalTrackParameters);
  std::unique_ptr<const AmgSymMatrix(5)> deltaErrorMatrix(
    Trk::MultiStateMaterialEffectsAdapter::extractDeltaCovariance(*updatedTrackParameters, *originalTrackParameters));
  cache.weights.push_back(1.);
  cache.deltaPs.push_back(deltaP);

  if (deltaErrorMatrix) {
    cache.deltaCovariances.push_back(std::move(deltaErrorMatrix));
  }
  // Clean up memory
  delete updatedTrackParameters;
}

double
Trk::MultiStateMaterialEffectsAdapter::extractDeltaP(const Trk::TrackParameters& updatedParameters,
                                                     const Trk::TrackParameters& originalParameters)
{

  double deltaP(0.);
  double originalP = 1. / originalParameters.parameters()[Trk::qOverP];
  double updatedP = 1. / updatedParameters.parameters()[Trk::qOverP];

  if (originalP < 0.)
    deltaP = fabs(originalP) + updatedP;
  else
    deltaP = fabs(originalP) - updatedP;

  // std::cout << "originalP " << originalP << "\tupdatedP " << updatedP << "\tDelta " << deltaP << std::endl;

  return deltaP;
}

const AmgSymMatrix(5) * Trk::MultiStateMaterialEffectsAdapter::extractDeltaCovariance(
                          const Trk::TrackParameters& updatedParameters,
                          const Trk::TrackParameters& originalParameters)
{

  const AmgSymMatrix(5)* originalMeasuredCov = originalParameters.covariance();
  const AmgSymMatrix(5)* updatedMeasuredCov = updatedParameters.covariance();

  if (!originalMeasuredCov || !updatedMeasuredCov)
    return 0;

  return new AmgSymMatrix(5)(*updatedMeasuredCov - *originalMeasuredCov);
}
