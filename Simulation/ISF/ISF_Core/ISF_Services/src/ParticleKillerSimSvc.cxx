/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ParticleKillerSimSvc.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header include
#include "ParticleKillerSimSvc.h"

// ISF includes
#include "ISF_Event/ISFParticle.h"

/** Constructor **/
ISF::ParticleKillerSimSvc::ParticleKillerSimSvc(const std::string& name, ISvcLocator* svc) :
  BaseSimulationSvc(name, svc)
{
}

ISF::ParticleKillerSimSvc::~ParticleKillerSimSvc()
{}

/** framework methods */
StatusCode ISF::ParticleKillerSimSvc::initialize()
{
  ATH_MSG_INFO ( m_screenOutputPrefix << "initialize() ...");
  return StatusCode::SUCCESS;
}

/** framework methods */
StatusCode ISF::ParticleKillerSimSvc::finalize()
{
  ATH_MSG_INFO ( m_screenOutputPrefix << "finalize() ...");
  return StatusCode::SUCCESS;
}

StatusCode ISF::ParticleKillerSimSvc::setupEvent()
{
  ATH_MSG_DEBUG ( m_screenOutputPrefix << "setup Event");
  return StatusCode::SUCCESS;
}

StatusCode ISF::ParticleKillerSimSvc::releaseEvent()
{
  ATH_MSG_DEBUG ( m_screenOutputPrefix << "release Event");
  return StatusCode::SUCCESS;
}

/** Simulation Call */
StatusCode ISF::ParticleKillerSimSvc::simulate(const ISF::ISFParticle& particle)
{

  // give a screen output that you entered ParticleKillerSimSvc
  ATH_MSG_VERBOSE( m_screenOutputPrefix << "Particle '" << particle
                   << "' received for simulation." );
  ATH_MSG_VERBOSE( m_screenOutputPrefix << "Killing this particle, since this"
                   << "is the soul purpose of this 'simulation engine'." );

  // particle 'simulation' done
  // (memory management, ie delete, of the ISFParticle is done inside the ISFKernel)
  return StatusCode::SUCCESS;
}
