/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrackInCaloTools/TrackDirectionTool.h"

#include "TrkTrack/Track.h"
#include "Particle/TrackParticle.h"
#include "EventKernel/INavigable4Momentum.h"
#include "muonEvent/Muon.h"

#include "TrkVolumes/Volume.h"
#include "TrkDetDescrInterfaces/ITrackingVolumesSvc.h"

#include "DataModel/DataVector.h"

/////////////////////////////////////////////////////////
TrackDirectionTool::TrackDirectionTool (const std::string&type, const std::string&name, const IInterface*parent)
  :AthAlgTool(type, name, parent),
  m_trackingVolumesSvc    ("TrackingVolumesSvc/TrackingVolumesSvc",name)
{
  declareInterface<ITrackDirectionTool>(this);
  declareProperty( "TrackingVolumeSvc", m_trackingVolumesSvc);
  
  m_calorimeterVolume = 0;
  m_indetVolume = 0;
  m_muonspectrometerVolume = 0;
}
// ------------------------------------------------------------------ 
TrackDirectionTool::~TrackDirectionTool() {
  delete m_calorimeterVolume;
  delete m_indetVolume;
  delete m_muonspectrometerVolume;
}
// ------------------------------------------------------------------ 
StatusCode TrackDirectionTool::initialize()
{
  ATH_MSG_DEBUG( "Initializing..." );
  
  //Get TrackingVolumeSvc and the Inner Detector and Muon Spectrometer volumes
  if (m_trackingVolumesSvc.retrieve().isFailure())
  {
    ATH_MSG_FATAL( "Failed to retrieve Svc " << m_trackingVolumesSvc );
    return StatusCode::FAILURE;
  } 
  else
  {
    ATH_MSG_INFO( "Retrieved Svc " << m_trackingVolumesSvc );
    m_muonspectrometerVolume = new Trk::Volume(
    m_trackingVolumesSvc->volume(Trk::ITrackingVolumesSvc::MuonSpectrometerExitLayer));
    m_calorimeterVolume      = new Trk::Volume(
    m_trackingVolumesSvc->volume(Trk::ITrackingVolumesSvc::MuonSpectrometerEntryLayer));
    m_indetVolume            = new Trk::Volume(
    m_trackingVolumesSvc->volume(Trk::ITrackingVolumesSvc::CalorimeterEntryLayer));
  }
  
  return StatusCode::SUCCESS;
}
// ------------------------------------------------------------------ 
StatusCode TrackDirectionTool::finalize() 
{
  return StatusCode::SUCCESS;
}
//--------------------------------------------------------------------
bool TrackDirectionTool::getPropagationDirection(const Trk::Track* track, Trk::PropDirection &direction)
{
  if (track && track->trackParameters()) {
    // convert DataVector<const Trk::TrackParameters*> to std::vector<const Trk::TrackParameters*>
    std::vector<const Trk::TrackParameters*> v;
    v.reserve(track->trackParameters()->size());
    for (unsigned int i=0; i < track->trackParameters()->size(); i++)
      v.push_back(track->trackParameters()->at(i));
    return getPropagationDirection(v, direction);
  }
  return false;
}
//--------------------------------------------------------------------
bool TrackDirectionTool::getPropagationDirection(const INavigable4Momentum *track, Trk::PropDirection &direction)
{
  const Analysis::Muon * muon = dynamic_cast<const Analysis::Muon*> (track);
  const Rec::TrackParticle * trackParticle = 0;
  if ( muon )
    trackParticle = muon->track();
  else
    trackParticle = dynamic_cast<const Rec::TrackParticle*>(track);

  if (trackParticle)
    return getPropagationDirection(trackParticle->trackParameters(), direction);
  return false;
}
//--------------------------------------------------------------------
bool TrackDirectionTool::getPropagationDirection(std::vector<const Trk::TrackParameters*> parameters, Trk::PropDirection &direction)
{
  ATH_MSG_DEBUG( "In getPropagationDirection(std::vector<const Trk::TrackParameters*>)" );
  if (parameters.size() < 2)
  {
    ATH_MSG_DEBUG( "Cannot determine the direction given only 1 parameter, giving up" );
    return false;
  }
  // Needs at least one set of parameters in the muon spectrometer and another in the Inner Detector,
  // to avoid strange tracks or MS only tracks
  // MS should start roughly at r = 4250 mm, z = 6750 mm, so I will use r=4000, z=6000
  // ID should end roughly at r = 100 mm, z = 300 mm, so I will use r=150, z=350
  std::vector<unsigned int> MSparameters, IDparameters;
  for (unsigned int i=0; i < parameters.size(); i++) {
    if (!parameters[i]) {
      ATH_MSG_WARNING( "Null pointer to TrackParameters in the lists of parameters" );
      return false;
    }
    const Amg::Vector3D position = parameters[i]->position();
    if (m_indetVolume->inside(position) )
      IDparameters.push_back(i);
    else if (!m_calorimeterVolume->inside(position) && m_muonspectrometerVolume->inside(position))
      MSparameters.push_back(i);
    else
      ATH_MSG_DEBUG( "Parameter outside muon spectrometer or inside calorimeter" );
  }
  if (MSparameters.size() == 0 || IDparameters.size() == 0) {
    ATH_MSG_DEBUG( "Could not find parameters in the MS and in the ID, giving up" );
    return false;
  }
  // If it has parameters in both hemispheres of the MS, it should be extrapolated against the momentum
  // Assuming there are no cosmics coming from below
  double phi = 0;
  for (unsigned int i=0; i < MSparameters.size(); i++) {
    if (i == 0)
      phi = parameters[MSparameters[i]]->position().phi();
    else if (phi*parameters[MSparameters[i]]->position().phi() < 0) {
      direction = Trk::oppositeMomentum;
      ATH_MSG_DEBUG( "Parameters in both MS hemispheres" );
      ATH_MSG_DEBUG( "Direction: oppositeMomentum" );
      return true;
    }
  }
  
  // Take the scalar product between the position and momentum vectors to 
  // see if the track is pointing at or away from the perigee
  const Amg::Vector3D position = parameters[MSparameters[0]]->position();
  const Amg::Vector3D momentum = parameters[MSparameters[0]]->momentum();
  if (momentum.mag() > 1 && position.mag() > 1) {
    ATH_MSG_DEBUG( "phi position: " << position.phi() );
    double cosAlpha = momentum.dot(position)/(momentum.mag()*position.mag());
    if (fabs(cosAlpha) > 0.5) { // to make sure track is not too tangent
      if (cosAlpha < 0) { // pointing at the perigee
        ATH_MSG_DEBUG( "Direction: oppositeMomentum" );
        direction = Trk::oppositeMomentum;
      }
      else {
        ATH_MSG_DEBUG( "Direction: alongMomentum" );
        direction = Trk::alongMomentum;
      }
      return true;
    }
  }
  ATH_MSG_DEBUG( "Could not determine direction from track parameters" );
  return false;
}
