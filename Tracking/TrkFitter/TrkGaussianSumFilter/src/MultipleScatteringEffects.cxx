/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************************
      MultipleScatteringEffects.cxx  -  description
      ------------------------------------------
begin                : Tuesday 22nd January 2005
author               : amorley
email                : amorley@cern.ch
decription           : Implementation code for multiple scatter effects
*********************************************************************************/

#include "TrkGaussianSumFilter/MultipleScatteringEffects.h"
#include "TrkGeometry/Layer.h"
#include "TrkGeometry/MaterialProperties.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSurfaces/Surface.h"

#include "TrkEventPrimitives/ParamDefs.h"

#include <cmath>

Trk::MultipleScatteringEffects::MultipleScatteringEffects(const std::string& type,
                                                          const std::string& name,
                                                          const IInterface* parent)
  : AthAlgTool(type, name, parent)
  , m_multipleScatterLogTermOn(true)
{
  declareInterface<IMultiStateMaterialEffects>(this);
  declareProperty("MultipleScatterLogarithmicTermOn", m_multipleScatterLogTermOn);
}

Trk::MultipleScatteringEffects::~MultipleScatteringEffects() = default;

StatusCode
Trk::MultipleScatteringEffects::initialize()
{
  ATH_CHECK(m_msUpdator.retrieve());
  ATH_MSG_DEBUG("Initialisation of " << name() << " was successful");
  return StatusCode::SUCCESS;
}

StatusCode
Trk::MultipleScatteringEffects::finalize()
{
  ATH_MSG_DEBUG("Finalisation of " << name() << " was successful");
  return StatusCode::SUCCESS;
}

void
Trk::MultipleScatteringEffects::compute(IMultiStateMaterialEffects::Cache& cache,
                                        const ComponentParameters& componentParameters,
                                        const MaterialProperties& materialProperties,
                                        double pathLength,
                                        PropDirection /*direction*/,
                                        ParticleHypothesis /*particleHypothesis*/) const
{
  // Reset the cache
  cache.reset();
  // Request track parameters from component parameters
  const Trk::TrackParameters* trackParameters = componentParameters.first.get();

  ATH_MSG_VERBOSE("Updating multiple scatter effects based on material properties and path length");

  const AmgSymMatrix(5)* measuredTrackCov = trackParameters->covariance();

  if (!measuredTrackCov) {
    ATH_MSG_DEBUG("No measurement associated with track parameters... returning original parameters");
    return;
  }

  const Amg::Vector3D& globalMomentum = trackParameters->momentum();
  double p = globalMomentum.mag();

  double pathcorrection = 1.;
  if (materialProperties.thickness() != 0)
    pathcorrection = pathLength / materialProperties.thickness();

  // Here we know the path length to be meff.thicknessX0, so we set pathcorrection = 1
  // and create a dummy materialProperties with the properties we are interested in
  MaterialProperties mprop(materialProperties.thicknessInX0(), 1., 0., 0., 0., 0.);
  double angularVariation = m_msUpdator->sigmaSquare(mprop, p, pathcorrection, Trk::muon);
  ATH_MSG_DEBUG("Sigma squared multiple scattering: " << angularVariation);

  std::unique_ptr<AmgSymMatrix(5)> deltaCov = std::make_unique<AmgSymMatrix(5)>();
  deltaCov->setZero();

  // double sign = (direction == Trk::oppositeMomentum) ? 1. : 1.;
  double sinTheta = std::sin(trackParameters->parameters()[Trk::theta]);

  (*deltaCov)(Trk::phi, Trk::phi) += angularVariation / (sinTheta * sinTheta);
  (*deltaCov)(Trk::theta, Trk::theta) += angularVariation;

  cache.weights.push_back(1.);
  cache.deltaPs.push_back(0.);
  cache.deltaCovariances.push_back(std::move(deltaCov));
}
