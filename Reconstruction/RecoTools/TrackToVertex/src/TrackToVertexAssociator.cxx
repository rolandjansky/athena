///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TrackToVertexAssociator.cxx 
// Implementation file for class TrackToVertexAssociator
/////////////////////////////////////////////////////////////////// 

// TrackToVertex includes
#include "TrackToVertex/TrackToVertexAssociator.h"
// Inner Detector includes
#include "InDetBeamSpotService/IBeamCondSvc.h"
// Tracking includes
#include "TrkTrack/Track.h"
#include "TrkParticleBase/TrackParticleBase.h"
#include "TrkSurfaces/StraightLineSurface.h"
#include "VxVertex/VxContainer.h"
#include "VxVertex/VxCandidate.h"

//#include "CLHEP/Units/SystemOfUnits.h"
// StoreGate
#include "StoreGate/StoreGateSvc.h"



/////////////////
// Constructors
////////////////
Reco::TrackToVertexAssociator::TrackToVertexAssociator( const std::string& type, 
		      const std::string& name, 
		      const IInterface* parent ) : 
  ::AthAlgTool  ( type, name, parent   ),
  m_storeGate( "StoreGateSvc", name ),
  m_beamCondSvc("BeamCondSvc", name),
  m_incidentSvc("IncidentSvc", name),
  m_extrapolator("Trk::Extrapolator/AtlasExtrapolator"),
  m_primaryVertexContainer("VxPrimaryVertices"),
  m_primaryVertices(0),
  m_beamLine(0),
  m_particleHypo(2),
  m_cut(0)
{
  declareInterface< Reco::ITrackToVertexAssociator >(this);
  // Property declaration
  // Services 
  declareProperty( "EventStore",            m_storeGate );
  declareProperty( "BeamConditionsService", m_beamCondSvc );
  declareProperty( "IncidentService",       m_incidentSvc );
  // Tools
  declareProperty( "Extrapolator",          m_extrapolator );
  declareProperty( "ParticleHypothesis",    m_particleHypo );
  // input / output vertex collection 
  declareProperty( "PrimaryVertexCollection",   m_primaryVertexContainer);
  
}

// Destructor
///////////////
Reco::TrackToVertexAssociator::~TrackToVertexAssociator()
{}

// Athena algtool's Hooks
////////////////////////////
StatusCode Reco::TrackToVertexAssociator::initialize()
{  
  if (m_storeGate.retrieve().isFailure()){
      ATH_MSG_WARNING("Can not retrieve " << m_storeGate << ". Exiting.");
      return StatusCode::FAILURE;
  }
  if (m_beamCondSvc.retrieve().isFailure()){
      ATH_MSG_WARNING("Can not retrieve " << m_beamCondSvc << ". Exiting.");
      return StatusCode::FAILURE;
  }
  if (m_incidentSvc.retrieve().isFailure()){
      ATH_MSG_WARNING("Can not retrieve " << m_incidentSvc << ". Exiting.");
      return StatusCode::FAILURE;
  }
  // register to the incident service: EndEvent needed for memory cleanup
  m_incidentSvc->addListener( this, IncidentType::EndEvent );
  
  return StatusCode::SUCCESS;
}

void Reco::TrackToVertexAssociator::handle(const Incident& inc) {
    
     if ( inc.type() == IncidentType::EndEvent ){
         ATH_MSG_VERBOSE("'EndEvent' incident caught. Refreshing Cache.");
         if (cleanCache().isFailure())
             ATH_MSG_WARNING("Failure triggered while refreshing Cache.");
     }     
}

/** Interface method for use with TrackParticle  - AOD - hiding templated method*/
const std::vector<Reco::TrackToVertexAssociation>* Reco::TrackToVertexAssociator::associateTrack(const Trk::TrackParticleBase& /*particle*/) const
{
  ATH_MSG_WARNING("Method not implemented");
  return 0;
  //return runAssociation<Trk::TrackParticleBase>(particle);
}

/** Interface method for use with Track  - ESD - hiding templated method*/
const std::vector<Reco::TrackToVertexAssociation>* Reco::TrackToVertexAssociator::associateTrack(const Trk::Track& track) const
{
    return runAssociation<Trk::Track>(track);
}
                                          
/** Interface method for use with Parameters  - ESD/AOD - hiding templated method*/
const std::vector<Reco::TrackToVertexAssociation>* Reco::TrackToVertexAssociator::associateTrack(const Trk::TrackParameters& parameters) const
{
    return runAssociation<Trk::TrackParameters>(parameters);
}
   

/** Interface method for use with TrackParticle  - AOD - hiding templated method*/
const std::vector<Reco::TrackToVertexAssociation>* Reco::TrackToVertexAssociator::associateTrack(const Trk::NeutralParameters& /*particle*/) const
{
  ATH_MSG_WARNING("Method not implemented");
  return 0;
  //return runAssociation<Trk::NeutralParameters>(particle);
}

StatusCode Reco::TrackToVertexAssociator::cleanCache() const
{
  // clean out what you have
  // delete the beam line
  delete m_beamLine; m_beamLine = 0;
  // set primary vertices to 0 - memory handled by storegate
  m_primaryVertices        = 0;
  m_primaryVerticesWoDummy.clear();
  m_primaryVerticesBLFrame.clear(); 
  // return SUCCESS
  return StatusCode::SUCCESS;  
}

StatusCode Reco::TrackToVertexAssociator::updateCache() const 
{
  
  if (m_storeGate->retrieve(m_primaryVertices, m_primaryVertexContainer).isFailure() || m_primaryVertices->size() == 1){
      ATH_MSG_DEBUG("Could not retrieve Primary Vertex Container, or only dummy vertex in event. Doing nothing.");
      return StatusCode::RECOVERABLE;
  }
  

  delete m_beamLine; m_beamLine = 0;

  // get the transform

  Amg::Transform3D* beamTransform = new Amg::Transform3D(Amg::AngleAxis3D(m_beamCondSvc->beamTilt(0),Amg::Vector3D(0.,1.,0.)));
  (*beamTransform) *= Amg::AngleAxis3D(m_beamCondSvc->beamTilt(1),Amg::Vector3D(1.,0.,0.));
  beamTransform->pretranslate(m_beamCondSvc->beamPos());
  m_beamLine = new Trk::StraightLineSurface(beamTransform);
	 
  
  Amg::Transform3D  invBeamTransform = beamTransform->inverse();
  // fill in the Vertices w/o dummy vertex and transform the position
  m_primaryVerticesWoDummy.reserve(m_primaryVertices->size());
  m_primaryVerticesBLFrame.reserve(m_primaryVertices->size());
  VxContainer::const_iterator vxIter    = m_primaryVertices->begin();
  VxContainer::const_iterator vxIterEnd = m_primaryVertices->end();
  for ( ; vxIter != vxIterEnd-1; ++vxIter ){
      m_primaryVerticesWoDummy.push_back(*vxIter);
      m_primaryVerticesBLFrame.push_back(invBeamTransform*((*vxIter)->recVertex().position()));
  }    
  return StatusCode::SUCCESS;
}


StatusCode Reco::TrackToVertexAssociator::finalize()
{
  return StatusCode::SUCCESS;
}




