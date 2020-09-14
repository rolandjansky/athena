/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file GsfCombinedMaterialEffects.cxx
 * @begin         Friday 11th January 2005
 * @author        Anthony Morley, Tom  Atkinson, Christos Anastopoulos
 * @brief         Implementation code for GsfCombinedMaterialEffects class
 */

#include "TrkGaussianSumFilter/GsfCombinedMaterialEffects.h"

#include "GaudiKernel/ToolHandle.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkGeometry/Layer.h"
#include "TrkMaterialOnTrack/EnergyLoss.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSurfaces/Surface.h"

namespace {
const Trk::ParticleMasses s_particleMasses{};
}

Trk::GsfCombinedMaterialEffects::GsfCombinedMaterialEffects(
  const std::string& type,
  const std::string& name,
  const IInterface* parent)
  : AthAlgTool(type, name, parent)
{
  declareInterface<IMultiStateMaterialEffects>(this);
}

Trk::GsfCombinedMaterialEffects::~GsfCombinedMaterialEffects() = default;

StatusCode
Trk::GsfCombinedMaterialEffects::initialize()
{
  // Retrieve and configure multiple scattering effects for multi-state
  // operation
  ATH_MSG_INFO("Configuring for multiple scattering");
  ATH_CHECK(m_msUpdator.retrieve());

  // Retrieve and configure the std energy loss effects for multi-state
  // operation
  ATH_MSG_INFO("Configuring for normal energy loss");
  ATH_CHECK(m_EnergyLossUpdator.retrieve());

  // Retrieve and configure the Bethe-Heitler effects for energy loss
  ATH_MSG_INFO("Configuring for Bethe-Heitler energy loss");
  ATH_CHECK(m_betheHeitlerEffects.retrieve());

  ATH_MSG_INFO("Initialisation of " << name() << " was successful");
  return StatusCode::SUCCESS;
}

StatusCode
Trk::GsfCombinedMaterialEffects::finalize()
{
  ATH_MSG_INFO("Finalisation of " << name() << " was successful");
  return StatusCode::SUCCESS;
}

void
Trk::GsfCombinedMaterialEffects::compute(
  IMultiStateMaterialEffects::Cache& cache,
  const Trk::ComponentParameters& componentParameters,
  const Trk::MaterialProperties& materialProperties,
  double pathLength,
  Trk::PropDirection direction,
  Trk::ParticleHypothesis particleHypothesis) const
{

  // Reset everything before computation
  cache.reset();

  const AmgSymMatrix(5)* measuredCov = componentParameters.first->covariance();

  /*
   * 1.  Retrieve multiple scattering corrections
   */
  IMultiStateMaterialEffects::Cache cache_multipleScatter;
  this->scattering(cache_multipleScatter,
                   componentParameters,
                   materialProperties,
                   pathLength,
                   direction,
                   particleHypothesis);

  // Protect if there are no new components
  if (cache_multipleScatter.weights.empty()) {
    ATH_MSG_DEBUG("WARNING: Multiple scattering effects are not determined");
    cache_multipleScatter.weights.push_back(1.);
    cache_multipleScatter.deltaPs.push_back(0.);
    AmgSymMatrix(5) newCov;
    newCov.setZero();
    cache_multipleScatter.deltaCovariances.push_back(std::move(newCov));
  }

  /*
   * 2. Retrieve energy loss corrections
   */
  Trk::GSFEnergyLossCache cache_energyLoss;
  if (particleHypothesis == electron) {
    m_betheHeitlerEffects->compute(cache_energyLoss,
                                   componentParameters,
                                   materialProperties,
                                   pathLength,
                                   direction);
  } else if (particleHypothesis != nonInteracting) {
    this->energyLoss(cache_energyLoss,
                     componentParameters,
                     materialProperties,
                     pathLength,
                     direction,
                     particleHypothesis);
  }

  // Protect if there are no new components
  if (cache_energyLoss.weights.empty()) {
    cache_energyLoss.weights.push_back(1.);
    cache_energyLoss.deltaPs.push_back(0.);
    cache_energyLoss.deltaQOvePCov.push_back(0.);
  }

  /*
   * 3. Combine the multiple scattering and energy loss components
   */

  // Iterators over the multiple scattering components
  auto multipleScatter_weightsIterator = cache_multipleScatter.weights.begin();
  auto multipleScatter_deltaPsIterator = cache_multipleScatter.deltaPs.begin();
  auto multipleScatter_deltaCovariancesIterator =
    cache_multipleScatter.deltaCovariances.begin();

  // Loop over multiple scattering components
  for (; multipleScatter_weightsIterator != cache_multipleScatter.weights.end();
       ++multipleScatter_weightsIterator,
       ++multipleScatter_deltaPsIterator,
       ++multipleScatter_deltaCovariancesIterator) {

    // Iterators over the energy loss components
    auto energyLoss_weightsIterator = cache_energyLoss.weights.begin();
    auto energyLoss_deltaPsIterator = cache_energyLoss.deltaPs.begin();
    auto energyLoss_deltaQOvePCovIterator =
      cache_energyLoss.deltaQOvePCov.begin();

    // Loop over energy loss components

    for (; energyLoss_weightsIterator != cache_energyLoss.weights.end();
         ++energyLoss_weightsIterator,
         ++energyLoss_deltaPsIterator,
         ++energyLoss_deltaQOvePCovIterator) {

      double combinedWeight =
        (*multipleScatter_weightsIterator) * (*energyLoss_weightsIterator);
      double combinedDeltaP =
        (*multipleScatter_deltaPsIterator) + (*energyLoss_deltaPsIterator);
      cache.weights.push_back(combinedWeight);
      cache.deltaPs.push_back(combinedDeltaP);

      if (measuredCov) {
        //Start from the multiple Scattering detla
        AmgSymMatrix(5) summedCovariance =
          (*multipleScatter_deltaCovariancesIterator);
        //Add to it the QoverP cov from energy loss
        summedCovariance(Trk::qOverP, Trk::qOverP) +=
          (*energyLoss_deltaQOvePCovIterator);
        cache.deltaCovariances.push_back(std::move(summedCovariance));
      }
    } // end for loop over energy loss components
  }   // end for loop over multiple scattering components
}

void
Trk::GsfCombinedMaterialEffects::scattering(
  IMultiStateMaterialEffects::Cache& cache,
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
  const AmgSymMatrix(5)* measuredTrackCov = trackParameters->covariance();

  if (!measuredTrackCov) {
    ATH_MSG_DEBUG("No measurement associated with track parameters... "
                  "returning original parameters");
    return;
  }

  const Amg::Vector3D& globalMomentum = trackParameters->momentum();
  double p = globalMomentum.mag();

  double pathcorrection = 1.;
  if (materialProperties.thickness() != 0) {
    pathcorrection = pathLength / materialProperties.thickness();
  }

  // Here we know the path length to be meff.thicknessX0, so we set
  // pathcorrection = 1 and create a dummy materialProperties with the
  // properties we are interested in
  MaterialProperties mprop(
    materialProperties.thicknessInX0(), 1., 0., 0., 0., 0.);
  const double angularVariation =
    m_msUpdator->sigmaSquare(mprop, p, pathcorrection, Trk::muon);

  AmgSymMatrix(5) deltaCov;
  deltaCov.setZero();
  // double sign = (direction == Trk::oppositeMomentum) ? 1. : 1.;
  const double sinTheta = std::sin(trackParameters->parameters()[Trk::theta]);
  deltaCov(Trk::phi, Trk::phi) += angularVariation / (sinTheta * sinTheta);
  deltaCov(Trk::theta, Trk::theta) += angularVariation;
  cache.weights.push_back(1.);
  cache.deltaPs.push_back(0.);
  cache.deltaCovariances.push_back(std::move(deltaCov));
}

void
Trk::GsfCombinedMaterialEffects::energyLoss(
  Trk::GSFEnergyLossCache& cache,
  const ComponentParameters& componentParameters,
  const MaterialProperties& materialProperties,
  double pathLength,
  PropDirection direction,
  ParticleHypothesis particleHypothesis) const
{
  // Reset the cache
  cache.reset();

  // Request track parameters from component parameters
  const Trk::TrackParameters* trackParameters = componentParameters.first.get();
  const AmgSymMatrix(5)* measuredCov = trackParameters->covariance();
  if (!measuredCov) {
    ATH_MSG_DEBUG("No measurement on track parameters... returning original "
                  "track parameters");
    return;
  }
  double pathcorrection = pathLength / materialProperties.thickness();
  const Amg::Vector3D& globalMomentum = trackParameters->momentum();

  std::unique_ptr<EnergyLoss> energyLoss(
    m_EnergyLossUpdator->energyLoss(materialProperties,
                                    globalMomentum.mag(),
                                    pathcorrection,
                                    direction,
                                    particleHypothesis,
                                    true));

  // update for mean energy loss
  const double deltaE = energyLoss ? energyLoss->deltaE() : 0;
  const double sigmaDeltaE = energyLoss ? energyLoss->sigmaDeltaE() : 0;

  // Calculate the pathlength encountered by the track
  const double p = globalMomentum.mag();
  const double m = s_particleMasses.mass[particleHypothesis];
  const double E = sqrt(p * p + m * m);
  const double beta = p / E;

  // Calculate energy loss values uncertainty
  const double sigmaQoverP = sigmaDeltaE / pow(beta * p, 2);

  cache.weights.push_back(1.);
  cache.deltaPs.push_back(deltaE);
  cache.deltaQOvePCov.push_back(sigmaQoverP * sigmaQoverP);
}
