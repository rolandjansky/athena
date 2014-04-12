/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// TruthParticlesInConeTool
//
//  (c) ATLAS software
//////////////////////////////////////////////////////////////////////////////

//<<<<<< INCLUDES                                                       >>>>>>
#include "TruthParticlesInConeTool.h"

namespace xAOD {
 
  //<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

  TruthParticlesInConeTool::TruthParticlesInConeTool (const std::string& type, const std::string& name, const IInterface* parent)
    :	AthAlgTool(type, name, parent),
        m_incidentSvc("IncidentSvc",name)
  {
    declareInterface<ITruthParticlesInConeTool>(this);
    declareProperty("TruthParticleLocation",m_truthParticleLocation = "TruthParticles");
  }

  TruthParticlesInConeTool::~TruthParticlesInConeTool()
  {}

  //<<<<<< PUBLIC MEMBER FUNCTION DEFINITIONS                             >>>>>>

  StatusCode TruthParticlesInConeTool::initialize() {
     // call handle in case of EndEvent
    ATH_CHECK(m_incidentSvc.retrieve());
    m_incidentSvc->addListener( this, IncidentType::EndEvent );

    return StatusCode::SUCCESS;
  }

  StatusCode TruthParticlesInConeTool::finalize() {
    return StatusCode::SUCCESS;
  }

  void TruthParticlesInConeTool::handle(const Incident& inc) {
    // Only clear cache for EndEvent incident
    if (inc.type()  == IncidentType::EndEvent){
      ATH_MSG_DEBUG(" clearing cache at end of event " );
      m_lookUpTable.clear();
    }  
  }

  const TruthParticleContainer* TruthParticlesInConeTool::retrieveTruthParticleContainer() const {
    // retrieve MuonSpectrometer tracks
    const TruthParticleContainer* truthParticles = 0;
    if(evtStore()->contains<TruthParticleContainer>(m_truthParticleLocation)) {
      if(evtStore()->retrieve(truthParticles,m_truthParticleLocation).isFailure()) {
        ATH_MSG_FATAL( "Unable to retrieve " << m_truthParticleLocation );
        return 0;
      }
    }
    return truthParticles;
  }

  bool TruthParticlesInConeTool::particlesInCone( float eta, float phi, float dr, std::vector< const TruthParticle*>& output ) {

    /// initialize if needed
    if( !m_lookUpTable.isInitialized() ){
      /// retrieve track particles
      const TruthParticleContainer* truthParticles = retrieveTruthParticleContainer();
      if( !truthParticles ) return false;
      m_lookUpTable.init(*truthParticles);
    }
    return m_lookUpTable.iParticlesInCone( eta, phi, dr, output );
  }
}	// end of namespace
