/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#include "TrkGaussianSumFilter/MultiStateMaterialEffectsAdapter.h"

#include "TrkParameters/TrackParameters.h"
#include "TrkExInterfaces/IMaterialEffectsUpdator.h"
#include "TrkSurfaces/Surface.h"
#include "TrkGeometry/Layer.h"


#include "GaudiKernel/ToolHandle.h"

Trk::GsfCombinedMaterialEffects::GsfCombinedMaterialEffects (const std::string& type,
                   const std::string& name,
                   const IInterface* parent)
  :
  MultiStateMaterialEffects(type, name, parent),
  m_multipleScatterEffects("Trk::MultipleScatterUpdator/MultipleScatterUpdator"),
  m_multiStateMultipleScatteringEffects("Trk::MultiStateMaterialEffectsAdapter/MultipleScatteringEffectsAdapter"),
  m_energyLossEffects("Trk::GsfEnergyLossUpdator/MyGsfEnergyLossUpdator"),
  //m_energyLossEffects("Trk::EnergyLossUpdator/MyEnergyLossUpdator"), 
  m_betheHeitlerEffects("Trk::GsfBetheHeitlerEffects/GsfBetheHeitlerEffects"),
  m_multiStateEnergyLossEffects("Trk::MultiStateMaterialEffectsAdapter/EnergyLossEffectsAdapter"),
  m_multiStateMultipleScatteringAdapter(0),
  m_multiStateEnergyLossAdapter(0)

{

  declareInterface<IMultiStateMaterialEffects>(this);

  declareProperty("MultipleScatteringUpdator", m_multipleScatterEffects);

  declareProperty("EnergyLossUpdator", m_energyLossEffects);

  declareProperty("BetheHeitlerEffects", m_betheHeitlerEffects);

}

Trk::GsfCombinedMaterialEffects::~GsfCombinedMaterialEffects()
{}

StatusCode Trk::GsfCombinedMaterialEffects::initialize()
{
  // Retrieve the multiple scattering effects
  if ( m_multipleScatterEffects.retrieve().isFailure() ){
    msg(MSG::FATAL)
          << "Could not retrieve multiple scattering AlgTool: " << m_multipleScatterEffects.typeAndName()
          << "... Exiting!" << endmsg;
    return StatusCode::FAILURE;
  }

  // Retrieve and configure multiple scattering effects for multi-state operation
  msg(MSG::INFO) << "Configuring for multiple scattering" << endmsg;

  if ( m_multiStateMultipleScatteringEffects.retrieve().isFailure() ){
    msg(MSG::WARNING) << "Cannot get the multiple scattering adapter... exiting" << endmsg;
    return StatusCode::FAILURE;
  } else {
      msg(MSG::INFO)
				  << "Retrieved MS adapter AlgTool: " << m_multiStateMultipleScatteringEffects.typeAndName()
			    << "... YAY!" << endmsg;
  }

  m_multiStateMultipleScatteringEffects->setMaterialEffects( m_multipleScatterEffects );
  m_multiStateMultipleScatteringAdapter = dynamic_cast<const MultiStateMaterialEffectsAdapter*>(&(*m_multiStateMultipleScatteringEffects));

  if ( !m_multiStateMultipleScatteringAdapter ){
    msg(MSG::WARNING) << "Error retrieving the multiple scattering adapter for multiple state configuration... exiting" << endmsg;
    return StatusCode::FAILURE;
  }

  
  // Retrieve and configure the energy loss effects for multi-state operation
  msg(MSG::INFO) << "Configuring for normal energy loss" << endmsg;

  if ( m_energyLossEffects.retrieve().isFailure() ){
    msg(MSG::FATAL)
          << "Could not retrieve energy loss AlgTool: " << m_energyLossEffects.typeAndName()
          << "... Exiting!" << endmsg;
    return StatusCode::FAILURE;
  } else {
		    msg(MSG::INFO)
				  << "Retrieved energy loss AlgTool: " << m_energyLossEffects.typeAndName()
			    << "... YAY!" << endmsg;

  }


  if ( m_multiStateEnergyLossEffects.retrieve().isFailure() ){
    msg(MSG::WARNING) << "Cannot get the energy loss adapter... exiting" << endmsg;
    return StatusCode::FAILURE;
  } else {
      msg(MSG::INFO)
				  << "Retrieved energy adapter loss AlgTool: " << m_multiStateEnergyLossEffects.typeAndName()
			    << "... YAY!" << endmsg;
  }

  
  m_multiStateEnergyLossEffects->setMaterialEffects( m_energyLossEffects );
  m_multiStateEnergyLossAdapter = dynamic_cast<const MultiStateMaterialEffectsAdapter*>(&(*m_multiStateEnergyLossEffects));

  if ( !m_multiStateEnergyLossAdapter ){
    msg(MSG::WARNING) << "Error retrieving the energy loss adapter for multiple state configuration... exiting" << endmsg;
    return StatusCode::FAILURE;
  }

  
  // Retrieve and configure the Bethe-Heitler effects for energy loss
  msg(MSG::INFO) << "Configuring for Bethe-Heitler energy loss" << endmsg;

  if ( m_betheHeitlerEffects.retrieve().isFailure() ){
    msg(MSG::FATAL)
          << "Could not retrieve energy loss AlgTool: " << m_betheHeitlerEffects.typeAndName()
          << "... Exiting!" << endmsg;
    return StatusCode::FAILURE;
  }

  msg(MSG::INFO) << "Initialisation of " << name() << " was successful" << endmsg;

  return StatusCode::SUCCESS;
}

StatusCode Trk::GsfCombinedMaterialEffects::finalize()
{

  msg(MSG::INFO) << "Finalisation of " << name() << " was successful" << endmsg;

  return StatusCode::SUCCESS;

}

void Trk::GsfCombinedMaterialEffects::reset() const
{

  if (msgLvl(MSG::VERBOSE)) msg() << "Resetting the material effects " << name() << endmsg;

  // Reset all effects
  m_multiStateMultipleScatteringAdapter->reset();
  m_multiStateEnergyLossAdapter->reset();
  m_betheHeitlerEffects->reset();

  // Clear this cache
  m_weights.clear();
  m_deltaPs.clear();

  if ( !m_deltaCovariances.empty() ){

    std::vector<const AmgSymMatrix(5)*>::const_iterator componentDeltaCovariance = m_deltaCovariances.begin();

    for ( ; componentDeltaCovariance != m_deltaCovariances.end(); ++componentDeltaCovariance )
      delete *componentDeltaCovariance;

    m_deltaCovariances.clear();

  }

  m_isReset = true;

}

void Trk::GsfCombinedMaterialEffects::compute ( const Trk::ComponentParameters& componentParameters,
            const Trk::MaterialProperties& materialProperties,
            double pathLength,
            Trk::PropDirection direction,
            Trk::ParticleHypothesis particleHypothesis ) const
{
  
  if (msgLvl(MSG::VERBOSE)) msg() << "Computing combined material effects" << endmsg;

  // Reset everything before computation
  this->reset();

  const AmgSymMatrix(5)* measuredCov = componentParameters.first->covariance();

  if (msgLvl(MSG::VERBOSE)) msg() << "Calculating multiple scattering material effects" << endmsg;

  
  /* ========================================================================
     Retrieve multiple scattering corrections
     ======================================================================== */

  std::vector<double> multipleScatter_weights = m_multiStateMultipleScatteringAdapter->weights(componentParameters,
                             materialProperties,
                             pathLength,
                             direction,
                             particleHypothesis);

  std::vector<double> multipleScatter_deltaPs = m_multiStateMultipleScatteringAdapter->deltaPs(componentParameters,
                             materialProperties,
                             pathLength,
                             direction,
                             particleHypothesis);

  std::vector<const AmgSymMatrix(5)*> multipleScatter_deltaCovariances;

  if ( measuredCov )
    multipleScatter_deltaCovariances = m_multiStateMultipleScatteringAdapter->deltaCovariances(componentParameters,
                             materialProperties,
                             pathLength,
                             direction,
                             particleHypothesis);

  // Protect if there are no new components
  if ( multipleScatter_weights.empty() ){
    if (msgLvl(MSG::DEBUG)) msg() << "WARNING: Multiple scattering effects are not determined" << endmsg;
    multipleScatter_weights.push_back(1.);
    multipleScatter_deltaPs.push_back(0.);
    multipleScatter_deltaCovariances.push_back( new AmgSymMatrix(5) );
  }

  if (msgLvl(MSG::VERBOSE)) msg() << "Calculating energy loss material effects" << endmsg;

  std::vector<double> energyLoss_weights;
  std::vector<double> energyLoss_deltaPs;
  std::vector<const AmgSymMatrix(5)*> energyLoss_deltaCovariances;

  /* ========================================================================
     Retrieve energy loss corrections
     ======================================================================== */

  
  if ( particleHypothesis != electron ){
	  if (msgLvl(MSG::VERBOSE)) msg() << "Considering standard energy loss effects" << endmsg;

    energyLoss_weights = m_multiStateEnergyLossAdapter->weights(componentParameters,
                materialProperties,
                pathLength,
                direction,
                particleHypothesis);

    energyLoss_deltaPs = m_multiStateEnergyLossAdapter->deltaPs(componentParameters,
                materialProperties,
                pathLength,
                direction,
                particleHypothesis);

    if ( measuredCov )
      energyLoss_deltaCovariances = m_multiStateEnergyLossAdapter->deltaCovariances(componentParameters,
                        materialProperties,
                        pathLength,
                        direction,
                        particleHypothesis);

  } else {
     
    if (msgLvl(MSG::VERBOSE)) msg() << "Considering Bethe-Heitler energy loss effects" << endmsg;

    energyLoss_weights = m_betheHeitlerEffects->weights(componentParameters,
              materialProperties,
              pathLength,
              direction);

    energyLoss_deltaPs = m_betheHeitlerEffects->deltaPs(componentParameters,
              materialProperties,
              pathLength,
              direction);

    if ( measuredCov )
      energyLoss_deltaCovariances = m_betheHeitlerEffects->deltaCovariances(componentParameters,
                      materialProperties, 
                      pathLength, 
                      direction,
                      particleHypothesis);

  }

  // Protect if there are no new components
  if ( energyLoss_weights.empty() ){
    if (msgLvl(MSG::DEBUG)) msg() << "WARNING: Energy loss effects are not determined" << endmsg;
    energyLoss_weights.push_back(1.);
    energyLoss_deltaPs.push_back(0.);
    AmgSymMatrix(5)* newCov = new AmgSymMatrix(5);
    newCov->setZero();
    energyLoss_deltaCovariances.push_back( newCov);
  }

  std::vector<double>::const_iterator deltaPsIterator = energyLoss_deltaPs.begin();

  
  if (msgLvl(MSG::VERBOSE)) 
    for ( ; deltaPsIterator != energyLoss_deltaPs.end(); ++deltaPsIterator)
      msg(MSG::VERBOSE) << "Energy loss deltaP: " << *deltaPsIterator << endmsg;
  
  
	
  
  /* ========================================================================
     Combine the multiple scattering and energy loss components
     ======================================================================== */
  
  if (msgLvl(MSG::VERBOSE)) msg() << "Combining the energy loss and multiple scattering components" << endmsg;

  // Iterators over the multiple scattering components
  std::vector<double>::const_iterator multipleScatter_weightsIterator = multipleScatter_weights.begin();
  std::vector<double>::const_iterator multipleScatter_deltaPsIterator = multipleScatter_deltaPs.begin();
  std::vector<const AmgSymMatrix(5)*>::const_iterator multipleScatter_deltaCovariancesIterator = multipleScatter_deltaCovariances.begin();
  
  // Loop over multiple scattering components
  for ( ; multipleScatter_weightsIterator != multipleScatter_weights.end(); 
        ++multipleScatter_weightsIterator, ++multipleScatter_deltaPsIterator, ++multipleScatter_deltaCovariancesIterator ){
    
    // Iterators over the energy loss components
    std::vector<double>::const_iterator energyLoss_weightsIterator = energyLoss_weights.begin();
    std::vector<double>::const_iterator energyLoss_deltaPsIterator = energyLoss_deltaPs.begin();  
    std::vector<const AmgSymMatrix(5)*>::const_iterator energyLoss_deltaCovariancesIterator = energyLoss_deltaCovariances.begin();
    
    // Loop over energy loss components
    
    for ( ; energyLoss_weightsIterator != energyLoss_weights.end(); ++energyLoss_weightsIterator,
            ++energyLoss_deltaPsIterator, ++energyLoss_deltaCovariancesIterator ){

      double combinedWeight = (*multipleScatter_weightsIterator) * (*energyLoss_weightsIterator);
      double combinedDeltaP = (*multipleScatter_deltaPsIterator) + (*energyLoss_deltaPsIterator);

      m_weights.push_back( combinedWeight );
      m_deltaPs.push_back( combinedDeltaP );

      if ( measuredCov ){
        AmgSymMatrix(5)* summedCovariance = new AmgSymMatrix(5) (  (*(*multipleScatter_deltaCovariancesIterator)) + (*(*energyLoss_deltaCovariancesIterator) ));
        //std::cout << "GSFCM DeltaPhi " << (*summedCovariance)[Trk::phi][Trk::phi] <<std::endl;

        m_deltaCovariances.push_back( summedCovariance );
      }
      
    } // end for loop over energy loss components
    
  } // end for loop over multiple scattering components

  // Reset all cashes before return - memory is deleted in the covariance matricies
  m_multiStateMultipleScatteringAdapter->reset();
  m_multiStateEnergyLossAdapter->reset();
  m_betheHeitlerEffects->reset();

  if (msgLvl(MSG::VERBOSE)) msg() << "Successfully included combined material effects" << endmsg;

}
