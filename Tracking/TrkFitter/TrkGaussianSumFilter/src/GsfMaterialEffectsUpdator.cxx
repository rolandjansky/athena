/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***********************************************************************************
                        GsfMaterialEffectsUpdator.cxx  -  description
                        ---------------------------------------------
begin                : Wednesday 9th January 2005
author               : atkinson,morley,anastopoulos
decription           : Implementation code for the class
GsfMaterialEffectsUpdator
***********************************************************************************/

#include "TrkGaussianSumFilter/GsfMaterialEffectsUpdator.h"

#include "TrkGaussianSumFilter/IMultiStateMaterialEffects.h"
#include "TrkGaussianSumFilter/MultiComponentStateAssembler.h"

#include "TrkParameters/TrackParameters.h"
#include "TrkSurfaces/Surface.h"

#include "TrkGeometry/Layer.h"
#include "TrkGeometry/MaterialProperties.h"

Trk::GsfMaterialEffectsUpdator::GsfMaterialEffectsUpdator(
  const std::string& type,
  const std::string& name,
  const IInterface* parent)
  : AthAlgTool(type, name, parent)
  , m_useReferenceMaterial(false)
  , m_momentumCut(250. * Gaudi::Units::MeV)
{
  declareInterface<IMultiStateMaterialEffectsUpdator>(this);
  declareProperty("UseReferenceMaterial", m_useReferenceMaterial);
  declareProperty("MinimalMomentum", m_momentumCut);
}

Trk::GsfMaterialEffectsUpdator::~GsfMaterialEffectsUpdator() = default;

StatusCode
Trk::GsfMaterialEffectsUpdator::initialize()
{
  // Retrieve the specific material effects
  ATH_CHECK(m_materialEffects.retrieve());
  ATH_MSG_INFO("Initialisation of " << name() << " was successful");
  return StatusCode::SUCCESS;
}

StatusCode
Trk::GsfMaterialEffectsUpdator::finalize()
{
  ATH_MSG_INFO("Finalisation of " << name() << " was successful");
  return StatusCode::SUCCESS;
}

/* ============================================================================
   Full update based on layer information
   ============================================================================
 */

Trk::MultiComponentState
Trk::GsfMaterialEffectsUpdator::updateState(
  const Trk::ComponentParameters& componentParameters,
  const Trk::Layer& layer,
  Trk::PropDirection direction,
  Trk::ParticleHypothesis particleHypothesis) const
{

  const Trk::TrackParameters* trackParameters = componentParameters.first.get();

  if (!trackParameters) {
    ATH_MSG_ERROR("Trying to update component without trackParameters!!!!");
    return {};
  }
  const double weight = componentParameters.second;
  // Extract the material properties from the layer
  const Trk::MaterialProperties* materialProperties(nullptr);
  double pathCorrection(0.);

  if (m_useReferenceMaterial) {

    // Get the surface associated with the parameters
    const Trk::Surface* surface = &(trackParameters->associatedSurface());

    // Only utilise the reference material if an associated detector element
    // exists
    if (surface && surface->associatedDetectorElement()) {

      // Get the layer material properties
      const Trk::LayerMaterialProperties* layerMaterial =
        layer.layerMaterialProperties();

      // Assign the material properties
      materialProperties =
        layerMaterial ? layerMaterial->fullMaterial(trackParameters->position())
                      : nullptr;

      // Determine the pathCorrection if the material properties exist
      pathCorrection =
        materialProperties
          ? 1. / std::abs(surface->normal().dot(trackParameters->momentum().unit()))
          : 0.;
    }
  }

  // Check that the material properties have been defined - if not define them
  // from the layer information
  materialProperties = materialProperties
                         ? materialProperties
                         : layer.fullUpdateMaterialProperties(*trackParameters);

  // Bail out if still no material properties can be found
  if (!materialProperties) {
    Trk::MultiComponentState clonedMultiComponentState;
    clonedMultiComponentState.emplace_back(trackParameters->clone(), weight);
    return clonedMultiComponentState;
  }

  // Define the path correction
  pathCorrection =
    pathCorrection > 0.
      ? pathCorrection
      : layer.surfaceRepresentation().pathCorrection(
          trackParameters->position(), trackParameters->momentum());

  // The pathlength ( in mm ) is the path correction * the thickness of the
  // material
  double pathLength = pathCorrection * materialProperties->thickness();

  auto updatedState = compute(componentParameters,
                              *materialProperties,
                              pathLength,
                              direction,
                              particleHypothesis);

  return updatedState;
}

/* ============================================================================
   Full update based on path-length & material properties information
   ============================================================================
 */

Trk::MultiComponentState
Trk::GsfMaterialEffectsUpdator::updateState(
  const Trk::ComponentParameters& componentParameters,
  const Trk::MaterialProperties& materialProperties,
  double pathLength,
  Trk::PropDirection direction,
  Trk::ParticleHypothesis particleHypothesis) const
{
  auto updatedState = compute(componentParameters,
                              materialProperties,
                              pathLength,
                              direction,
                              particleHypothesis);
  return updatedState;
}

/* ============================================================================
   Pre-update based on layer information
   ============================================================================
 */
Trk::MultiComponentState
Trk::GsfMaterialEffectsUpdator::preUpdateState(
  const Trk::ComponentParameters& componentParameters,
  const Trk::Layer& layer,
  Trk::PropDirection direction,
  Trk::ParticleHypothesis particleHypothesis) const
{

  ATH_MSG_DEBUG("Material effects update prior to propagation using layer "
                "information and particle hypothesis: "
                << particleHypothesis);

  const Trk::TrackParameters* trackParameters = componentParameters.first.get();

  if (!trackParameters) {
    ATH_MSG_ERROR(
      "Trying to update component without trackParameters... returing 0!");
    return {};
  }

  // Get the pre-update factor
  double preUpdateFactor =
    layer.preUpdateMaterialFactor(*trackParameters, direction);

  // Bail if the pre-update factor is small
  if (preUpdateFactor < 0.01) {
    return {};
  }

  const Trk::MaterialProperties* materialProperties(nullptr);
  double pathCorrection(0.);

  // Incorporate the reference material

  if (m_useReferenceMaterial) {

    // Get the surface associated with the parameters
    const Trk::Surface* surface = &(trackParameters->associatedSurface());

    // Only utilise the reference material if an associated detector element
    // exists
    if (surface && surface->associatedDetectorElement()) {

      // Get the layer material properties
      const Trk::LayerMaterialProperties* layerMaterial =
        layer.layerMaterialProperties();

      // Assign the material properties
      materialProperties =
        layerMaterial ? layerMaterial->fullMaterial(trackParameters->position())
                      : nullptr;

      // Determine the pathCorrection if the material properties exist
      pathCorrection =
        materialProperties
          ? 1. / std::abs(surface->normal().dot(trackParameters->momentum().unit()))
          : 0.;
    }
  }

  // Check that the material properties have been defined - if not define them
  // from the layer information
  materialProperties = materialProperties
                         ? materialProperties
                         : layer.fullUpdateMaterialProperties(*trackParameters);

  // Bail out if still no material properties can be found
  if (!materialProperties) {
    Trk::MultiComponentState clonedMultiComponentState{};
    clonedMultiComponentState.emplace_back(componentParameters.first->clone(),
                                           componentParameters.second);
    return clonedMultiComponentState;
  }

  // Define the path correction
  pathCorrection =
    pathCorrection > 0.
      ? pathCorrection
      : layer.surfaceRepresentation().pathCorrection(
          trackParameters->position(), trackParameters->momentum());

  // Scale the correction by the pre-update factor
  pathCorrection *= preUpdateFactor;

  // The pathlength ( in mm ) is the path correction * the thickness of the
  // material
  double pathLength = pathCorrection * materialProperties->thickness();

  auto updatedState = compute(componentParameters,
                              *materialProperties,
                              pathLength,
                              direction,
                              particleHypothesis);

  return updatedState;
}

/* ============================================================================
   Post-update based on layer information
   ============================================================================
 */

Trk::MultiComponentState
Trk::GsfMaterialEffectsUpdator::postUpdateState(
  const Trk::ComponentParameters& componentParameters,
  const Layer& layer,
  PropDirection direction,
  ParticleHypothesis particleHypothesis) const
{

  ATH_MSG_DEBUG("Material effects update after propagation using layer "
                "information and particle hypothesis: "
                << particleHypothesis);

  Trk::TrackParameters* trackParameters = componentParameters.first.get();

  if (!trackParameters) {
    ATH_MSG_ERROR("Trying to update component without trackParameters... "
                  "returing component!");
    return {};
  }

  // Get the post-update factor
  double postUpdateFactor =
    layer.postUpdateMaterialFactor(*trackParameters, direction);

  // Bail if the postUpdateFactor is small
  if (postUpdateFactor < 0.01) {
    return {};
  }

  const Trk::MaterialProperties* materialProperties(nullptr);
  double pathCorrection(0.);

  if (m_useReferenceMaterial) {

    // Get the surface associated with the parameters
    const Trk::Surface* surface = &(trackParameters->associatedSurface());

    // Only utilise the reference material if an associated detector element
    // exists
    if (surface && surface->associatedDetectorElement()) {

      // Get the layer material properties
      const Trk::LayerMaterialProperties* layerMaterial =
        layer.layerMaterialProperties();

      // Assign the material properties
      materialProperties =
        layerMaterial ? layerMaterial->fullMaterial(trackParameters->position())
                      : nullptr;

      // Determine the pathCorrection if the material properties exist
      pathCorrection =
        materialProperties
          ? 1. / std::abs(surface->normal().dot(trackParameters->momentum().unit()))
          : 0.;
    }
  }

  // Check that the material properties have been defined - if not define them
  // from the layer information
  materialProperties = materialProperties
                         ? materialProperties
                         : layer.fullUpdateMaterialProperties(*trackParameters);

  // Bail out if still no material properties can be found
  if (!materialProperties) {
    Trk::MultiComponentState clonedMultiComponentState{};
    clonedMultiComponentState.emplace_back(componentParameters.first->clone(),
                                           componentParameters.second);
    return clonedMultiComponentState;
  }

  // Define the path correction
  pathCorrection =
    pathCorrection > 0.
      ? pathCorrection
      : layer.surfaceRepresentation().pathCorrection(
          trackParameters->position(), trackParameters->momentum());
  // Scale the correction by the pre-update factor
  pathCorrection *= postUpdateFactor;

  // The pathlength ( in mm ) is the path correction * the thickness of the
  // material
  double pathLength = pathCorrection * materialProperties->thickness();

  return compute(componentParameters,
                 *materialProperties,
                 pathLength,
                 direction,
                 particleHypothesis);
}

/* ============================================================================
   Compute method
   ============================================================================
 */

Trk::MultiComponentState
Trk::GsfMaterialEffectsUpdator::compute(
  const Trk::ComponentParameters& componentParameters,
  const Trk::MaterialProperties& materialProperties,
  double pathLength,
  Trk::PropDirection direction,
  Trk::ParticleHypothesis particleHypothesis) const
{

  double momentum = componentParameters.first->momentum().mag();

  if (momentum <= m_momentumCut) {
    ATH_MSG_DEBUG("Ignoring material effects... Momentum too low");
    Trk::MultiComponentState clonedMultiComponentState{};
    clonedMultiComponentState.emplace_back(componentParameters.first->clone(),
                                           componentParameters.second);
    return clonedMultiComponentState;
  }

  /* Get values for:
   - Change in momentum
   - Change in covariance of momentum
   - Weights of new states
   */
  const Trk::TrackParameters* trackParameters = componentParameters.first.get();
  const AmgSymMatrix(5)* measuredCov = trackParameters->covariance();

  Trk::IMultiStateMaterialEffects::Cache cache;
  m_materialEffects->compute(cache,
                             componentParameters,
                             materialProperties,
                             pathLength,
                             direction,
                             particleHypothesis);

  // check all vectors have the same size
  if (cache.weights.size() != cache.deltaPs.size()) {
    ATH_MSG_ERROR(
      "Inconsistent number of components in the updator!!! return nullptr");
    return {};
  }

  Trk::MultiComponentState computedState{};
  computedState.reserve(cache.weights.size());

  // Prepare  an output state
  unsigned int componentIndex = 0;
  for (; componentIndex < cache.weights.size(); ++componentIndex) {
    AmgVector(5) updatedStateVector = trackParameters->parameters();

    // Adjust the momentum of the component's parameters vector here. Check to
    // make sure update is good.
    if (!updateP(updatedStateVector, cache.deltaPs[componentIndex])) {
      ATH_MSG_ERROR("Cannot update state vector momentum!!! return nullptr");
      return {};
    }

    AmgSymMatrix(5)* updatedCovariance = nullptr;
    if (measuredCov && cache.deltaCovariances.size() > componentIndex) {
      updatedCovariance = new AmgSymMatrix(5)(
        cache.deltaCovariances[componentIndex] + *measuredCov);
    }
    Trk::TrackParameters* updatedTrackParameters =
      trackParameters->associatedSurface().createTrackParameters(
        updatedStateVector[Trk::loc1],
        updatedStateVector[Trk::loc2],
        updatedStateVector[Trk::phi],
        updatedStateVector[Trk::theta],
        updatedStateVector[Trk::qOverP],
        updatedCovariance);
    double updatedWeight =
      componentParameters.second * cache.weights[componentIndex];
    computedState.emplace_back(updatedTrackParameters, updatedWeight);
  }
  return computedState;
}

/* ============================================================================
   updateP method
   ============================================================================
 */

bool
Trk::GsfMaterialEffectsUpdator::updateP(AmgVector(5) & stateVector,
                                        double deltaP) const
{
  double p = 1. / std::abs(stateVector[Trk::qOverP]);
  p += deltaP;
  if (p <= 0.) {
    return false;
  }
  double updatedIp = stateVector[Trk::qOverP] > 0. ? 1. / p : -1. / p;
  stateVector[Trk::qOverP] = updatedIp;
  return true;
}
