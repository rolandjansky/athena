/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

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
  ATH_CHECK (m_simulatorTool.retrieve());
  return StatusCode::SUCCESS;
}

/** Simulation Call */
StatusCode ISF::ParticleKillerSimSvc::simulate(const ISF::ISFParticle& particle, McEventCollection* mcEventCollection)
{
  ATH_MSG_VERBOSE( m_screenOutputPrefix << " simulate" );
  ISFParticleContainer secondaries;
  ATH_CHECK(m_simulatorTool->simulate( particle,  secondaries, mcEventCollection));
  ATH_MSG_VERBOSE( "Returned "<< secondaries.size() << " secondaries.");
  return StatusCode::SUCCESS;
}
