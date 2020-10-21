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
#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkExUtils/MaterialInteraction.h"
#include "TrkGeometry/MaterialProperties.h"
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
  ATH_CHECK(m_EnergyLossUpdator.retrieve());
  ATH_CHECK(m_betheHeitlerEffects.retrieve());
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
  GSFScatteringCache cache_multipleScatter;
  this->scattering(
    cache_multipleScatter, componentParameters, materialProperties, pathLength);

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

  // Protect if there are no new energy loss
  // components
  // we want at least on dummy to "combine"
  // with scattering
  if (cache_energyLoss.weights.empty()) {
    cache_energyLoss.weights.push_back(1.);
    cache_energyLoss.deltaPs.push_back(0.);
    cache_energyLoss.deltaQOvePCov.push_back(0.);
  }
  /*
   * 3. Combine the multiple scattering with each of the  energy loss components
   */
  auto energyLoss_weightsIterator = cache_energyLoss.weights.begin();
  auto energyLoss_deltaPsIterator = cache_energyLoss.deltaPs.begin();
  auto energyLoss_deltaQOvePCovIterator =
    cache_energyLoss.deltaQOvePCov.begin();

  for (; energyLoss_weightsIterator != cache_energyLoss.weights.end();
       ++energyLoss_weightsIterator,
       ++energyLoss_deltaPsIterator,
       ++energyLoss_deltaQOvePCovIterator) {

    double combinedWeight = (*energyLoss_weightsIterator);
    double combinedDeltaP = (*energyLoss_deltaPsIterator);
    cache.weights.push_back(combinedWeight);
    cache.deltaPs.push_back(combinedDeltaP);

    if (measuredCov) {
      // Create the covariance
      const double covPhi = cache_multipleScatter.deltaPhiCov;
      const double covTheta = cache_multipleScatter.deltaThetaCov;
      const double covQoverP = (*energyLoss_deltaQOvePCovIterator);
      AmgSymMatrix(5) cov;
      cov << 0, 0, 0, 0, 0,   // 5
        0, 0, 0, 0, 0,        // 10
        0, 0, covPhi, 0, 0,   // 15
        0, 0, 0, covTheta, 0, // 20
        0, 0, 0, 0, covQoverP;
      cache.deltaCovariances.emplace_back(std::move(cov));
    }
  } // end for loop over energy loss components
}

void
Trk::GsfCombinedMaterialEffects::scattering(
  GSFScatteringCache& cache,
  const ComponentParameters& componentParameters,
  const MaterialProperties& materialProperties,
  double pathLength) const
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
  const double p = globalMomentum.mag();
  double pathcorrection = 1.;
  if (materialProperties.thickness() != 0) {
    pathcorrection = pathLength / materialProperties.thickness();
  }
  const double t = pathcorrection * materialProperties.thicknessInX0();
  // We were/are using muon here,
  // not sure is what we want 100%.
  const double m = s_particleMasses.mass[Trk::muon];
  const double E = sqrt(p * p + m * m);
  const double beta = p / E;
  const Trk::MaterialInteraction matInt;
  const double sigma = matInt.sigmaMS(t, p, beta);
  const double angularVariation = sigma * sigma;

  const double sinTheta = std::sin(trackParameters->parameters()[Trk::theta]);
  cache.deltaThetaCov = angularVariation;
  cache.deltaPhiCov = angularVariation / (sinTheta * sinTheta);
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
