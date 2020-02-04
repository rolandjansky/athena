/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************************
      GsfCombinedMaterialEffects.cxx  -  description
      ----------------------------------------------
begin                : Friday 11th January 2005
author               : atkinson
email                : Tom.Atkinson@cern.ch
decription           : Implementation code for GsfCombinedMaterialEffects class
*********************************************************************************/

#include "TrkGaussianSumFilter/GsfCombinedMaterialEffects.h"

#include "TrkGeometry/Layer.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSurfaces/Surface.h"

#include "GaudiKernel/ToolHandle.h"

Trk::GsfCombinedMaterialEffects::GsfCombinedMaterialEffects(const std::string& type,
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
  // Retrieve and configure multiple scattering effects for multi-state operation
  ATH_MSG_INFO("Configuring for multiple scattering");

  // Retrieve the multiple scattering effects
  if (m_multipleScatterEffects.retrieve().isFailure()) {
    ATH_MSG_FATAL("Could not retrieve multiple scattering AlgTool: " << m_multipleScatterEffects.typeAndName()
                                                                     << "... Exiting!");
    return StatusCode::FAILURE;
  }

  // Retrieve and configure the energy loss effects for multi-state operation
  ATH_MSG_INFO("Configuring for normal energy loss");

  if (m_energyLossEffects.retrieve().isFailure()) {
    ATH_MSG_FATAL("Could not retrieve energy loss AlgTool: " << m_energyLossEffects.typeAndName() << "... Exiting!");
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO("Retrieved energy loss AlgTool: " << m_energyLossEffects.typeAndName() << "... YAY!");
  }

  // Retrieve and configure the Bethe-Heitler effects for energy loss
  ATH_MSG_INFO("Configuring for Bethe-Heitler energy loss");

  if (m_betheHeitlerEffects.retrieve().isFailure()) {
    ATH_MSG_FATAL("Could not retrieve energy loss AlgTool: " << m_betheHeitlerEffects.typeAndName() << "... Exiting!");
    return StatusCode::FAILURE;
  }

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
Trk::GsfCombinedMaterialEffects::compute(IMultiStateMaterialEffects::Cache& cache,
                                         const Trk::ComponentParameters& componentParameters,
                                         const Trk::MaterialProperties& materialProperties,
                                         double pathLength,
                                         Trk::PropDirection direction,
                                         Trk::ParticleHypothesis particleHypothesis) const
{

  ATH_MSG_DEBUG("Computing combined material effects");

  // Reset everything before computation
  cache.reset();

  const AmgSymMatrix(5)* measuredCov = componentParameters.first->covariance();

  /* ========================================================================
     Retrieve multiple scattering corrections
     ======================================================================== */

  IMultiStateMaterialEffects::Cache cache_multipleScatter;
  m_multipleScatterEffects->compute(
    cache_multipleScatter, componentParameters, materialProperties, pathLength, direction, particleHypothesis);

  // Protect if there are no new components
  if (cache_multipleScatter.weights.empty()) {
    ATH_MSG_DEBUG("WARNING: Multiple scattering effects are not determined");
    cache_multipleScatter.weights.push_back(1.);
    cache_multipleScatter.deltaPs.push_back(0.);
    std::unique_ptr<AmgSymMatrix(5)> newCov = std::make_unique<AmgSymMatrix(5)>();
    newCov->setZero();
    cache_multipleScatter.deltaCovariances.push_back(std::move(newCov));
  }

  /* ========================================================================
     Retrieve energy loss corrections
     ======================================================================== */

  IMultiStateMaterialEffects::Cache cache_energyLoss;

  if (particleHypothesis == electron) {

    ATH_MSG_VERBOSE("Considering Bethe-Heitler energy loss effects");
    m_betheHeitlerEffects->compute(cache_energyLoss, componentParameters, materialProperties, pathLength, direction);

  } else if (particleHypothesis != nonInteracting) {
    ATH_MSG_VERBOSE("Considering standard energy loss effects");

    m_energyLossEffects->compute(
      cache_energyLoss, componentParameters, materialProperties, pathLength, direction, particleHypothesis);
  }

  // Protect if there are no new components
  if (cache_energyLoss.weights.empty()) {
    cache_energyLoss.weights.push_back(1.);
    cache_energyLoss.deltaPs.push_back(0.);
    std::unique_ptr<AmgSymMatrix(5)> newCov = std::make_unique<AmgSymMatrix(5)>();
    newCov->setZero();
    cache_energyLoss.deltaCovariances.push_back(std::move(newCov));
  }

  /* ========================================================================
     Combine the multiple scattering and energy loss components
     ======================================================================== */

  ATH_MSG_DEBUG("Combining the energy loss and multiple scattering components");

  // Iterators over the multiple scattering components
  std::vector<double>::const_iterator multipleScatter_weightsIterator = cache_multipleScatter.weights.begin();
  std::vector<double>::const_iterator multipleScatter_deltaPsIterator = cache_multipleScatter.deltaPs.begin();
  std::vector<std::unique_ptr<const AmgSymMatrix(5)>>::const_iterator multipleScatter_deltaCovariancesIterator =
    cache_multipleScatter.deltaCovariances.begin();

  // Loop over multiple scattering components
  for (; multipleScatter_weightsIterator != cache_multipleScatter.weights.end();
       ++multipleScatter_weightsIterator,
       ++multipleScatter_deltaPsIterator,
       ++multipleScatter_deltaCovariancesIterator) {

    // Iterators over the energy loss components
    std::vector<double>::const_iterator energyLoss_weightsIterator = cache_energyLoss.weights.begin();
    std::vector<double>::const_iterator energyLoss_deltaPsIterator = cache_energyLoss.deltaPs.begin();
    std::vector<std::unique_ptr<const AmgSymMatrix(5)>>::const_iterator energyLoss_deltaCovariancesIterator =
      cache_energyLoss.deltaCovariances.begin();

    // Loop over energy loss components

    for (; energyLoss_weightsIterator != cache_energyLoss.weights.end();
         ++energyLoss_weightsIterator, ++energyLoss_deltaPsIterator, ++energyLoss_deltaCovariancesIterator) {

      double combinedWeight = (*multipleScatter_weightsIterator) * (*energyLoss_weightsIterator);
      double combinedDeltaP = (*multipleScatter_deltaPsIterator) + (*energyLoss_deltaPsIterator);

      cache.weights.push_back(combinedWeight);
      cache.deltaPs.push_back(combinedDeltaP);

      if (measuredCov) {
        std::unique_ptr<AmgSymMatrix(5)> summedCovariance = std::make_unique<AmgSymMatrix(5)>(
          (*(*multipleScatter_deltaCovariancesIterator)) + (*(*energyLoss_deltaCovariancesIterator)));
        cache.deltaCovariances.push_back(std::move(summedCovariance));
      }
    } // end for loop over energy loss components
  }   // end for loop over multiple scattering components
  ATH_MSG_DEBUG("Successfully included combined material effects");
}
