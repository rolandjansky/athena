/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ParametricSimSvc.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header include
#include "ISF_ParSimServices/ParametricSimSvc.h"

// Remaining ISF include
#include "ISF_Interfaces/IParticleBroker.h"
#include "ISF_Interfaces/IParticleProcessor.h"
#include "ISF_Event/ISFParticle.h"

/** Constructor **/
iParSim::ParametricSimSvc::ParametricSimSvc(const std::string& name, ISvcLocator* svc) :
  BaseSimulationSvc(name, svc),
  m_trackingGeometrySvc("TrackingGeometrySvc/AtlasTrackingGeometrySvc",name)

{
  // retrieve the simulation tool and the transport tool
  declareProperty("ParticleSmearers"     ,  m_particleSmearers     ); 
  declareProperty("TrackingGeometrySvc"  ,  m_trackingGeometrySvc  ,  "TrackingGeometrySvc used for track extrapolation" );

}

iParSim::ParametricSimSvc::~ParametricSimSvc() 
{}

/** framework methods */
StatusCode iParSim::ParametricSimSvc::initialize()
{
  // retrieve simulation tool
  if ( retrieveTools<ISF::IParticleProcessor>(m_particleSmearers).isFailure() ) {
    ATH_MSG_ERROR("Could not retrieve " << m_particleSmearers << ". Exiting.");
    return StatusCode::FAILURE;
  }

  /*if (m_trackingGeometrySvc.retrieve().isFailure()){
    ATH_MSG_ERROR("Could not retrieve " << m_trackingGeometrySvc << ". Exiting.");
    return StatusCode::FAILURE;
  }*/

  ATH_MSG_INFO( "initialize() successful." );
  return StatusCode::SUCCESS;
}

/** framework methods */
StatusCode iParSim::ParametricSimSvc::finalize()
{
  ATH_MSG_INFO ( m_screenOutputPrefix << "Finalizing ...");
  return StatusCode::SUCCESS;
}

StatusCode iParSim::ParametricSimSvc::setupEvent()
{ 
  ATH_MSG_DEBUG ( m_screenOutputPrefix << "setup Event");
  return StatusCode::SUCCESS;
}  

StatusCode iParSim::ParametricSimSvc::releaseEvent() 
{ 
  ATH_MSG_DEBUG ( m_screenOutputPrefix << "release Event");  
  return StatusCode::SUCCESS; 
}

/** Simulation Call */
StatusCode iParSim::ParametricSimSvc::simulate(const ISF::ISFParticle& isp)
{
  // give a screen output that you entered ParametricSimSvc
  ATH_MSG_VERBOSE( m_screenOutputPrefix << "Particle " << isp << " received for simulation." );
  for (auto& smearer : m_particleSmearers ){
      /** Process Particle from particle broker */
      ISF::ISFParticle* nISP = smearer->process(isp);
      ATH_MSG_VERBOSE( m_screenOutputPrefix << "Simulation created : " << ( nISP ? "" : "no") << " new particle");
      if (nISP) {
        // new particle into the stack
        m_particleBroker->push(nISP, &isp);
      }
  }
  return StatusCode::SUCCESS;
}


