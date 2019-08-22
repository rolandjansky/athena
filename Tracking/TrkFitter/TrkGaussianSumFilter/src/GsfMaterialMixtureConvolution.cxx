/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*************************************************************************************
      GsfMaterialMixtureConvolution.cxx  -  description
      -------------------------------------------------
begin                : Thursday 7th September 2006
author               : atkinson
email                : Tom.Atkinson@cern.ch
decription           : Implementation code for GSF material mixture convolution
************************************************************************************/

#include "TrkGaussianSumFilter/GsfMaterialMixtureConvolution.h"
#include "TrkGaussianSumFilter/IMultiStateMaterialEffectsUpdator.h"
#include "TrkGeometry/Layer.h"
#include "TrkGeometry/MaterialProperties.h"
#include "TrkMultiComponentStateOnSurface/MultiComponentState.h"
#include "TrkSurfaces/PerigeeSurface.h"

Trk::GsfMaterialMixtureConvolution::GsfMaterialMixtureConvolution(const std::string& type,
                                                                  const std::string& name,
                                                                  const IInterface* parent)
  : AthAlgTool(type, name, parent)
{
  declareInterface<IMaterialMixtureConvolution>(this);
}

Trk::GsfMaterialMixtureConvolution::~GsfMaterialMixtureConvolution() {}

StatusCode
Trk::GsfMaterialMixtureConvolution::initialize()
{

  if (m_updator.retrieve().isFailure()) {
    ATH_MSG_ERROR("Could not retrieve the material effects updator instance " << m_updator.typeAndName()
                                                                              << "... Exiting");
    return StatusCode::FAILURE;
  }

  // Retrieve the multi-state combiner
  if (m_stateCombiner.retrieve().isFailure()) {
    ATH_MSG_ERROR("Could not retrieve the multi-component state combiner... Exiting");
    return StatusCode::FAILURE;
  }

  // Retrieve the state assembler ( a new instance )
  if (m_stateAssembler.retrieve().isFailure()) {
    ATH_MSG_ERROR("Could not retrieve the multi-component state assembler... Exiting");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode
Trk::GsfMaterialMixtureConvolution::finalize()
{
  return StatusCode::SUCCESS;
}

/* ==========================================
   Update with full material effects
   ========================================== */

const Trk::MultiComponentState*
Trk::GsfMaterialMixtureConvolution::update(const Trk::MultiComponentState& multiComponentState,
                                           const Trk::Layer& layer,
                                           Trk::PropDirection direction,
                                           Trk::ParticleHypothesis particleHypothesis) const
{

  const Trk::MaterialProperties* materialProperties =
    layer.fullUpdateMaterialProperties(*(multiComponentState.begin()->first));

  if (!materialProperties) {
    ATH_MSG_DEBUG("UPDATE but no material properties!!!");
  }

  /* -------------------------------------
     Preliminary checks
     ------------------------------------- */

  // Assembler Cache
  IMultiComponentStateAssembler::Cache cache;
  // Reset the assembler and check
  bool isAssemblerReset = m_stateAssembler->reset(cache);

  if (!isAssemblerReset) {
    ATH_MSG_ERROR("Could not reset the state assembler... returning clone of original state");
    return multiComponentState.clone();
  }

  // Check the multi-component state is populated
  if (multiComponentState.empty()) {
    ATH_MSG_DEBUG("Multi component state passed to extrapolateInsideVolume is not populated... returning 0");
    return 0;
  }

  // Loop over all components and perform material effects update separately
  Trk::MultiComponentState::const_iterator component = multiComponentState.begin();

  for (; component != multiComponentState.end(); ++component) {

    std::unique_ptr<Trk::SimpleMultiComponentState> updatedState =
      m_updator->updateState(*component, layer, direction, particleHypothesis);

    if(!updatedState)
      continue;

    bool componentAdded = m_stateAssembler->addMultiState(cache, std::move(*updatedState));

    if (!componentAdded) {
      ATH_MSG_WARNING("Component could not be added to the state in the assembler");
    }
  }

  Trk::MultiComponentState* assembledState = m_stateAssembler->assembledState(cache);

  if (!assembledState) {
    return nullptr;
  }
  // Renormalise state
  assembledState->renormaliseState();

  return assembledState;
}

/* ==========================================
   Update with pre-update material effects
   ========================================== */

const Trk::MultiComponentState*
Trk::GsfMaterialMixtureConvolution::preUpdate(const Trk::MultiComponentState& multiComponentState,
                                              const Trk::Layer& layer,
                                              Trk::PropDirection direction,
                                              Trk::ParticleHypothesis particleHypothesis) const
{
  const Trk::MaterialProperties* materialProperties =
    layer.fullUpdateMaterialProperties(*(multiComponentState.begin()->first));
  if (!materialProperties) {
    ATH_MSG_DEBUG("PREUPDATE but no material properties!!!");
  }
  /* -------------------------------------
     Preliminary checks
     ------------------------------------- */
  // Assembler Cache
  IMultiComponentStateAssembler::Cache cache;
  // Reset the assembler and check
  bool isAssemblerReset = m_stateAssembler->reset(cache);

  if (!isAssemblerReset) {
    ATH_MSG_ERROR("Could not reset the state assembler... returning clone of original state");
    return multiComponentState.clone();
  }

  // Check the multi-component state is populated
  if (multiComponentState.empty()) {
    ATH_MSG_DEBUG("Multi component state passed to extrapolateInsideVolume is not populated... returning 0");
    return 0;
  }

  // Loop over all components and perform material effects update separately
  Trk::MultiComponentState::const_iterator component = multiComponentState.begin();

  for (; component != multiComponentState.end(); ++component) {

    std::unique_ptr<Trk::SimpleMultiComponentState> updatedState =
      m_updator->preUpdateState(*component, layer, direction, particleHypothesis);

    if(!updatedState)
      continue;

    bool componentAdded = m_stateAssembler->addMultiState(cache, std::move(*updatedState));

    if (!componentAdded)
      ATH_MSG_WARNING("Component could not be added to the state in the assembler");
  }

  Trk::MultiComponentState* assembledState = m_stateAssembler->assembledState(cache);

  if (!assembledState) {
    return nullptr;
  }

  // Renormalise state
  assembledState->renormaliseState();

  // Clean up memory
  return assembledState;
}

/* ==========================================
   Update with post-update material effects
   ========================================== */

const Trk::MultiComponentState*
Trk::GsfMaterialMixtureConvolution::postUpdate(const Trk::MultiComponentState& multiComponentState,
                                               const Trk::Layer& layer,
                                               Trk::PropDirection direction,
                                               Trk::ParticleHypothesis particleHypothesis) const
{

  const Trk::MaterialProperties* materialProperties =
    layer.fullUpdateMaterialProperties(*(multiComponentState.begin()->first));
  if (!materialProperties) {
    ATH_MSG_DEBUG("POSTUPDATE but no material properties!!!");
  }
  /* -------------------------------------
     Preliminary checks
     ------------------------------------- */

  // Assembler Cache
  IMultiComponentStateAssembler::Cache cache;
  // Reset the assembler and check
  bool isAssemblerReset = m_stateAssembler->reset(cache);

  if (!isAssemblerReset) {
    ATH_MSG_WARNING("Could not reset the state assembler... returning clone of original state");
    return multiComponentState.clone();
  }

  // Check the multi-component state is populated
  if (multiComponentState.empty()) {
    ATH_MSG_DEBUG("Multi component state passed to extrapolateInsideVolume is not populated... returning 0");
    return 0;
  }

  // Loop over all components and perform material effects update separately
  Trk::MultiComponentState::const_iterator component = multiComponentState.begin();

  for (; component != multiComponentState.end(); ++component) {

    std::unique_ptr<Trk::SimpleMultiComponentState> updatedState =
      m_updator->postUpdateState(*component, layer, direction, particleHypothesis);

    if(!updatedState)
      continue;

    bool componentAdded = m_stateAssembler->addMultiState(cache, std::move(*updatedState));

    if (!componentAdded) {
      ATH_MSG_WARNING("Component could not be added to the state in the assembler");
    }
  }

  Trk::MultiComponentState* assembledState = m_stateAssembler->assembledState(cache);

  if (!assembledState) {
    return nullptr;
  }

  assembledState->renormaliseState();

  return assembledState;
}

/* ==========================================
   Update with simplified material effects
   ========================================== */

const Trk::MultiComponentState*
Trk::GsfMaterialMixtureConvolution::simpliedMaterialUpdate(const Trk::MultiComponentState& multiComponentState,
                                                           Trk::PropDirection direction,
                                                           Trk::ParticleHypothesis particleHypothesis) const
{
  /* -------------------------------------
     Preliminary checks
     ------------------------------------- */
  // Assembler Cache
  IMultiComponentStateAssembler::Cache cache;
  // Reset the assembler and check
  bool isAssemblerReset = m_stateAssembler->reset(cache);

  if (!isAssemblerReset) {
    ATH_MSG_WARNING("Could not reset the state assembler... returning clone of original state");
    return multiComponentState.clone();
  }

  // Check the multi-component state is populated
  if (multiComponentState.empty()) {
    ATH_MSG_DEBUG("Multi component state passed to extrapolateInsideVolume is not populated... returning 0");
    return 0;
  }

  // Hardwired material effects based on approximate material distribution
  const Trk::TrackParameters* combinedState = m_stateCombiner->combine(multiComponentState);
  const Amg::Vector3D& globalPosition = combinedState->position();

  const Trk::MaterialProperties* materialProperties = 0;

  // Material properties 2.5% X0 - pixels
  if (globalPosition.x() >= 180. && globalPosition.x() < 350.) {
    materialProperties = new Trk::MaterialProperties(2.34, 93.6, 0.3879, 28.0855, 14, 0.00233);
  }

  // Material properties 0.75% X0 - SCT ( total for a module = 1.5% - 2 measurement surfaces )
  else if (globalPosition.x() >= 350. && globalPosition.x() < 1200.) {
    materialProperties = new Trk::MaterialProperties(0.702, 93.6, 0.3879, 28.0855, 14, 0.00233);
  }

  if (!materialProperties) {
    return multiComponentState.clone();
  }

  // Exclude material effects on the perigee surface
  const Trk::Surface* associatedSurface = &(combinedState->associatedSurface());
  const Trk::PerigeeSurface* perigeeSurface = nullptr;
  if (associatedSurface && associatedSurface->type() == Trk::Surface::Perigee) {
    perigeeSurface = static_cast<const Trk::PerigeeSurface*>(associatedSurface);
  }
  if (perigeeSurface) {
    delete materialProperties;
    return multiComponentState.clone();
  }

  // Assume tracks normal to detector surface. Approximation resonable for the CTB
  double pathLength = materialProperties->thickness();

  // Loop over all components and perform material effects update separately
  Trk::MultiComponentState::const_iterator component = multiComponentState.begin();

  for (; component != multiComponentState.end(); ++component) {
    std::unique_ptr<Trk::SimpleMultiComponentState> updatedState =
      m_updator->updateState(*component, *materialProperties, pathLength, direction, particleHypothesis);

    if(!updatedState)
      continue;

    bool componentAdded = m_stateAssembler->addMultiState(cache, std::move(*updatedState));

    if (!componentAdded) {
      ATH_MSG_WARNING("Component could not be added to the state in the assembler");
    }

  } // end loop over components

  Trk::MultiComponentState* assembledState = m_stateAssembler->assembledState(cache);
  // Renormalise the state
  if(!assembledState)
  {
    delete materialProperties;
    return nullptr;
  }

  assembledState->renormaliseState();

  // Clean up memory
  return assembledState;
}
