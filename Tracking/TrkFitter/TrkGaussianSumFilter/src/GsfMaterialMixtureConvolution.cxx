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

#include "TrkMultiComponentStateOnSurface/MultiComponentState.h"

#include "TrkGaussianSumFilter/IMultiStateMaterialEffectsUpdator.h"
#include "TrkGaussianSumFilter/IMultiComponentStateAssembler.h"
#include "TrkGaussianSumFilter/IMultiComponentStateCombiner.h"

#include "TrkSurfaces/PerigeeSurface.h"

#include "TrkGeometry/MaterialProperties.h"
#include "TrkGeometry/Layer.h"

Trk::GsfMaterialMixtureConvolution::GsfMaterialMixtureConvolution( const std::string& type, const std::string& name, const IInterface* parent )
  :
  AthAlgTool( type, name, parent ),
  m_outputlevel(0),
  m_updator("Trk::GsfMaterialEffectsUpdator/GsfMaterialEffectsUpdator"),
  m_stateCombiner("Trk::MultiComponentStateCombiner/MultiComponentStateCombiner"),
  m_stateAssembler("Trk::MultiComponentStateAssembler/MaterialConvolutionAssembler")
{

  declareInterface< IMaterialMixtureConvolution >( this );

  declareProperty( "MaterialEffectsUpdator", m_updator );

}

Trk::GsfMaterialMixtureConvolution::~GsfMaterialMixtureConvolution()
{}

StatusCode Trk::GsfMaterialMixtureConvolution::initialize()
{

  m_outputlevel = msg().level()-MSG::DEBUG;   // save the threshold for debug printout in private member
  
  if ( m_updator.retrieve().isFailure() ){
    msg(MSG::FATAL) << "Could not retrieve the material effects updator instance " << m_updator.typeAndName() << "... Exiting" << endmsg;
    return StatusCode::FAILURE;
  }

  // Retrieve the multi-state combiner
  if ( m_stateCombiner.retrieve().isFailure() ){
    msg(MSG::FATAL) << "Could not retrieve the multi-component state combiner... Exiting" << endmsg;
    return StatusCode::FAILURE;
  }

  // Retrieve the state assembler ( a new instance )
  if ( m_stateAssembler.retrieve().isFailure() ){
    msg(MSG::FATAL) << "Could not retrieve the multi-component state assembler... Exiting" << endmsg;
    return StatusCode::FAILURE;
  }

  msg(MSG::INFO) << "Initialisation of " << name() << " was successful" << endmsg;

  return StatusCode::SUCCESS;

}

StatusCode Trk::GsfMaterialMixtureConvolution::finalize()
{

  msg(MSG::INFO) << "Finalisation of " << name() << " was successful" << endmsg;

  return StatusCode::SUCCESS;

}

/* ==========================================
   Update with full material effects
   ========================================== */

const Trk::MultiComponentState* Trk::GsfMaterialMixtureConvolution::update( const Trk::MultiComponentState& multiComponentState,
                      const Trk::Layer& layer,
                      Trk::PropDirection direction,
                      Trk::ParticleHypothesis particleHypothesis ) const
{

  if (m_outputlevel < 0) msg(MSG::VERBOSE) << "Applying full material effects" << endmsg;


  const Trk::MaterialProperties* materialProperties = layer.fullUpdateMaterialProperties( *(multiComponentState.begin()->first) );
  if (!materialProperties)
    ATH_MSG_DEBUG( "UPDATE but no material properties!!!");
  


  /* -------------------------------------
     Preliminary checks
     ------------------------------------- */
  
  // Reset the assembler and check 
  bool isAssemblerReset = m_stateAssembler->reset();

  if ( !isAssemblerReset ){
    msg(MSG::ERROR) << "Could not reset the state assembler... returning clone of original state" << endmsg;
    return multiComponentState.clone();
  }
  
  // Check the multi-component state is populated
  if ( multiComponentState.empty() ){
    if (m_outputlevel <= 0) msg(MSG::DEBUG) << "Multi component state passed to extrapolateInsideVolume is not populated... returning 0" << endmsg;
    return 0;
  }
  
  // Loop over all components and perform material effects update separately
  Trk::MultiComponentState::const_iterator component = multiComponentState.begin();

  for ( ; component != multiComponentState.end(); ++component ){

    const Trk::MultiComponentState* updatedState = m_updator->updateState( *component, layer, direction, particleHypothesis );

    if ( !updatedState ){
      //msg(MSG::WARNING) << "Material effects update failed... returning 0" << endmsg;
      continue;
    }

    bool componentAdded = m_stateAssembler->addMultiState( *updatedState );
    
    // New memory allocated in the combiner so it is ok to delete the original state
    delete updatedState;

    if ( !componentAdded )
      msg(MSG::WARNING) << "Component could not be added to the state in the assembler" << endmsg;
    
  }
  
  const Trk::MultiComponentState* assembledState = m_stateAssembler->assembledState();
   
  if(!assembledState)
    return 0;
      
  // Renormalise state
  const Trk::MultiComponentState* renormalisedState = assembledState->clonedRenormalisedState();
  
  // Clean up memory
  delete assembledState;

  return renormalisedState;

}

/* ==========================================
   Update with pre-update material effects
   ========================================== */

const Trk::MultiComponentState* Trk::GsfMaterialMixtureConvolution::preUpdate( const Trk::MultiComponentState& multiComponentState,
                         const Trk::Layer& layer,
                         Trk::PropDirection direction,
                         Trk::ParticleHypothesis particleHypothesis ) const
{

  ATH_MSG_VERBOSE( "Applying pre-propagation material effects" );


  const Trk::MaterialProperties* materialProperties = layer.fullUpdateMaterialProperties( *(multiComponentState.begin()->first) );
  if (!materialProperties)
    ATH_MSG_DEBUG( "PREUPDATE but no material properties!!!" );
 

  /* -------------------------------------
     Preliminary checks
     ------------------------------------- */

  // Reset the assembler and check 
  bool isAssemblerReset = m_stateAssembler->reset();

  if ( !isAssemblerReset ){
    msg(MSG::ERROR) << "Could not reset the state assembler... returning clone of original state" << endmsg;
    return multiComponentState.clone();
  }
  
  // Check the multi-component state is populated
  if ( multiComponentState.empty() ){
    if (m_outputlevel <= 0) msg(MSG::DEBUG) << "Multi component state passed to extrapolateInsideVolume is not populated... returning 0" << endmsg;
    return 0;
  }
  
  // Loop over all components and perform material effects update separately
  Trk::MultiComponentState::const_iterator component = multiComponentState.begin();

  for ( ; component != multiComponentState.end(); ++component ){

    const Trk::MultiComponentState* updatedState = m_updator->preUpdateState( *component, layer, direction, particleHypothesis );
     
    if ( !updatedState ){
      //msg(MSG::WARNING) << "Material effects update failed... returning 0" << endmsg;
      continue;
    }

    bool componentAdded = m_stateAssembler->addMultiState( *updatedState );
    
    // New memory allocated in the combiner so it is ok to delete the original state
    delete updatedState;

    if ( !componentAdded )
      msg(MSG::WARNING) << "Component could not be added to the state in the assembler" << endmsg;
    
  }
  
  const Trk::MultiComponentState* assembledState = m_stateAssembler->assembledState();
  
 if(!assembledState)
    return 0;
      
  // Renormalise state
  const Trk::MultiComponentState* renormalisedState = assembledState->clonedRenormalisedState();
  
  // Clean up memory
  delete assembledState;

  return renormalisedState;

}

/* ==========================================
   Update with post-update material effects
   ========================================== */

const Trk::MultiComponentState* Trk::GsfMaterialMixtureConvolution::postUpdate( const Trk::MultiComponentState& multiComponentState,
                    const Trk::Layer& layer,
                    Trk::PropDirection direction,
                    Trk::ParticleHypothesis particleHypothesis ) const
{

  if (m_outputlevel < 0)
    msg(MSG::VERBOSE) << "Applying post-propagation material effects" << endmsg;


  const Trk::MaterialProperties* materialProperties = layer.fullUpdateMaterialProperties( *(multiComponentState.begin()->first) );
  if (!materialProperties)
    ATH_MSG_DEBUG("POSTUPDATE but no material properties!!!");
  
  /* -------------------------------------
     Preliminary checks
     ------------------------------------- */

  // Reset the assembler and check 
  bool isAssemblerReset = m_stateAssembler->reset();

  if ( !isAssemblerReset ){
    msg(MSG::ERROR) << "Could not reset the state assembler... returning clone of original state" << endmsg;
    return multiComponentState.clone();
  }
  
  // Check the multi-component state is populated
  if ( multiComponentState.empty() ){
    if (m_outputlevel <= 0) msg(MSG::DEBUG) << "Multi component state passed to extrapolateInsideVolume is not populated... returning 0" << endmsg;
    return 0;
  }
  
  // Loop over all components and perform material effects update separately
  Trk::MultiComponentState::const_iterator component = multiComponentState.begin();

  for ( ; component != multiComponentState.end(); ++component ){

    const Trk::MultiComponentState* updatedState = m_updator->postUpdateState( *component, layer, direction, particleHypothesis );
    
    if ( !updatedState ){
      //msg(MSG::WARNING) << "Material effects update failed... returning 0" << endmsg;
      continue;
    }

    bool componentAdded = m_stateAssembler->addMultiState( *updatedState );
    
    // New memory allocated in the combiner so it is ok to delete the original state
    delete updatedState;

    if ( !componentAdded )
      msg(MSG::WARNING) << "Component could not be added to the state in the assembler" << endmsg;
    
  }
  
  const Trk::MultiComponentState* assembledState = m_stateAssembler->assembledState();
  
  if(!assembledState)
    return 0;
      
  // Renormalise state
  const Trk::MultiComponentState* renormalisedState = assembledState->clonedRenormalisedState();
  
  // Clean up memory
  delete assembledState;

  return renormalisedState;

}

/* ==========================================
   Update with simplified material effects
   ========================================== */

const Trk::MultiComponentState* Trk::GsfMaterialMixtureConvolution::simpliedMaterialUpdate ( const Trk::MultiComponentState& multiComponentState,
                           Trk::PropDirection direction,
                           Trk::ParticleHypothesis particleHypothesis ) const
{

  if (m_outputlevel < 0) 
    msg(MSG::VERBOSE) << "Simplifed material effects update" << endmsg;

  /* -------------------------------------
     Preliminary checks
     ------------------------------------- */

  // Reset the assembler and check 
  bool isAssemblerReset = m_stateAssembler->reset();

  if ( !isAssemblerReset ){
    msg(MSG::ERROR) << "Could not reset the state assembler... returning clone of original state" << endmsg;
    return multiComponentState.clone();
  }
  
  // Check the multi-component state is populated
  if ( multiComponentState.empty() ){
    if (m_outputlevel <= 0)
      msg(MSG::DEBUG) << "Multi component state passed to extrapolateInsideVolume is not populated... returning 0" << endmsg;
    return 0;
  }

  // Hardwired material effects based on approximate material distribution
  const Trk::TrackParameters* combinedState = m_stateCombiner->combine( multiComponentState );
  const Amg::Vector3D& globalPosition = combinedState->position();

  const Trk::MaterialProperties* materialProperties = 0;

  // Material properties 2.5% X0 - pixels
  if ( globalPosition.x() >= 180. && globalPosition.x() < 350. ){
    materialProperties = new Trk::MaterialProperties( 2.34, 93.6, 0.3879, 28.0855, 14, 0.00233 );
  }

  // Material properties 0.75% X0 - SCT ( total for a module = 1.5% - 2 measurement surfaces )
  else if ( globalPosition.x() >= 350. && globalPosition.x() < 1200. ){
    materialProperties = new Trk::MaterialProperties( 0.702, 93.6, 0.3879, 28.0855, 14, 0.00233 );
  }

  // No hardwired material effects in the TRT - too hard
  else if ( globalPosition.x() >= 1200. )
    if (m_outputlevel < 0)
      msg(MSG::VERBOSE) << "Extrapolating in the TRT... turining material effects off" << endmsg;

  if ( !materialProperties ){
    if (m_outputlevel < 0)
      msg(MSG::VERBOSE) << "No MaterialProperties associated with surface" << endmsg;
    return multiComponentState.clone();
  }

  // Exclude material effects on the perigee surface
  const Trk::Surface* associatedSurface     = &(combinedState->associatedSurface());
  const Trk::PerigeeSurface* perigeeSurface = dynamic_cast<const Trk::PerigeeSurface*>(associatedSurface);
    
  if ( perigeeSurface ){
    if (m_outputlevel < 0)
      msg(MSG::VERBOSE) << "Material effects excluded at perigee surface" << endmsg;
    delete materialProperties;
    return multiComponentState.clone();
  }
    
  // Assume tracks normal to detector surface. Approximation resonable for the CTB
  double pathLength = materialProperties->thickness();
  
  // Loop over all components and perform material effects update separately
  Trk::MultiComponentState::const_iterator component = multiComponentState.begin();
  
  for ( ; component != multiComponentState.end(); ++component){
    
    const Trk::MultiComponentState* updatedState = m_updator->updateState( *component, *materialProperties, pathLength, direction, particleHypothesis );
    
    bool componentAdded = false;
    
    if ( updatedState )
      componentAdded = m_stateAssembler->addMultiState( *updatedState );
    
    if ( !componentAdded )
      msg(MSG::WARNING) << "Component could not be added to the state in the assembler" << endmsg;
    
    // Clean up memory allocated in the material effects updator
    delete updatedState;
    
  } // end loop over components
  
  const Trk::MultiComponentState* assembledState = m_stateAssembler->assembledState();
  
  // Renormalise the state
  const Trk::MultiComponentState* renormalisedState = assembledState->clonedRenormalisedState();
  
  // Clean up memory
  delete assembledState;

  delete materialProperties;

  return renormalisedState;

}
