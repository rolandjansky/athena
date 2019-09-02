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
#include "TrkGeometry/Layer.h"
#include "TrkGeometry/MaterialProperties.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSurfaces/Surface.h"

#include "TrkEventPrimitives/ParamDefs.h"

#include <cmath>

Trk::MultipleScatterUpdator::MultipleScatterUpdator(const std::string& type,
                                                    const std::string& name,
                                                    const IInterface* parent)
  : AthAlgTool(type, name, parent)
  , m_multipleScatterLogTermOn(true)
{
  declareInterface<IGSFMaterialEffects>(this);
  declareProperty("MultipleScatterLogarithmicTermOn", m_multipleScatterLogTermOn);
}

Trk::MultipleScatterUpdator::~MultipleScatterUpdator() {}

StatusCode
Trk::MultipleScatterUpdator::initialize()
{
  ATH_CHECK(m_msUpdator.retrieve());
  ATH_MSG_DEBUG("Initialisation of " << name() << " was successful");
  return StatusCode::SUCCESS;
}

StatusCode
Trk::MultipleScatterUpdator::finalize()
{
  ATH_MSG_DEBUG("Finalisation of " << name() << " was successful");
  return StatusCode::SUCCESS;
}

std::unique_ptr<Trk::TrackParameters>
Trk::MultipleScatterUpdator::update(const Trk::TrackParameters* trackParameters,
                                    const Trk::Layer& layer,
                                    Trk::PropDirection direction,
                                    Trk::ParticleHypothesis particleHypothesis,
                                    Trk::MaterialUpdateMode) const
{
  ATH_MSG_VERBOSE("Performing multiple scatter update using layer information");
  // Extract the material properties from the layer
  const Trk::MaterialProperties* materialProperties(0);
  double pathCorrection(0.);

  // Get the surface associated with the parameters
  const Trk::Surface& surface = trackParameters->associatedSurface();

  // Only utilise the reference material if an associated detector element exists
  if (surface.associatedDetectorElement()) {

    // Get the layer material properties
    const Trk::LayerMaterialProperties* layerMaterial = layer.layerMaterialProperties();

    // Assign the material properties
    materialProperties = layerMaterial ? layerMaterial->fullMaterial(trackParameters->position()) : 0;

    // Determine the pathCorrection if the material properties exist
    pathCorrection = materialProperties ? 1. / std::fabs(surface.normal().dot(trackParameters->momentum().unit())) : 0.;
  }

  // Check that the material properties have been defined - if not define them from the layer information
  materialProperties = materialProperties ? materialProperties : layer.fullUpdateMaterialProperties(*trackParameters);

  if (!materialProperties) {
    ATH_MSG_DEBUG("No material properties associated with layer... returning original parameters");
    return std::unique_ptr<Trk::TrackParameters> (trackParameters->clone());
  }

  const AmgSymMatrix(5)* measuredTrackCov = trackParameters->covariance();

  if (!measuredTrackCov) {
    ATH_MSG_DEBUG("No measured track parameters for multiple scatter... returning original parameters");
    return std::unique_ptr<Trk::TrackParameters> (trackParameters->clone());
  }

  // Define the path correction
  pathCorrection = pathCorrection > 0. ? pathCorrection
                                       : layer.surfaceRepresentation().pathCorrection(trackParameters->position(),
                                                                                      trackParameters->momentum());

  double pathLength = pathCorrection * materialProperties->thickness();


  return   update(*trackParameters, *materialProperties, pathLength, direction, particleHypothesis);
}

std::unique_ptr<Trk::TrackParameters>
Trk::MultipleScatterUpdator::update(const Trk::TrackParameters& trackParameters,
                                    const Trk::MaterialProperties& materialProperties,
                                    double pathLength,
                                    PropDirection /*direction*/,
                                    ParticleHypothesis /*particleHypothesis*/,
                                    Trk::MaterialUpdateMode) const
{

  ATH_MSG_VERBOSE("Updating multiple scatter effects based on material properties and path length");

  const AmgSymMatrix(5)* measuredTrackCov = trackParameters.covariance();

  if (!measuredTrackCov) {
    ATH_MSG_DEBUG( "No measurement associated with track parameters... returning original parameters");
    return std::unique_ptr<Trk::TrackParameters>(trackParameters.clone());
  }

  const Amg::Vector3D& globalMomentum = trackParameters.momentum();
  double p = globalMomentum.mag();

  double pathcorrection = 1.;
  if (materialProperties.thickness() != 0)
    pathcorrection = pathLength / materialProperties.thickness();

  // Here we know the path length to be meff.thicknessX0, so we set pathcorrection = 1
  // and create a dummy materialProperties with the properties we are interested in
  MaterialProperties mprop(materialProperties.thicknessInX0(), 1., 0., 0., 0., 0.);
  double angularVariation = m_msUpdator->sigmaSquare(mprop, p, pathcorrection, Trk::muon);
  ATH_MSG_DEBUG("Sigma squared multiple scattering: " << angularVariation);
  AmgSymMatrix(5)* cov_out = new AmgSymMatrix(5)(*measuredTrackCov);

  // double sign = (direction == Trk::oppositeMomentum) ? 1. : 1.;
  double sinTheta = std::sin(trackParameters.parameters()[Trk::theta]);

  //(*cov_out)(Trk::phi,Trk::phi) += sign * angularVariation / (sinTheta*sinTheta);
  //(*cov_out)(Trk::theta,Trk::theta) += sign * angularVariation;
  // std::cout << "MSU   DeltaPhi " << (*updatedCovarianceMatrix)[Trk::phi][Trk::phi] <<'\t' << angularVariation /
  // (sinTheta * sinTheta) << std::endl;
  (*cov_out)(Trk::phi, Trk::phi) += angularVariation / (sinTheta * sinTheta);
  (*cov_out)(Trk::theta, Trk::theta) += angularVariation;
  const AmgVector(5)& par = trackParameters.parameters();
  // return trackParameters.cloneToNew( trackParameters.parameters(), measuredTrackCov );
  return std::unique_ptr<Trk::TrackParameters> (
    trackParameters.associatedSurface().createTrackParameters(par[Trk::loc1], 
                                                              par[Trk::loc2], 
                                                              par[Trk::phi], 
                                                              par[Trk::theta], 
                                                              par[Trk::qOverP], 
                                                              cov_out));
}
