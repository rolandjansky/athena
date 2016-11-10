/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************************
      MultiStateMaterialEffectsAdapter.cxx  -  description
      ----------------------------------------------------
begin                : Tuesday 22nd February 2005
author               : atkinson
email                : Tom.Atkinson@cern.ch
decription           : Implementation code for MultiStateMaterialEffectsAdapter class
**************************************************************************************/

#include "TrkGaussianSumFilter/MultiStateMaterialEffectsAdapter.h"

#include "TrkExInterfaces/IMaterialEffectsUpdator.h"

Trk::MultiStateMaterialEffectsAdapter::MultiStateMaterialEffectsAdapter(const std::string& type, const std::string& name, const IInterface* parent)
  :
  MultiStateMaterialEffects(type, name, parent)
{

  declareInterface<IMultiStateMaterialEffects>(this);

}

Trk::MultiStateMaterialEffectsAdapter::~MultiStateMaterialEffectsAdapter()
{}

StatusCode Trk::MultiStateMaterialEffectsAdapter::initialize()
{

  msg(MSG::INFO) << "Initialisation of " << name() << " was successful" << endmsg;
  return StatusCode::SUCCESS;

}

StatusCode Trk::MultiStateMaterialEffectsAdapter::finalize()
{
  
  msg(MSG::INFO) << "Finalisation of " << name() << " was successful" << endmsg;
  return StatusCode::SUCCESS;

}

void Trk::MultiStateMaterialEffectsAdapter::compute ( const ComponentParameters& componentParameters, 
                  const MaterialProperties& materialProperties,
                  double pathLength,
                  PropDirection direction,
                  ParticleHypothesis particleHypothesis ) const
{

  if (msgLvl(MSG::VERBOSE)) msg() << "Running adapter to bring standard energy loss effects to multiple component state reigeme: " << name() << endmsg;

  // Reset the cashe
  this->reset(); 

  // Request track parameters from component parameters
  const Trk::TrackParameters* originalTrackParameters = componentParameters.first;

  // Update this track parameters object
  const Trk::TrackParameters* updatedTrackParameters 
    = m_materialEffectsUpdator->update( *originalTrackParameters, materialProperties, pathLength, direction, particleHypothesis );

  /* Extract the required values for the Gsf Material Effects Updator
     - weight = 1 for single state
     - deltaP
     - deltaSigma
  */

  double deltaP = extractDeltaP( *updatedTrackParameters, *originalTrackParameters );
	const AmgSymMatrix(5)* deltaErrorMatrix = extractDeltaCovariance( *updatedTrackParameters, *originalTrackParameters );

  m_weights.push_back(1.);
  m_deltaPs.push_back(deltaP);

  if (deltaErrorMatrix)
    m_deltaCovariances.push_back(deltaErrorMatrix);

  // Clean up memory
  delete updatedTrackParameters;
  
}

double 
Trk::MultiStateMaterialEffectsAdapter::extractDeltaP ( const Trk::TrackParameters& updatedParameters, const Trk::TrackParameters& originalParameters) const
{

  double deltaP(0.);
  double originalP = 1. / originalParameters.parameters()[Trk::qOverP];
  double updatedP  = 1. / updatedParameters.parameters()[Trk::qOverP];

  if (originalP < 0.)
    deltaP = fabs(originalP) + updatedP;
  else
    deltaP = fabs(originalP) - updatedP;

  //std::cout << "originalP " << originalP << "\tupdatedP " << updatedP << "\tDelta " << deltaP << std::endl;

  return deltaP;

}

const AmgSymMatrix(5)*
Trk::MultiStateMaterialEffectsAdapter::extractDeltaCovariance ( const Trk::TrackParameters& updatedParameters, const Trk::TrackParameters& originalParameters ) const
{

  const AmgSymMatrix(5)* originalMeasuredCov = originalParameters.covariance();
  const AmgSymMatrix(5)* updatedMeasuredCov  = updatedParameters.covariance();

  if (!originalMeasuredCov || !updatedMeasuredCov)
    return 0;

  return new AmgSymMatrix(5)( *updatedMeasuredCov - *originalMeasuredCov);

}
