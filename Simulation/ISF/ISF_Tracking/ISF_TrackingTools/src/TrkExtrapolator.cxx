/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrkExtrapolator.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header include
#include "TrkExtrapolator.h"

#include "TrkDetDescrInterfaces/ITrackingGeometrySvc.h"

#include "TrkExInterfaces/IExtrapolator.h"

#include "TrkGeometry/TrackingGeometry.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkEventPrimitives/PdgToParticleHypothesis.h"


/** Constructor **/
ISF::TrkExtrapolator::TrkExtrapolator(const std::string& t, const std::string& n, const IInterface* p)
  : AthAlgTool(t,n,p),
    m_trackingGeometry(0),
    m_trackingGeometrySvc("TrackingGeometrySvc/AtlasTrackingGeometrySvc",n),
    m_trackingGeometryName("AtlasTrackingGeometry"),
    m_extrapolator("Trk::Extrapolator/AtlasExtrapolator"),
    m_trackingVolumeName(""),
    m_trackingVolume(0),
    m_pdgToParticleHypothesis(new Trk::PdgToParticleHypothesis())
{
  declareInterface<ISF::ITrkExtrapolator>(this);

   declareProperty( "TrackingGeometrySvc", 
         m_trackingGeometrySvc,
         "TrackingGeometrySvc used for track extrapolation" );
   declareProperty( "TrackingVolumeName",
         m_trackingVolumeName,
         "Name of the TrackingVolume within which the extrapolation is to be carried out");
   declareProperty( "Extrapolator", 
         m_extrapolator,
         "Extrapolator used for track extrapolation" );
}

/** Destructor **/
ISF::TrkExtrapolator::~TrkExtrapolator()
{
  delete m_pdgToParticleHypothesis;
}


/** Athena AlgTool initialization **/
StatusCode  ISF::TrkExtrapolator::initialize()
{
  ATH_MSG_VERBOSE("initialize() begin");

  StatusCode sc = m_extrapolator.retrieve();
  if (sc.isFailure()){
    ATH_MSG_FATAL( "Could not retrieve " << m_extrapolator );
    return StatusCode::FAILURE;
  }

  ATH_MSG_VERBOSE("initialize() successful");
  return StatusCode::SUCCESS;
}

/** Athena AlgTool finalization **/
StatusCode  ISF::TrkExtrapolator::finalize()
{
  ATH_MSG_VERBOSE("finalize() begin");

  ATH_MSG_VERBOSE("finalize() successful");
  return StatusCode::SUCCESS;
}

/** Extrapolate the given ISFParticle to the given TrackingVolume name */
ISF::ISFParticle* ISF::TrkExtrapolator::extrapolate( const ISF::ISFParticle &particle ) const {

  if( m_extrapolator.empty() ) {
    ATH_MSG_ERROR( "Problem with extrapolator!" );
    return 0;
  }
  
  if ( !m_trackingVolume ) {
    if ( !m_trackingGeometry ) { 
      if ( !retrieveTrackingGeometry() ) {
        ATH_MSG_ERROR( "Problem with TrackingGeometrySvc!" );
        return 0;
      }
    }
    m_trackingVolume = m_trackingGeometry->trackingVolume( m_trackingGeometryName);
    if (!m_trackingVolume) {
      ATH_MSG_FATAL("Failed to retrieve TrackingVolume: " << m_trackingGeometryName);
      return 0;
    }
  }
  
  // create objects from ISFParticle needed for extrapolation
  Trk::CurvilinearParameters par(particle.position(),particle.momentum(),particle.charge());
  
  int absPdg   = abs(particle.pdgCode());
  //bool photon   = (absPdg == 22);
  //bool geantino = (absPdg == 999);
  //bool charged  = photon || geantino ? false : (particle.charge()*particle.charge() > 0) ;
  
  Trk::ParticleHypothesis particleHypo = 
       m_pdgToParticleHypothesis->convert(particle.pdgCode(),particle.charge());
  if ( absPdg == 999 ) particleHypo = Trk::geantino;
  
  // extrapolate to calorimeter entry
  const Trk::TrackParameters* extrapolatedPars = m_extrapolator->extrapolateToVolume(par,*m_trackingVolume,Trk::alongMomentum,particleHypo);
  
  // create a new ISF particle representing the given particle at the extrapolated position
  ISFParticle *extrapolatedParticle = new ISFParticle( extrapolatedPars->position(),
                                                       extrapolatedPars->momentum(),
                                                       particle.mass(),
                                                       particle.charge(),
                                                       particle.pdgCode(),
                                                       particle.timeStamp(),
                                                       particle );
  
  // cleanup
  delete extrapolatedPars;

  return extrapolatedParticle;
}

bool ISF::TrkExtrapolator::retrieveTrackingGeometry() const 
{
  if ( m_trackingGeometry ) return true;
  if ( m_trackingGeometrySvc.empty() ) return false;
  MsgStream log(msgSvc(),name());
  StatusCode sc = m_trackingGeometrySvc.retrieve();
  if ( sc.isFailure() ){
    log << MSG::WARNING << " failed to retrieve geometry Svc " << m_trackingGeometrySvc << endmsg;
    return false;
  } 
  log << MSG::INFO << "  geometry Svc " << m_trackingGeometrySvc << " retrieved " << endmsg;
  
  m_trackingGeometry = m_trackingGeometrySvc->trackingGeometry();
  if ( !m_trackingGeometry ) return false;
  return true;
}
