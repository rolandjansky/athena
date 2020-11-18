/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// class header include
#include "LegacySimSvc.h"
#include "ISF_Interfaces/IParticleBroker.h"

/** Constructor **/
ISF::LegacySimSvc::LegacySimSvc(const std::string& name,ISvcLocator* svc) :
  BaseSimulationSvc(name, svc)
{
}

/** framework methods */
StatusCode ISF::LegacySimSvc::initialize()
{
  m_checkToolDeps = false;
  ATH_CHECK ( m_simulatorTool.retrieve() );
  return StatusCode::SUCCESS;
}

StatusCode ISF::LegacySimSvc::setupEvent()
{
  return m_simulatorTool->setupEventST();
}

StatusCode ISF::LegacySimSvc::releaseEvent()
{
  return m_simulatorTool->releaseEventST();
}

/** Simulation Call */
StatusCode ISF::LegacySimSvc::simulate(const ISF::ISFParticle& isfp, McEventCollection* mcEventCollection)
{
  ISF::ISFParticleContainer secondaries;
  ATH_CHECK(m_simulatorTool->simulate(isfp, secondaries, mcEventCollection));
  if (not secondaries.empty()) {
    for (auto particle : secondaries) {
      m_particleBroker->push( particle, &isfp);
    }
  }
  return StatusCode::SUCCESS;
}
