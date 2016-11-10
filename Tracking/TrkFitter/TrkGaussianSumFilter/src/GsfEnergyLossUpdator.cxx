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

#include "TrkGeometry/MaterialProperties.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSurfaces/Surface.h"
#include "TrkGeometry/Layer.h"


#include "TrkMaterialOnTrack/EnergyLoss.h"
#include "TrkExInterfaces/IEnergyLossUpdator.h"

#include "TrkEventPrimitives/ParamDefs.h"

// static particle masses
Trk::ParticleMasses Trk::GsfEnergyLossUpdator::s_particleMasses;


Trk::GsfEnergyLossUpdator::GsfEnergyLossUpdator(const std::string& type, const std::string& name, const IInterface* parent)
  :
  AthAlgTool(type, name, parent),
  m_outputlevel(0),
  m_EnergyLossUpdator("Trk::EnergyLossUpdator/AtlasEnergyLossUpdator")
{

  declareInterface<IMaterialEffectsUpdator>(this);

}

Trk::GsfEnergyLossUpdator::~GsfEnergyLossUpdator()
{}

StatusCode Trk::GsfEnergyLossUpdator::initialize()
{

  m_outputlevel = msg().level()-MSG::DEBUG;   // save the threshold for debug printout in private member



 if ( m_EnergyLossUpdator.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << m_EnergyLossUpdator << ". No energy effects will be taken into account." );
    return StatusCode::FAILURE;
  } else 
    ATH_MSG_INFO( "Retrieved tool " << m_EnergyLossUpdator );


  msg(MSG::INFO) << "Initialisation of " << name() << " was successful" << endmsg;
  return StatusCode::SUCCESS;

}

StatusCode Trk::GsfEnergyLossUpdator::finalize()
{

  msg(MSG::INFO) << "Finalisation of " << name() << " was successful" << endmsg;
  return StatusCode::SUCCESS;

}

const Trk::TrackParameters* Trk::GsfEnergyLossUpdator::update( const Trk::TrackParameters* trackParameters,
                                                               const Trk::Layer& layer,
                                                               Trk::PropDirection direction,
                                                               Trk::ParticleHypothesis,
                                                               Trk::MaterialUpdateMode) const
{

 const AmgSymMatrix(5)* measuredCov = trackParameters->covariance();

  if (!measuredCov){
    if (m_outputlevel <= 0) 
      msg(MSG::DEBUG) << "No measurement on track parameters... returning original track parameters" << endmsg;
    return trackParameters->clone();
  }
  
  // Request the material properties
  const Trk::MaterialProperties* materialProperties = layer.fullUpdateMaterialProperties(*trackParameters);

  if (!materialProperties)
    return trackParameters->clone();

  double pathLength = layer.surfaceRepresentation().pathCorrection(trackParameters->position(),trackParameters->momentum()) * materialProperties->thickness();
  return this->update( *trackParameters, *materialProperties, pathLength, direction );

}

const Trk::TrackParameters* Trk::GsfEnergyLossUpdator::update( const TrackParameters& trackParameters,
                                                               const MaterialProperties& materialProperties,
                                                               double pathLength,
                                                               PropDirection direction,
                                                               ParticleHypothesis particle,
                                                               Trk::MaterialUpdateMode) const
{
  if (m_outputlevel < 0) msg(MSG::VERBOSE) << "Updating energy loss effects using material properties and path length" << endmsg;

  const AmgSymMatrix(5)* measuredCov = trackParameters.covariance();

  if (!measuredCov){
    if (m_outputlevel <= 0) 
      msg(MSG::DEBUG) << "No measurement on track parameters... returning original track parameters" << endmsg;
    return trackParameters.clone();
  }

  AmgSymMatrix(5)* updatedCovarianceMatrix = new AmgSymMatrix(5)( *measuredCov );


  double pathcorrection = pathLength/materialProperties.thickness();

  const Amg::VectorX& trackStateVector = trackParameters.parameters();
  const Amg::Vector3D& globalMomentum = trackParameters.momentum();
   
  EnergyLoss* energyLoss =
                 m_EnergyLossUpdator->energyLoss(materialProperties, globalMomentum.mag(), pathcorrection, direction, particle, true);
    
  // update for mean energy loss
  double deltaE      = energyLoss ? energyLoss->deltaE() : 0;
  double sigmaDeltaE = energyLoss ? energyLoss->sigmaDeltaE() : 0;
  delete energyLoss;

  // Calculate the pathlength encountered by the track
  double p = globalMomentum.mag();
  double inverseMomentum = 1. / p;

  double m     = s_particleMasses.mass[particle];
  double E     = sqrt(p*p+m*m);
  double beta  = p/E;
  
  // Calculate energy loss values
  double momentumFractionLost     =  deltaE * inverseMomentum ;
  
  double sigmaQoverP = sigmaDeltaE/pow(beta*p,2);
  
  // Update diagonal and off-diagonal covariance matrix elements
  (*updatedCovarianceMatrix)(Trk::qOverP,Trk::qOverP) += sigmaQoverP*sigmaQoverP;
  
  //Amg::VectorX updatedTrackStateVector(5);
  //updatedTrackStateVector[Trk::locX]   = trackStateVector[Trk::locX];
  //updatedTrackStateVector[Trk::locY]   = trackStateVector[Trk::locY];
  //updatedTrackStateVector[Trk::phi]    = trackStateVector[Trk::phi];
  //updatedTrackStateVector[Trk::theta]  = trackStateVector[Trk::theta];
  //updatedTrackStateVector[Trk::qOverP] = trackStateVector[Trk::qOverP] * (1. +  momentumFractionLost);
     
  return trackParameters.associatedSurface().createTrackParameters(trackStateVector[Trk::locX],trackStateVector[Trk::locY],trackStateVector[Trk::phi],trackStateVector[Trk::theta],trackStateVector[Trk::qOverP] * (1. +  momentumFractionLost), updatedCovarianceMatrix );
  
}

