/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************************
      GsfEnergyLossUpdator.cxx  -  description
      ----------------------------------------
begin                : Tuesday 22nd January 2005
author               : atkinson
email                : Tom.Atkinson@cern.ch
decription           : Implementation code for EnergyLossUpdator class
*********************************************************************************/

#include "TrkGaussianSumFilter/GsfEnergyLossUpdator.h"

#include "TrkGeometry/Layer.h"
#include "TrkGeometry/MaterialProperties.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSurfaces/Surface.h"

#include "TrkMaterialOnTrack/EnergyLoss.h"

#include "TrkEventPrimitives/ParamDefs.h"

namespace {
const Trk::ParticleMasses s_particleMasses{};
}

Trk::GsfEnergyLossUpdator::GsfEnergyLossUpdator(const std::string& type,
                                                const std::string& name,
                                                const IInterface* parent)
  : AthAlgTool(type, name, parent)
{

  declareInterface<IMultiStateMaterialEffects>(this);
}

Trk::GsfEnergyLossUpdator::~GsfEnergyLossUpdator() {}

StatusCode
Trk::GsfEnergyLossUpdator::initialize()
{
  if (m_EnergyLossUpdator.retrieve().isFailure()) {
    ATH_MSG_FATAL("Failed to retrieve tool " << m_EnergyLossUpdator
                                             << ". No energy effects will be taken into account.");
    return StatusCode::FAILURE;
  } else{
    ATH_MSG_INFO("Retrieved tool " << m_EnergyLossUpdator);
  }
  ATH_MSG_INFO("Initialisation of " << name() << " was successful");
  return StatusCode::SUCCESS;
}

StatusCode
Trk::GsfEnergyLossUpdator::finalize()
{
  ATH_MSG_INFO("Finalisation of " << name() << " was successful" );
  return StatusCode::SUCCESS;
}


void Trk::GsfEnergyLossUpdator::compute(IMultiStateMaterialEffects::Cache& cache,
                                               const ComponentParameters& componentParameters,
                                               const MaterialProperties& materialProperties,
                                               double pathLength,
                                               PropDirection direction,
                                               ParticleHypothesis particleHypothesis) const
{
  // Reset the cache
  cache.reset();
  // Request track parameters from component parameters
  const Trk::TrackParameters* trackParameters = componentParameters.first;

  const AmgSymMatrix(5)* measuredCov = trackParameters->covariance();

  if (!measuredCov) {
      ATH_MSG_DEBUG("No measurement on track parameters... returning original track parameters");
      return;
  }


  double pathcorrection = pathLength / materialProperties.thickness();

  const Amg::VectorX& trackStateVector = trackParameters->parameters();
  const Amg::Vector3D& globalMomentum = trackParameters->momentum();

  EnergyLoss* energyLoss = m_EnergyLossUpdator->energyLoss(
    materialProperties, globalMomentum.mag(), pathcorrection, direction, particleHypothesis, true);

  // update for mean energy loss
  double deltaE = energyLoss ? energyLoss->deltaE() : 0;
  double sigmaDeltaE = energyLoss ? energyLoss->sigmaDeltaE() : 0;
  delete energyLoss;

  // Calculate the pathlength encountered by the track
  double p = globalMomentum.mag();
  double inverseMomentum = 1. / p;

  double m = s_particleMasses.mass[particleHypothesis];
  double E = sqrt(p * p + m * m);
  double beta = p / E;

  // Calculate energy loss values
  double momentumFractionLost = deltaE * inverseMomentum;

  double sigmaQoverP = sigmaDeltaE / pow(beta * p, 2);

  // Update diagonal and off-diagonal covariance matrix elements
  std::unique_ptr<AmgSymMatrix(5)> deltaCov = std::make_unique<AmgSymMatrix(5)>();
  deltaCov->setZero();
  (*deltaCov)(Trk::qOverP, Trk::qOverP) += sigmaQoverP * sigmaQoverP;


  cache.weights.push_back(1.);
  cache.deltaPs.push_back(deltaE);
  cache.deltaCovariances.push_back(std::move(deltaCov));

}
