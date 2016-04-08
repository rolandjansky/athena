/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// TrackParticlesInConeTool
//
//  (c) ATLAS software
//////////////////////////////////////////////////////////////////////////////

//<<<<<< INCLUDES                                                       >>>>>>
#include "TrackParticlesInConeTool.h"

namespace xAOD {
 
  //<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

  TrackParticlesInConeTool::TrackParticlesInConeTool (const std::string& type, const std::string& name, const IInterface* parent)
    :	AthAlgTool(type, name, parent),
        m_incidentSvc("IncidentSvc",name)
  {
    declareInterface<ITrackParticlesInConeTool>(this);
    declareProperty("TrackParticleLocation",m_indetTrackParticleLocation = "InDetTrackParticles");
  }

  TrackParticlesInConeTool::~TrackParticlesInConeTool()
  {}

  //<<<<<< PUBLIC MEMBER FUNCTION DEFINITIONS                             >>>>>>

  StatusCode TrackParticlesInConeTool::initialize() {
     // call handle in case of EndEvent
    ATH_CHECK(m_incidentSvc.retrieve());
    m_incidentSvc->addListener( this, IncidentType::EndEvent );

    return StatusCode::SUCCESS;
  }

  StatusCode TrackParticlesInConeTool::finalize() {
    return StatusCode::SUCCESS;
  }

  void TrackParticlesInConeTool::handle(const Incident& inc) {
    // Only clear cache for EndEvent incident
    if (inc.type()  == IncidentType::EndEvent){
      ATH_MSG_DEBUG(" clearing cache at end of event " );
      m_lookUpTable.clear();
    }  
  }

  const TrackParticleContainer* TrackParticlesInConeTool::retrieveTrackParticleContainer() const {
    // retrieve MuonSpectrometer tracks
    const TrackParticleContainer* indetTrackParticles = 0;
    if(evtStore()->contains<TrackParticleContainer>(m_indetTrackParticleLocation)) {
      if(evtStore()->retrieve(indetTrackParticles,m_indetTrackParticleLocation).isFailure()) {
        ATH_MSG_FATAL( "Unable to retrieve " << m_indetTrackParticleLocation );
        return 0;
      }
    }
    return indetTrackParticles;
  }

  bool TrackParticlesInConeTool::particlesInCone( float eta, float phi, float dr, std::vector< const TrackParticle*>& output ) {
    
    /// initialize if needed
    if( !m_lookUpTable.isInitialized() ){
      /// retrieve track particles
      const TrackParticleContainer* indetTrackParticles = retrieveTrackParticleContainer();
      if( !indetTrackParticles ) return false;
      m_lookUpTable.init(*indetTrackParticles);
    }
    return m_lookUpTable.iParticlesInCone( eta, phi, dr, output );
  }
}	// end of namespace
