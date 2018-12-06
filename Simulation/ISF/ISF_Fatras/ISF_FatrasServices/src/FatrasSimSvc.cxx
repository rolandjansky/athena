/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// class header include
#include "FatrasSimSvc.h"
#include "ISF_Interfaces/IParticleBroker.h"

/** Constructor **/
iFatras::FatrasSimSvc::FatrasSimSvc(const std::string& name, ISvcLocator* svc) :
  BaseSimulationSvc(name, svc)
{
}

iFatras::FatrasSimSvc::~FatrasSimSvc()
{}

/** framework methods */
StatusCode iFatras::FatrasSimSvc::initialize()
{
  ATH_CHECK (m_simulatorTool.retrieve());
  return StatusCode::SUCCESS;
}

/** framework methods */
StatusCode iFatras::FatrasSimSvc::finalize()
{
  return StatusCode::SUCCESS;
}

StatusCode iFatras::FatrasSimSvc::setupEvent()
{
  return m_simulatorTool->setupEventST(); // empty method
}

StatusCode iFatras::FatrasSimSvc::releaseEvent()
{
  return m_simulatorTool->releaseEventST(); // empty method
}

/** Simulation Call */
StatusCode iFatras::FatrasSimSvc::simulate(const ISF::ISFParticle& isfp)
{
  ISF::ISFParticleContainer secondaries;
  ATH_CHECK(m_simulatorTool->simulate(isfp, secondaries));
  if (not secondaries.empty()) {
    for (auto particle : secondaries) {
      m_particleBroker->push( particle, &isfp);
    }
  }

  // Fatras call done
  return StatusCode::SUCCESS;
}
