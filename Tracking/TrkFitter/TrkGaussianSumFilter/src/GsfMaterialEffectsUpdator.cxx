/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***********************************************************************************
			GsfMaterialEffectsUpdator.cxx  -  description
			---------------------------------------------
begin                : Wednesday 9th January 2005
author               : atkinson
email                : Tom.Atkinson@cern.ch
decription           : Implementation code for the class GsfMaterialEffectsUpdator
***********************************************************************************/

#include "TrkGaussianSumFilter/GsfMaterialEffectsUpdator.h"

#include "TrkGaussianSumFilter/MultiComponentStateAssembler.h"
#include "TrkGaussianSumFilter/IMultiStateMaterialEffects.h"

#include "TrkParameters/TrackParameters.h"
#include "TrkSurfaces/Surface.h"

#include "TrkGeometry/MaterialProperties.h"
#include "TrkGeometry/Layer.h"

Trk::GsfMaterialEffectsUpdator::GsfMaterialEffectsUpdator(const std::string& type, const std::string& name, const IInterface* parent)
  :
  AthAlgTool(type, name, parent),
  m_outputlevel(0),
  m_stateAssembler("Trk::MultiComponentStateAssembler/GsfMaterialEffectsStateAssembler"),
  m_materialEffects("Trk::GsfCombinedMaterialEffects/GsfCombinedMaterialEffects"),
  m_useReferenceMaterial(false),
  m_momentumCut(250.*Gaudi::Units::MeV)
{

  declareInterface<IMultiStateMaterialEffectsUpdator>(this);

  declareProperty("MaterialEffects",         m_materialEffects         );
  declareProperty("UseReferenceMaterial",    m_useReferenceMaterial        );
  declareProperty("MinimalMomentum",         m_momentumCut                 );

}

Trk::GsfMaterialEffectsUpdator::~GsfMaterialEffectsUpdator ()
{}

StatusCode Trk::GsfMaterialEffectsUpdator::initialize ()
{

  m_outputlevel = msg().level()-MSG::DEBUG;   // save the threshold for debug printout in private member

  // Retrieve the specific material effects
  if ( m_materialEffects.retrieve().isFailure() ){
    msg(MSG::FATAL)
	  << "Could not retrieve Material Effects AlgTool: " << m_materialEffects.typeAndName()
	  << "... Exiting!" << endmsg;
    return StatusCode::FAILURE;
  }

  // Retrieve an instance of the multi-component state assembler service
  if ( m_stateAssembler.retrieve().isFailure() ){
    msg(MSG::FATAL) << "Could not retrieve multi-component state assembler tool... Exiting!" << endmsg;
    return StatusCode::FAILURE;
  }

  msg(MSG::INFO) << "Initialisation of " << name() << " was successful" << endmsg;

  return StatusCode::SUCCESS;

}

StatusCode Trk::GsfMaterialEffectsUpdator::finalize()
{

  msg(MSG::INFO) << "Finalisation of " << name() << " was successful" << endmsg;

  return StatusCode::SUCCESS;

}

/* ============================================================================
   ============================================================================
   Full update based on layer information
   ============================================================================
   ============================================================================ */

const Trk::MultiComponentState* Trk::GsfMaterialEffectsUpdator::updateState ( const Trk::ComponentParameters& componentParameters,
									      const Trk::Layer& layer,
									      Trk::PropDirection direction,
									      Trk::ParticleHypothesis particleHypothesis ) const
{
  
  if (m_outputlevel < 0)
    msg(MSG::VERBOSE) << "Material effects update using layer information and particle hypothesis: " << particleHypothesis << endmsg;
   
  const Trk::TrackParameters* trackParameters = componentParameters.first;
  
  if (!trackParameters){
    msg(MSG::ERROR) << "Trying to update component without trackParameters... returing component!" << endmsg;
    const Trk::ComponentParameters* clonedParameters = componentParameters.clone();
    const Trk::MultiComponentState* clonedMultiComponentState = new Trk::MultiComponentState( *clonedParameters );
    delete clonedParameters;
    return clonedMultiComponentState;
  }
    
  // Extract the material properties from the layer
  const Trk::MaterialProperties* materialProperties(0);
  double pathCorrection(0.);

  if ( m_useReferenceMaterial ){

    // Get the surface associated with the parameters
    const Trk::Surface* surface = &(trackParameters->associatedSurface());

    // Only utilise the reference material if an associated detector element exists
    if ( surface && surface->associatedDetectorElement() ){

      // Get the layer material properties
      const Trk::LayerMaterialProperties* layerMaterial = layer.layerMaterialProperties();

      // Assign the material properties
      materialProperties = layerMaterial ? layerMaterial->fullMaterial( trackParameters->position() ) : 0;
	
      // Determine the pathCorrection if the material properties exist
      pathCorrection = materialProperties ? 1. / fabs( surface->normal().dot( trackParameters->momentum().unit() ) ) : 0.;

    }

  }

  // Check that the material properties have been defined - if not define them from the layer information
  materialProperties = materialProperties ? materialProperties : layer.fullUpdateMaterialProperties( *trackParameters );

  // Bail out if still no material properties can be found
  if ( !materialProperties ){
    if (m_outputlevel < 0)
      msg(MSG::VERBOSE) << "No material properties found for this layer... cloning initial state" << endmsg;
    const Trk::ComponentParameters* clonedParameters = componentParameters.clone();
    const Trk::MultiComponentState* clonedMultiComponentState = new Trk::MultiComponentState( *clonedParameters );
    delete clonedParameters;
    return clonedMultiComponentState;
  }


  // Define the path correction
  pathCorrection = pathCorrection > 0. ? pathCorrection : layer.surfaceRepresentation().pathCorrection(trackParameters->position(),trackParameters->momentum());

  // The pathlength ( in mm ) is the path correction * the thickness of the material
  double pathLength = pathCorrection * materialProperties->thickness();
  //std::cout << "1PL, PC, X0  :" << pathLength  << '\t' << pathCorrection<< '\t' <<pathLength / materialProperties->x0() <<'\t' << materialProperties->thickness() <<std::endl;

 
  ATH_MSG_DEBUG("Calculated path-length (mm): " << pathLength << "\t" << pathLength / materialProperties->x0());


  const Trk::MultiComponentState* updatedState = compute( componentParameters, *materialProperties, pathLength, direction, particleHypothesis );
  
  return updatedState;
}

/* ============================================================================
   ============================================================================
   Full update based on path-length & material properties information
   ============================================================================
   ============================================================================ */

const Trk::MultiComponentState* Trk::GsfMaterialEffectsUpdator::updateState ( const Trk::ComponentParameters& componentParameters,
									      const Trk::MaterialProperties& materialProperties,
									      double pathLength,
									      Trk::PropDirection direction,
									      Trk::ParticleHypothesis particleHypothesis ) const
{
  
  if (m_outputlevel < 0)
    msg(MSG::VERBOSE) << "Material effects update using material properties and path length information" << endmsg;
  
  const Trk::MultiComponentState* updatedState = 0;
    
  updatedState = compute( componentParameters, materialProperties, pathLength, direction, particleHypothesis );

  return updatedState;

}

/* ============================================================================
   ============================================================================
   Pre-update based on layer information
   ============================================================================
   ============================================================================ */

const Trk::MultiComponentState* Trk::GsfMaterialEffectsUpdator::preUpdateState( const Trk::ComponentParameters& componentParameters,
										const Trk::Layer& layer,
										Trk::PropDirection direction,
										Trk::ParticleHypothesis particleHypothesis ) const
{
  
  if (m_outputlevel < 0)
    msg(MSG::VERBOSE) << "Material effects update prior to propagation using layer information and particle hypothesis: " << particleHypothesis << endmsg;
   
  const Trk::TrackParameters* trackParameters = componentParameters.first;
  
  if (!trackParameters){
    msg(MSG::ERROR) << "Trying to update component without trackParameters... returing 0!" << endmsg;
    return 0;
  }

  // Get the pre-update factor
  double preUpdateFactor = layer.preUpdateMaterialFactor( *trackParameters, direction );

  // Bail if the pre-update factor is small
  if ( preUpdateFactor < 0.01 ){
    if (m_outputlevel < 0)
      msg(MSG::VERBOSE) << "No pre-update necessary... return 0" << endmsg;
    return 0;
  }

  const Trk::MaterialProperties* materialProperties(0);
  double pathCorrection(0.);

  // Incorporate the reference material

  if ( m_useReferenceMaterial ){

    // Get the surface associated with the parameters
    const Trk::Surface* surface = &(trackParameters->associatedSurface());

    // Only utilise the reference material if an associated detector element exists
    if ( surface && surface->associatedDetectorElement() ){

      // Get the layer material properties
      const Trk::LayerMaterialProperties* layerMaterial = layer.layerMaterialProperties();


      // Assign the material properties
      materialProperties = layerMaterial ? layerMaterial->fullMaterial( trackParameters->position() ) : 0;
	
      // Determine the pathCorrection if the material properties exist
      pathCorrection = materialProperties ? 1. / fabs( surface->normal().dot( trackParameters->momentum().unit() ) ) : 0.;

    }

  }

  // Check that the material properties have been defined - if not define them from the layer information
  materialProperties = materialProperties ? materialProperties : layer.fullUpdateMaterialProperties( *trackParameters );

  // Bail out if still no material properties can be found
  if ( !materialProperties ){
    if (m_outputlevel < 0)
      msg(MSG::VERBOSE) << "No material properties found for this layer... cloning initial state" << endmsg;
    const Trk::ComponentParameters* clonedParameters = componentParameters.clone();
    const Trk::MultiComponentState* clonedMultiComponentState = new Trk::MultiComponentState( *clonedParameters );
    delete clonedParameters;
    return clonedMultiComponentState;
  }


  // Define the path correction
  pathCorrection = pathCorrection > 0. ? pathCorrection : layer.surfaceRepresentation().pathCorrection(trackParameters->position(),trackParameters->momentum());
  
  // Scale the correction by the pre-update factor
  pathCorrection *= preUpdateFactor;

  // The pathlength ( in mm ) is the path correction * the thickness of the material
  double pathLength = pathCorrection * materialProperties->thickness();
  //std::cout << "2PL, PC, X0  :" << pathLength  << '\t' << pathCorrection<< '\t' <<pathLength / materialProperties->x0() <<std::endl;

  if (m_outputlevel <= 0)
     msg(MSG::DEBUG) << "Calculated path-length (mm): " << pathLength << "\t" << pathLength / materialProperties->x0() << endmsg;

  const Trk::MultiComponentState* updatedState = compute( componentParameters, *materialProperties, pathLength, direction, particleHypothesis );
  
  return updatedState;

}

/* ============================================================================
   ============================================================================
   Post-update based on layer information
   ============================================================================
   ============================================================================ */

const Trk::MultiComponentState* Trk::GsfMaterialEffectsUpdator::postUpdateState ( const Trk::ComponentParameters& componentParameters,
										  const Layer& layer,
										  PropDirection direction,
										  ParticleHypothesis particleHypothesis ) const
{
  
  if (m_outputlevel < 0)
    msg(MSG::VERBOSE) << "Material effects update after propagation using layer information and particle hypothesis: " << particleHypothesis << endmsg;
   
  const Trk::TrackParameters* trackParameters = componentParameters.first;
  
  if (!trackParameters){
    msg(MSG::ERROR) << "Trying to update component without trackParameters... returing component!" << endmsg;
    const Trk::ComponentParameters* clonedParameters = componentParameters.clone();
    const Trk::MultiComponentState* clonedMultiComponentState = new Trk::MultiComponentState( *clonedParameters );
    delete clonedParameters;
    return clonedMultiComponentState;
  }
    
  // Get the post-update factor
  double postUpdateFactor = layer.postUpdateMaterialFactor( *trackParameters, direction );
  ATH_MSG_VERBOSE("Post update factor: " << postUpdateFactor  );

  // Bail if the postUpdateFactor is small
  if ( postUpdateFactor < 0.01 ){
    if (m_outputlevel < 0)
      msg(MSG::VERBOSE) << "No material properties could be retrieved for this layer... cloning initial state" << endmsg;
    //const Trk::ComponentParameters* clonedParameters = componentParameters.clone();
    //const Trk::MultiComponentState* clonedMultiComponentState = new Trk::MultiComponentState( *clonedParameters );
    //delete clonedParameters;
    //return clonedMultiComponentState;
    return 0;
  }

  const Trk::MaterialProperties* materialProperties(0);
  double pathCorrection(0.);

  if ( m_useReferenceMaterial ){

    // Get the surface associated with the parameters
    const Trk::Surface* surface = &(trackParameters->associatedSurface());

    // Only utilise the reference material if an associated detector element exists
    if ( surface && surface->associatedDetectorElement() ){

      // Get the layer material properties
      const Trk::LayerMaterialProperties* layerMaterial = layer.layerMaterialProperties();

      // Assign the material properties
      materialProperties = layerMaterial ? layerMaterial->fullMaterial( trackParameters->position() ) : 0;
	
      // Determine the pathCorrection if the material properties exist
      pathCorrection = materialProperties ? 1. / fabs( surface->normal().dot( trackParameters->momentum().unit() ) ) : 0.;

    }

  }

  // Check that the material properties have been defined - if not define them from the layer information
  materialProperties = materialProperties ? materialProperties : layer.fullUpdateMaterialProperties( *trackParameters );

  // Bail out if still no material properties can be found
  if ( !materialProperties ){
    if (m_outputlevel < 0)
      msg(MSG::VERBOSE) << "No material properties found for this layer... cloning initial state" << endmsg;
    const Trk::ComponentParameters* clonedParameters = componentParameters.clone();
    const Trk::MultiComponentState* clonedMultiComponentState = new Trk::MultiComponentState( *clonedParameters );
    delete clonedParameters;
    return clonedMultiComponentState;
  }

  // Define the path correction
  pathCorrection = pathCorrection > 0. ? pathCorrection : layer.surfaceRepresentation().pathCorrection(trackParameters->position(),trackParameters->momentum());
  // Scale the correction by the pre-update factor
  pathCorrection *= postUpdateFactor;

  // The pathlength ( in mm ) is the path correction * the thickness of the material
  double pathLength = pathCorrection * materialProperties->thickness();

  //std::cout << "3PL, PC, X0  :" << pathLength  << '\t' << pathCorrection<< '\t' <<pathLength / materialProperties->x0() <<std::endl;

  ATH_MSG_DEBUG( "Calculated path-length (mm): " << pathLength << "\t" << pathLength / materialProperties->x0() );

  const Trk::MultiComponentState* updatedState = compute( componentParameters, *materialProperties, pathLength, direction, particleHypothesis );
  
  return updatedState;

}

/* ============================================================================
   ============================================================================
   Compute method
   ============================================================================
   ============================================================================ */

const Trk::MultiComponentState* Trk::GsfMaterialEffectsUpdator::compute ( const Trk::ComponentParameters& componentParameters,
									  const Trk::MaterialProperties& materialProperties,
									  double pathLength,
									  Trk::PropDirection direction,
									  Trk::ParticleHypothesis particleHypothesis ) const
{

  double momentum = componentParameters.first->momentum().mag();

  if ( momentum <= m_momentumCut ){

    if (m_outputlevel <= 0)
      msg(MSG::DEBUG) << "Ignoring material effects... Momentum too low" << endmsg;

    const Trk::ComponentParameters* clonedParameters = componentParameters.clone();
    const Trk::MultiComponentState* clonedMultiComponentState = new Trk::MultiComponentState( *clonedParameters );
    delete clonedParameters;
    return clonedMultiComponentState;

  }

  // msg(MSG::VERBOSE) << "Computing updated parameters, covariances and weights" << endmsg;

  Trk::MultiComponentState* computedState = new Trk::MultiComponentState();

  // Check that the assember is reset
  bool isAssemblerReset = m_stateAssembler->reset();

  if ( !isAssemblerReset ){
    msg(MSG::ERROR) << "Could not reset the state assembler... returning 0" << endmsg;
    return 0;
  }

   /* Get values for:
     - Change in momentum
     - Change in covariance of momentum
     - Weights of new states
  */

  const Trk::TrackParameters* trackParameters = componentParameters.first;
  const AmgSymMatrix(5)* measuredCov = trackParameters->covariance();

  // msg(MSG::VERBOSE) << "Calculating, weights, deltaP and delta-covariance" << endmsg;

  const std::vector<double> componentWeights = m_materialEffects->weights(componentParameters, materialProperties, pathLength, direction, particleHypothesis );
  const std::vector<double> componentDeltaPs = m_materialEffects->deltaPs(componentParameters, materialProperties, pathLength, direction, particleHypothesis );
  
  std::vector<const AmgSymMatrix(5)*> componentDeltaCovariances;
  
  if (measuredCov)
    componentDeltaCovariances = m_materialEffects->deltaCovariances(componentParameters, materialProperties, pathLength, direction, particleHypothesis );
  else
    if (m_outputlevel < 0)
      msg(MSG::VERBOSE) << "No covariance associated with this component... Cannot update covariance with material effects. Continuing" << endmsg;
  
  // check all vectors have the same size
  if ( componentWeights.size() != componentDeltaPs.size() ){
    msg(MSG::ERROR) << "Inconsistent number of components in the updator... returning original component" << endmsg;
    if (m_outputlevel <= 0)
      msg(MSG::DEBUG) << "Number of weights components: " << componentWeights.size() << " Number of deltaP entries: " << componentDeltaPs.size()
	          << " number of deltaCovariance entries: " << componentDeltaCovariances.size() << endmsg;
    return new Trk::MultiComponentState( *( componentParameters.clone() ) );
  }
  
  if (m_outputlevel < 0)
    msg(MSG::VERBOSE) << "Updator found: " << componentWeights.size() << " components" << endmsg;
  
  // Prepare  an output state
  unsigned int componentIndex = 0;

  for ( ; componentIndex < componentWeights.size(); ++componentIndex){
    if (m_outputlevel < 0){
      if ( !measuredCov ){
        msg(MSG::VERBOSE) << "Printing updated parameters" << std::endl
        << " ************* Component number: " << componentIndex << " *************" << std::endl
        << "* - Component weight: " << componentWeights[componentIndex] << std::endl
        << "* - Component deltaP: " << componentDeltaPs[componentIndex] << std::endl
        << " **********************************************************************" << endmsg;
      }else{
        msg(MSG::VERBOSE) << "Printing updated parameters" << std::endl
        << " ************* Component number: " << componentIndex << " *************" << std::endl
        << "* - Component weight:     " << componentWeights[componentIndex] << std::endl
        << "* - Component deltaP:     " << componentDeltaPs[componentIndex] << std::endl
        << "* - Component deltaSigma: " << sqrt( (*componentDeltaCovariances[componentIndex])(Trk::qOverP,Trk::qOverP) )  << std::endl
        << " **********************************************************************" << endmsg;
      }
    }
    const Amg::VectorX& stateVector = trackParameters->parameters();
    Amg::VectorX updatedStateVector(5);
    updatedStateVector = stateVector;
    
    // Adjust the momentum of the component's parameters vector here. Check to make sure update is good.
    if ( !updateP( updatedStateVector, componentDeltaPs[componentIndex] ) ){
      m_materialEffects->reset();
      msg(MSG::ERROR) << "Cannot update state vector momentum... returning original component" << endmsg;
      return new Trk::MultiComponentState( *(componentParameters.clone() ) );
    }
    
    const TrackParameters* updatedTrackParameters = 0;
    
    if ( measuredCov && !componentDeltaCovariances.empty() && componentDeltaCovariances[componentIndex] != 0 ){
      AmgSymMatrix(5)* updatedCovariance = new AmgSymMatrix(5)( *componentDeltaCovariances[componentIndex] + *measuredCov );
      updatedTrackParameters = trackParameters->associatedSurface().createTrackParameters(updatedStateVector[Trk::loc1],updatedStateVector[Trk::loc2],updatedStateVector[Trk::phi],updatedStateVector[Trk::theta],updatedStateVector[Trk::qOverP], updatedCovariance );
    }
    else
      updatedTrackParameters = trackParameters->associatedSurface().createTrackParameters(updatedStateVector[Trk::loc1],updatedStateVector[Trk::loc2],updatedStateVector[Trk::phi],updatedStateVector[Trk::theta],updatedStateVector[Trk::qOverP], 0 );
    
    double updatedWeight = componentParameters.second * componentWeights[componentIndex];

    const Trk::ComponentParameters updatedComponent(updatedTrackParameters, updatedWeight);
    
    computedState->push_back( updatedComponent );
  }

  // Clean up memory allocated in the material effects
  m_materialEffects->reset();

  return computedState;

}

/* ============================================================================
   ============================================================================
   updateP method
   ============================================================================
   ============================================================================ */
  
bool Trk::GsfMaterialEffectsUpdator::updateP (Amg::VectorX& stateVector, double deltaP) const
{
  
  // msg(MSG::VERBOSE) << "Original q/p: " << stateVector[Trk::qOverP] << endmsg;

  double p = 1./fabs( stateVector[Trk::qOverP] );
  p += deltaP;
  
  if (p <= 0.){
    return false;
  }
  double updatedIp = stateVector[Trk::qOverP] > 0. ? 1./p : -1./p;
  
  stateVector[Trk::qOverP] = updatedIp;

  //msg(MSG::VERBOSE) << "Updated q/p: " << stateVector[Trk::qOverP] << endmsg;
  
  return true;

}
