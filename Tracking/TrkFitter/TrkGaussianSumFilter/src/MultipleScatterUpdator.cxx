/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************************
      MultipleScatterUpdator.cxx  -  description
      ------------------------------------------
begin                : Tuesday 22nd January 2005
author               : atkinson
email                : Tom.Atkinson@cern.ch
decription           : Implementation code for multiple scatter updator
*********************************************************************************/

#include "TrkGaussianSumFilter/MultipleScatterUpdator.h"
#include "TrkGeometry/MaterialProperties.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSurfaces/Surface.h"
#include "TrkGeometry/Layer.h"

#include "TrkEventPrimitives/ParamDefs.h"

#include "TrkExInterfaces/IMultipleScatteringUpdator.h"

Trk::ParticleMasses Trk::MultipleScatterUpdator::s_particleMasses;
double Trk::MultipleScatterUpdator::s_multipleScatterMainFactor = 13.6 * Gaudi::Units::MeV;
double Trk::MultipleScatterUpdator::s_multipleScatterLogFactor = 0.038;
double Trk::MultipleScatterUpdator::s_main_RossiGreisen = 17.5 * Gaudi::Units::MeV;
double Trk::MultipleScatterUpdator::s_log_RossiGreisen = 0.125;

Trk::MultipleScatterUpdator::MultipleScatterUpdator(const std::string& type, const std::string& name, const IInterface* parent)
  :
  AthAlgTool(type, name, parent),
  m_outputlevel(1),
  m_msUpdator("Trk::MultipleScatteringUpdator/AtlasMultipleScatteringUpdator"),
  m_multipleScatterLogTermOn(true) 
{

  declareInterface<IMaterialEffectsUpdator>(this);

  declareProperty("MultipleScatterLogarithmicTermOn", m_multipleScatterLogTermOn);
  declareProperty("MultipleScatterMainFactor",        s_multipleScatterMainFactor);
  declareProperty("MultipleScatterLogFactor",         s_multipleScatterLogFactor);
  declareProperty("RossiGreisenMainFactor",           s_main_RossiGreisen);
  declareProperty("RossiGreisenLogFactor",            s_log_RossiGreisen);

}

Trk::MultipleScatterUpdator::~MultipleScatterUpdator()
{}

StatusCode Trk::MultipleScatterUpdator::initialize()
{

  m_outputlevel = msg().level()-MSG::DEBUG;   // save the threshold for debug printout in private member

  if( m_msUpdator.retrieve().isFailure() ) {
    ATH_MSG_FATAL("Failed to retrieve tool " << m_msUpdator << ". No energy loss effects will be taken into account." );
    return StatusCode::FAILURE;
  } else
    ATH_MSG_INFO( "Retrieved tool " << m_msUpdator );

  
  msg(MSG::INFO) << "Initialisation of " << name() << " was successful" << endmsg;
  return StatusCode::SUCCESS;

}

StatusCode Trk::MultipleScatterUpdator::finalize()
{

  msg(MSG::INFO) << "Finalisation of " << name() << " was successful" << endmsg;
  return StatusCode::SUCCESS;

}

const Trk::TrackParameters* Trk::MultipleScatterUpdator::update( const Trk::TrackParameters* trackParameters,
                 const Trk::Layer& layer,
                 Trk::PropDirection direction,
                 Trk::ParticleHypothesis particleHypothesis,
                 Trk::MaterialUpdateMode) const
{

  if (m_outputlevel < 0) 
    msg(MSG::VERBOSE) << "Performing multiple scatter update using layer information" << endmsg;



    
  // Extract the material properties from the layer
  const Trk::MaterialProperties* materialProperties(0);
  double pathCorrection(0.);


  // Get the surface associated with the parameters
  const Trk::Surface& surface = trackParameters->associatedSurface();

  // Only utilise the reference material if an associated detector element exists
  if ( surface.associatedDetectorElement() ){

    // Get the layer material properties
    const Trk::LayerMaterialProperties* layerMaterial = layer.layerMaterialProperties();

    // Assign the material properties
    materialProperties = layerMaterial ? layerMaterial->fullMaterial(trackParameters->position()) : 0;

    // Determine the pathCorrection if the material properties exist
    pathCorrection = materialProperties ? 1. / fabs( surface.normal().dot( trackParameters->momentum().unit() ) ) : 0.;

  }


  // Check that the material properties have been defined - if not define them from the layer information
  materialProperties = materialProperties ? materialProperties : layer.fullUpdateMaterialProperties( *trackParameters );

  if ( !materialProperties ) {
    msg(MSG::DEBUG) << "No material properties associated with layer... returning original parameters" << endmsg;
    return trackParameters->clone();
  }

  const AmgSymMatrix(5)* measuredTrackCov = trackParameters->covariance();

  if (!measuredTrackCov){
    msg(MSG::DEBUG) << "No measured track parameters for multiple scatter... returning original parameters" << endmsg;
    return trackParameters->clone();
  }
  
  // Define the path correction
  pathCorrection = pathCorrection > 0. ? pathCorrection : layer.surfaceRepresentation().pathCorrection(trackParameters->position(), trackParameters->momentum() );

  double pathLength = pathCorrection * materialProperties->thickness();

  const Trk::TrackParameters* updatedTrackParameters = update(*trackParameters, *materialProperties, pathLength, direction, particleHypothesis);

  return updatedTrackParameters;

}

const Trk::TrackParameters* 
Trk::MultipleScatterUpdator::update( const Trk::TrackParameters& trackParameters,
             const Trk::MaterialProperties& materialProperties,
             double pathLength,
             PropDirection direction,
             ParticleHypothesis /*particleHypothesis*/,
             Trk::MaterialUpdateMode ) const
{

  if (m_outputlevel < 0) 
    msg(MSG::VERBOSE) << "Updating multiple scatter effects based on material properties and path length" << endmsg;

  const AmgSymMatrix(5)* measuredTrackCov = trackParameters.covariance();

  if (!measuredTrackCov){
    msg(MSG::DEBUG) << "No measurement associated with track parameters... returning original parameters" << endmsg;
    return trackParameters.clone();
  }


  const Amg::Vector3D& globalMomentum = trackParameters.momentum();
  double p = globalMomentum.mag();

  double pathcorrection = pathLength/materialProperties.thickness();

  //Here we know the path length to be meff.thicknessX0, so we set pathcorrection = 1
  //and create a dummy materialProperties with the properties we are interested in
  MaterialProperties mprop(materialProperties.thicknessInX0(),1.,0.,0.,0.,0.);
  double angularVariation = m_msUpdator->sigmaSquare(mprop, p, pathcorrection, Trk::muon);  
  

  if (m_outputlevel <= 0) 
    msg(MSG::DEBUG) << "Sigma squared multiple scattering: " << angularVariation << endmsg;
  
  AmgSymMatrix(5)* cov_out = new AmgSymMatrix(5)(*measuredTrackCov);
 

  double sign = (direction == Trk::oppositeMomentum) ? 1. : 1.;
  double sinTheta = sin( trackParameters.parameters()[Trk::theta] );

  (*cov_out)(Trk::phi,Trk::phi) += sign * angularVariation / (sinTheta*sinTheta);
  (*cov_out)(Trk::theta,Trk::theta) += sign * angularVariation;
  //std::cout << "MSU   DeltaPhi " << (*updatedCovarianceMatrix)[Trk::phi][Trk::phi] <<'\t' << angularVariation / (sinTheta * sinTheta) << std::endl;
  
  const AmgVector(5)& par = trackParameters.parameters();
  //return trackParameters.cloneToNew( trackParameters.parameters(), measuredTrackCov );
  return trackParameters.associatedSurface().createTrackParameters(par[Trk::loc1],par[Trk::loc2],par[Trk::phi],par[Trk::theta],par[Trk::qOverP],cov_out);

}


   
